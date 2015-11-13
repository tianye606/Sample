﻿//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using SDKTemplate;
using Windows.ApplicationModel.Background;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace BackgroundTask
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SampleBackgroundTaskWithCondition : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public SampleBackgroundTaskWithCondition()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            foreach (var task in BackgroundTaskRegistration.AllTasks)
            {
                if (task.Value.Name == BackgroundTaskSample.SampleBackgroundTaskWithConditionName)
                {
                    AttachProgressAndCompletedHandlers(task.Value);
                    BackgroundTaskSample.UpdateBackgroundTaskStatus(BackgroundTaskSample.SampleBackgroundTaskWithConditionName, true);
                    break;
                }
            }

            UpdateUI();
        }

        /// <summary>
        /// Register a SampleBackgroundTaskWithCondition.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void RegisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            var task = BackgroundTaskSample.RegisterBackgroundTask(BackgroundTaskSample.SampleBackgroundTaskEntryPoint,
                                                                   BackgroundTaskSample.SampleBackgroundTaskWithConditionName,
                                                                   new SystemTrigger(SystemTriggerType.TimeZoneChange, false),
                                                                   new SystemCondition(SystemConditionType.InternetAvailable));
            await task;
            AttachProgressAndCompletedHandlers(task.Result);
            UpdateUI();
        }

        /// <summary>
        /// Unregister a SampleBackgroundTaskWithCondition.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UnregisterBackgroundTask(object sender, RoutedEventArgs e)
        {
            BackgroundTaskSample.UnregisterBackgroundTasks(BackgroundTaskSample.SampleBackgroundTaskWithConditionName);
            UpdateUI();
        }

        /// <summary>
        /// Attach progress and completed handers to a background task.
        /// </summary>
        /// <param name="task">The task to attach progress and completed handlers to.</param>
        private void AttachProgressAndCompletedHandlers(IBackgroundTaskRegistration task)
        {
            task.Progress += new BackgroundTaskProgressEventHandler(OnProgress);
            task.Completed += new BackgroundTaskCompletedEventHandler(OnCompleted);
        }

        /// <summary>
        /// Handle background task progress.
        /// </summary>
        /// <param name="task">The task that is reporting progress.</param>
        /// <param name="e">Arguments of the progress report.</param>
        private void OnProgress(IBackgroundTaskRegistration task, BackgroundTaskProgressEventArgs args)
        {
            var progress = "Progress: " + args.Progress + "%";
            BackgroundTaskSample.SampleBackgroundTaskWithConditionProgress = progress;
            UpdateUI();
        }

        /// <summary>
        /// Handle background task completion.
        /// </summary>
        /// <param name="task">The task that is reporting completion.</param>
        /// <param name="e">Arguments of the completion report.</param>
        private void OnCompleted(IBackgroundTaskRegistration task, BackgroundTaskCompletedEventArgs args)
        {
            UpdateUI();
        }

        /// <summary>
        /// Update the scenario UI.
        /// </summary>
        private async void UpdateUI()
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
            () =>
            {
                RegisterButton.IsEnabled = !BackgroundTaskSample.SampleBackgroundTaskWithConditionRegistered;
                UnregisterButton.IsEnabled = BackgroundTaskSample.SampleBackgroundTaskWithConditionRegistered;
                Progress.Text = BackgroundTaskSample.SampleBackgroundTaskWithConditionProgress;
                Status.Text = BackgroundTaskSample.GetBackgroundTaskStatus(BackgroundTaskSample.SampleBackgroundTaskWithConditionName);
            });
        }

    }
}
