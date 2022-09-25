using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace HCM.Model
{
    public class TreeItem : INotifyPropertyChanged
    {
        private string _name;

        public TreeItem(string name)
        { 
        _name = name;
        }

        public string Name
        { 
            get
            { 
                return _name; 
            }
            set
            {
                _name = value;
                this.OnPropertyChanged(nameof(Name));
            }
        }

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler? handler = this.PropertyChanged;
            if (handler != null)
                handler(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion // INotifyPropertyChanged Members
    }
}
