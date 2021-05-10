//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TFrame2 *Frame2;
//---------------------------------------------------------------------------

__fastcall TFrame2::TFrame2(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------

void TFrame2::Fail()
{
    enabled_ = false;
    ColorAnimation1->Enabled = false;
    Rectangle1->Fill->Color = claRed;
}
//---------------------------------------------------------------------------

void TFrame2::Ok()
{
    enabled_ = false;
    ColorAnimation1->Enabled = false;
    Rectangle1->Fill->Color = claGreen;
}
//---------------------------------------------------------------------------

void __fastcall TFrame2::Click(TObject *Sender)
{
    if ( enabled_ && OnDblClick ) {
        OnDblClick( this );
    }
}
//---------------------------------------------------------------------------

