using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SharpDX.XInput;

namespace SevROVClient
{
    class XInputController
    {
        private readonly Controller controller;
        private Gamepad gamepad;
        public bool Connected { get; }
        private readonly int deadZone = 6000;
        public PointF LeftThumb { get; private set; }
        public PointF RightThumb { get; private set; }
        public float LT { get; private set; }
        public float RT { get; private set; }
        public bool LB { get; private set; }
        public bool RB { get; private set; }
        public bool A { get; private set; }
        public bool B { get; private set; }
        public bool X { get; private set; }
        public bool Y { get; private set; }
        public bool Back { get; private set; }
        public bool Start { get; private set; }
        public bool Up { get; private set; }
        public bool Down { get; private set; }
        public bool Left { get; private set; }
        public bool Right { get; private set; }


        public XInputController()
        {
            controller = new Controller(UserIndex.One);
            Connected = controller.IsConnected;
        }

        // Call this method to update all class values
        public void Update()
        {
            if(!Connected) 
                return;

            gamepad = controller.GetState().Gamepad;

            
            
            var leftX  = (Math.Abs((float)gamepad.LeftThumbX ) < deadZone) ?  0 : (float)gamepad.LeftThumbX  / short.MinValue * -1;
            var leftY  = (Math.Abs((float)gamepad.LeftThumbY ) < deadZone) ?  0 : (float)gamepad.LeftThumbY  / short.MaxValue * 1;
            var rightY = (Math.Abs((float)gamepad.RightThumbX) < deadZone) ?  0 : (float)gamepad.RightThumbX / short.MaxValue * 1;
            var rightX = (Math.Abs((float)gamepad.RightThumbY) < deadZone) ?  0 : (float)gamepad.RightThumbY / short.MaxValue * 1;

            LeftThumb = new PointF(leftX, leftY);
            RightThumb = new PointF(rightX, rightY);

            LT  = (float)gamepad.LeftTrigger / byte.MaxValue;
            RT = (float)gamepad.RightTrigger / byte.MaxValue;
            LB = (gamepad.Buttons & GamepadButtonFlags.LeftShoulder) != 0;
            RB = (gamepad.Buttons & GamepadButtonFlags.RightShoulder) != 0;

            A = (gamepad.Buttons & GamepadButtonFlags.A) != 0;
            B = (gamepad.Buttons & GamepadButtonFlags.B) != 0;
            X = (gamepad.Buttons & GamepadButtonFlags.X) != 0;
            Y = (gamepad.Buttons & GamepadButtonFlags.Y) != 0;

            Back = (gamepad.Buttons & GamepadButtonFlags.Back) != 0;
            Start = (gamepad.Buttons & GamepadButtonFlags.Start) != 0;

            Up = (gamepad.Buttons & GamepadButtonFlags.DPadUp) != 0;
            Down = (gamepad.Buttons & GamepadButtonFlags.DPadDown) != 0;
            Left = (gamepad.Buttons & GamepadButtonFlags.DPadLeft) != 0;
            Right = (gamepad.Buttons & GamepadButtonFlags.DPadRight) != 0;

        }

    }
}
