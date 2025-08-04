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

            // 엔진 모델 추가 및 선택
            ComboBoxEngineModel.Items.AddRange(["NVR", "NQM+"]);
            ComboBoxEngineModel.SelectedItem = "NQM+";
        }

        //public async Task<string> ConnectAsync(string ipAddress, int port = 52852)
        //{
        //    try
        //    {
        //        // 이전에 연결된 리소스가 있다면 정리
        //        Disconnect();

        //        tcpClient = new TcpClient();
        //        cts = new CancellationTokenSource(); // 취소 토큰 초기화

        //        // Wait() 대신 await를 사용하여 UI 스레드를 차단하지 않음
        //        await tcpClient.ConnectAsync(ipAddress, port);

        //        if (!tcpClient.Connected)
        //        {
        //            throw new Exception("Connection failed.");
        //        }

        //        stream = tcpClient.GetStream();

        //        using (var reader = new StreamReader(stream, Encoding.UTF8, true, 1024, true)) // 마지막 true: 스트림을 닫지 않음
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

        // 서버로부터의 메시지를 지속적으로 수신하는 비동기 메서드
        private async Task ListenForServerMessagesAsync(CancellationToken token)
        {
            bool isFirstMessage = true; // 첫 메시지인지 확인하기 위한 플래그
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
                            // 첫 번째 메시지는 연결 결과로 처리
                            if (isFirstMessage)
                            {
                                TextBoxResult.Text = $"연결 성공: {message}";
                                isFirstMessage = false;
                            }
                            // 두 번째 이후 메시지는 명령어 응답으로 처리
                            else if (message.StartsWith("CurrentValue:"))
                            {
                                string currentValue = message.Substring(13);
                                TextBoxGetCurrent.Text = currentValue;
                                TextBoxResult.Text = $"현재 값 수신: {currentValue}";
                            }
                            // 다른 종류의 서버 응답 처리
                        }));
                    }
                }
            }
            catch (Exception)
            {
                this.Invoke((Action)(() => TextBoxResult.Text = "서버와의 연결이 끊어졌습니다."));
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
                // Disconnect 중 발생하는 오류는 무시
            }
        }

        private async void ButtonConnect_Click(object sender, EventArgs e)
        {
            // 연결 버튼을 누르면 "연결 중..." 같은 메시지를 표시하고 UI를 비활성화할 수 있습니다.
            ButtonConnect.Enabled = false;
            TextBoxResult.Text = "연결 중...";

            // ConnectAsync를 직접 호출하는 대신, 연결과 리스너 시작을 이 이벤트 핸들러에서 관리합니다.
            try
            {
                Disconnect(); // 이전 연결 정리

                tcpClient = new TcpClient();
                cts = new CancellationTokenSource();

                await tcpClient.ConnectAsync(TextBoxIP.Text, int.Parse(TextBoxPort.Text));
                if (!tcpClient.Connected)
                {
                    throw new Exception("서버에 연결할 수 없습니다.");
                }

                stream = tcpClient.GetStream();

                // 중요: 연결 성공 직후, 서버 메시지 수신을 위한 백그라운드 Task 시작
                _ = ListenForServerMessagesAsync(cts.Token);

                TextBoxResult.Text = "서버에 연결되었습니다. 초기 정보를 기다리는 중...";
            }
            catch (Exception ex)
            {
                Disconnect();
                TextBoxResult.Text = $"연결 실패: {ex.Message}";
            }
            finally
            {
                ButtonConnect.Enabled = true;
            }
        }

        private void ButtonDisconnect_Click(object sender, EventArgs e)
        {
            Disconnect();
            TextBoxResult.Text = "연결이 종료되었습니다.";
        }

        private async Task SendCommandAsync(string command)
        {
            // 1. 스트림 유효성 검사
            if (stream == null || !stream.CanWrite)
            {
                TextBoxResult.Text = "서버에 연결되어 있지 않습니다.";
                return;
            }

            try
            {
                // 명령어 끝에 \n(줄바꿈) 추가하여 서버가 라인 단위로 읽게 함
                byte[] messageBytes = System.Text.Encoding.UTF8.GetBytes(command + "\n");

                // 비동기적으로 데이터 전송
                await stream.WriteAsync(messageBytes, 0, messageBytes.Length);
                TextBoxResult.Text = $"명령 전송: {command}";
            }
            catch (Exception ex)
            {
                TextBoxResult.Text = $"명령 전송 실패: {ex.Message}";
                Disconnect();
            }
        }

        private async void ButtonEngineSet_Click(object sender, EventArgs e)
        {
            // 콤보박스에서 선택된 항목 가져오기
            string? selectedEngine = ComboBoxEngineModel.SelectedItem?.ToString();
            if (string.IsNullOrEmpty(selectedEngine))
            {
                TextBoxResult.Text = "엔진 모델을 먼저 선택하세요.";
                return;
            }

            // "EngineSet:모델명\n" 형식의 메시지 생성, 서버가 라인 단위로 읽을 수 있도록 끝에 \n 추가
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
            // TextBox에서 값을 읽고 유효성 검사 (0~1023)
            if (int.TryParse(TextBoxSetCurrent.Text, out int value) && value >= 0 && value <= 1023)
            {
                // 유효하면 "SetCurrent:{value}" 명령 전송
                await SendCommandAsync($"SetCurrent:{value}");
            }
            else
            {
                TextBoxResult.Text = "0에서 1023 사이의 숫자를 입력하세요.";
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
                TextBoxResult.Text = "서버에 연결되지 않았습니다.";
                return false;
            }
            if (string.IsNullOrEmpty(filePath) || !File.Exists(filePath))
            {
                TextBoxResult.Text = "유효한 이미지 파일을 선택하세요.";
                return false;
            }

            try
            {
                // 1. "이제부터 이미지 데이터를 보내겠다"는 신호를 먼저 보냅니다.
                //    SendCommandAsync를 재사용하여 코드를 간결하게 유지합니다.
                await SendCommandAsync("Image:");
                TextBoxResult.Text = "이미지 전송을 시작합니다...";

                // 2. 실제 이미지 파일 데이터를 읽습니다.
                byte[] imageData = await File.ReadAllBytesAsync(filePath);
                long imageSize = imageData.Length;
                byte[] sizeInfo = BitConverter.GetBytes(imageSize);

                // 3. 8바이트의 크기 정보(헤더)를 보냅니다.
                await stream.WriteAsync(sizeInfo, 0, sizeInfo.Length);

                // 4. 실제 이미지 데이터를 보냅니다.
                await stream.WriteAsync(imageData, 0, imageData.Length);

                await stream.FlushAsync(); // 버퍼에 남은 데이터를 즉시 전송
                TextBoxResult.Text = "이미지 전송 완료!";

                return true;
            }
            catch (Exception ex)
            {
                TextBoxResult.Text = $"이미지 전송 실패: {ex.Message}";
                Disconnect();
                return false;
            }
        }
    }
}
