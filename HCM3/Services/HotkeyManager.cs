using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using XInputium;
using XInputium.XInput;
using System.Diagnostics;
using NonInvasiveKeyboardHookLibrary;
using System.IO;
using System.Xml.Serialization;
using System.Xml;
using System.Collections;
using System.Xml.Linq;


namespace HCM3.Services
{
    public class HotkeyManager
    {
        public readonly XGamepad gamepad;

        public readonly KeyboardHookManager keyboard;
        public bool IgnoreGamepad { get; set; }

        public Dictionary<string, Tuple<int?, Action?>> RegisteredKBbindings { get; set; }

        public Dictionary<string, Tuple<XInputButton?, Action?>> RegisteredGPbindings { get; set; }
       // public Dictionary<string, Tuple<int, int>> SavedBindings { get; set; }   
        public HotkeyManager()
        { 
        
            // Grabs the first connected controller. Mainwindow will subscribe to it's buttonpress.
            // Mainwindow will also need to call gamepad.Update in it's OnRender
        gamepad = new XGamepad();

            //Keyboard listener using NonInvasiveKeyboardHookLibrary
            keyboard = new();
            keyboard.Start();

            RegisteredKBbindings = new();
            RegisteredGPbindings = new();

            //Load bindings from file
            DeserializeBindings();

            gamepad.ButtonPressed += Gamepad_ButtonPressed;
            //registering will happen in button viewmodels
            this.IgnoreGamepad = false;

        }

        private void Gamepad_ButtonPressed(object? sender, DigitalButtonEventArgs<XInputButton> e)
        {
            if (this.IgnoreGamepad) return;
            if (e.Button.Duration > TimeSpan.Zero) return;
            foreach (var binding in RegisteredGPbindings)
            {
                if (e.Button == binding.Value.Item1 && binding.Value.Item2 != null)
                {
                    Trace.WriteLine("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
                    binding.Value.Item2.Invoke();
                    //return // returning here would make only the first bind check worked. But user may want to bind more than one thing to a single button.
                }
            }
        }

        public void KB_ReloadHotkeys()
        {
            keyboard.UnregisterAll();
            Dictionary<int, List<Action>> multiBindings = new();

            foreach (var binding in RegisteredKBbindings)
            {
                Trace.WriteLine("reloadhotkeys: " + binding.Key + ", " + binding.Value.Item1 + ", " + binding.Value.Item2);
                if (binding.Value.Item1 != null && binding.Value.Item2 != null)
                {
                    int keycode = binding.Value.Item1.Value;
                    Action action = binding.Value.Item2;
                    if (!multiBindings.ContainsKey(keycode)) //this key has not been bound yet
                    {
                        keyboard.RegisterHotkey(keycode, action);
                        multiBindings.Add(keycode, new List<Action>() { action });
                    }
                    else //key HAS been bound before
                    {
                        //first unregister previous binding
                        keyboard.UnregisterHotkey(keycode);
                        //add to multiBindings dictionary
                        multiBindings[keycode].Add(action);

                        //create a new action that is the sum of all actions that want to be bound to this key
                        Action executeAllActions = () => {
                            foreach (var action in multiBindings[keycode])
                            { 
                            action.Invoke();
                            }
                        };

                        //register
                        keyboard.RegisterHotkey(keycode, executeAllActions);
                    }
                    
                }
            }
        }

        public void KB_ChangeHotkey(string hotkeyName, int? keycode, Action? onPress)
        { 
        KB_InitHotkey(hotkeyName, keycode, onPress);
            KB_ReloadHotkeys();
        }

        public void GP_ChangeHotkey(string hotkeyName, XInputButton? keycode, Action? onPress)
        {
            GP_InitHotkey(hotkeyName, keycode, onPress);
            //OKAY I did not need this function but it's NEAT okay
        }

        public void KB_InitHotkey(string hotkeyName, int? keycode, Action? onPress)
        {
            if (keycode == null || onPress == null)
            {
                if (RegisteredKBbindings.ContainsKey(hotkeyName))
                { 
                RegisteredKBbindings.Remove(hotkeyName);
                }
                return;
            }


            if (RegisteredKBbindings.ContainsKey(hotkeyName))
            {
                RegisteredKBbindings[hotkeyName] = new Tuple<int?, Action?>(keycode, onPress);
            }
            else
            {
                RegisteredKBbindings.Add(hotkeyName, new Tuple<int?, Action?>(keycode, onPress));
            }
        }

        public void GP_InitHotkey(string hotkeyName, XInputButton? keycode, Action? onPress)
        {
            if (onPress == null) return;
            if (RegisteredGPbindings.ContainsKey(hotkeyName))
            {
                RegisteredGPbindings[hotkeyName] = new Tuple<XInputButton?, Action?>(keycode, onPress);
            }
            else
            {
                RegisteredGPbindings.Add(hotkeyName, new Tuple<XInputButton?, Action?>(keycode, onPress));
            }
        }


        private void DeserializeBindings()
        {
            
            string localBindingsFile = Directory.GetCurrentDirectory() + "\\Bindings.xml";

            if (!File.Exists(localBindingsFile))
            {
               
                return;
            }
            else
            {
                string xml = File.ReadAllText(localBindingsFile);
                XDocument doc = XDocument.Parse(xml);

                if (doc.Root == null) throw new Exception("Something went wrong parsing the binding data xml file; 'doc.Root' was null.");
               
                foreach (XElement entry in doc.Root.Elements())
                {
                    try
                    {

                        string dicString = entry.Name.ToString();
                        int? KBint = entry.Element("KB") == null ? null : ParseKeyboardKeyFromString(entry.Element("KB").Value);
                        XInputButton? GPint = entry.Element("GP") == null ? null : ParseXInputButtonFromString(entry.Element("GP").Value);


                        if (KBint != null)
                        {

                            // null action - button viewmodels will update this
                            RegisteredKBbindings.Add(dicString, new Tuple<int?, Action?>(KBint, null));
                        }
                        else
                        {

                        }


                        if (KBint != null)
                        {
                            // null action - button viewmodels will update this
                            RegisteredGPbindings.Add(dicString, new Tuple<XInputButton?, Action?>(GPint, null));
                        }


                    }
                    catch (Exception ex)
                    {
                        System.Windows.MessageBox.Show("Binding.xml error: " + ex.ToString());
                    }
                }
            }

        }

        public void SerializeBindings()
        {
         
            string localBindingsFile = Directory.GetCurrentDirectory() + "\\Bindings.xml";
            if (File.Exists(localBindingsFile))
            { 
            File.Delete(localBindingsFile);
            }
            using (TextWriter writer = File.CreateText(localBindingsFile))
            {
                writer.WriteLine("<?xml version=\"1.0\" encoding=\"utf - 8\" ?>");
                writer.WriteLine("<Root>");

              foreach (KeyValuePair<string, Tuple<int?,Action?>> kvp in RegisteredKBbindings)
                {
                    if (kvp.Value.Item1 == null) continue;
                    
                    writer.WriteLine($"<{kvp.Key}>");
                    writer.WriteLine($"<KB>{kvp.Value.Item1.Value.ToString()}</KB>");
                    writer.WriteLine($"</{kvp.Key}>");
                }
                writer.WriteLine("</Root>");
            }
        }

        private XInputButton? ParseXInputButtonFromString(string? str)
        {
            throw new NotImplementedException();
        }

        private int? ParseKeyboardKeyFromString(string? str)
        {
            if (str == null) return null;

            try
            {
                return Convert.ToInt32(str);
            }
            catch
            {
                return null;
            }
        }


    }
}
