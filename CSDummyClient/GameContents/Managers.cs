using CSDummyClient.Main;
using Google.Protobuf;
using Google.Protobuf.Protocol;
using Microsoft.VisualBasic;
using Protocol;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSDummyClient.GameContents
{
    internal class Managers
    {
        #region Singleton
        static Managers _instance = new Managers();
        public static Managers Instance { get { return _instance; } }
        #endregion

        object _lock = new object();
        List<Tuple<string, string>> _chatData = new List<Tuple<string, string>>();

        public Player MyPlayer { get; set; } = new Player();
        public SessionManager SessionManager { get; set; } = new SessionManager();

        public void Init()
        {
            MyPlayer.State = PLAYER_STATE.INLOBBY;
        }

        public void Update()
        {
            switch (MyPlayer.State)
            {
                case PLAYER_STATE.INLOBBY:
                    InLobby();
                    break;
                case PLAYER_STATE.WATINGROOM:
                case PLAYER_STATE.INGAME:
                    Chat();
                    break;
                default:
                    break;
            }
        }

        public void Broadcast(IMessage pkt)
        {
            SessionManager.Broadcast(pkt);
        }

        public void InLobby()
        {
            if (MyPlayer.State != PLAYER_STATE.INLOBBY)
                return;
        }

        public void WaitingForGame(RoomInfo pkt)
        {
            if (MyPlayer.State != PLAYER_STATE.WATINGROOM)
                return;

            lock (_lock)
            {
                Console.Clear();
                Console.WriteLine($"현재 방은 {pkt.Id}번 방 입니다. 인원 : {pkt.PlayerCount} / 3");
                Console.Write("| 명단 | ");
                foreach (var p in pkt.Players)
                {
                    Console.Write("| ");
                    if (p.Name == MyPlayer.Name)
                    {
                        Console.Write("나 : ");
                    }
                    Console.Write($"{p.Name} | ");
                }
                Console.WriteLine();
                Console.WriteLine("방을 나가시려면 \'/exit\'를 입력하세요.");

                foreach (var p in _chatData)
                {
                    Console.WriteLine($"{p.Item1} : {p.Item2}");
                }
            }
        }

        public void Chat()
        {
            if (MyPlayer.State == PLAYER_STATE.WATINGROOM || MyPlayer.State == PLAYER_STATE.INGAME)
            {
                string msg = Console.ReadLine();
                if (string.IsNullOrEmpty(msg))
                    return;

                if (msg == "/exit")
		        {
                    LeaveRoom();
                }

                C_CHAT chatPkt = new C_CHAT() { Msg = msg };
                Broadcast(chatPkt);
            }
        }

        public void RecvChat(S_CHAT pkt)
        {
            lock(_lock)
            {
                _chatData.Add(new Tuple<string, string>(pkt.Player.Name, pkt.Msg));
            }

            Managers.Instance.WaitingForGame(pkt.RoomInfo);
        }

        public void LeaveRoom()
        {
            MyPlayer.State = PLAYER_STATE.INLOBBY;
            C_LEAVE_GAME leavePkt = new C_LEAVE_GAME();
            Broadcast(leavePkt);
        }
    }
}
