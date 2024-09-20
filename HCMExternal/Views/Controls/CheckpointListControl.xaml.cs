using Serilog;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

namespace HCMExternal.Views.Controls
{
    /// <summary>
    /// Interaction logic for CheckpointListControl.xaml
    /// </summary>
    public partial class CheckpointListControl : UserControl
    {
        public CheckpointListControl()
        {
            InitializeComponent();
        }

        private static readonly double originalLastColumnWidth = 145;

        // called when window containing the list view is resized. We want to expand/shrink the name column to fill the space.
        private void ListView_SizeChanged(object sender, SizeChangedEventArgs e)
        {

            double currentColumnTotalWidth = 0;
            foreach (GridViewColumn c in ((GridView)listView.View).Columns)
            {
                currentColumnTotalWidth += c.ActualWidth;
            }

            double newNameSize = ((GridView)listView.View).Columns[1].ActualWidth;
            newNameSize += e.NewSize.Width - currentColumnTotalWidth - 10;

            if (newNameSize > 55)
            {
                ((GridView)listView.View).Columns[1].Width = newNameSize;
            }
        }

        private void ListView_Loaded(object sender, RoutedEventArgs e)
        {
            Log.Debug("LIST VIEW LOADED");



            // Add the handler to know when resizing a column is done
            ((ListView)sender).AddHandler(Thumb.DragCompletedEvent, new DragCompletedEventHandler(ListViewHeader_DragCompleted), true);
        }

        private void ListViewHeader_DragCompleted(object sender, System.Windows.Controls.Primitives.DragCompletedEventArgs e)
        {
            double currentColumnTotalWidth = 0;
            foreach (GridViewColumn c in ((GridView)listView.View).Columns)
            {
                currentColumnTotalWidth += c.ActualWidth;
            }

            if (currentColumnTotalWidth > listView.ActualWidth)
            {
                return; // user wants scroll bar
            }

            double newNameSize = ((GridView)listView.View).Columns[1].ActualWidth;
            newNameSize += listView.ActualWidth - currentColumnTotalWidth - 10;

            if (newNameSize > 55)
            {
                ((GridView)listView.View).Columns[1].Width = newNameSize;
            }
        }
    }
}
