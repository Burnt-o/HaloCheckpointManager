using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace HCM3.Model.CheckpointModels
{
    public class SaveFolder : INotifyPropertyChanged
    {
        public string SaveFolderPath { get; set; }
        public string SaveFolderName { get; set; }

        private bool _isSelected;
        public bool IsSelected
        {
            get => _isSelected;
            set
            {
                _isSelected = value;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsSelected)));
            }
        }
        public event PropertyChangedEventHandler? PropertyChanged;

        public ObservableCollection<SaveFolder> Children { get; set; }

        public string? ParentPath { get; set; }

        //So uh should the savefolder objects store info about their hierarchy in the tree view?
        public SaveFolder(string saveFolderPath, string saveFolderName, ObservableCollection<SaveFolder> children, string? parentPath)
        {
            SaveFolderPath = saveFolderPath;
            SaveFolderName = saveFolderName;
            Children = children;
            ParentPath = parentPath;
        }

        protected void OnPropertyChanged([CallerMemberName] string? name = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }

    }
}
