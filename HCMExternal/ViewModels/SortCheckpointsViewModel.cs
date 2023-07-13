using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCMExternal.ViewModels.Interfaces;
using System.Windows;
using System.Collections.ObjectModel;
using HCMExternal.Services;
using System.Windows.Input;
using HCMExternal.ViewModels.Commands;
using HCMExternal.Models;
using HCMExternal.Services.CheckpointServiceNS;
using HCMExternal.Helpers.DictionariesNS;

namespace HCMExternal.ViewModels
{
    internal class SortCheckpointsViewModel : Presenter
    {
        public Action CloseAction { get; set; }

        public string FolderName { get; set; }

        public ObservableCollection<ICompareCheckpoints> ComboBoxOptions { get; init; }

        public ICompareCheckpoints Sort1 { get; set; }
        public ICompareCheckpoints Sort2 { get; set; }
        public ICompareCheckpoints Sort3 { get; set; }
        public ICompareCheckpoints Sort4 { get; set; }
        public ICompareCheckpoints Sort5 { get; set; }
        public ICompareCheckpoints Sort6 { get; set; }
        public ICompareCheckpoints Sort7 { get; set; }

        public bool Sort1Reverse { get; set; }
        public bool Sort2Reverse { get; set; }
        public bool Sort3Reverse { get; set; }
        public bool Sort4Reverse { get; set; }
        public bool Sort5Reverse { get; set; }
        public bool Sort6Reverse { get; set; }
        public bool Sort7Reverse { get; set; }

        private ICommand _accept;
        public ICommand Accept
        {
            get { return _accept ?? (_accept = new RelayCommand(o => { AcceptAction(); }, o => { return true; })); }
            set { _accept = value; }
        }

        private ICommand _cancel;
        public ICommand Cancel
        {
            get { return _cancel ?? (_cancel = new RelayCommand(o => { CancelAction(); }, o => { return true; })); }
            set { _cancel = value; }
        }

        public CheckpointService CheckpointServices { get; init; }
        public CheckpointViewModel CheckpointViewModel { get; init; }



        public SortCheckpointsViewModel(CheckpointService checkpointServices, CheckpointViewModel checkpointViewModel)
        {
            FolderName = "???";
            this.CheckpointServices = checkpointServices;
            this.CheckpointViewModel = checkpointViewModel;

            ComboBoxOptions = new ObservableCollection<ICompareCheckpoints>()
            {
                new SortNothing(),
                new SortAlphabetical(),
                new SortVersion(),
                new SortDifficulty(),
                new SortLevel(),
                new SortCreationTime(),
                new SortInGameTime()
            };


            Sort1 = new SortNothing();
            Sort2 = new SortNothing();
            Sort3 = new SortNothing();
            Sort4 = new SortNothing();
            Sort5 = new SortNothing();
            Sort6 = new SortNothing();
        }

        public void AcceptAction()
        {
            if (!Enum.IsDefined(typeof(HaloTabEnum), CheckpointViewModel.SelectedGame)) return;
            // get data together then call service, then close window
            //get data
            List<Tuple<ICompareCheckpoints, bool>> comparers = new();

            comparers.Add(new Tuple<ICompareCheckpoints, bool>(Sort1, Sort1Reverse));
            comparers.Add(new Tuple<ICompareCheckpoints, bool>(Sort2, Sort2Reverse));
            comparers.Add(new Tuple<ICompareCheckpoints, bool>(Sort3, Sort3Reverse));
            comparers.Add(new Tuple<ICompareCheckpoints, bool>(Sort4, Sort4Reverse));
            comparers.Add(new Tuple<ICompareCheckpoints, bool>(Sort5, Sort5Reverse));
            comparers.Add(new Tuple<ICompareCheckpoints, bool>(Sort6, Sort6Reverse));
            comparers.Add(new Tuple<ICompareCheckpoints, bool>(new SortLastWriteTime(), Sort7Reverse));

            Application.Current.Dispatcher.Invoke((Action)delegate
            {
                CheckpointServices.SortCheckpoints(CheckpointViewModel.SelectedSaveFolder, CheckpointViewModel.CheckpointCollection, comparers, (HaloTabEnum)CheckpointViewModel.SelectedGame);
            CheckpointViewModel.RefreshCheckpointList();


                CloseAction();
            });
        }

        public void CancelAction()
        {
            Application.Current.Dispatcher.Invoke((Action)delegate
            {
                CloseAction();
            });
        }



    }
}
