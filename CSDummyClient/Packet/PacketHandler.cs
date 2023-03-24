using CSDummyClient.GameContents;
using CSDummyClient.Main;
using CSDummyClient.Network;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSDummyClient.Packet
{
    internal class PacketHandler
    {
        internal static void S_LOGINHandler(PacketSession session, IMessage pkt)
        {
            ServerSession serverSession = session as ServerSession;
            S_LOGIN packet = pkt as S_LOGIN;
            if (packet.Success == false)
                return;

            if(packet.RoomInfos.Count==0)
            {
                // 뭔가 문제있는 상황
                return;
            }

            if(packet.IsNewId)
            {
                Console.WriteLine("새롭게 계정을 생성했습니다.");
                Console.WriteLine($"안녕하세요 {packet.Player.Name}님");
            }

            var roomInfos = packet.RoomInfos;
            foreach (RoomInfo roomInfo in roomInfos)
            {
                Console.WriteLine($"{roomInfo.Id}번방 | Player Count : {roomInfo.PlayerCount}");
            }

            C_ENTER_GAME enterGamePkt = new C_ENTER_GAME();
            while(true)
            {
                Console.WriteLine("들어갈 방을 선택하세요");
                uint roomId;
                bool success = uint.TryParse(Console.ReadLine(), out roomId);
                if (success && (roomId < 1 || roomId > packet.RoomInfos.Count))
                {
                    Console.WriteLine("잘못된 방 번호입니다. 다시 입력하세요");
                    continue;
                }

                enterGamePkt.RoomId = roomId;
                break;
            }

            Managers.Instance.Broadcast(enterGamePkt);
        }

        internal static void S_ENTER_GAMEHandler(PacketSession session, IMessage pkt)
        {
            ServerSession serverSession = session as ServerSession;
            S_ENTER_GAME packet = pkt as S_ENTER_GAME;
            if (packet.Success)
            {
                // 내가 속한 방의 정보를 저장
                Managers.Instance.MyPlayer.RoomId = packet.RoomInfo.Id;
                Managers.Instance.MyPlayer.State = PLAYER_STATE.WATINGROOM;
                Managers.Instance.WaitingForGame(packet.RoomInfo);
            }
            else
            {

                Console.Clear();
                Console.WriteLine("방을 다시 선택해주세요.");
                var roomInfos = packet.RoomInfos;
                foreach (RoomInfo roomInfo in roomInfos)
                {
                    Console.WriteLine($"{roomInfo.Id}번방 | Player Count : {roomInfo.PlayerCount}");
                }

                C_ENTER_GAME enterGamePkt = new C_ENTER_GAME();
                while (true)
                {
                    Console.WriteLine("들어갈 방을 선택하세요.");
                    uint roomId;
                    bool success = uint.TryParse(Console.ReadLine(), out roomId);
                    if (success && (roomId < 1 || roomId > packet.RoomInfos.Count))
                    {
                        Console.WriteLine("잘못된 방 번호입니다. 다시 입력하세요.");
                        continue;
                    }

                    enterGamePkt.RoomId = roomId;
                    break;
                }

                serverSession.Send(enterGamePkt);
            }
        }

        internal static void S_LEAVE_GAMEHandler(PacketSession session, IMessage pkt)
        {
            ServerSession serverSession = session as ServerSession;
            S_LEAVE_GAME packet = pkt as S_LEAVE_GAME;

            if (packet.RoomInfos.Count == 0)
                return;

            Managers.Instance.MyPlayer.RoomId = 0;

            Console.Clear();
            var roomInfos = packet.RoomInfos;
            foreach (RoomInfo roomInfo in roomInfos)
            {
                Console.WriteLine($"{roomInfo.Id}번방 | Player Count : {roomInfo.PlayerCount}");
            }

            C_ENTER_GAME enterGamePkt = new C_ENTER_GAME();
            while (true)
            {
                Console.WriteLine("들어갈 방을 선택하세요.");
                uint roomId;
                bool success = uint.TryParse(Console.ReadLine(), out roomId);
                if (success && (roomId < 1 || roomId > packet.RoomInfos.Count)) 
                {
                    Console.WriteLine("잘못된 방 번호입니다. 다시 입력하세요.");
                    continue;
                }

                enterGamePkt.RoomId = roomId;
                break;
            }

            Managers.Instance.Broadcast(enterGamePkt);
        }

        internal static void S_ROOM_UPDATEHandler(PacketSession session, IMessage pkt)
        {
            ServerSession serverSession = session as ServerSession;
            S_ROOM_UPDATE packet = pkt as S_ROOM_UPDATE;
            if (packet.RoomInfo.Id != Managers.Instance.MyPlayer.RoomId)
                return;

            Managers.Instance.WaitingForGame(packet.RoomInfo);
        }

        internal static void S_CHATHandler(PacketSession session, IMessage pkt)
        {
            ServerSession serverSession = session as ServerSession;
            S_CHAT packet = pkt as S_CHAT;
            if (packet.RoomInfo.Id != Managers.Instance.MyPlayer.RoomId)
                return;

            Managers.Instance.RecvChat(packet);
        }
    }
}
