using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using HCM.Model;

namespace HCM.Views
{
    /// <summary>
    /// Interaction logic for HCMWindowView.xaml
    /// </summary>
    public partial class HCMWindowView : UserControl
    {
        public HCMWindowView()
        {
            InitializeComponent();

            List<GameSaveFolder> folders = new List<GameSaveFolder>();
            GameSaveCollection testCollection1 = new GameSaveCollection();
            testCollection1.Load();
            GameSaveCollection testCollection2 = new GameSaveCollection();
            testCollection2.Load();
            GameSaveFolder folder1 = new GameSaveFolder("folder1", testCollection1);
            GameSaveFolder folder2 = new GameSaveFolder("folder1", testCollection2);

            folders.Add(folder1);
            folders.Add(folder2);

            testTreeView.ItemsSource = folders;
        }
    }
}
