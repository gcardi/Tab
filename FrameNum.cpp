//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include "FrameNum.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TfrmeNum *frmeNum;
//---------------------------------------------------------------------------

__fastcall TfrmeNum::TfrmeNum(TComponent* Owner)
    : TFrame(Owner)
{
}
//---------------------------------------------------------------------------

void TfrmeNum::Fail()
{
    //enabled_ = false;
    state_ = State::Fail;
    ColorAnimation1->Enabled = false;
    Rectangle1->Fill->Color = claRed;
}
//---------------------------------------------------------------------------

void TfrmeNum::Ok()
{
    //enabled_ = false;
    state_ = State::Ok;
    ColorAnimation1->Enabled = false;
    Rectangle1->Fill->Color = claGreen;
}
//---------------------------------------------------------------------------

void __fastcall TfrmeNum::Click(TObject *Sender)
{
    //if ( enabled_ && OnDblClick ) {
    if ( state_ == State::Enabled && OnDblClick ) {
        OnDblClick( this );
    }
}
//---------------------------------------------------------------------------

