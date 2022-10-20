// See https://aka.ms/new-console-template for more information
using Keystone;
using System.Diagnostics;


Trace.WriteLine("Hello, World!");

//mov dword ptr [rbp-28]; mov [rsp+74], r10d; mov rax, 00007FFF87E9346Ch; cmp r14, [rax]; mov rax 00007FFF86E32FB5h; jmp [rax]
// somethings wrong with the final operand here
string ASMstring = "je 3h";


    using (Engine keystone = new Engine(Architecture.X86, Mode.X64) { ThrowOnError = true })
    {
    try
    {
        byte[] parsed = keystone.Assemble(ASMstring, 0, out _, out _);
        Trace.WriteLine("");
        for (int i = 0; i < parsed.Length; i++)
        {
            Trace.Write(parsed[i].ToString("X2"));
        }
        Trace.WriteLine("");
        Trace.WriteLine("");

    }
    catch(KeystoneException ex)
{
        Trace.WriteLine("Ex: " + ex.Error.ToString());
        
        Trace.WriteLine(keystone.GetLastKeystoneError().ToString());
}

}

