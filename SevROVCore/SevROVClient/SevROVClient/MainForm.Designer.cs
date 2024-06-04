namespace SevROVClient
{
    partial class MainForm
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.connectButton = new System.Windows.Forms.Button();
            this.hostInput = new System.Windows.Forms.TextBox();
            this.portInput = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.connectionStatus = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.depth_stabilization_check = new System.Windows.Forms.CheckBox();
            this.yaw_stabilization_check = new System.Windows.Forms.CheckBox();
            this.pitch_stabilization_check = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.rollSetPoint = new System.Windows.Forms.Label();
            this.pitchSetPoint = new System.Windows.Forms.Label();
            this.powerLimitValue1 = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.roll_stabilization_check = new System.Windows.Forms.CheckBox();
            this.depthBox = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.yawBox = new System.Windows.Forms.Label();
            this.pitchBox = new System.Windows.Forms.Label();
            this.rollBox = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.rollKp = new System.Windows.Forms.NumericUpDown();
            this.pitchKp = new System.Windows.Forms.NumericUpDown();
            this.depthKp = new System.Windows.Forms.NumericUpDown();
            this.rollKi = new System.Windows.Forms.NumericUpDown();
            this.pitchKi = new System.Windows.Forms.NumericUpDown();
            this.depthKi = new System.Windows.Forms.NumericUpDown();
            this.depthKd = new System.Windows.Forms.NumericUpDown();
            this.pitchKd = new System.Windows.Forms.NumericUpDown();
            this.rollKd = new System.Windows.Forms.NumericUpDown();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.SaveButton = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.CameraBtn = new System.Windows.Forms.Button();
            this.imageBox1 = new Emgu.CV.UI.ImageBox();
            this.label16 = new System.Windows.Forms.Label();
            this.yawKd = new System.Windows.Forms.NumericUpDown();
            this.yawKi = new System.Windows.Forms.NumericUpDown();
            this.yawKp = new System.Windows.Forms.NumericUpDown();
            this.button2 = new System.Windows.Forms.Button();
            this.timerGamepad = new System.Windows.Forms.Timer(this.components);
            this.buttonGamepad = new System.Windows.Forms.Button();
            this.lbHorizontalVector = new System.Windows.Forms.Label();
            this.tbHorizontalVector = new System.Windows.Forms.TextBox();
            this.tbPowerTarget = new System.Windows.Forms.TextBox();
            this.lbPowerTarget = new System.Windows.Forms.Label();
            this.tbAngularVelocityZ = new System.Windows.Forms.TextBox();
            this.lbAngularVelocityZ = new System.Windows.Forms.Label();
            this.tbVericalThrust = new System.Windows.Forms.TextBox();
            this.lbVericalThrust = new System.Windows.Forms.Label();
            this.tbManipulatorState = new System.Windows.Forms.TextBox();
            this.lbManipulatorState = new System.Windows.Forms.Label();
            this.tbManipulatorRotate = new System.Windows.Forms.TextBox();
            this.lbManipulatorRotate = new System.Windows.Forms.Label();
            this.tbCameraRotate = new System.Windows.Forms.TextBox();
            this.lbCameraRotate = new System.Windows.Forms.Label();
            this.tbRollInc = new System.Windows.Forms.TextBox();
            this.lbRollInc = new System.Windows.Forms.Label();
            this.tbPitchInc = new System.Windows.Forms.TextBox();
            this.lbPitchInc = new System.Windows.Forms.Label();
            this.tbResetInitialization = new System.Windows.Forms.TextBox();
            this.lbResetInitialization = new System.Windows.Forms.Label();
            this.tbLightsState = new System.Windows.Forms.TextBox();
            this.lbLightsState = new System.Windows.Forms.Label();
            this.tbStabilizationState = new System.Windows.Forms.TextBox();
            this.lbStabilizationState = new System.Windows.Forms.Label();
            this.tbResetPosition = new System.Windows.Forms.TextBox();
            this.lbResetPosition = new System.Windows.Forms.Label();
            this.buttonSendPacket = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.portInput)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.powerLimitValue1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.rollKp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pitchKp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.depthKp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.rollKi)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pitchKi)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.depthKi)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.depthKd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pitchKd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.rollKd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.imageBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.yawKd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.yawKi)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.yawKp)).BeginInit();
            this.SuspendLayout();
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(11, 59);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(216, 23);
            this.connectButton.TabIndex = 9;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // hostInput
            // 
            this.hostInput.Location = new System.Drawing.Point(43, 26);
            this.hostInput.Name = "hostInput";
            this.hostInput.Size = new System.Drawing.Size(100, 20);
            this.hostInput.TabIndex = 0;
            this.hostInput.Text = "192.169.10.100";
            this.hostInput.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // portInput
            // 
            this.portInput.Location = new System.Drawing.Point(181, 27);
            this.portInput.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.portInput.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.portInput.Name = "portInput";
            this.portInput.Size = new System.Drawing.Size(46, 20);
            this.portInput.TabIndex = 1;
            this.portInput.Value = new decimal(new int[] {
            1337,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 29);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(32, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Host:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(149, 29);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Port:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 90);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(40, 13);
            this.label6.TabIndex = 10;
            this.label6.Text = "Status:";
            // 
            // connectionStatus
            // 
            this.connectionStatus.AutoSize = true;
            this.connectionStatus.Location = new System.Drawing.Point(58, 90);
            this.connectionStatus.Name = "connectionStatus";
            this.connectionStatus.Size = new System.Drawing.Size(73, 13);
            this.connectionStatus.TabIndex = 11;
            this.connectionStatus.Text = "Disconnected";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.connectionStatus);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.hostInput);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.portInput);
            this.groupBox1.Controls.Add(this.connectButton);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(265, 127);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Connection";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.depth_stabilization_check);
            this.groupBox2.Controls.Add(this.yaw_stabilization_check);
            this.groupBox2.Controls.Add(this.pitch_stabilization_check);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.rollSetPoint);
            this.groupBox2.Controls.Add(this.pitchSetPoint);
            this.groupBox2.Controls.Add(this.powerLimitValue1);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.roll_stabilization_check);
            this.groupBox2.Controls.Add(this.depthBox);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.yawBox);
            this.groupBox2.Controls.Add(this.pitchBox);
            this.groupBox2.Controls.Add(this.rollBox);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Location = new System.Drawing.Point(12, 156);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(265, 118);
            this.groupBox2.TabIndex = 12;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Telemetry";
            // 
            // depth_stabilization_check
            // 
            this.depth_stabilization_check.AutoSize = true;
            this.depth_stabilization_check.Location = new System.Drawing.Point(197, 75);
            this.depth_stabilization_check.Margin = new System.Windows.Forms.Padding(2, 1, 2, 1);
            this.depth_stabilization_check.Name = "depth_stabilization_check";
            this.depth_stabilization_check.Size = new System.Drawing.Size(77, 17);
            this.depth_stabilization_check.TabIndex = 23;
            this.depth_stabilization_check.Text = "DepthStab";
            this.depth_stabilization_check.UseVisualStyleBackColor = true;
            // 
            // yaw_stabilization_check
            // 
            this.yaw_stabilization_check.AutoSize = true;
            this.yaw_stabilization_check.Location = new System.Drawing.Point(137, 75);
            this.yaw_stabilization_check.Margin = new System.Windows.Forms.Padding(2, 1, 2, 1);
            this.yaw_stabilization_check.Name = "yaw_stabilization_check";
            this.yaw_stabilization_check.Size = new System.Drawing.Size(69, 17);
            this.yaw_stabilization_check.TabIndex = 22;
            this.yaw_stabilization_check.Text = "YawStab";
            this.yaw_stabilization_check.UseVisualStyleBackColor = true;
            // 
            // pitch_stabilization_check
            // 
            this.pitch_stabilization_check.AutoSize = true;
            this.pitch_stabilization_check.Location = new System.Drawing.Point(73, 75);
            this.pitch_stabilization_check.Margin = new System.Windows.Forms.Padding(2, 1, 2, 1);
            this.pitch_stabilization_check.Name = "pitch_stabilization_check";
            this.pitch_stabilization_check.Size = new System.Drawing.Size(72, 17);
            this.pitch_stabilization_check.TabIndex = 21;
            this.pitch_stabilization_check.Text = "PitchStab";
            this.pitch_stabilization_check.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(149, 7);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(57, 13);
            this.label3.TabIndex = 20;
            this.label3.Text = "Set points:";
            // 
            // rollSetPoint
            // 
            this.rollSetPoint.AutoSize = true;
            this.rollSetPoint.Location = new System.Drawing.Point(178, 20);
            this.rollSetPoint.Name = "rollSetPoint";
            this.rollSetPoint.Size = new System.Drawing.Size(10, 13);
            this.rollSetPoint.TabIndex = 19;
            this.rollSetPoint.Text = "-";
            // 
            // pitchSetPoint
            // 
            this.pitchSetPoint.AutoSize = true;
            this.pitchSetPoint.Location = new System.Drawing.Point(178, 33);
            this.pitchSetPoint.Name = "pitchSetPoint";
            this.pitchSetPoint.Size = new System.Drawing.Size(10, 13);
            this.pitchSetPoint.TabIndex = 17;
            this.pitchSetPoint.Text = "-";
            // 
            // powerLimitValue1
            // 
            this.powerLimitValue1.DecimalPlaces = 1;
            this.powerLimitValue1.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.powerLimitValue1.Location = new System.Drawing.Point(79, 92);
            this.powerLimitValue1.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.powerLimitValue1.Name = "powerLimitValue1";
            this.powerLimitValue1.Size = new System.Drawing.Size(35, 20);
            this.powerLimitValue1.TabIndex = 16;
            this.powerLimitValue1.Value = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(11, 92);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 13);
            this.label5.TabIndex = 15;
            this.label5.Text = "Power Limit";
            // 
            // roll_stabilization_check
            // 
            this.roll_stabilization_check.AutoSize = true;
            this.roll_stabilization_check.Location = new System.Drawing.Point(14, 75);
            this.roll_stabilization_check.Margin = new System.Windows.Forms.Padding(2, 1, 2, 1);
            this.roll_stabilization_check.Name = "roll_stabilization_check";
            this.roll_stabilization_check.Size = new System.Drawing.Size(66, 17);
            this.roll_stabilization_check.TabIndex = 14;
            this.roll_stabilization_check.Text = "RollStab";
            this.roll_stabilization_check.UseVisualStyleBackColor = true;
            this.roll_stabilization_check.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // depthBox
            // 
            this.depthBox.AutoSize = true;
            this.depthBox.Location = new System.Drawing.Point(58, 59);
            this.depthBox.Name = "depthBox";
            this.depthBox.Size = new System.Drawing.Size(10, 13);
            this.depthBox.TabIndex = 13;
            this.depthBox.Text = "-";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 59);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Depth";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // yawBox
            // 
            this.yawBox.AutoSize = true;
            this.yawBox.Location = new System.Drawing.Point(58, 46);
            this.yawBox.Name = "yawBox";
            this.yawBox.Size = new System.Drawing.Size(10, 13);
            this.yawBox.TabIndex = 8;
            this.yawBox.Text = "-";
            // 
            // pitchBox
            // 
            this.pitchBox.AutoSize = true;
            this.pitchBox.Location = new System.Drawing.Point(58, 33);
            this.pitchBox.Name = "pitchBox";
            this.pitchBox.Size = new System.Drawing.Size(10, 13);
            this.pitchBox.TabIndex = 6;
            this.pitchBox.Text = "-";
            // 
            // rollBox
            // 
            this.rollBox.AutoSize = true;
            this.rollBox.Location = new System.Drawing.Point(58, 20);
            this.rollBox.Name = "rollBox";
            this.rollBox.Size = new System.Drawing.Size(10, 13);
            this.rollBox.TabIndex = 4;
            this.rollBox.Text = "-";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(11, 46);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(28, 13);
            this.label9.TabIndex = 2;
            this.label9.Text = "Yaw";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(11, 33);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(31, 13);
            this.label8.TabIndex = 1;
            this.label8.Text = "Pitch";
            this.label8.Click += new System.EventHandler(this.label8_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(11, 20);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(25, 13);
            this.label7.TabIndex = 0;
            this.label7.Text = "Roll";
            this.label7.Click += new System.EventHandler(this.label7_Click);
            // 
            // rollKp
            // 
            this.rollKp.DecimalPlaces = 1;
            this.rollKp.Increment = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            this.rollKp.Location = new System.Drawing.Point(65, 303);
            this.rollKp.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.rollKp.Name = "rollKp";
            this.rollKp.Size = new System.Drawing.Size(61, 20);
            this.rollKp.TabIndex = 21;
            this.rollKp.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // pitchKp
            // 
            this.pitchKp.DecimalPlaces = 1;
            this.pitchKp.Increment = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            this.pitchKp.Location = new System.Drawing.Point(65, 329);
            this.pitchKp.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.pitchKp.Name = "pitchKp";
            this.pitchKp.Size = new System.Drawing.Size(61, 20);
            this.pitchKp.TabIndex = 22;
            this.pitchKp.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // depthKp
            // 
            this.depthKp.DecimalPlaces = 1;
            this.depthKp.Increment = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            this.depthKp.Location = new System.Drawing.Point(65, 382);
            this.depthKp.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.depthKp.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.depthKp.Name = "depthKp";
            this.depthKp.Size = new System.Drawing.Size(61, 20);
            this.depthKp.TabIndex = 23;
            this.depthKp.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // rollKi
            // 
            this.rollKi.DecimalPlaces = 1;
            this.rollKi.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.rollKi.Location = new System.Drawing.Point(132, 303);
            this.rollKi.Name = "rollKi";
            this.rollKi.Size = new System.Drawing.Size(53, 20);
            this.rollKi.TabIndex = 24;
            // 
            // pitchKi
            // 
            this.pitchKi.DecimalPlaces = 1;
            this.pitchKi.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.pitchKi.Location = new System.Drawing.Point(132, 329);
            this.pitchKi.Name = "pitchKi";
            this.pitchKi.Size = new System.Drawing.Size(53, 20);
            this.pitchKi.TabIndex = 25;
            // 
            // depthKi
            // 
            this.depthKi.DecimalPlaces = 1;
            this.depthKi.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.depthKi.Location = new System.Drawing.Point(132, 382);
            this.depthKi.Name = "depthKi";
            this.depthKi.Size = new System.Drawing.Size(53, 20);
            this.depthKi.TabIndex = 26;
            // 
            // depthKd
            // 
            this.depthKd.DecimalPlaces = 2;
            this.depthKd.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.depthKd.Location = new System.Drawing.Point(191, 382);
            this.depthKd.Name = "depthKd";
            this.depthKd.Size = new System.Drawing.Size(53, 20);
            this.depthKd.TabIndex = 29;
            // 
            // pitchKd
            // 
            this.pitchKd.DecimalPlaces = 2;
            this.pitchKd.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.pitchKd.Location = new System.Drawing.Point(191, 330);
            this.pitchKd.Name = "pitchKd";
            this.pitchKd.Size = new System.Drawing.Size(53, 20);
            this.pitchKd.TabIndex = 28;
            // 
            // rollKd
            // 
            this.rollKd.DecimalPlaces = 2;
            this.rollKd.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.rollKd.Location = new System.Drawing.Point(191, 303);
            this.rollKd.Name = "rollKd";
            this.rollKd.Size = new System.Drawing.Size(53, 20);
            this.rollKd.TabIndex = 27;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(88, 287);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(20, 13);
            this.label10.TabIndex = 21;
            this.label10.Text = "Kp";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(148, 287);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(16, 13);
            this.label11.TabIndex = 30;
            this.label11.Text = "Ki";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(207, 287);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(20, 13);
            this.label12.TabIndex = 31;
            this.label12.Text = "Kd";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(32, 384);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(36, 13);
            this.label13.TabIndex = 23;
            this.label13.Text = "Depth";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(32, 332);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(31, 13);
            this.label14.TabIndex = 22;
            this.label14.Text = "Pitch";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(32, 305);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(25, 13);
            this.label15.TabIndex = 21;
            this.label15.Text = "Roll";
            // 
            // SaveButton
            // 
            this.SaveButton.Location = new System.Drawing.Point(29, 407);
            this.SaveButton.Name = "SaveButton";
            this.SaveButton.Size = new System.Drawing.Size(216, 23);
            this.SaveButton.TabIndex = 12;
            this.SaveButton.Text = "Save";
            this.SaveButton.UseVisualStyleBackColor = true;
            this.SaveButton.Click += new System.EventHandler(this.SaveButton_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(29, 436);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(216, 23);
            this.button1.TabIndex = 32;
            this.button1.Text = "Update";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // CameraBtn
            // 
            this.CameraBtn.Location = new System.Drawing.Point(29, 465);
            this.CameraBtn.Name = "CameraBtn";
            this.CameraBtn.Size = new System.Drawing.Size(216, 23);
            this.CameraBtn.TabIndex = 33;
            this.CameraBtn.Text = "Камера";
            this.CameraBtn.UseVisualStyleBackColor = true;
            this.CameraBtn.Click += new System.EventHandler(this.button2_Click);
            // 
            // imageBox1
            // 
            this.imageBox1.Location = new System.Drawing.Point(771, 10);
            this.imageBox1.Name = "imageBox1";
            this.imageBox1.Size = new System.Drawing.Size(271, 586);
            this.imageBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.imageBox1.TabIndex = 2;
            this.imageBox1.TabStop = false;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(32, 356);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(28, 13);
            this.label16.TabIndex = 34;
            this.label16.Text = "Yaw";
            this.label16.Click += new System.EventHandler(this.label16_Click);
            // 
            // yawKd
            // 
            this.yawKd.DecimalPlaces = 2;
            this.yawKd.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.yawKd.Location = new System.Drawing.Point(191, 354);
            this.yawKd.Name = "yawKd";
            this.yawKd.Size = new System.Drawing.Size(53, 20);
            this.yawKd.TabIndex = 37;
            this.yawKd.ValueChanged += new System.EventHandler(this.numericUpDown1_ValueChanged);
            // 
            // yawKi
            // 
            this.yawKi.DecimalPlaces = 1;
            this.yawKi.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.yawKi.Location = new System.Drawing.Point(132, 354);
            this.yawKi.Name = "yawKi";
            this.yawKi.Size = new System.Drawing.Size(53, 20);
            this.yawKi.TabIndex = 36;
            this.yawKi.ValueChanged += new System.EventHandler(this.numericUpDown2_ValueChanged);
            // 
            // yawKp
            // 
            this.yawKp.DecimalPlaces = 1;
            this.yawKp.Increment = new decimal(new int[] {
            2,
            0,
            0,
            65536});
            this.yawKp.Location = new System.Drawing.Point(65, 354);
            this.yawKp.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.yawKp.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.yawKp.Name = "yawKp";
            this.yawKp.Size = new System.Drawing.Size(61, 20);
            this.yawKp.TabIndex = 35;
            this.yawKp.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.yawKp.ValueChanged += new System.EventHandler(this.numericUpDown3_ValueChanged);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(29, 494);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(216, 23);
            this.button2.TabIndex = 40;
            this.button2.Text = "Камера Стоп";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // timerGamepad
            // 
            this.timerGamepad.Tick += new System.EventHandler(this.timerGamepad_Tick);
            // 
            // buttonGamepad
            // 
            this.buttonGamepad.Location = new System.Drawing.Point(29, 523);
            this.buttonGamepad.Name = "buttonGamepad";
            this.buttonGamepad.Size = new System.Drawing.Size(216, 23);
            this.buttonGamepad.TabIndex = 41;
            this.buttonGamepad.Text = "Gamepad Start";
            this.buttonGamepad.UseVisualStyleBackColor = true;
            this.buttonGamepad.Click += new System.EventHandler(this.buttonGamepad_Click);
            // 
            // lbHorizontalVector
            // 
            this.lbHorizontalVector.AutoSize = true;
            this.lbHorizontalVector.Location = new System.Drawing.Point(289, 25);
            this.lbHorizontalVector.Name = "lbHorizontalVector";
            this.lbHorizontalVector.Size = new System.Drawing.Size(85, 13);
            this.lbHorizontalVector.TabIndex = 42;
            this.lbHorizontalVector.Text = "HorizontalVector";
            // 
            // tbHorizontalVector
            // 
            this.tbHorizontalVector.Location = new System.Drawing.Point(391, 22);
            this.tbHorizontalVector.Name = "tbHorizontalVector";
            this.tbHorizontalVector.Size = new System.Drawing.Size(374, 20);
            this.tbHorizontalVector.TabIndex = 43;
            // 
            // tbPowerTarget
            // 
            this.tbPowerTarget.Location = new System.Drawing.Point(391, 48);
            this.tbPowerTarget.Name = "tbPowerTarget";
            this.tbPowerTarget.Size = new System.Drawing.Size(70, 20);
            this.tbPowerTarget.TabIndex = 45;
            // 
            // lbPowerTarget
            // 
            this.lbPowerTarget.AutoSize = true;
            this.lbPowerTarget.Location = new System.Drawing.Point(289, 51);
            this.lbPowerTarget.Name = "lbPowerTarget";
            this.lbPowerTarget.Size = new System.Drawing.Size(68, 13);
            this.lbPowerTarget.TabIndex = 44;
            this.lbPowerTarget.Text = "PowerTarget";
            // 
            // tbAngularVelocityZ
            // 
            this.tbAngularVelocityZ.Location = new System.Drawing.Point(391, 74);
            this.tbAngularVelocityZ.Name = "tbAngularVelocityZ";
            this.tbAngularVelocityZ.Size = new System.Drawing.Size(70, 20);
            this.tbAngularVelocityZ.TabIndex = 47;
            // 
            // lbAngularVelocityZ
            // 
            this.lbAngularVelocityZ.AutoSize = true;
            this.lbAngularVelocityZ.Location = new System.Drawing.Point(289, 77);
            this.lbAngularVelocityZ.Name = "lbAngularVelocityZ";
            this.lbAngularVelocityZ.Size = new System.Drawing.Size(87, 13);
            this.lbAngularVelocityZ.TabIndex = 46;
            this.lbAngularVelocityZ.Text = "AngularVelocityZ";
            // 
            // tbVericalThrust
            // 
            this.tbVericalThrust.Location = new System.Drawing.Point(391, 100);
            this.tbVericalThrust.Name = "tbVericalThrust";
            this.tbVericalThrust.Size = new System.Drawing.Size(70, 20);
            this.tbVericalThrust.TabIndex = 49;
            // 
            // lbVericalThrust
            // 
            this.lbVericalThrust.AutoSize = true;
            this.lbVericalThrust.Location = new System.Drawing.Point(289, 103);
            this.lbVericalThrust.Name = "lbVericalThrust";
            this.lbVericalThrust.Size = new System.Drawing.Size(69, 13);
            this.lbVericalThrust.TabIndex = 48;
            this.lbVericalThrust.Text = "VericalThrust";
            // 
            // tbManipulatorState
            // 
            this.tbManipulatorState.Location = new System.Drawing.Point(391, 126);
            this.tbManipulatorState.Name = "tbManipulatorState";
            this.tbManipulatorState.Size = new System.Drawing.Size(70, 20);
            this.tbManipulatorState.TabIndex = 51;
            // 
            // lbManipulatorState
            // 
            this.lbManipulatorState.AutoSize = true;
            this.lbManipulatorState.Location = new System.Drawing.Point(289, 129);
            this.lbManipulatorState.Name = "lbManipulatorState";
            this.lbManipulatorState.Size = new System.Drawing.Size(87, 13);
            this.lbManipulatorState.TabIndex = 50;
            this.lbManipulatorState.Text = "ManipulatorState";
            // 
            // tbManipulatorRotate
            // 
            this.tbManipulatorRotate.Location = new System.Drawing.Point(391, 152);
            this.tbManipulatorRotate.Name = "tbManipulatorRotate";
            this.tbManipulatorRotate.Size = new System.Drawing.Size(70, 20);
            this.tbManipulatorRotate.TabIndex = 53;
            // 
            // lbManipulatorRotate
            // 
            this.lbManipulatorRotate.AutoSize = true;
            this.lbManipulatorRotate.Location = new System.Drawing.Point(289, 155);
            this.lbManipulatorRotate.Name = "lbManipulatorRotate";
            this.lbManipulatorRotate.Size = new System.Drawing.Size(94, 13);
            this.lbManipulatorRotate.TabIndex = 52;
            this.lbManipulatorRotate.Text = "ManipulatorRotate";
            // 
            // tbCameraRotate
            // 
            this.tbCameraRotate.Location = new System.Drawing.Point(391, 178);
            this.tbCameraRotate.Name = "tbCameraRotate";
            this.tbCameraRotate.Size = new System.Drawing.Size(70, 20);
            this.tbCameraRotate.TabIndex = 55;
            // 
            // lbCameraRotate
            // 
            this.lbCameraRotate.AutoSize = true;
            this.lbCameraRotate.Location = new System.Drawing.Point(289, 181);
            this.lbCameraRotate.Name = "lbCameraRotate";
            this.lbCameraRotate.Size = new System.Drawing.Size(75, 13);
            this.lbCameraRotate.TabIndex = 54;
            this.lbCameraRotate.Text = "CameraRotate";
            // 
            // tbRollInc
            // 
            this.tbRollInc.Location = new System.Drawing.Point(391, 204);
            this.tbRollInc.Name = "tbRollInc";
            this.tbRollInc.Size = new System.Drawing.Size(70, 20);
            this.tbRollInc.TabIndex = 57;
            // 
            // lbRollInc
            // 
            this.lbRollInc.AutoSize = true;
            this.lbRollInc.Location = new System.Drawing.Point(289, 207);
            this.lbRollInc.Name = "lbRollInc";
            this.lbRollInc.Size = new System.Drawing.Size(40, 13);
            this.lbRollInc.TabIndex = 56;
            this.lbRollInc.Text = "RollInc";
            // 
            // tbPitchInc
            // 
            this.tbPitchInc.Location = new System.Drawing.Point(391, 229);
            this.tbPitchInc.Name = "tbPitchInc";
            this.tbPitchInc.Size = new System.Drawing.Size(70, 20);
            this.tbPitchInc.TabIndex = 59;
            // 
            // lbPitchInc
            // 
            this.lbPitchInc.AutoSize = true;
            this.lbPitchInc.Location = new System.Drawing.Point(289, 232);
            this.lbPitchInc.Name = "lbPitchInc";
            this.lbPitchInc.Size = new System.Drawing.Size(46, 13);
            this.lbPitchInc.TabIndex = 58;
            this.lbPitchInc.Text = "PitchInc";
            // 
            // tbResetInitialization
            // 
            this.tbResetInitialization.Location = new System.Drawing.Point(391, 255);
            this.tbResetInitialization.Name = "tbResetInitialization";
            this.tbResetInitialization.Size = new System.Drawing.Size(70, 20);
            this.tbResetInitialization.TabIndex = 61;
            // 
            // lbResetInitialization
            // 
            this.lbResetInitialization.AutoSize = true;
            this.lbResetInitialization.Location = new System.Drawing.Point(289, 258);
            this.lbResetInitialization.Name = "lbResetInitialization";
            this.lbResetInitialization.Size = new System.Drawing.Size(89, 13);
            this.lbResetInitialization.TabIndex = 60;
            this.lbResetInitialization.Text = "ResetInitialization";
            // 
            // tbLightsState
            // 
            this.tbLightsState.Location = new System.Drawing.Point(391, 281);
            this.tbLightsState.Name = "tbLightsState";
            this.tbLightsState.Size = new System.Drawing.Size(70, 20);
            this.tbLightsState.TabIndex = 63;
            // 
            // lbLightsState
            // 
            this.lbLightsState.AutoSize = true;
            this.lbLightsState.Location = new System.Drawing.Point(289, 284);
            this.lbLightsState.Name = "lbLightsState";
            this.lbLightsState.Size = new System.Drawing.Size(60, 13);
            this.lbLightsState.TabIndex = 62;
            this.lbLightsState.Text = "LightsState";
            // 
            // tbStabilizationState
            // 
            this.tbStabilizationState.Location = new System.Drawing.Point(391, 307);
            this.tbStabilizationState.Name = "tbStabilizationState";
            this.tbStabilizationState.Size = new System.Drawing.Size(70, 20);
            this.tbStabilizationState.TabIndex = 65;
            // 
            // lbStabilizationState
            // 
            this.lbStabilizationState.AutoSize = true;
            this.lbStabilizationState.Location = new System.Drawing.Point(289, 310);
            this.lbStabilizationState.Name = "lbStabilizationState";
            this.lbStabilizationState.Size = new System.Drawing.Size(88, 13);
            this.lbStabilizationState.TabIndex = 64;
            this.lbStabilizationState.Text = "StabilizationState";
            // 
            // tbResetPosition
            // 
            this.tbResetPosition.Location = new System.Drawing.Point(391, 333);
            this.tbResetPosition.Name = "tbResetPosition";
            this.tbResetPosition.Size = new System.Drawing.Size(70, 20);
            this.tbResetPosition.TabIndex = 67;
            // 
            // lbResetPosition
            // 
            this.lbResetPosition.AutoSize = true;
            this.lbResetPosition.Location = new System.Drawing.Point(289, 336);
            this.lbResetPosition.Name = "lbResetPosition";
            this.lbResetPosition.Size = new System.Drawing.Size(72, 13);
            this.lbResetPosition.TabIndex = 66;
            this.lbResetPosition.Text = "ResetPosition";
            // 
            // buttonSendPacket
            // 
            this.buttonSendPacket.Location = new System.Drawing.Point(386, 379);
            this.buttonSendPacket.Name = "buttonSendPacket";
            this.buttonSendPacket.Size = new System.Drawing.Size(75, 23);
            this.buttonSendPacket.TabIndex = 68;
            this.buttonSendPacket.Text = "Send packet";
            this.buttonSendPacket.UseVisualStyleBackColor = true;
            this.buttonSendPacket.Click += new System.EventHandler(this.buttonSendPacket_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1051, 610);
            this.Controls.Add(this.buttonSendPacket);
            this.Controls.Add(this.tbResetPosition);
            this.Controls.Add(this.lbResetPosition);
            this.Controls.Add(this.tbStabilizationState);
            this.Controls.Add(this.lbStabilizationState);
            this.Controls.Add(this.tbLightsState);
            this.Controls.Add(this.lbLightsState);
            this.Controls.Add(this.tbResetInitialization);
            this.Controls.Add(this.lbResetInitialization);
            this.Controls.Add(this.tbPitchInc);
            this.Controls.Add(this.lbPitchInc);
            this.Controls.Add(this.tbRollInc);
            this.Controls.Add(this.lbRollInc);
            this.Controls.Add(this.tbCameraRotate);
            this.Controls.Add(this.lbCameraRotate);
            this.Controls.Add(this.tbManipulatorRotate);
            this.Controls.Add(this.lbManipulatorRotate);
            this.Controls.Add(this.tbManipulatorState);
            this.Controls.Add(this.lbManipulatorState);
            this.Controls.Add(this.tbVericalThrust);
            this.Controls.Add(this.lbVericalThrust);
            this.Controls.Add(this.tbAngularVelocityZ);
            this.Controls.Add(this.lbAngularVelocityZ);
            this.Controls.Add(this.tbPowerTarget);
            this.Controls.Add(this.lbPowerTarget);
            this.Controls.Add(this.tbHorizontalVector);
            this.Controls.Add(this.lbHorizontalVector);
            this.Controls.Add(this.buttonGamepad);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.yawKd);
            this.Controls.Add(this.yawKi);
            this.Controls.Add(this.yawKp);
            this.Controls.Add(this.imageBox1);
            this.Controls.Add(this.CameraBtn);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.SaveButton);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.depthKd);
            this.Controls.Add(this.pitchKd);
            this.Controls.Add(this.rollKd);
            this.Controls.Add(this.depthKi);
            this.Controls.Add(this.pitchKi);
            this.Controls.Add(this.rollKi);
            this.Controls.Add(this.depthKp);
            this.Controls.Add(this.pitchKp);
            this.Controls.Add(this.rollKp);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "MainForm";
            this.Text = "SEVROV v2";
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.portInput)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.powerLimitValue1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.rollKp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pitchKp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.depthKp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.rollKi)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pitchKi)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.depthKi)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.depthKd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pitchKd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.rollKd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.imageBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.yawKd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.yawKi)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.yawKp)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown portInput;
        private System.Windows.Forms.TextBox hostInput;
        private System.Windows.Forms.Label connectionStatus;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label yawBox;
        private System.Windows.Forms.Label pitchBox;
        private System.Windows.Forms.Label rollBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label depthBox;
        private System.Windows.Forms.CheckBox roll_stabilization_check;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown powerLimitValue1;
        private System.Windows.Forms.Label rollSetPoint;
        private System.Windows.Forms.Label pitchSetPoint;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown rollKp;
        private System.Windows.Forms.NumericUpDown pitchKp;
        private System.Windows.Forms.NumericUpDown depthKp;
        private System.Windows.Forms.NumericUpDown rollKi;
        private System.Windows.Forms.NumericUpDown pitchKi;
        private System.Windows.Forms.NumericUpDown depthKi;
        private System.Windows.Forms.NumericUpDown depthKd;
        private System.Windows.Forms.NumericUpDown pitchKd;
        private System.Windows.Forms.NumericUpDown rollKd;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Button SaveButton;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button CameraBtn;
        private Emgu.CV.UI.ImageBox imageBox1;
        private System.Windows.Forms.CheckBox depth_stabilization_check;
        private System.Windows.Forms.CheckBox yaw_stabilization_check;
        private System.Windows.Forms.CheckBox pitch_stabilization_check;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.NumericUpDown yawKd;
        private System.Windows.Forms.NumericUpDown yawKi;
        private System.Windows.Forms.NumericUpDown yawKp;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Timer timerGamepad;
        private System.Windows.Forms.Button buttonGamepad;
        private System.Windows.Forms.Label lbHorizontalVector;
        private System.Windows.Forms.TextBox tbHorizontalVector;
        private System.Windows.Forms.TextBox tbPowerTarget;
        private System.Windows.Forms.Label lbPowerTarget;
        private System.Windows.Forms.TextBox tbAngularVelocityZ;
        private System.Windows.Forms.Label lbAngularVelocityZ;
        private System.Windows.Forms.TextBox tbVericalThrust;
        private System.Windows.Forms.Label lbVericalThrust;
        private System.Windows.Forms.TextBox tbManipulatorState;
        private System.Windows.Forms.Label lbManipulatorState;
        private System.Windows.Forms.TextBox tbManipulatorRotate;
        private System.Windows.Forms.Label lbManipulatorRotate;
        private System.Windows.Forms.TextBox tbCameraRotate;
        private System.Windows.Forms.Label lbCameraRotate;
        private System.Windows.Forms.TextBox tbRollInc;
        private System.Windows.Forms.Label lbRollInc;
        private System.Windows.Forms.TextBox tbPitchInc;
        private System.Windows.Forms.Label lbPitchInc;
        private System.Windows.Forms.TextBox tbResetInitialization;
        private System.Windows.Forms.Label lbResetInitialization;
        private System.Windows.Forms.TextBox tbLightsState;
        private System.Windows.Forms.Label lbLightsState;
        private System.Windows.Forms.TextBox tbStabilizationState;
        private System.Windows.Forms.Label lbStabilizationState;
        private System.Windows.Forms.TextBox tbResetPosition;
        private System.Windows.Forms.Label lbResetPosition;
        private System.Windows.Forms.Button buttonSendPacket;
    }
}

