//---------------------------------------------------------------------------

#include <fmx.h>
#ifdef _WIN32
#include <tchar.h>
#endif
#pragma hdrstop
#include <System.StartUpCopy.hpp>
//---------------------------------------------------------------------------
USEFORM("FrameNum.cpp", frmeNum); /* TFrame: File Type */
USEFORM("FormMain.cpp", frmMain);
//---------------------------------------------------------------------------
extern "C" int FMXmain()
{
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TfrmMain), &frmMain);
         Application->Run();
        while ( auto const Cnt = Screen->FormCount ) {
            delete Screen->Forms[Cnt - 1];
        }
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
