using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Dia2.Interop;
using System.Reflection;

namespace PESymbolEnum
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("------------------");
            DiaSource source = new DiaSource();
            source.loadDataFromPdb(@"\\winbuilds\release\Winmain\10161.0.150628-1330\amd64fre\symbols.pri\test\dll\cilogcheck.pdb");
            IDiaSession session;
            source.openSession(out session);
            IDiaEnumSymbols functions = null;
            session.globalScope.findChildren(SymTagEnum.SymTagFunction, null, 0, out functions);
            foreach(IDiaSymbol func in functions)
            {
                string name = func.name;
                Console.Write(name + "\t");
                IDiaEnumLineNumbers lineNumbers = GetLineNumbers(session, func);
                if (lineNumbers == null) continue;
                foreach (IDiaLineNumber lineNumber in lineNumbers)
                {
                    if (lineNumber.sourceFile != null)
                    {
                        string SourceFile = lineNumber.sourceFile.fileName.Trim();
                        Console.WriteLine(SourceFile);
                        break;
                    }
                }
                //string name = func.name;
                //Console.WriteLine(name);
                //IDiaLineNumber lineNumber=null;
                //try
                //{
                //    func.getSrcLineOnTypeDefn(out lineNumber);
                //    if(lineNumber!=null)
                //        Console.WriteLine(lineNumber.sourceFile.fileName.Trim());
                //}
                //catch(Exception){}
            }

            //if (lineNumbers == null) continue;
            //foreach (IDiaLineNumber lineNumber in lineNumbers)
            //{
            //    if (lineNumber.sourceFile != null)
            //    {
            //        string SourceFile = lineNumber.sourceFile.fileName.Trim();
            //        Console.WriteLine(SourceFile);
            //        break;
            //    }
            //}
            //Console.WriteLine(typeof(IDiaSymbol).GetProperties().Count());
            //foreach (PropertyInfo pi in typeof(IDiaSymbol).GetProperties())
            //{
            //    Console.WriteLine("{0}={1}",pi.Name, pi.GetValue(func));
            //}
        }

        public static IDiaEnumLineNumbers GetLineNumbers(IDiaSession session,IDiaSymbol function)
        {
            IDiaEnumLineNumbers lines = null;

            if (function.relativeVirtualAddress != 0)
            {
                session.findLinesByRVA(function.relativeVirtualAddress, (uint)function.length, out lines);
            }
            else
            {
                try
                {
                    session.findLinesByAddr(
                        function.addressSection,
                        function.addressOffset,
                        (uint)function.length,
                        out lines);
                }
                catch{}
            }

            return lines;
        }
    }
}
