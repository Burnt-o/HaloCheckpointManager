using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Model.CheckpointModels
{
    public class PreserveLocation
    {
        public int Offset { get; init; }
        public int Length { get; init; }

        public PreserveLocation(int off, int len)
        {
            Offset = off;
            Length = len;
        }
    }
}
