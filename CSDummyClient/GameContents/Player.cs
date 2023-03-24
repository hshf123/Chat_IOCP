using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CSDummyClient.GameContents
{
    enum PLAYER_STATE : byte
    {
        NONE,
	    INLOBBY,
	    WATINGROOM,
	    INGAME,
    }

    internal class Player
    {
        public ulong PlayerId { get; set; } = 0;
        public string? Name { get; set; }
        public uint RoomId { get; set; } = 0;
        public PLAYER_STATE State { get; set; } = PLAYER_STATE.NONE;
    }
}
