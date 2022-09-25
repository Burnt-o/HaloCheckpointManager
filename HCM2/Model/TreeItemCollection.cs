using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace HCM.Model
{
    public class TreeItemCollection : ObservableCollection<TreeItem>
    {
        public TreeItemCollection()
        {
            this.TreeItems = new ObservableCollection<TreeItem>();
        }

        public ObservableCollection<TreeItem> TreeItems { get; set; }

    }
}
