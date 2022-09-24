using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace HCM.Model
{
    public class CheckpointCollection : ObservableCollection<Checkpoint>
    {
        protected CheckpointCollection()
        { 
        }

        public static CheckpointCollection Load()
        { 
        CheckpointCollection checkpoints = new CheckpointCollection();
            checkpoints.Add(new Checkpoint("My First Checkpoint", DateTime.Now, "a10", 3, TimeSpan.FromSeconds(6), DateTime.Now));
            checkpoints.Add(new Checkpoint("My Second Checkpoint", DateTime.Now, "a10", 0, TimeSpan.FromSeconds(12), DateTime.Now));
            checkpoints.Add(new Checkpoint("My Third Checkpoint", DateTime.Now, "a50", 3, TimeSpan.FromSeconds(78), DateTime.Now));
            return checkpoints;
        }

    }
}
