using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

using Emgu;
using Emgu.CV;
using Emgu.CV.Util;
using Emgu.CV.Structure;
using Emgu.CV.UI;
using Emgu.CV.XPhoto;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text.RegularExpressions;

namespace SevROVClient
{
    public partial class MainForm : Form
    {
        private readonly XInputController _gamepad;
        private readonly UsbCamera _camera;
        private readonly RovConnection _connection;
        // private readonly Graphics _cameraGraphics;
        private readonly Timer _inputTimer;

        private long _lastDataTransferTime;
        private Bitmap _cameraBuffer;
        private Timer _connectionTimeoutTimer;
        private bool _connected;
        private readonly ControlData _controlData;
        private int XbtnCounter = 0;
        private bool updatePID = false;
        private Emgu.CV.Capture captureL = null; // приватное поле, в данном поле будем хранить захватываемые данные
        private int selectedCameraId = 0;
        private sbyte stab_state = 0b00000000;
        //string urlSTR ="http://localhost:800/axis-cgi/mjpg/video.cgi?x.mjpeg";
        int iii = 0;
        public MainForm()
        {
            InitializeComponent();
            _controlData = new ControlData();
            _gamepad = new XInputController();
            _connection = new RovConnection();
            _connection.DataReceived += ConnectionOnDataReceived;

            _inputTimer = new Timer();
            _inputTimer.Tick += inputTimer_Tick;

            _connected = false;
            // _cameraGraphics = cameraImageBox.CreateGraphics();



            if (File.Exists("settings.txt"))
            {
                using (StreamReader reader = new StreamReader("settings.txt"))
                {
                    rollKp.Value = decimal.Parse(reader.ReadLine());
                    rollKi.Value = decimal.Parse(reader.ReadLine());
                    rollKd.Value = decimal.Parse(reader.ReadLine());
                    pitchKp.Value = decimal.Parse(reader.ReadLine());
                    pitchKi.Value = decimal.Parse(reader.ReadLine());
                    pitchKd.Value = decimal.Parse(reader.ReadLine());
                    depthKp.Value = decimal.Parse(reader.ReadLine());
                    depthKi.Value = decimal.Parse(reader.ReadLine());
                    depthKd.Value = decimal.Parse(reader.ReadLine());
                    yawKp.Value = decimal.Parse(reader.ReadLine());
                    yawKi.Value = decimal.Parse(reader.ReadLine());
                    yawKd.Value = decimal.Parse(reader.ReadLine());
                }
            }

            /*
                        var devices = UsbCamera.FindDevices();
                        var index = Array.FindIndex(devices, name => name.StartsWith("AVerMedia"));
                        if (index < 0)
                        {
                            MessageBox.Show("Камера не обнаружена", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }

                        var formats = UsbCamera.GetVideoFormat(index);
                        _camera = new UsbCamera(index, formats[0]);
                        _camera.Start();

                        var videoTimer = new Timer();
                        videoTimer.Interval = 1000 / 25;
                        videoTimer.Tick += VideoTimerOnTick;
                        videoTimer.Start();

                        FormClosing += (s, ev) => videoTimer.Stop();
                        FormClosing += (s, ev) => _camera.Stop();*/

        }

        private void inputTimer_Tick(object sender, EventArgs e)
        {

            if (!_connected) return;

            _gamepad.Update();

            _controlData.HorizontalVector = _gamepad.LeftThumb;
            _controlData.PowerTarget = ((float)powerLimitValue1.Value);
            _controlData.AngularVelocityZ = _gamepad.RightThumb.Y;
            _controlData.VericalThrust = _gamepad.RightThumb.X;

            var gripDirection = (_gamepad.LB ? 1 : 0) - (_gamepad.RB ? 1 : 0);
            _controlData.ManipulatorState = gripDirection;

            var manipulatorRotateDirection = _gamepad.LT - _gamepad.RT;
            _controlData.ManipulatorRotate = manipulatorRotateDirection;

            var cameraRotateDirection = (_gamepad.Y ? 1 : 0) - (_gamepad.A ? 1 : 0);
            _controlData.CameraRotate = cameraRotateDirection;

            var rollIncDir = (_gamepad.Right ? 1 : 0) - (_gamepad.Left ? 1 : 0);
            _controlData.RollInc = rollIncDir;

            var pitchIncDir = (_gamepad.Up ? 1 : 0) - (_gamepad.Down ? 1 : 0);
            _controlData.PitchInc = pitchIncDir;

            _controlData.ResetInitialization = (sbyte)(_gamepad.Start ? 1 : 0);
            if (_gamepad.X)
            {
                if (XbtnCounter == 0)
                {
                    switch (_controlData.LightsState)
                    {
                        case (sbyte)0:
                            _controlData.LightsState = (sbyte)1;
                            break;
                        case (sbyte)1:
                            _controlData.LightsState = (sbyte)0;
                            break;
                    }
                    XbtnCounter++;
                }
            }
            else
            {
                XbtnCounter = 0;
            }
            stab_state = (sbyte)(roll_stabilization_check.Checked ? (stab_state | 0b00000001) : (stab_state & 0b11111110));
            stab_state = (sbyte)(pitch_stabilization_check.Checked ? (stab_state | 0b00000010) : (stab_state & 0b11111101));
            stab_state = (sbyte)(yaw_stabilization_check.Checked ? (stab_state | 0b00000100) : (stab_state & 0b11111011));
            stab_state = (sbyte)(depth_stabilization_check.Checked ? (stab_state | 0b00001000) : (stab_state & 0b11110111));
            _controlData.StabilizationState = stab_state;
            _controlData.ResetPosition = (sbyte)(_gamepad.B ? 1 : 0);

            _controlData.RollKp = (float)rollKp.Value;
            _controlData.RollKi = (float)rollKi.Value;
            _controlData.RollKd = (float)rollKd.Value;
            _controlData.PitchKp = (float)pitchKp.Value;
            _controlData.PitchKi = (float)pitchKi.Value;
            _controlData.PitchKd = (float)pitchKd.Value;
            _controlData.YawKp = (float)yawKp.Value;
            _controlData.YawKi = (float)yawKi.Value;
            _controlData.YawKd = (float)yawKd.Value;
            _controlData.DepthKp = (float)depthKp.Value;
            _controlData.DepthKi = (float)depthKi.Value;
            _controlData.DepthKd = (float)depthKd.Value;

            if (updatePID)
            {
                _controlData.UpdatePID = 1;
                updatePID = false;
            }
            else
            {
                _controlData.UpdatePID = 0;
            }

            _connection.Send(_controlData);
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            DoubleBuffered = true;
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
        }

        private void ConnectionOnDataReceived(object sender, TelemetryDataEventArgs e)
        {

            _lastDataTransferTime = Environment.TickCount;


            if (rollBox.InvokeRequired)
            {
                BeginInvoke(new Action(() =>
                {
                    rollBox.Text = e.Roll.ToString("F");
                    pitchBox.Text = e.Pitch.ToString("F");
                    yawBox.Text = e.Yaw.ToString("F");
                    depthBox.Text = e.Depth.ToString("F");

                    rollSetPoint.Text = e.RollSetPoint.ToString("F");
                    pitchSetPoint.Text = e.PitchSetPoint.ToString("F");

                }));
            }
            else
            {
                rollBox.Text = e.Roll.ToString("F");
                pitchBox.Text = e.Pitch.ToString("F");
                yawBox.Text = e.Yaw.ToString("F");
                depthBox.Text = e.Depth.ToString("F");

                rollSetPoint.Text = e.RollSetPoint.ToString("F");
                pitchSetPoint.Text = e.PitchSetPoint.ToString("F");
            }

            //if (pitchBox.InvokeRequired)
            //{
            //    BeginInvoke(new Action(() =>
            //    {
            //        pitchBox.Text = e.Pitch.ToString("F");
            //    }));
            //}
            //else
            //{
            //    pitchBox.Text = e.Pitch.ToString("F");
            //}

            //if (yawBox.InvokeRequired)
            //{
            //    BeginInvoke(new Action(() =>
            //    {
            //        yawBox.Text = e.Yaw.ToString("F");
            //    }));
            //}
            //else
            //{
            //    yawBox.Text = e.Yaw.ToString("F");
            //}
            //if (depthBox.InvokeRequired)
            //{
            //    BeginInvoke(new Action(() =>
            //    {
            //        depthBox.Text = e.Depth.ToString("F");
            //        label17.Text = "Глубина " + e.Depth.ToString("F");
            //    }));
            //}
            //else
            //{
            //    depthBox.Text = e.Depth.ToString("F");
            //    label17.Text = "Глубина " + e.Depth.ToString("F");
            //}
            //if (rollSetPoint.InvokeRequired)
            //{
            //    BeginInvoke(new Action(() =>
            //    {
            //        rollSetPoint.Text = e.RollSetPoint.ToString("F");
            //    }));
            //}
            //else
            //{
            //    rollSetPoint.Text = e.RollSetPoint.ToString("F");
            //}
            //if (pitchSetPoint.InvokeRequired)
            //{
            //    BeginInvoke(new Action(() =>
            //    {
            //        pitchSetPoint.Text = e.PitchSetPoint.ToString("F");
            //    }));
            //}
            //else
            //{
            //    pitchSetPoint.Text = e.PitchSetPoint.ToString("F");
            //}
        }

        private void VideoTimerOnTick(object sender, EventArgs e)
        {
            if (_cameraBuffer == null)
            {
                _cameraBuffer = _camera.GetBitmap();
            }
            else
            {
                _cameraBuffer.Dispose();
                _cameraBuffer = _camera.GetBitmap();
            }

            // var graphics = _cameraGraphics;
            // graphics.DrawImageUnscaled(_cameraBuffer, new Point(0, 0));
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            var host = hostInput.Text;
            var port = (int)portInput.Value;
            _connection.Connect(host, port);
            if (_connectionTimeoutTimer == null)
            {
                _connectionTimeoutTimer = new Timer();
                _connectionTimeoutTimer.Interval = 1000;
                _connectionTimeoutTimer.Tick += ConnectionTimeoutTimerOnTick;
            }

            _lastDataTransferTime = 0;
            _connectionTimeoutTimer.Start();
        }

        private void ConnectionTimeoutTimerOnTick(object sender, EventArgs e)
        {
            var now = Environment.TickCount;
            if (now - _lastDataTransferTime > 5000)
            {
                _inputTimer.Start();

                connectionStatus.Text = "Disconnected";
                _connectionTimeoutTimer.Stop();
                _connected = false;
            }
            else
            {
                _inputTimer.Start();
                connectionStatus.Text = "Connected";
                _connected = true;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            bool a = roll_stabilization_check.Checked;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            hostInput.Text = "127.0.0.1";
            portInput.Text = "2525";
        }

        private void label7_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label8_Click(object sender, EventArgs e)
        {

        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            using (StreamWriter writer = new StreamWriter("settings.txt"))
            {
                writer.WriteLine(rollKp.Value);
                writer.WriteLine(rollKi.Value);
                writer.WriteLine(rollKd.Value);
                writer.WriteLine(pitchKp.Value);
                writer.WriteLine(pitchKi.Value);
                writer.WriteLine(pitchKd.Value);
                writer.WriteLine(depthKp.Value);
                writer.WriteLine(depthKi.Value);
                writer.WriteLine(depthKd.Value);
                writer.WriteLine(yawKp.Value);
                writer.WriteLine(yawKi.Value);
                writer.WriteLine(yawKd.Value);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            updatePID = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                if (captureL != null)
                {
                    // capture.SetCaptureProperty(Emgu.CV.CvEnum.CapProp.FrameWidth, 800);
                    //capture.SetCaptureProperty(Emgu.CV.CvEnum.CapProp.FrameHeight, 600);
                    captureL.Start();
                    Application.DoEvents();
                }
                else
                {
                    //    captureL = new Capture(selectedCameraId); // создадим новый экземпляр класса capture, присвоим текущую камеру
                    // captureL = new Capture("http://localhost:800/axis-cgi/mjpg/video.cgi?x.mjpeg");
                    //captureL = new Capture("http://127.0.0.1:800/");
                    string urlSTR = "http://" + hostInput.Text + ":8000/video_feed";// /axis-cgi/mjpg/video.cgi?x.mjpeg";
                    captureL = new Emgu.CV.Capture(urlSTR);


                    // capture.SetCaptureProperty(Emgu.CV.CvEnum.CapProp.FrameWidth, 800);
                    //capture.SetCaptureProperty(Emgu.CV.CvEnum.CapProp.FrameHeight, 600);
                    captureL.ImageGrabbed += Capture_ImageGrabbed; // подписать ImageGrabbed на обработчик этого события
                    Application.DoEvents();
                    captureL.Start(); ; // получение видео данных
                    Application.DoEvents();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Ошибка!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        // получаем изображение и находим все контура на левом изображении
        private void Capture_ImageGrabbed(object sender, EventArgs e)
        {
            if (captureL != null)
            {
                Mat m = new Mat(); // класс мат необходим для хранения картинки
                captureL.Retrieve(m);
                Application.DoEvents();
                imageBox1.Image = m;
            }

        }

        private void label16_Click(object sender, EventArgs e)
        {

        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {

        }

        private void numericUpDown2_ValueChanged(object sender, EventArgs e)
        {

        }

        private void numericUpDown3_ValueChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            try
            {
                if (captureL != null)
                {
                    captureL.Pause();
                    captureL.Stop();
                    // captureL.Dispose();
                    captureL = null;
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Ошибка!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void buttonGamepad_Click(object sender, EventArgs e)
        {
            if (timerGamepad.Enabled)
            {
                timerGamepad.Stop();
                buttonGamepad.Text = "Gamepad Start";
            }
            else
            {
                timerGamepad.Start();
                buttonGamepad.Text = "Gamepad Stop";
            }

        }

        private void timerGamepad_Tick(object sender, EventArgs e)
        {
            _gamepad.Update();

            _controlData.HorizontalVector = _gamepad.LeftThumb;
            _controlData.PowerTarget = ((float)powerLimitValue1.Value);
            _controlData.AngularVelocityZ = _gamepad.RightThumb.Y;
            _controlData.VericalThrust = _gamepad.RightThumb.X;

            var gripDirection = (_gamepad.LB ? 1 : 0) - (_gamepad.RB ? 1 : 0);
            _controlData.ManipulatorState = gripDirection;

            var manipulatorRotateDirection = _gamepad.LT - _gamepad.RT;
            _controlData.ManipulatorRotate = manipulatorRotateDirection;

            var cameraRotateDirection = (_gamepad.Y ? 1 : 0) - (_gamepad.A ? 1 : 0);
            _controlData.CameraRotate = cameraRotateDirection;

            var rollIncDir = (_gamepad.Right ? 1 : 0) - (_gamepad.Left ? 1 : 0);
            _controlData.RollInc = rollIncDir;

            var pitchIncDir = (_gamepad.Up ? 1 : 0) - (_gamepad.Down ? 1 : 0);
            _controlData.PitchInc = pitchIncDir;

            _controlData.ResetInitialization = (sbyte)(_gamepad.Start ? 1 : 0);
            if (_gamepad.X)
            {
                if (XbtnCounter == 0)
                {
                    switch (_controlData.LightsState)
                    {
                        case (sbyte)0:
                            _controlData.LightsState = (sbyte)1;
                            break;
                        case (sbyte)1:
                            _controlData.LightsState = (sbyte)0;
                            break;
                    }
                    XbtnCounter++;
                }
            }
            else
            {
                XbtnCounter = 0;
            }
            stab_state = (sbyte)(roll_stabilization_check.Checked ? (stab_state | 0b00000001) : (stab_state & 0b11111110));
            stab_state = (sbyte)(pitch_stabilization_check.Checked ? (stab_state | 0b00000010) : (stab_state & 0b11111101));
            stab_state = (sbyte)(yaw_stabilization_check.Checked ? (stab_state | 0b00000100) : (stab_state & 0b11111011));
            stab_state = (sbyte)(depth_stabilization_check.Checked ? (stab_state | 0b00001000) : (stab_state & 0b11110111));
            _controlData.StabilizationState = stab_state;
            _controlData.ResetPosition = (sbyte)(_gamepad.B ? 1 : 0);

            _controlData.RollKp = (float)rollKp.Value;
            _controlData.RollKi = (float)rollKi.Value;
            _controlData.RollKd = (float)rollKd.Value;
            _controlData.PitchKp = (float)pitchKp.Value;
            _controlData.PitchKi = (float)pitchKi.Value;
            _controlData.PitchKd = (float)pitchKd.Value;
            _controlData.YawKp = (float)yawKp.Value;
            _controlData.YawKi = (float)yawKi.Value;
            _controlData.YawKd = (float)yawKd.Value;
            _controlData.DepthKp = (float)depthKp.Value;
            _controlData.DepthKi = (float)depthKi.Value;
            _controlData.DepthKd = (float)depthKd.Value;

            if (updatePID)
            {
                _controlData.UpdatePID = 1;
                updatePID = false;
            }
            else
            {
                _controlData.UpdatePID = 0;
            }

            tbHorizontalVector.BeginInvoke(new Action(() =>
            {
                tbHorizontalVector.Text = $"{_controlData.HorizontalVector.X.ToString("n2")} : {_controlData.HorizontalVector.Y.ToString("n2")}";
            }));

            tbPowerTarget.BeginInvoke(new Action(() =>
            {
                tbPowerTarget.Text = _controlData.PowerTarget.ToString("n2");
            }));

            tbAngularVelocityZ.BeginInvoke(new Action(() =>
            {
                tbAngularVelocityZ.Text = _controlData.AngularVelocityZ.ToString("n2");
            }));

            tbVericalThrust.BeginInvoke(new Action(() =>
            {
                tbVericalThrust.Text = _controlData.VericalThrust.ToString("F");
            }));

            tbManipulatorState.BeginInvoke(new Action(() =>
            {
                tbManipulatorState.Text = _controlData.ManipulatorState.ToString("n2");
            }));

            tbManipulatorRotate.BeginInvoke(new Action(() =>
            {
                tbManipulatorRotate.Text = _controlData.ManipulatorRotate.ToString("n2");
            }));

            tbCameraRotate.BeginInvoke(new Action(() =>
            {
                tbCameraRotate.Text = _controlData.CameraRotate.ToString("F");
            }));

            tbRollInc.BeginInvoke(new Action(() =>
            {
                tbRollInc.Text = _controlData.RollInc.ToString("n2");
            }));

            tbPitchInc.BeginInvoke(new Action(() =>
            {
                tbPitchInc.Text = _controlData.PitchInc.ToString("n2");
            }));

            tbResetInitialization.BeginInvoke(new Action(() =>
            {
                tbResetInitialization.Text = _controlData.ResetInitialization.ToString("n2");
            }));

            tbLightsState.BeginInvoke(new Action(() =>
            {
                tbLightsState.Text = _controlData.LightsState.ToString("F");
            }));

            tbStabilizationState.BeginInvoke(new Action(() =>
            {
                tbStabilizationState.Text = _controlData.StabilizationState.ToString("n2");
            }));

            tbResetPosition.BeginInvoke(new Action(() =>
            {
                tbResetPosition.Text = _controlData.ResetPosition.ToString("F");
            }));


            //_controlData.RollKp;
            //_controlData.RollKi;
            //_controlData.RollKd;
            //_controlData.PitchKp;
            //_controlData.PitchKi;
            //_controlData.PitchKd;
            //_controlData.YawKp;
            //_controlData.YawKi;
            //_controlData.YawKd;
            //_controlData.DepthKp;
            //_controlData.DepthKi;
            //_controlData.DepthKd;
            //_controlData.UpdatePID;
        }

        private void buttonSendPacket_Click(object sender, EventArgs e)
        {
            // if (!_connected) return;

            _gamepad.Update();

            _controlData.HorizontalVector = _gamepad.LeftThumb;
            _controlData.PowerTarget = ((float)powerLimitValue1.Value);
            _controlData.AngularVelocityZ = _gamepad.RightThumb.Y;
            _controlData.VericalThrust = _gamepad.RightThumb.X;

            var gripDirection = (_gamepad.LB ? 1 : 0) - (_gamepad.RB ? 1 : 0);
            _controlData.ManipulatorState = gripDirection;

            var manipulatorRotateDirection = _gamepad.LT - _gamepad.RT;
            _controlData.ManipulatorRotate = manipulatorRotateDirection;

            var cameraRotateDirection = (_gamepad.Y ? 1 : 0) - (_gamepad.A ? 1 : 0);
            _controlData.CameraRotate = cameraRotateDirection;

            var rollIncDir = (_gamepad.Right ? 1 : 0) - (_gamepad.Left ? 1 : 0);
            _controlData.RollInc = rollIncDir;

            var pitchIncDir = (_gamepad.Up ? 1 : 0) - (_gamepad.Down ? 1 : 0);
            _controlData.PitchInc = pitchIncDir;

            _controlData.ResetInitialization = (sbyte)(_gamepad.Start ? 1 : 0);
            if (_gamepad.X)
            {
                if (XbtnCounter == 0)
                {
                    switch (_controlData.LightsState)
                    {
                        case (sbyte)0:
                            _controlData.LightsState = (sbyte)1;
                            break;
                        case (sbyte)1:
                            _controlData.LightsState = (sbyte)0;
                            break;
                    }
                    XbtnCounter++;
                }
            }
            else
            {
                XbtnCounter = 0;
            }
            stab_state = (sbyte)(roll_stabilization_check.Checked ? (stab_state | 0b00000001) : (stab_state & 0b11111110));
            stab_state = (sbyte)(pitch_stabilization_check.Checked ? (stab_state | 0b00000010) : (stab_state & 0b11111101));
            stab_state = (sbyte)(yaw_stabilization_check.Checked ? (stab_state | 0b00000100) : (stab_state & 0b11111011));
            stab_state = (sbyte)(depth_stabilization_check.Checked ? (stab_state | 0b00001000) : (stab_state & 0b11110111));
            _controlData.StabilizationState = stab_state;
            _controlData.ResetPosition = (sbyte)(_gamepad.B ? 1 : 0);

            _controlData.RollKp = (float)rollKp.Value;
            _controlData.RollKi = (float)rollKi.Value;
            _controlData.RollKd = (float)rollKd.Value;
            _controlData.PitchKp = (float)pitchKp.Value;
            _controlData.PitchKi = (float)pitchKi.Value;
            _controlData.PitchKd = (float)pitchKd.Value;
            _controlData.YawKp = (float)yawKp.Value;
            _controlData.YawKi = (float)yawKi.Value;
            _controlData.YawKd = (float)yawKd.Value;
            _controlData.DepthKp = (float)depthKp.Value;
            _controlData.DepthKi = (float)depthKi.Value;
            _controlData.DepthKd = (float)depthKd.Value;

            if (updatePID)
            {
                _controlData.UpdatePID = 1;
                updatePID = false;
            }
            else
            {
                _controlData.UpdatePID = 0;
            }

            _connection.Send(_controlData);
        }
    }
}
