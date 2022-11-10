// See https://aka.ms/new-console-template for more information
using Keystone;
using System.Diagnostics;



Trace.WriteLine("Hello, World!");

//mov dword ptr [rbp-28]; mov [rsp+74], r10d; mov rax, 00007FFF87E9346Ch; cmp r14, [rax]; mov rax 00007FFF86E32FB5h; jmp [rax]
// somethings wrong with the final operand here
string ASMstring = "push rax; mov eax,[0]; cmp r13d, eax; pop rax; je 23h; push rax; mov eax,[1]; cmp r13d, eax; pop rax; je 13h; mov rax, [rdx+rsi]; cmp byte ptr [rax+11h], 04; jmp 3; mov rax, [rdx+rsi]; cmp byte ptr [rax+11h], 04; mov r15, [rbp-80h]; jmp 0";


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
    catch (KeystoneException ex)
    {
        Trace.WriteLine("Ex: " + ex.Error.ToString());

        Trace.WriteLine(keystone.GetLastKeystoneError().ToString());
    }

}
/*
byte[] test = new byte[5] { 0x0, 0x34, 0xFF, 0xFF, 0x90 };
string teststring = ByteArrayToString(test);
Trace.WriteLine(teststring);


byte[] test2 = StringToByteArray(teststring);
Trace.WriteLine("test2 length " + test2.Length);
for (int i = 0; i < test2.Length; i++)
{
    if (test2[i] == test[i])
    {
        Trace.WriteLine("Matched bytes");
    }
    else
    {
        Trace.WriteLine("MISMATCHED BYTES: old: " + test[i].ToString("X2") + ", new: " + test2[i].ToString("X2"));
    }
}




string ByteArrayToString(byte[] byteArray)
{
    string ByteArrayToString = "";
    for (int i = 0; i < byteArray.Length; i++)
    {
        ByteArrayToString = ByteArrayToString + "0x" +  byteArray[i].ToString("X2") + ",";
    }

    Trace.WriteLine(ByteArrayToString);
    ByteArrayToString = ByteArrayToString.Remove(ByteArrayToString.Length - 1);
    Trace.WriteLine(ByteArrayToString);
    return ByteArrayToString;
}

byte[] StringToByteArray(string str)
{
    
    str = str.Replace("0x", "");
    str = str.Replace(" ", "");
    string[] splitstr = str.Split(',');
    byte[] byteArray = new byte[splitstr.Length];
    for (int i = 0; i < splitstr.Length; i++)
    {
        byteArray[i] = Convert.ToByte(splitstr[i], 16);
    }
    return byteArray;
}*/