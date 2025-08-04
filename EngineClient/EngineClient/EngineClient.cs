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
            ComboBoxEngineModel.SelectedItem = "NVR";
        }

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
                            else if (message.StartsWith("GetCurrent:"))
                            {
                                string currentValue = message.Substring(11);
                                TextBoxGetCurrent.Text = currentValue;
                                TextBoxResult.Text = $"���� �� ����: {currentValue}";
                            }
                            else if (message.StartsWith("GetFlipX:"))
                            {
                                string currentValue = message.Substring(9);
                                TextBoxResult.Text = $"���� �� ����: {currentValue}";
                                if (Convert.ToInt16(currentValue) == 0)
                                    CheckBoxGetFlipX.Checked = false;
                                else
                                    CheckBoxGetFlipX.Checked = true;
                            }
                            else if (message.StartsWith("GetFlipY:"))
                            {
                                string currentValue = message.Substring(9);
                                TextBoxResult.Text = $"���� �� ����: {currentValue}";
                                if (Convert.ToInt16(currentValue) == 0)
                                    CheckBoxGetFlipY.Checked = false;
                                else
                                    CheckBoxGetFlipY.Checked = true;
                            }
                            else if (message.StartsWith("GetRotationAngle:"))
                            {
                                string currentValue = message.Substring(17);
                                TextBoxResult.Text = $"���� �� ����: {currentValue}";
                                TextBoxGetRotationAngle.Text = currentValue;
                            }
                            else if (message.StartsWith("GetTemp:"))
                            {
                                string currentValue = message.Substring(8);
                                TextBoxResult.Text = $"���� �� ����: {currentValue}";
                                TextBoxGetTemp.Text = currentValue;
                            }
                            // ... �ʿ��� ��� Ȯ���� ��
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

        private async void ButtonSetFlipX_Click(object sender, EventArgs e)
        {
            if (CheckBoxSetFlipX.Checked)
                await SendCommandAsync("SetFlipX:1");
            else
                await SendCommandAsync("SetFlipX:0");
        }

        private async void ButtonSetFlipY_Click(object sender, EventArgs e)
        {
            if (CheckBoxSetFlipY.Checked)
                await SendCommandAsync("SetFlipY:1");
            else
                await SendCommandAsync("SetFlipY:0");
        }

        private async void ButtonGetFlipX_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("GetFlipX");
        }

        private async void ButtonGetFlipY_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("GetFlipY");
        }

        private async void ButtonSetRotationAngle_Click(object sender, EventArgs e)
        {
            // ȸ�� ������ 0, 90, 180, 270���� �Է��� �� ����
            if (int.TryParse(TextBoxSetRotationAngle.Text, out int angle) && (angle == 0 || angle == 90 || angle == 180 || angle == 270))
            {
                // ��ȿ�ϸ� "SetRotationAngle:{angle}" ��� ����
                await SendCommandAsync($"SetRotationAngle:{angle}");
            }
            else
            {
                TextBoxResult.Text = "0, 90, 180 ,270 �� �ϳ��� �Է��ϼ���";
            }
        }

        private async void ButtonGetRotationAngle_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("GetRotationAngle");
        }

        private async void ButtonGetTemp_Click(object sender, EventArgs e)
        {
            await SendCommandAsync("GetTemp");
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
