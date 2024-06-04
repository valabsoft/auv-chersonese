using System.Drawing;
using System.IO;
using System.Net.Sockets;
using System.Xml;

namespace SevROVClient
{
    public class ControlData
    {
        public PointF HorizontalVector { get; set; }
        public float VericalThrust { get; set; }
        public float PowerTarget { get; set; }
        public float AngularVelocityZ { get; set; }
        public float ManipulatorState { get; set; }
        public float ManipulatorRotate { get; set; }
        public float CameraRotate { get; set; }
        public sbyte ResetInitialization { get; set; }
        public sbyte LightsState { get; set; }
        public sbyte StabilizationState{ get; set; }
        public float RollInc { get; set; }
        public float PitchInc { get; set; }
        public sbyte ResetPosition{ get; set; }
        public float RollKp { get; set; }
        public float RollKi { get; set; }
        public float RollKd { get; set; }
        public float PitchKp { get; set; }
        public float PitchKi { get; set; }
        public float PitchKd { get; set; }
        public float YawKp { get; set; }
        public float YawKi { get; set; }
        public float YawKd { get; set; }
        public float DepthKp { get; set; }
        public float DepthKi { get; set; }
        public float DepthKd { get; set; }
        public sbyte UpdatePID { get; set; }
        public byte[] ToBytes()
        {
            var result = new byte[93];
            using (var writer = new BinaryWriter(new MemoryStream(result)))
            {
                writer.Write(HorizontalVector.X);
                writer.Write(HorizontalVector.Y);
                writer.Write(VericalThrust);
                writer.Write(PowerTarget);
                writer.Write(AngularVelocityZ);
                writer.Write(ManipulatorState);
                writer.Write(ManipulatorRotate);
                writer.Write(CameraRotate);
                writer.Write(ResetInitialization);
                writer.Write(LightsState);
                writer.Write(StabilizationState);
                writer.Write(RollInc);
                writer.Write(PitchInc);
                writer.Write(ResetPosition);
                writer.Write(RollKp);
                writer.Write(RollKi);
                writer.Write(RollKd);
                writer.Write(PitchKp);
                writer.Write(PitchKi);
                writer.Write(PitchKd);
                writer.Write(YawKp);
                writer.Write(YawKi);
                writer.Write(YawKd);
                writer.Write(DepthKp);
                writer.Write(DepthKi);
                writer.Write(DepthKd);
                writer.Write(UpdatePID);
            }
            
            return result;
        }
    }
}