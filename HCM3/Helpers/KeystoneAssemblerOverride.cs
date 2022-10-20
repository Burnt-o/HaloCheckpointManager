using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Keystone;
using System.Diagnostics;

namespace HCM3.Helpers
{
    public static class KeystoneAssemblerOverride
    {



        public static byte[] Assemble(Engine keystone, Dictionary<string, IntPtr> resolver, string ASMstring, ulong startingAddy, out int bytesAssembledTotal, out int statementsAssembledTotal)
        {

            byte[] assembledBytes = new byte[0];

            char delimiter = ';';

            List<string> splitASMstring = ASMstring.Split(delimiter).ToList();
            ulong rip = startingAddy;


            bytesAssembledTotal = 0;
            statementsAssembledTotal = 0;
            
            foreach (string line in splitASMstring)
            {
                string resolvedline = line;
                foreach (KeyValuePair<string, IntPtr> entry in resolver)
                {
                    if (line.Contains(entry.Key))
                    {
                        //string resolved = "0x" + ((ulong)entry.Value.ToInt64() - rip).ToString("X");
                        string resolved = "0" + ((ulong)entry.Value.ToInt64() - rip).ToString("X") + "h";
                        Trace.WriteLine("Resolved? : " + resolved);
                    resolvedline = line.Replace(entry.Key, resolved);
                        Trace.WriteLine("Replaced line: " + resolvedline);
                        break;
                    }
                }
                Trace.WriteLine("assembling: " + resolvedline);
                //assemble and increment rip by bytesAssembled
                byte[] resolvedBytes = keystone.Assemble(resolvedline, 0, out int bytesAssembled, out _);
                assembledBytes = assembledBytes.Concat(resolvedBytes).ToArray();
                bytesAssembledTotal += bytesAssembled;
                statementsAssembledTotal++;
                rip = rip + (ulong)bytesAssembled;
            }

            if (assembledBytes.Length == 0) throw new Exception("Couldn't resolve any bytes");

            return assembledBytes;

        }


    }
}
