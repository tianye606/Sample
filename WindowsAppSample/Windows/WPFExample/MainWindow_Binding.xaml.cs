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

namespace WPFExample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            DataTemplate obj = (DataTemplate)this.FindResource("template1");
        }
        
        private void click_test(object sender, RoutedEventArgs e)
        {
            Window1 win1 = new Window1();
            win1.Show();
        }
    }
    public class BindToTemplate1
    {
        public object CFEState 
        { 
            get
            {
                //DataTemplate obj = (DataTemplate)Application.Current.FindResource("template1");
                return "template1";
            }
        }
        public BindToTemplate1()
        {
            Console.WriteLine("BindToTemplate1");
        }
    }
    public class MyConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return (DataTemplate)Application.Current.MainWindow.FindResource(value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return value; //We don't care about this!
        }
    }
}
