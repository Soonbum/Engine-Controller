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
                // 이전에 연결된 리소스가 있다면 정리
                Disconnect();

                tcpClient = new TcpClient();

                // Wait() 대신 await를 사용하여 UI 스레드를 차단하지 않음
                await tcpClient.ConnectAsync(ipAddress, port);

                if (!tcpClient.Connected)
                {
                    throw new Exception("Connection failed.");
                }

                stream = tcpClient.GetStream();

                using (var reader = new StreamReader(stream, Encoding.UTF8, true, 1024, true)) // 마지막 true: 스트림을 닫지 않음
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
                // Disconnect 중 발생하는 오류는 무시
            }
        }

        private async void ButtonConnect_Click(object sender, EventArgs e)
        {
            // 연결 버튼을 누르면 "연결 중..." 같은 메시지를 표시하고 UI를 비활성화할 수 있습니다.
            ButtonConnect.Enabled = false;
            TextBoxResult.Text = "연결 중...";

            // await 키워드로 비동기 함수의 완료를 기다립니다. 이 동안 UI는 멈추지 않습니다.
            string result = await ConnectAsync(TextBoxIP.Text, int.Parse(TextBoxPort.Text));
            TextBoxResult.Text = result;

            // 작업이 끝나면 다시 버튼 활성화
            ButtonConnect.Enabled = true;
        }

        private void ButtonDisconnect_Click(object sender, EventArgs e)
        {
            Disconnect();
            TextBoxResult.Text = "연결이 종료되었습니다.";
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
            openFileDialog.Title = "이미지 선택";
            openFileDialog.Filter = "이미지 파일 (*.jpg;*.jpeg;*.png;*.bmp;*.gif)|*.jpg;*.jpeg;*.png;*.bmp;*.gif";
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
                //Console.WriteLine("서버에 연결되지 않았습니다. 먼저 ConnectAsync를 호출하세요.");
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
                //Console.WriteLine($"이미지 전송 실패: {ex.Message}");
                return false;
            }
        }

        private void ButtonEngineSet_Click(object sender, EventArgs e)
        {

        }
    }
}
