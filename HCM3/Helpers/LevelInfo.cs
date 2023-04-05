using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Helpers
{

    // One half of HaloState class, along with GameStateEnum
    public class LevelInfo
    {
        public string LevelCode;
        public string? FullName;
        public int? LevelPosition;
        public bool? IsMultiplayer;

        public LevelInfo(string levelCode, string? fullName, int? levelPosition)
        {
            this.LevelCode = levelCode;
            this.FullName = fullName;
            this.LevelPosition = levelPosition;

            if (levelPosition == null)
                IsMultiplayer = null;
            else
                IsMultiplayer = levelPosition == -1;

        }

        public override bool Equals(object? obj)
        {
            // If same object them yes, duh
            if (base.Equals(obj)) return true;

            // If both are null then yes
            if (obj == null && this == null) return true;

            // Needs to be a LevelInfo..
            if (obj != null && obj.GetType() != typeof(LevelInfo)) return false;


            // Check the members for equality
            LevelInfo li = obj as LevelInfo;
            // We only care about the LevelCode, as all other members are derived from that via the LevelCodeToLevelInfo dictionaries
            return (li.LevelCode == this.LevelCode);
        }
    }
}
