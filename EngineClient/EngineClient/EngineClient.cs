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
        private string selectedImagePath = "";

        public EngineClient()
        {
            InitializeComponent();
        }

        public async Task<string> ConnectAsync(string ipAddress, int port = 52852)
        {
            try
            {
                // ������ ����� ���ҽ��� �ִٸ� ����
                Disconnect();

                tcpClient = new TcpClient();

                // Wait() ��� await�� ����Ͽ� UI �����带 �������� ����
                await tcpClient.ConnectAsync(ipAddress, port);

                if (!tcpClient.Connected)
                {
                    throw new Exception("Connection failed.");
                }

                stream = tcpClient.GetStream();

                using (var reader = new StreamReader(stream, Encoding.UTF8, true, 1024, true)) // ������ true: ��Ʈ���� ���� ����
                {
                    return await reader.ReadLineAsync();
                }
            }
            catch (Exception ex)
            {
                Disconnect();
                return $"Connection Failed: {ex.Message}";
            }
        }

        public void Disconnect()
        {
            try
            {
                stream?.Close();
                tcpClient?.Close();
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

            // await Ű����� �񵿱� �Լ��� �ϷḦ ��ٸ��ϴ�. �� ���� UI�� ������ �ʽ��ϴ�.
            string result = await ConnectAsync(TextBoxIP.Text, int.Parse(TextBoxPort.Text));
            TextBoxResult.Text = result;

            // �۾��� ������ �ٽ� ��ư Ȱ��ȭ
            ButtonConnect.Enabled = true;
        }

        private void ButtonDisconnect_Click(object sender, EventArgs e)
        {
            Disconnect();
            TextBoxResult.Text = "������ ����Ǿ����ϴ�.";
        }

        private void ButtonProjectorOn_Click(object sender, EventArgs e)
        {

        }

        private void ButtonProjectorOff_Click(object sender, EventArgs e)
        {

        }

        private void ButtonLEDOn_Click(object sender, EventArgs e)
        {

        }

        private void ButtonLEDOff_Click(object sender, EventArgs e)
        {

        }

        private void ButtonSetCurrent_Click(object sender, EventArgs e)
        {

        }

        private void ButtonGetCurrent_Click(object sender, EventArgs e)
        {

        }

        private void ButtonImageMount_Click(object sender, EventArgs e)
        {
            if (selectedImagePath == null || selectedImagePath == "")
                return;

            SendImage(selectedImagePath);
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

        }

        private void ButtonSetFlipY_Click(object sender, EventArgs e)
        {

        }

        public bool SendImage(string filePath)
        {
            if (stream == null || !stream.CanWrite)
            {
                //Console.WriteLine("������ ������� �ʾҽ��ϴ�. ���� ConnectAsync�� ȣ���ϼ���.");
                return false;
            }

            try
            {
                byte[] imageData = File.ReadAllBytes(filePath);
                long imageSize = imageData.Length;

                byte[] sizeInfo = BitConverter.GetBytes(imageSize);
                stream.Write(sizeInfo, 0, sizeInfo.Length);
                stream.Write(imageData, 0, imageData.Length);

                return true;
            }
            catch (Exception ex)
            {
                //Console.WriteLine($"�̹��� ���� ����: {ex.Message}");
                return false;
            }
        }

        private void ButtonEngineSet_Click(object sender, EventArgs e)
        {

        }
    }
}
