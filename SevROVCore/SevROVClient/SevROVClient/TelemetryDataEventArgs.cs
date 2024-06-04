using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;

namespace SevROVClient
{
    public class TelemetryDataEventArgs : EventArgs
    {
        public float Roll { get; private set; }
        public float Pitch { get; private set; }
        public float Yaw { get; private set; }
        public float Heading { get; private set; }
        public float Depth { get; private set; }
        public float RollSetPoint { get; private set; }
        public float PitchSetPoint { get; private set; }

        private TelemetryDataEventArgs()
        {
            
        }
        
        public static TelemetryDataEventArgs FromBytes(byte[] data)
        {

            if (data.Length != 28)
            {
                EventLog.WriteEntry("SEVROV", "Telemetry data packet has wrong format", EventLogEntryType.Warning);
                return null;
            }

            using (var reader = new BinaryReader(new MemoryStream(data)))
            {
                var roll = reader.ReadSingle();
                var pitch = reader.ReadSingle();
                var yaw = reader.ReadSingle();
                var heading = reader.ReadSingle();
                var depth = reader.ReadSingle();
                var rollSP = reader.ReadSingle();
                var pitchSP = reader.ReadSingle();

                return new TelemetryDataEventArgs()
                {
                    Roll = roll, 
                    Pitch = pitch,
                    Yaw = yaw,
                    Heading = heading,
                    Depth = depth,
                    RollSetPoint = rollSP,
                    PitchSetPoint = pitchSP
                };
            }
        }
    }
}