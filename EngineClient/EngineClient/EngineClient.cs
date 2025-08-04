using System;
using System.IO;
using System.Net.Http;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace EngineClient
{
    public partial class EngineClient : Form
    {
        private TcpClient tcpClient;
        private NetworkStream stream;
        private CancellationTokenSource cts;
        private string selectedImagePath = "";

        public EngineClient()
        {
            InitializeComponent();

            // ���� �� �߰� �� ����
            ComboBoxEngineModel.Items.AddRange(["NVR", "NQM+"]);
            ComboBoxEngineModel.SelectedItem = "NQM+";
        }

        //public async Task<string> ConnectAsync(string ipAddress, int port = 52852)
        //{
        //    try
        //    {
        //        // ������ ����� ���ҽ��� �ִٸ� ����
        //        Disconnect();

        //        tcpClient = new TcpClient();
        //        cts = new CancellationTokenSource(); // ��� ��ū �ʱ�ȭ

        //        // Wait() ��� await�� ����Ͽ� UI �����带 �������� ����
        //        await tcpClient.ConnectAsync(ipAddress, port);

        //        if (!tcpClient.Connected)
        //        {
        //            throw new Exception("Connection failed.");
        //        }

        //        stream = tcpClient.GetStream();

        //        using (var reader = new StreamReader(stream, Encoding.UTF8, true, 1024, true)) // ������ true: ��Ʈ���� ���� ����
        //        {
        //            return await reader.ReadLineAsync();
        //        }
        //    }
        //    catch (Exception ex)
        //    {
        //        Disconnect();
        //        return $"Connection Failed: {ex.Message}";
        //    }
        //}

        // �����κ����� �޽����� ���������� �����ϴ� �񵿱� �޼���
        private async Task ListenForServerMessagesAsync(CancellationToken token)
        {
            bool isFirstMessage = true; // ù �޽������� Ȯ���ϱ� ���� �÷���
            try
            {
                using (var reader = new StreamReader(stream, Encoding.UTF8, true, 1024, true))
                {
                    while (!token.IsCancellationRequested)
                    {
                        var message = await reader.ReadLineAsync();
                        if (message == null) break;

                        this.Invoke((Action)(() =>
                        {
                            // ù ��° �޽����� ���� ����� ó��
                            if (isFirstMessage)
                            {
                                TextBoxResult.Text = $"���� ����: {message}";
                                isFirstMessage = false;
                            }
                            // �� ��° ���� �޽����� ��ɾ� �������� ó��
                            else if (message.StartsWith("CurrentValue:"))
                            {
                                string currentValue = message.Substring(13);
                                TextBoxGetCurrent.Text = currentValue;
                                TextBoxResult.Text = $"���� �� ����: {currentValue}";
                            }
                            // �ٸ� ������ ���� ���� ó��
                        }));
                    }
                }
            }
            catch (Exception)
            {
                this.Invoke((Action)(() => TextBoxResult.Text = "�������� ������ ���������ϴ�."));
            }
            finally
            {
                Disconnect();
            }
        }

        public void Disconnect()
        {
            try
            {
                cts?.Cancel();
                stream?.Close();
                tcpClient?.Close();

                cts = null;
                stream = null;
                tcpClient = null;
            }
            catch (Exception)
            {
                // Disconnect �� �߻��ϴ� ������ ����
            }
        }

        private async void ButtonConnect_Click(object sender, EventArgs e)
        {
            // ���� ��ư�� ������ "���� ��..." ���� �޽����� ǥ���ϰ� UI�� ��Ȱ��ȭ�� �� �ֽ��ϴ�.
            ButtonConnect.Enabled = false;
            TextBoxResult.Text = "���� ��...";

            // ConnectAsync�� ���� ȣ���ϴ� ���, ����� ������ ������ �� �̺�Ʈ �ڵ鷯���� �����մϴ�.
            try
            {
                Disconnect(); // ���� ���� ����

                tcpClient = new TcpClient();
                cts = new CancellationTokenSource();

                await tcpClient.ConnectAsync(TextBoxIP.Text, int.Parse(TextBoxPort.Text));
                if (!tcpClient.Connected)
                {
                    throw new Exception("������ ������ �� �����ϴ�.");
                }

                stream = tcpClient.GetStream();

                // �߿�: ���� ���� ����, ���� �޽��� ������ ���� ��׶��� Task ����
                _ = ListenForServerMessagesAsync(cts.Token);

                TextBoxResult.Text = "������ ����Ǿ����ϴ�. �ʱ� ������ ��ٸ��� ��...";
            }
            catch (Exception ex)
            {
                Disconnect();
                TextBoxResult.Text = $"���� ����: {ex.Message}";
            }
            finally
            {
                ButtonConnect.Enabled = true;
            }
        }

        private void ButtonDisconnect_Click(object sender, EventArgs e)
        {
            Disconnect();
            TextBoxResult.Text = "������ ����Ǿ����ϴ�.";
        }

        private async Task SendCommandAsync(string command)
        {
            // 1. ��Ʈ�� ��ȿ�� �˻�
            if (stream == null || !stream.CanWrite)
            {
                TextBoxResult.Text = "������ ����Ǿ� ���� �ʽ��ϴ�.";
                return;
            }

            try
            {
                // ��ɾ� ���� \n(�ٹٲ�) �߰��Ͽ� ������ ���� ������ �а� ��
                byte[] messageBytes = System.Text.Encoding.UTF8.GetBytes(command + "\n");

                // �񵿱������� ������ ����
                await stream.WriteAsync(messageBytes, 0, messageBytes.Length);
                TextBoxResult.Text = $"��� ����: {command}";
            }
            catch (Exception ex)
            {
                TextBoxResult.Text = $"��� ���� ����: {ex.Message}";
                Disconnect();
            }
        }

        private async void ButtonEngineSet_Click(object sender, EventArgs e)
        {
            // �޺��ڽ����� ���õ� �׸� ��������
            string? selectedEngine = ComboBoxEngineModel.SelectedItem?.ToString();
            if (string.IsNullOrEmpty(selectedEngine))
            {
                TextBoxResult.Text = "���� ���� ���� �����ϼ���.";
                return;
            }

            // "EngineSet:�𵨸�\n" ������ �޽��� ����, ������ ���� ������ ���� �� �ֵ��� ���� \n �߰�
            string message = $"EngineSet:{selectedEngine}\n";

            await SendCommandAsync(message);
        }

        private async void ButtonProjectorOn_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("ProjectorOn");
        }

        private async void ButtonProjectorOff_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("ProjectorOff");
        }

        private async void ButtonLEDOn_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("LEDOn");
        }

        private async void ButtonLEDOff_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("LEDOff");
        }

        private async void ButtonSetCurrent_Click(object sender, EventArgs e)
        {
            // TextBox���� ���� �а� ��ȿ�� �˻� (0~1023)
            if (int.TryParse(TextBoxSetCurrent.Text, out int value) && value >= 0 && value <= 1023)
            {
                // ��ȿ�ϸ� "SetCurrent:{value}" ��� ����
                await SendCommandAsync($"SetCurrent:{value}");
            }
            else
            {
                TextBoxResult.Text = "0���� 1023 ������ ���ڸ� �Է��ϼ���.";
            }
        }

        private async void ButtonGetCurrent_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("GetCurrent");
        }

        private async void ButtonImageMount_Click(object sender, EventArgs e)
        {
            if (selectedImagePath == null || selectedImagePath == "")
                return;

            await SendImageAsync(selectedImagePath);
        }

        private void ButtonSelectImage_Click(object sender, EventArgs e)
        {
            using OpenFileDialog openFileDialog = new();
            openFileDialog.Title = "�̹��� ����";
            openFileDialog.Filter = "�̹��� ���� (*.jpg;*.jpeg;*.png;*.bmp;*.gif)|*.jpg;*.jpeg;*.png;*.bmp;*.gif";
            openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyPictures);
            openFileDialog.Multiselect = false;

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                selectedImagePath = openFileDialog.FileName;
            }
        }

        private void ButtonSetFlipX_Click(object sender, EventArgs e)
        {
            // ...
        }

        private void ButtonSetFlipY_Click(object sender, EventArgs e)
        {
            // ...
        }

        public async Task<bool> SendImageAsync(string filePath)
        {
            if (stream == null || !stream.CanWrite)
            {
                TextBoxResult.Text = "������ ������� �ʾҽ��ϴ�.";
                return false;
            }
            if (string.IsNullOrEmpty(filePath) || !File.Exists(filePath))
            {
                TextBoxResult.Text = "��ȿ�� �̹��� ������ �����ϼ���.";
                return false;
            }

            try
            {
                // 1. "�������� �̹��� �����͸� �����ڴ�"�� ��ȣ�� ���� �����ϴ�.
                //    SendCommandAsync�� �����Ͽ� �ڵ带 �����ϰ� �����մϴ�.
                await SendCommandAsync("Image:");
                TextBoxResult.Text = "�̹��� ������ �����մϴ�...";

                // 2. ���� �̹��� ���� �����͸� �н��ϴ�.
                byte[] imageData = await File.ReadAllBytesAsync(filePath);
                long imageSize = imageData.Length;
                byte[] sizeInfo = BitConverter.GetBytes(imageSize);

                // 3. 8����Ʈ�� ũ�� ����(���)�� �����ϴ�.
                await stream.WriteAsync(sizeInfo, 0, sizeInfo.Length);

                // 4. ���� �̹��� �����͸� �����ϴ�.
                await stream.WriteAsync(imageData, 0, imageData.Length);

                await stream.FlushAsync(); // ���ۿ� ���� �����͸� ��� ����
                TextBoxResult.Text = "�̹��� ���� �Ϸ�!";

                return true;
            }
            catch (Exception ex)
            {
                TextBoxResult.Text = $"�̹��� ���� ����: {ex.Message}";
                Disconnect();
                return false;
            }
        }
    }
}
