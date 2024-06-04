using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace SevROVClient
{
    class RovConnection
    {
        private readonly UdpClient _client;
        private IPEndPoint _endPoint;

        public event EventHandler<TelemetryDataEventArgs> DataReceived;


        public RovConnection()
        {
            
            _client = new UdpClient();
        }

        public void Connect(string host, int port)
        {
            _endPoint = new IPEndPoint(IPAddress.Parse(host), port); 
            _client.Connect(_endPoint);

            _client.Send(new byte[] {0xAA, 0xFF}, 2);
            _client.BeginReceive(OnReceive, _client);
        }

        public void Send(ControlData controlData)
        {
            var bytes = controlData.ToBytes();
            _client.BeginSend(bytes, bytes.Length, OnSend, _client);
        }

        private void OnSend(IAsyncResult ar)
        {
            _client.EndSend(ar);
        }

        private void OnReceive(IAsyncResult ar)
        {
            UdpClient client = (UdpClient)ar.AsyncState;
            IPEndPoint senderIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            var receivedBytes = client.EndReceive(ar, ref senderIpEndPoint);
            var telemetryData = TelemetryDataEventArgs.FromBytes(receivedBytes);
            if (telemetryData != null)
            {
                OnDataReceived(telemetryData);
            }     
            

            

            _client.BeginReceive(OnReceive, ar.AsyncState);
        }

        protected virtual void OnDataReceived(TelemetryDataEventArgs e)
        {
            DataReceived?.Invoke(this, e);
        }
    }
}
