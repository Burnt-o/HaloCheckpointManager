// See https://aka.ms/new-console-template for more information
using Keystone;
using System.Diagnostics;



Trace.WriteLine("Hello, World!");

//mov dword ptr [rbp-28]; mov [rsp+74], r10d; mov rax, 00007FFF87E9346Ch; cmp r14, [rax]; mov rax 00007FFF86E32FB5h; jmp [rax]
// somethings wrong with the final operand here
string ASMstring = "add rax, 34h; add rax, rcx; push rcx; push rax; push rcx; push rbp; push rdx; mov rcx, 0; add rcx, 250h; mov rdx, 0; mov edx, [rcx]; cmp edi, ecx; jne 50h; mov ebx, [rax + 0F0h]; cmp ebx, FFFFFFFFh; cmovne [rcx + 0B0h], ebx; cmove [rcx + 0B0h], edx; cmp [rcx + 0B0h], edi; jne 100h; mov ebp, [rax + 18h]; mov [rcx], ebp; mov ebp, [rax + 1Ch]; mov [rcx + 4h], ebp; mov ebp, [rax + 20h]; mov [rcx + 8h], ebp; mov ebp, [rax + 24h]; mov [rcx + 0Ch], ebp; mov ebp, [rax + 28h]; mov [rcx + 10h], ebp; mov ebp, [rax + 2Ch]; mov [rcx + 14h], ebp; mov ebp, [rax + 9Ch]; mov [rcx + 18h], ebp; mov ebp, [rax + 00A0h]; mov [rcx + 1Ch], ebp; mov rax, 0; mov ebp, [rax]; mov [rcx + 20h], ebp; mov rax, 0; mov ebp, [rax]; mov [rcx + 24h], ebp; pop rcx; pop rax; pop rcx; pop rbp; pop rdx; jmp 0";
ASMstring = "cmp [rsp+048h], r10b";

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