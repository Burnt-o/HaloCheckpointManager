using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.Helpers;
using System.Diagnostics;


namespace HCM3
{

    public class HaloState
    {
        public GameStateEnum GameState { get; init; }
        public LevelInfo? LevelInfo { get; init; }
        public string? MCCVersion { get; init; }

        public HaloState(GameStateEnum gameState, LevelInfo? levelInfo, string? mccversion)
        { 
            GameState = gameState;
            LevelInfo = levelInfo;
            MCCVersion = mccversion;
        }

        public override bool Equals(object? obj)
        {
            // If same object them yes, duh
            if (base.Equals(obj)) return true;

            // Needs to be a HaloState..
            if (obj != null && obj.GetType() != typeof(HaloState)) return false;

            // Check the members for equality
            HaloState hs = obj as HaloState;

            bool isSame = hs.GameState == this.GameState;

            isSame = isSame && hs.LevelInfo == this.LevelInfo;

            isSame = isSame && hs.MCCVersion == this.MCCVersion;


            // For debugging
            if (!isSame)
            {
                string s1 = ("Checking HaloState members for equality");
                string s2 = ($"isSame: {hs.LevelInfo == this.LevelInfo}, hs.LevelInfo: {hs.LevelInfo}, this.LevelInfo: {this.LevelInfo}");
                string s3 = ($"isSame: {hs.GameState == this.GameState}, hs.GameState: {hs.GameState}, this.GameState: {this.GameState}");
                string s4 = ($"isSame: {hs.MCCVersion == this.MCCVersion}, hs.MCCVersion: {hs.MCCVersion}, this.MCCVersion: {this.MCCVersion}");
                Trace.WriteLine($"{s1}\n{s2}\n{s3}\n{s4}\n");
                Trace.WriteLine("EXTRA: hs.LevelInfo is null? " ); Trace.Write(hs.LevelInfo == null);
                Trace.WriteLine("EXTRA: this.LevelInfo is null? "); Trace.Write(this.LevelInfo == null);
                if (hs.LevelInfo != null && this.LevelInfo != null)
                {
                    Trace.WriteLine("More info: ");
                    Trace.WriteLine("hs.LevelInfo.LevelCode: " + hs.LevelInfo.LevelCode + ", this.LevelInfo.LevelCode" + this.LevelInfo.LevelCode);
                    Trace.WriteLine("hs.LevelInfo.LevelPosition: " + hs.LevelInfo.LevelPosition + ", this.LevelInfo.LevelPosition" + this.LevelInfo.LevelPosition);
                    Trace.WriteLine("hs.LevelInfo.FullName: " + hs.LevelInfo.FullName + ", this.LevelInfo.FullName" + this.LevelInfo.FullName);
                }

            }

            return isSame;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }


        public bool? IsMultiplayer()
        {
            if (this.LevelInfo == null) return null;

            return this.LevelInfo.IsMultiplayer;
        }
    }
}
