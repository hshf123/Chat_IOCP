using CSDummyClient.GameContents;
using CSDummyClient.Network;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace CSDummyClient.Main
{
    internal class ServerSession : PacketSession
    {
        public void Send(IMessage packet)
        {
            string msgName = "PKT_" + packet.Descriptor.Name;
            MsgId msgId = (MsgId)Enum.Parse(typeof(MsgId), msgName);
            ushort size = (ushort)packet.CalculateSize();
            byte[] sendBuffer = new byte[size + 4];
            Array.Copy(BitConverter.GetBytes((ushort)(size + 4)), 0, sendBuffer, 0, sizeof(ushort));
            Array.Copy(BitConverter.GetBytes((ushort)msgId), 0, sendBuffer, 2, sizeof(ushort));
            Array.Copy(packet.ToByteArray(), 0, sendBuffer, 4, size);
            Send(new ArraySegment<byte>(sendBuffer));
        }

        public override void OnConnected(EndPoint endPoint)
        {
            Console.WriteLine($"OnConnected : {endPoint}");
            Managers.Instance.SessionManager.Add(this);

            C_LOGIN loginPkt = new C_LOGIN();
            string? name;
            while (true)
            {
                Console.Write("이름을 입력하세요 : ");
                name = Console.ReadLine();
                // TODO : 이름 컨벤션
                if (string.IsNullOrEmpty(name) == false)
                    break;
            }

            loginPkt.Name = name;
            Send(loginPkt);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            Console.WriteLine($"OnDisconnected : {endPoint}");
            Managers.Instance.SessionManager.Remove(this);
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
            // TODO : packetId 대역 체크
            PacketManager.Instance.OnRecvPacket(this, buffer);
        }

        public override void OnSend(int numOfBytes)
        {
        }
    }
}
