using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using HCM3.ViewModels.MVVM;
using System.Windows.Input;
using HCM3.ViewModels.Commands;
using HCM3.Services.Cheats;
using System.Windows;
using HCM3.Views.Controls.Trainer.ButtonViews.ButtonOptions;
using System.Diagnostics;
using HCM3.Services;
using XInputium.XInput;
using System.ComponentModel;
using System.Configuration;
using HCM3.Services.HaloMemory;


namespace HCM3.ViewModels
{
    public class CheatViewModel : ActionToggleBase
    {

        public Dictionary<string, SettingParameter> Settings { get; init; }


        //parameterless constructor for design view
        public CheatViewModel()
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");
            this.EffectText = "Effect";
            this.HotkeyText = "Hotkey";
        }

        //also used in design view
        public CheatViewModel(string effecttext)
        {
            if (!IsInDesignModeStatic) throw new Exception("This should only be run in design mode");
            this.EffectText = effecttext;
            this.HotkeyText = "Hotkey";
        }


        public ICheat? Cheat;
        public AbstractToggleCheat? AbstractToggleCheat { get; init; } //only assigned if the Cheat actually is that type, otherwise null. So that view can bind to IsChecked
        public CheckpointViewModel? CheckpointViewModel;
        public CheckpointServices? CheckpointServices;
        public CheatManagerService? CheatManagerService;

        public ICommand? UpdateCheatCommand { get; init; }



        public ICommand? ApplyExplicitSettings { get; init; }
        public ICommand? CancelSettingsChanges { get; init; }
        public ICommand? ResetSettings { get; init; }

        public Dictionary<string, ICommand>? CustomCommands { get; init; }





        private void ApplyAutoUpdateSettings(object? sender, PropertyChangedEventArgs? e)
        {
            SettingParameter settingParameter = Settings[e.PropertyName];

            if (settingParameter.BoundValue.GetType() != Properties.Settings.Default[e.PropertyName].GetType()) throw new Exception("User inputted invalid type into settings");
            Properties.Settings.Default[e.PropertyName] = settingParameter.BoundValue;
        }




        public CheatViewModel(string effectText, HotkeyManager hotkeyManager, ICheat? cheat, List<Tuple<string, bool>>? settingsToLoad, Dictionary<string, ICommand>? customCommands, Type? optionsWindow, CheatManagerService cheatManagerService)
        {
            if (cheat == null)
            {
                Trace.WriteLine("ICheat passed to CheatViewModel was null - forgot to add to CheatManagerService? : " + effectText);
                this.IsEnabled = false;
            }
                

            this.EffectText = effectText;
            this.NameOfBinding = effectText.Replace(" ", "");
            this.HotkeyManager = hotkeyManager;
            this.Cheat = cheat;
            this.CheatManagerService = cheatManagerService;

            if (cheat != null && cheat.GetType().BaseType == typeof(AbstractToggleCheat))
            {
                Trace.WriteLine("DETECTED TOGGLE CHEAT");
                this.AbstractToggleCheat = cheat as AbstractToggleCheat;
                this.UpdateCheatCommand = new RelayCommand(o =>
                {
                    try
                    {
                        ApplyExplicitSettings.Execute(this);
                        AbstractToggleCheat.UpdateCheat();
                    }
                    catch (Exception ex)
                    {
                        HCM3.Helpers.ErrorMessage.Show("Error updaing cheat " + this.EffectText + ", exception: " + ex.Message + "\n\n" + ex.ToString());
                    }
                    finally
                    {

                        UpdateIsCheckedAndUpdateActiveCheatString(AbstractToggleCheat.IsCheatApplied());
                        Trace.WriteLine("Updating cheat; checkbox should be applied? " + this.IsChecked);
                    }


                    




                });
                UpdateIsCheckedAndUpdateActiveCheatString(AbstractToggleCheat.IsCheatApplied());

                HaloStateEvents.HALOSTATECHANGED_EVENT += CheckToggleOnHaloStateChange;
                //HaloStateEvents.REMOVEALLTOGGLECHEATSEVENT += HandleRemoveAllToggleCheats;

                //Application.Current.Exit += HandleRemoveAllToggleCheats;


            }


            this.CustomCommands = customCommands;

            this.ExecuteCommand = new RelayCommand(o => // Called when the user hits the hotkey or clicks the do button or checkbox
            { 
                Trace.WriteLine("Executecommand called for " + this.EffectText);
                    ApplyExplicitSettings.Execute(this);
                try
                {
                    Cheat.DoCheat();
                }
                catch (Exception ex)
                {
                    HCM3.Helpers.ErrorMessage.Show("Something went wrong doing cheat action \"" + this.NameOfBinding + "\", error: " + ex.Message + "\n\nMore info for Burnt:\n" + ex.ToString());
                }
                finally
                {
                    if (this.AbstractToggleCheat != null)
                    {
                        UpdateIsCheckedAndUpdateActiveCheatString(AbstractToggleCheat.IsCheatApplied());
                    }
                }

            });
            

            


            // Setup binding for Settings Parameters, if there are any.
            // The bool is a flag determining whether the parameter wants to update it's binding source (Properties.Settings) automatically, or only explicitly (when ApplyExplicitSettings command is sent by view)
            if (settingsToLoad != null && settingsToLoad.Any())
            {
                Settings = new();
                foreach (Tuple<string, bool> settingToLoad in settingsToLoad)
                {
                    SettingParameter Setting = new(settingToLoad.Item1, settingToLoad.Item2, this);
                    Settings.Add(settingToLoad.Item1, Setting);
                }
            }
            // SettingParameter's listen to these events
            this.ApplyExplicitSettings = new RelayCommand(o =>
            {
                APPLYEXPLICITSETTINGS_EVENT?.Invoke(this, EventArgs.Empty);
            });

            this.CancelSettingsChanges = new RelayCommand(o =>
            {
                CANCELEXPLICITSETTINGS_EVENT?.Invoke(this, EventArgs.Empty);
            });

            this.ResetSettings = new RelayCommand(o =>
            {
                RESETEXPLICITSETTINGS_EVENT?.Invoke(this, EventArgs.Empty);
            });

            if (optionsWindow != null)
            {
                this.OpenOptionsWindowCommand = new RelayCommand(o => { OpenOptionsWindow(optionsWindow); }, o => { return true; });
            }



            this.OpenHotkeyWindowCommand = new RelayCommand(o => { OpenHotkeyWindow(); }, o => { return true; });

            // Load from deserialised bindings
            if (HotkeyManager.RegisteredKBbindings.ContainsKey(NameOfBinding))
            {
                Tuple<int?, Action?> KBbinding = HotkeyManager.RegisteredKBbindings[NameOfBinding];
                HotkeyManager.KB_InitHotkey(NameOfBinding, KBbinding.Item1, OnHotkeyPress);
                this.KBhotkey = KBbinding.Item1;
            }

            if (HotkeyManager.RegisteredGPbindings.ContainsKey(NameOfBinding))
            {
                Tuple<XInputButton?, Action?> GPbinding = HotkeyManager.RegisteredGPbindings[NameOfBinding];
                HotkeyManager.GP_InitHotkey(NameOfBinding, GPbinding.Item1, OnHotkeyPress);
                this.GPhotkey = GPbinding.Item1;
            }


            SetHotkeyText();

        }

        //private void HandleRemoveAllToggleCheats(object? sender, ExitEventArgs e)
        //{
        //    Trace.WriteLine($"CheatViewModel of {this.EffectText} handling RemoveAllToggleCheats");
        //    try
        //    {
        //        AbstractToggleCheat.RemoveCheat();
        //    }
        //    catch (Exception ex)
        //    {
        //        Trace.WriteLine("Error removing cheat " + this.EffectText + " on RemoveAllToggleCheats, " + ex.ToString());
        //        UpdateIsCheckedAndUpdateActiveCheatString(AbstractToggleCheat.IsCheatApplied());
        //    }
        //}

        private void CheckToggleOnHaloStateChange(object? sender, HaloStateEvents.HaloStateChangedEventArgs e)
        {
            Trace.WriteLine($"Checking cheatstate of {this.EffectText} on HaloStateChange event");
            UpdateIsCheckedAndUpdateActiveCheatString(AbstractToggleCheat.IsCheatApplied());
        }

        public void UpdateIsCheckedAndUpdateActiveCheatString(bool newValue)
        {
            this.CheatManagerService.SendToggleCheatMessage(AbstractToggleCheat, null, newValue);
            UpdateIsChecked(newValue);
        }

        public void UpdateIsChecked(bool newValue)
        {
            System.Windows.Application.Current.Dispatcher.Invoke((Action)delegate {
                IsChecked = newValue;
            });
        }

        public event EventHandler<EventArgs>? APPLYEXPLICITSETTINGS_EVENT;
        public void APPLYEXPLICITSETTINGS_EVENT_INVOKE(object sender, EventArgs e)
        {
            EventHandler<EventArgs>? handler = APPLYEXPLICITSETTINGS_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }

        public event EventHandler<EventArgs>? CANCELEXPLICITSETTINGS_EVENT;
        public void CANCELEXPLICITSETTINGS_EVENT_INVOKE(object sender, EventArgs e)
        {
            EventHandler<EventArgs>? handler = CANCELEXPLICITSETTINGS_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }

        public event EventHandler<EventArgs>? RESETEXPLICITSETTINGS_EVENT;
        public void RESETEXPLICITSETTINGS_EVENT_INVOKE(object sender, EventArgs e)
        {
            EventHandler<EventArgs>? handler = RESETEXPLICITSETTINGS_EVENT;
            if (handler != null)
            {
                handler(sender, e);
            }
        }



    }
}
