//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include <sapi.h>

#include <System.Win.ComObj.hpp>

#include <iterator>

#include "Unit1.h"

using std::make_unique;
using std::async;
using std::launch;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Unit2"
#pragma resource "*.fmx"
TForm1 *Form1;
//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    Init();
}
//---------------------------------------------------------------------------

__fastcall TForm1::~TForm1()
{
    if ( voiceFut_.valid() ) {
        voiceFut_.wait();
    }
}
//---------------------------------------------------------------------------

void TForm1::Init()
{
    int v {};
    for ( auto& n : nums_ ) {
        n = make_unique<TFrame2>( nullptr );
        n->Parent = GridLayout1;
        n->Label1->Text = Format( _D( "%d✕%d" ), v / 10, v % 10 );
        n->OnDblClick = &NumClick;
        n->Tag = v;
        ++v;
    }
    punteggio_ = {};
    idx_ = -1;
    MostraPunteggio();
    Edit1->Text = String();
}
//---------------------------------------------------------------------------

void TForm1::SayAsync( String Text )
{
    if ( voiceFut_.valid() ) {
        voiceFut_.wait();
    }
    voiceFut_ = async(
        launch::async,
        []( auto Txt ) {
            ::CoInitialize( 0 );

            using _di_ISpVoice = DelphiInterface<ISpVoice>;

            _di_ISpVoice pVoice;
            CoCreateInstance(
                CLSID_SpVoice,
                0,
                CLSCTX_INPROC_SERVER,
                IID_ISpVoice, ( LPVOID * ) &pVoice
            );

            pVoice->SetOutput( 0, TRUE );

            pVoice->Speak( Txt.c_str(), 0, nullptr );
            ::CoUninitialize();
        },
        Text
    );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NumClick(TObject *Sender)
{
    decltype( auto ) Num = static_cast<TFrame2&>( *Sender );
    if ( idx_ >= 0 ) {
        nums_[idx_]->ColorAnimation1->Enabled = false;
        nums_[idx_]->Rectangle1->Fill->Color = 0xFFE0E0E0;
    }
    Num.ColorAnimation1->Enabled = true;
    idx_ = Num.Tag;

    SayAsync( Format(_D("%d per %d" ), idx_ / 10, idx_ % 10 ) );
}
//---------------------------------------------------------------------------

void TForm1::MostraPunteggio()
{
    Label1->Text = Format( _D( "Punteggio: %d" ), punteggio_ );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::actTestExecute(TObject *Sender)
{
    auto const N = Edit1->Text.ToIntDef( -1 );
    auto Match = ( idx_ / 10 ) * ( idx_ % 10 );
    if ( N == Match ) {
        //ShowMessage( _D( "BENE!" ) );
        SayAsync( _D("BENE!" ) );

        ++punteggio_;
        MostraPunteggio();
        nums_[idx_]->Ok();
    }
    else {
        //ShowMessage( Format( _D( "ERRORE!!! Il risultato era %d" ), Match ) );
        SayAsync( Format( _D( "ERRORE!!! Il risultato era %d" ), Match ) );
        nums_[idx_]->Fail();
    }
    //nums_[idx_]->Visible = false;

    idx_ = -1;
    Edit1->Text = String();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::actTestUpdate(TObject *Sender)
{
    TAction& Act = static_cast<TAction&>( *Sender );
    auto const N = Edit1->Text.ToIntDef( -1 );
    Act.Enabled = idx_ >= 0 && N >= 0 && N <= 81;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::actRicominciaExecute(TObject *Sender)
{
    Init();
}
//---------------------------------------------------------------------------

