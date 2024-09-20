using System.Windows;
using System.Windows.Controls;

namespace HCMExternal.Views
{
    /// <summary>
    /// Interaction logic for Halo1Control.xaml
    /// </summary>
    internal partial class CheckpointControl : UserControl
    {

        public CheckpointControl()
        {
            InitializeComponent();
        }
    }

    // Needed a way to set a minimum width on ListView columns, sooo
    // yoink https://stackoverflow.com/a/61475351
    public static class GridColumn
    {
        public static readonly DependencyProperty MinWidthProperty =
            DependencyProperty.RegisterAttached("MinWidth", typeof(double), typeof(GridColumn), new PropertyMetadata(75d, (s, e) =>
            {
                if (s is GridViewColumn gridColumn)
                {
                    SetMinWidth(gridColumn);
                    ((System.ComponentModel.INotifyPropertyChanged)gridColumn).PropertyChanged += (cs, ce) =>
                    {
                        if (ce.PropertyName == nameof(GridViewColumn.ActualWidth))
                        {
                            SetMinWidth(gridColumn);
                        }
                    };
                }
            }));

        private static void SetMinWidth(GridViewColumn column)
        {
            double minWidth = (double)column.GetValue(MinWidthProperty);

            if (column.Width < minWidth)
            {
                column.Width = minWidth;
            }
        }

        public static double GetMinWidth(DependencyObject obj)
        {
            return (double)obj.GetValue(MinWidthProperty);
        }

        public static void SetMinWidth(DependencyObject obj, double value)
        {
            obj.SetValue(MinWidthProperty, value);
        }
    }
}
