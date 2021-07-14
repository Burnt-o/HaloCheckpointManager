using Newtonsoft.Json;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Input;

namespace WpfApp3 {
    public class HCMConfig
    {
        public string[] RanVersions;
        //public string CoreFolderPath;
        public bool ClassicMode = true;
        public int Reset_Tab;
        public int[] Reset_Folder;
        public int[] Reset_SelCP;
        public double[] Reset_Col;
        public bool LockoutLevels = true;
        public int LevelListOption = 0;

        public Key CPHotkey = Key.None;
        public Key RevertHotkey = Key.None;
        public Key DoubleRevertHotkey = Key.None;

        public SortConfig Sort = new SortConfig();

        public class SortConfig
        {
            [JsonProperty] // serialise even though private
            private List<(int Key, bool Reverse)> Criteria;
            public bool ReversePreviousPosition;

            public (int, bool) this[int i]
            {
                get
                {
                    // default to (None, not reverse)
                    if (Criteria == null || i >= Criteria.Count)
                        return (0, false);
                    return Criteria[i];
                }
                set
                {
                    // default to (None, not reverse)
                    while (i >= Criteria.Count)
                        Criteria.Add((0, false));
                    Criteria[i] = value;
                }
            }

            public void SetCriteria((int, bool)[] criteria)
            {
                Criteria = criteria.ToList();
            }
        }
    }
}
