using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;
using System.Net;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=391641

namespace HttpSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            this.NavigationCacheMode = NavigationCacheMode.Required;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // TODO: Prepare page for display here.

            // TODO: If your application contains multiple pages, ensure that you are
            // handling the hardware Back button by registering for the
            // Windows.Phone.UI.Input.HardwareButtons.BackPressed event.
            // If you are using the NavigationHelper provided by some templates,
            // this event is handled for you.
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            GetWebContentByWinRT();
        }

        private async void GetWebContentByWinRT()
        {
            var uri = new Uri("http://www.msftncsi.com/ncsi.txt");
            var httpClient = new HttpClient();

            // Always catch network exceptions for async methods
            try
            {
                var result = await httpClient.GetStringAsync(uri);
                Textbox1.Text = result;
            }
            catch
            {
                // Details in ex.Message and ex.HResult.       
            }
        }

        private void Button2_Click(object sender, RoutedEventArgs e)
        {
            GetWebContentBySilverlight();
        }

        private async void GetWebContentBySilverlight()
        {
            System.Net.HttpWebRequest webRequest = (HttpWebRequest)HttpWebRequest.Create("http://www.msftncsi.com/ncsi.txt");
            try
            {
                WebResponse response = (WebResponse)await webRequest.GetResponseAsync();
                Stream st = response.GetResponseStream();
                StreamReader sr = new StreamReader(st);
                Textbox1.Text = sr.ReadToEnd();
                sr.Dispose();
            }
            catch
            { }
        }
    }
}
