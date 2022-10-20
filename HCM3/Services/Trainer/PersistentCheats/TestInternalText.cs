using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace HCM3.Services.Trainer
{
    public partial class PC_ToggleInvuln : IPersistentCheatService
    {


        public void Do()
        {
            try
            {
                if (!this.InternalServices.CheckInternalLoaded())
                {
                    List<string> listOfInternalFunctions = new();
                    listOfInternalFunctions.Add("ChangeDisplayText");
                    listOfInternalFunctions.Add("IsTextDisplaying");
                    this.InternalServices.SetupInternal(listOfInternalFunctions);
                }

                if (!this.InternalServices.CheckInternalLoaded())
                {
                    Trace.WriteLine("WTF");
                }

                /*            uint? exit = this.InternalServices.CallInternalFunction("IsTextDisplaying", null);

                            Trace.WriteLine("IsTextDisplaying returned " + exit.ToString());*/


                // Looks like the error is in the parameter stuff, specifically it's not getting written to process memory.
                uint? exit2 = this.InternalServices.CallInternalFunction("ChangeDisplayText", "Hellooo");
                Trace.WriteLine("ChangeDisplayText returned " + exit2.ToString());
            }
            catch (Exception ex)
            { 
            Trace.WriteLine("oh dear god: " + ex.Message);
            }
        }
    }
}
