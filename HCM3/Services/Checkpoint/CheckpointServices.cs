using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HCM3.Services
{
    public partial class CheckpointServices
    {
        public HaloMemoryService HaloMemoryService { get; init; }
        public DataPointersService DataPointersService { get; init; }
        public CheckpointServices(HaloMemoryService haloMemoryService, DataPointersService dataPointersService)
        { 
        this.HaloMemoryService = haloMemoryService;
        this.DataPointersService = dataPointersService;
        }
        
    }
}
