namespace EngineClient
{
    partial class EngineClient
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            ButtonConnect = new Button();
            LabelIP = new Label();
            LabelPort = new Label();
            LabelResult = new Label();
            TextBoxIP = new TextBox();
            TextBoxPort = new TextBox();
            TextBoxResult = new TextBox();
            ButtonDisconnect = new Button();
            ButtonProjectorOn = new Button();
            ButtonProjectorOff = new Button();
            ButtonLEDOn = new Button();
            ButtonLEDOff = new Button();
            ButtonSetCurrent = new Button();
            TextBoxSetCurrent = new TextBox();
            ButtonGetCurrent = new Button();
            TextBoxGetCurrent = new TextBox();
            ButtonImageMount = new Button();
            ButtonSelectImage = new Button();
            ButtonSetFlipX = new Button();
            ButtonSetFlipY = new Button();
            ComboBoxEngineModel = new ComboBox();
            ButtonEngineSet = new Button();
            ButtonGetFlipY = new Button();
            ButtonGetFlipX = new Button();
            TextBoxGetTemp = new TextBox();
            ButtonGetTemp = new Button();
            CheckBoxGetFlipX = new CheckBox();
            CheckBoxGetFlipY = new CheckBox();
            CheckBoxSetFlipY = new CheckBox();
            CheckBoxSetFlipX = new CheckBox();
            TextBoxSetRotationAngle = new TextBox();
            ButtonSetRotationAngle = new Button();
            TextBoxGetRotationAngle = new TextBox();
            ButtonGetRotationAngle = new Button();
            SuspendLayout();
            // 
            // ButtonConnect
            // 
            ButtonConnect.Location = new Point(30, 21);
            ButtonConnect.Name = "ButtonConnect";
            ButtonConnect.Size = new Size(120, 77);
            ButtonConnect.TabIndex = 0;
            ButtonConnect.Text = "Connect";
            ButtonConnect.UseVisualStyleBackColor = true;
            ButtonConnect.Click += ButtonConnect_Click;
            // 
            // LabelIP
            // 
            LabelIP.AutoSize = true;
            LabelIP.Location = new Point(156, 24);
            LabelIP.Name = "LabelIP";
            LabelIP.Size = new Size(17, 15);
            LabelIP.TabIndex = 1;
            LabelIP.Text = "IP";
            // 
            // LabelPort
            // 
            LabelPort.AutoSize = true;
            LabelPort.Location = new Point(156, 51);
            LabelPort.Name = "LabelPort";
            LabelPort.Size = new Size(29, 15);
            LabelPort.TabIndex = 2;
            LabelPort.Text = "Port";
            // 
            // LabelResult
            // 
            LabelResult.AutoSize = true;
            LabelResult.Location = new Point(156, 78);
            LabelResult.Name = "LabelResult";
            LabelResult.Size = new Size(39, 15);
            LabelResult.TabIndex = 3;
            LabelResult.Text = "Result";
            // 
            // TextBoxIP
            // 
            TextBoxIP.Location = new Point(201, 21);
            TextBoxIP.Name = "TextBoxIP";
            TextBoxIP.Size = new Size(217, 23);
            TextBoxIP.TabIndex = 4;
            TextBoxIP.Text = "192.168.1.143";
            // 
            // TextBoxPort
            // 
            TextBoxPort.Location = new Point(201, 48);
            TextBoxPort.Name = "TextBoxPort";
            TextBoxPort.Size = new Size(217, 23);
            TextBoxPort.TabIndex = 5;
            TextBoxPort.Text = "52852";
            // 
            // TextBoxResult
            // 
            TextBoxResult.Location = new Point(201, 75);
            TextBoxResult.Name = "TextBoxResult";
            TextBoxResult.ReadOnly = true;
            TextBoxResult.Size = new Size(217, 23);
            TextBoxResult.TabIndex = 6;
            // 
            // ButtonDisconnect
            // 
            ButtonDisconnect.Location = new Point(30, 104);
            ButtonDisconnect.Name = "ButtonDisconnect";
            ButtonDisconnect.Size = new Size(120, 45);
            ButtonDisconnect.TabIndex = 7;
            ButtonDisconnect.Text = "Disconnect";
            ButtonDisconnect.UseVisualStyleBackColor = true;
            ButtonDisconnect.Click += ButtonDisconnect_Click;
            // 
            // ButtonProjectorOn
            // 
            ButtonProjectorOn.Location = new Point(30, 174);
            ButtonProjectorOn.Name = "ButtonProjectorOn";
            ButtonProjectorOn.Size = new Size(120, 41);
            ButtonProjectorOn.TabIndex = 8;
            ButtonProjectorOn.Text = "ProjectorOn";
            ButtonProjectorOn.UseVisualStyleBackColor = true;
            ButtonProjectorOn.Click += ButtonProjectorOn_Click;
            // 
            // ButtonProjectorOff
            // 
            ButtonProjectorOff.Location = new Point(30, 221);
            ButtonProjectorOff.Name = "ButtonProjectorOff";
            ButtonProjectorOff.Size = new Size(120, 41);
            ButtonProjectorOff.TabIndex = 9;
            ButtonProjectorOff.Text = "ProjectorOff";
            ButtonProjectorOff.UseVisualStyleBackColor = true;
            ButtonProjectorOff.Click += ButtonProjectorOff_Click;
            // 
            // ButtonLEDOn
            // 
            ButtonLEDOn.Location = new Point(156, 174);
            ButtonLEDOn.Name = "ButtonLEDOn";
            ButtonLEDOn.Size = new Size(120, 41);
            ButtonLEDOn.TabIndex = 10;
            ButtonLEDOn.Text = "LEDOn";
            ButtonLEDOn.UseVisualStyleBackColor = true;
            ButtonLEDOn.Click += ButtonLEDOn_Click;
            // 
            // ButtonLEDOff
            // 
            ButtonLEDOff.Location = new Point(156, 221);
            ButtonLEDOff.Name = "ButtonLEDOff";
            ButtonLEDOff.Size = new Size(120, 41);
            ButtonLEDOff.TabIndex = 11;
            ButtonLEDOff.Text = "LEDOff";
            ButtonLEDOff.UseVisualStyleBackColor = true;
            ButtonLEDOff.Click += ButtonLEDOff_Click;
            // 
            // ButtonSetCurrent
            // 
            ButtonSetCurrent.Location = new Point(326, 174);
            ButtonSetCurrent.Name = "ButtonSetCurrent";
            ButtonSetCurrent.Size = new Size(103, 41);
            ButtonSetCurrent.TabIndex = 12;
            ButtonSetCurrent.Text = "SetCurrent";
            ButtonSetCurrent.UseVisualStyleBackColor = true;
            ButtonSetCurrent.Click += ButtonSetCurrent_Click;
            // 
            // TextBoxSetCurrent
            // 
            TextBoxSetCurrent.Font = new Font("맑은 고딕", 18F);
            TextBoxSetCurrent.Location = new Point(326, 221);
            TextBoxSetCurrent.Name = "TextBoxSetCurrent";
            TextBoxSetCurrent.Size = new Size(103, 39);
            TextBoxSetCurrent.TabIndex = 13;
            // 
            // ButtonGetCurrent
            // 
            ButtonGetCurrent.Location = new Point(435, 174);
            ButtonGetCurrent.Name = "ButtonGetCurrent";
            ButtonGetCurrent.Size = new Size(103, 41);
            ButtonGetCurrent.TabIndex = 14;
            ButtonGetCurrent.Text = "GetCurrent";
            ButtonGetCurrent.UseVisualStyleBackColor = true;
            ButtonGetCurrent.Click += ButtonGetCurrent_Click;
            // 
            // TextBoxGetCurrent
            // 
            TextBoxGetCurrent.Font = new Font("맑은 고딕", 18F);
            TextBoxGetCurrent.Location = new Point(435, 221);
            TextBoxGetCurrent.Name = "TextBoxGetCurrent";
            TextBoxGetCurrent.ReadOnly = true;
            TextBoxGetCurrent.Size = new Size(103, 39);
            TextBoxGetCurrent.TabIndex = 15;
            // 
            // ButtonImageMount
            // 
            ButtonImageMount.Location = new Point(30, 338);
            ButtonImageMount.Name = "ButtonImageMount";
            ButtonImageMount.Size = new Size(120, 41);
            ButtonImageMount.TabIndex = 16;
            ButtonImageMount.Text = "ImageMount";
            ButtonImageMount.UseVisualStyleBackColor = true;
            ButtonImageMount.Click += ButtonImageMount_Click;
            // 
            // ButtonSelectImage
            // 
            ButtonSelectImage.Location = new Point(30, 293);
            ButtonSelectImage.Name = "ButtonSelectImage";
            ButtonSelectImage.Size = new Size(120, 41);
            ButtonSelectImage.TabIndex = 17;
            ButtonSelectImage.Text = "SelectImage";
            ButtonSelectImage.UseVisualStyleBackColor = true;
            ButtonSelectImage.Click += ButtonSelectImage_Click;
            // 
            // ButtonSetFlipX
            // 
            ButtonSetFlipX.Location = new Point(326, 293);
            ButtonSetFlipX.Name = "ButtonSetFlipX";
            ButtonSetFlipX.Size = new Size(103, 41);
            ButtonSetFlipX.TabIndex = 18;
            ButtonSetFlipX.Text = "SetFlipX";
            ButtonSetFlipX.UseVisualStyleBackColor = true;
            ButtonSetFlipX.Click += ButtonSetFlipX_Click;
            // 
            // ButtonSetFlipY
            // 
            ButtonSetFlipY.Location = new Point(326, 338);
            ButtonSetFlipY.Name = "ButtonSetFlipY";
            ButtonSetFlipY.Size = new Size(103, 41);
            ButtonSetFlipY.TabIndex = 19;
            ButtonSetFlipY.Text = "SetFlipY";
            ButtonSetFlipY.UseVisualStyleBackColor = true;
            ButtonSetFlipY.Click += ButtonSetFlipY_Click;
            // 
            // ComboBoxEngineModel
            // 
            ComboBoxEngineModel.FormattingEnabled = true;
            ComboBoxEngineModel.Location = new Point(435, 21);
            ComboBoxEngineModel.Name = "ComboBoxEngineModel";
            ComboBoxEngineModel.Size = new Size(103, 23);
            ComboBoxEngineModel.TabIndex = 20;
            // 
            // ButtonEngineSet
            // 
            ButtonEngineSet.Location = new Point(435, 48);
            ButtonEngineSet.Name = "ButtonEngineSet";
            ButtonEngineSet.Size = new Size(103, 41);
            ButtonEngineSet.TabIndex = 21;
            ButtonEngineSet.Text = "EngineSet";
            ButtonEngineSet.UseVisualStyleBackColor = true;
            ButtonEngineSet.Click += ButtonEngineSet_Click;
            // 
            // ButtonGetFlipY
            // 
            ButtonGetFlipY.Location = new Point(435, 338);
            ButtonGetFlipY.Name = "ButtonGetFlipY";
            ButtonGetFlipY.Size = new Size(103, 41);
            ButtonGetFlipY.TabIndex = 23;
            ButtonGetFlipY.Text = "GetFlipY";
            ButtonGetFlipY.UseVisualStyleBackColor = true;
            ButtonGetFlipY.Click += ButtonGetFlipY_Click;
            // 
            // ButtonGetFlipX
            // 
            ButtonGetFlipX.Location = new Point(435, 293);
            ButtonGetFlipX.Name = "ButtonGetFlipX";
            ButtonGetFlipX.Size = new Size(103, 41);
            ButtonGetFlipX.TabIndex = 22;
            ButtonGetFlipX.Text = "GetFlipX";
            ButtonGetFlipX.UseVisualStyleBackColor = true;
            ButtonGetFlipX.Click += ButtonGetFlipX_Click;
            // 
            // TextBoxGetTemp
            // 
            TextBoxGetTemp.Font = new Font("맑은 고딕", 18F);
            TextBoxGetTemp.Location = new Point(156, 340);
            TextBoxGetTemp.Name = "TextBoxGetTemp";
            TextBoxGetTemp.ReadOnly = true;
            TextBoxGetTemp.Size = new Size(120, 39);
            TextBoxGetTemp.TabIndex = 25;
            // 
            // ButtonGetTemp
            // 
            ButtonGetTemp.Location = new Point(156, 293);
            ButtonGetTemp.Name = "ButtonGetTemp";
            ButtonGetTemp.Size = new Size(120, 41);
            ButtonGetTemp.TabIndex = 24;
            ButtonGetTemp.Text = "GetTemp";
            ButtonGetTemp.UseVisualStyleBackColor = true;
            ButtonGetTemp.Click += ButtonGetTemp_Click;
            // 
            // CheckBoxGetFlipX
            // 
            CheckBoxGetFlipX.AutoSize = true;
            CheckBoxGetFlipX.Location = new Point(544, 305);
            CheckBoxGetFlipX.Name = "CheckBoxGetFlipX";
            CheckBoxGetFlipX.Size = new Size(15, 14);
            CheckBoxGetFlipX.TabIndex = 26;
            CheckBoxGetFlipX.UseVisualStyleBackColor = true;
            // 
            // CheckBoxGetFlipY
            // 
            CheckBoxGetFlipY.AutoSize = true;
            CheckBoxGetFlipY.Location = new Point(544, 350);
            CheckBoxGetFlipY.Name = "CheckBoxGetFlipY";
            CheckBoxGetFlipY.Size = new Size(15, 14);
            CheckBoxGetFlipY.TabIndex = 27;
            CheckBoxGetFlipY.UseVisualStyleBackColor = true;
            // 
            // CheckBoxSetFlipY
            // 
            CheckBoxSetFlipY.AutoSize = true;
            CheckBoxSetFlipY.Location = new Point(305, 350);
            CheckBoxSetFlipY.Name = "CheckBoxSetFlipY";
            CheckBoxSetFlipY.Size = new Size(15, 14);
            CheckBoxSetFlipY.TabIndex = 29;
            CheckBoxSetFlipY.UseVisualStyleBackColor = true;
            // 
            // CheckBoxSetFlipX
            // 
            CheckBoxSetFlipX.AutoSize = true;
            CheckBoxSetFlipX.Location = new Point(305, 305);
            CheckBoxSetFlipX.Name = "CheckBoxSetFlipX";
            CheckBoxSetFlipX.Size = new Size(15, 14);
            CheckBoxSetFlipX.TabIndex = 28;
            CheckBoxSetFlipX.UseVisualStyleBackColor = true;
            // 
            // TextBoxSetRotationAngle
            // 
            TextBoxSetRotationAngle.Font = new Font("맑은 고딕", 18F);
            TextBoxSetRotationAngle.Location = new Point(30, 455);
            TextBoxSetRotationAngle.Name = "TextBoxSetRotationAngle";
            TextBoxSetRotationAngle.Size = new Size(246, 39);
            TextBoxSetRotationAngle.TabIndex = 31;
            // 
            // ButtonSetRotationAngle
            // 
            ButtonSetRotationAngle.Font = new Font("맑은 고딕", 9F);
            ButtonSetRotationAngle.Location = new Point(30, 408);
            ButtonSetRotationAngle.Name = "ButtonSetRotationAngle";
            ButtonSetRotationAngle.Size = new Size(246, 41);
            ButtonSetRotationAngle.TabIndex = 30;
            ButtonSetRotationAngle.Text = "SetRotationAngle";
            ButtonSetRotationAngle.UseVisualStyleBackColor = true;
            ButtonSetRotationAngle.Click += ButtonSetRotationAngle_Click;
            // 
            // TextBoxGetRotationAngle
            // 
            TextBoxGetRotationAngle.Font = new Font("맑은 고딕", 18F);
            TextBoxGetRotationAngle.Location = new Point(326, 455);
            TextBoxGetRotationAngle.Name = "TextBoxGetRotationAngle";
            TextBoxGetRotationAngle.ReadOnly = true;
            TextBoxGetRotationAngle.Size = new Size(212, 39);
            TextBoxGetRotationAngle.TabIndex = 33;
            // 
            // ButtonGetRotationAngle
            // 
            ButtonGetRotationAngle.Font = new Font("맑은 고딕", 9F);
            ButtonGetRotationAngle.Location = new Point(326, 408);
            ButtonGetRotationAngle.Name = "ButtonGetRotationAngle";
            ButtonGetRotationAngle.Size = new Size(212, 41);
            ButtonGetRotationAngle.TabIndex = 32;
            ButtonGetRotationAngle.Text = "GetRotationAngle";
            ButtonGetRotationAngle.UseVisualStyleBackColor = true;
            ButtonGetRotationAngle.Click += ButtonGetRotationAngle_Click;
            // 
            // EngineClient
            // 
            AutoScaleMode = AutoScaleMode.None;
            ClientSize = new Size(577, 515);
            Controls.Add(TextBoxGetRotationAngle);
            Controls.Add(ButtonGetRotationAngle);
            Controls.Add(TextBoxSetRotationAngle);
            Controls.Add(ButtonSetRotationAngle);
            Controls.Add(CheckBoxSetFlipY);
            Controls.Add(CheckBoxSetFlipX);
            Controls.Add(CheckBoxGetFlipY);
            Controls.Add(CheckBoxGetFlipX);
            Controls.Add(TextBoxGetTemp);
            Controls.Add(ButtonGetTemp);
            Controls.Add(ButtonGetFlipY);
            Controls.Add(ButtonGetFlipX);
            Controls.Add(ButtonEngineSet);
            Controls.Add(ComboBoxEngineModel);
            Controls.Add(ButtonSetFlipY);
            Controls.Add(ButtonSetFlipX);
            Controls.Add(ButtonSelectImage);
            Controls.Add(ButtonImageMount);
            Controls.Add(TextBoxGetCurrent);
            Controls.Add(ButtonGetCurrent);
            Controls.Add(TextBoxSetCurrent);
            Controls.Add(ButtonSetCurrent);
            Controls.Add(ButtonLEDOff);
            Controls.Add(ButtonLEDOn);
            Controls.Add(ButtonProjectorOff);
            Controls.Add(ButtonProjectorOn);
            Controls.Add(ButtonDisconnect);
            Controls.Add(TextBoxResult);
            Controls.Add(TextBoxPort);
            Controls.Add(TextBoxIP);
            Controls.Add(LabelResult);
            Controls.Add(LabelPort);
            Controls.Add(LabelIP);
            Controls.Add(ButtonConnect);
            Name = "EngineClient";
            Text = "EngineClient";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button ButtonConnect;
        private Label LabelIP;
        private Label LabelPort;
        private Label LabelResult;
        private TextBox TextBoxIP;
        private TextBox TextBoxPort;
        private TextBox TextBoxResult;
        private Button ButtonDisconnect;
        private Button ButtonProjectorOn;
        private Button ButtonProjectorOff;
        private Button ButtonLEDOn;
        private Button ButtonLEDOff;
        private Button ButtonSetCurrent;
        private TextBox TextBoxSetCurrent;
        private Button ButtonGetCurrent;
        private TextBox TextBoxGetCurrent;
        private Button ButtonImageMount;
        private Button ButtonSelectImage;
        private Button ButtonSetFlipX;
        private Button ButtonSetFlipY;
        private ComboBox ComboBoxEngineModel;
        private Button ButtonEngineSet;
        private Button ButtonGetFlipY;
        private Button ButtonGetFlipX;
        private TextBox TextBoxGetTemp;
        private Button ButtonGetTemp;
        private CheckBox CheckBoxGetFlipX;
        private CheckBox CheckBoxGetFlipY;
        private CheckBox CheckBoxSetFlipY;
        private CheckBox CheckBoxSetFlipX;
        private TextBox TextBoxSetRotationAngle;
        private Button ButtonSetRotationAngle;
        private TextBox TextBoxGetRotationAngle;
        private Button ButtonGetRotationAngle;
    }
}
