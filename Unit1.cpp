//---------------------------------------------------------------------------

#include <fmx.h>
#pragma hdrstop

#include <sapi.h>

#include <System.Win.ComObj.hpp>

#include <IdSync.hpp>

#include <iterator>
#include <chrono>
#include <random>
#include <ctime>

#include "Unit1.h"
#include "OleUtils.h"

using std::make_unique;
using std::async;
using std::launch;
using std::chrono::milliseconds;
using std::future_status;
using std::random_device;
using std::mt19937;
using std::shuffle;
using std::array;
using std::generate_n;
using std::time;
using std::seed_seq;

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
    Switch1->IsChecked = false;
    Timer1->Enabled = false;
}
//---------------------------------------------------------------------------

void TForm1::SayAsync( String Text, std::function<void(void)> OnCompletion )
{

    if ( voiceFut_.valid() ) {
        voiceFut_.wait();
    }
    voiceFut_ = async(
        launch::async,
        [this]( auto Txt, auto Completion ) {
            //TOleSession OleSession;
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
            //ready_ = true;
            if( Completion ) {
                Completion();
            }
        },
        Text,
        OnCompletion
    );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NumClick(TObject *Sender)
{
    if ( !GetAuto() ) {
        decltype( auto ) Num = static_cast<TFrame2&>( *Sender );
        NumClickInt( static_cast<NumFrames::size_type>( Num.Tag ) );
    }
}
//---------------------------------------------------------------------------

bool TForm1::VoiceStopped() const
{
    return !voiceFut_.valid() ||
           voiceFut_.wait_for( milliseconds( 10 ) ) == future_status::ready;
}
//---------------------------------------------------------------------------

class TRestartBarNotify : public TIdNotify
{
protected:
    TForm1* FForm;

    void __fastcall DoNotify()
    {
        FForm->StartCountdown();
    }

public:
    __fastcall TRestartBarNotify( TForm1* Form )
        : TIdNotify(), FForm( Form )
    {
    }
};

void TForm1::StartCountdown()
{
    if ( GetAuto() ) {
        ResetBar();
        StartBar();
    }
}
//---------------------------------------------------------------------------

void TForm1::StopCountdown()
{
    StopBar();
}
//---------------------------------------------------------------------------

void TForm1::NumClickInt( NumFrames::size_type Idx )
{
    if ( VoiceStopped() ) {
        if ( idx_ >= 0 ) {
            nums_[idx_]->ColorAnimation1->Enabled = false;
            nums_[idx_]->Rectangle1->Fill->Color = 0xFFE0E0E0;
        }
        idx_ = Idx;

        SayAsync(
            Format(_D("%d per %d" ), idx_ / 10, idx_ % 10 ),
            [this]() {
                ( new TRestartBarNotify( this ) )->Notify();
            }
        );
        nums_[idx_]->ColorAnimation1->Enabled = true;
    }
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
    if ( N >= 0 ) {
        StopCountdown();
        auto Match = ( idx_ / 10 ) * ( idx_ % 10 );
        if ( N == Match ) {
            auto Pnt = GetNumScore();
            //ShowMessage( _D( "BENE!" ) );
            SayAsync( N );
            SayAsync( GetOkSentence( Pnt ) );

            punteggio_ += Pnt;
            MostraPunteggio();
            nums_[idx_]->Ok();
        }
        else {
            //ShowMessage( Format( _D( "ERRORE!!! Il risultato era %d" ), Match ) );
            SayAsync( Format( _D( "ERRORE!!! Hai scritto %d. Il risultato corretto è %d" ), N, Match ) );
            nums_[idx_]->Fail();
        }
        //nums_[idx_]->Visible = false;

        idx_ = -1;
        Edit1->Text = String();

        if ( GetAuto() ) {
            if ( !autoNums_.empty() ) {
                ready_ = true;
            }
            else {
                ClearAuto();
            }
        }
    }
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

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    if ( voiceFut_.valid() ) {
        voiceFut_.wait();
    }
}
//---------------------------------------------------------------------------

bool TForm1::GetAuto() const
{
    return Switch1->IsChecked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    if ( GetAuto() && !autoNums_.empty() && ready_ && VoiceStopped() ) {
        ready_ = false;
        NumClickInt( autoNums_.back() );
        autoNums_.pop_back();
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Switch1Switch(TObject *Sender)
{
    if ( GetAuto() ) {
        Shuffle();
        ready_ = true;
        Timer1->Enabled = true;
        ResetBar();
        Edit1->SetFocus();
    }
    else {
        HideBar();
        ready_ = false;
        Timer1->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void TForm1::Shuffle()
{
    AutoNums Cnt;
    for ( NumFrames::size_type i = 0 ; i < nums_.size() ; ++i ) {
        if ( nums_[i]->IsEnabled() ) {
            Cnt.push_back( i );
        }
    }

    random_device rd;
    array<int, mt19937::state_size> seed_data;
    generate_n(seed_data.data(), seed_data.size(),
        [&rd](){ return rd() ^ std::time(nullptr); }
        //std::ref(rd)
    );
    seed_seq seq(std::begin(seed_data), std::end(seed_data));
    mt19937 g(seq);

    shuffle( std::begin( Cnt ), std::end( Cnt ), g );

    autoNums_ = std::move( Cnt );

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
    ProgressBar1->Value =
        std::max( ProgressBar1->Value - 1, ProgressBar1->Min );
    if ( !ProgressBar1->Value ) {
        Timer2->Enabled = false;
    }
    //Caption = GetNumScore();
}
//---------------------------------------------------------------------------

void TForm1::ResetBar()
{
    ProgressBar1->Value = ProgressBar1->Max;
    ProgressBar1->Visible = true;
    //Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

void TForm1::StartBar()
{
    Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

void TForm1::StopBar()
{
    Timer2->Enabled = false;
}
//---------------------------------------------------------------------------

void TForm1::HideBar()
{
    Timer2->Enabled = false;
    ProgressBar1->Visible = false;
}
//---------------------------------------------------------------------------

void TForm1::ClearAuto()
{
    Switch1->IsChecked = false;
}
//---------------------------------------------------------------------------

int TForm1::GetNumScore() const
{
    return
        ( static_cast<double>( MaxP ) + 0.75 - MinP ) *
        ( ProgressBar1->Value - ProgressBar1->Min ) /
        ProgressBar1->Max - ProgressBar1->Min + MinP;
}
//---------------------------------------------------------------------------

String TForm1::GetOkSentence( int Val ) const
{
    switch ( Val ) {
        case 5:  return _D( "FANTASTICO!" );
        case 4:  return _D( "ECCEZIONALE!" );
        case 3:  return _D( "BENISSIMO!" );
        case 2:  return _D( "MOLTO BENE!" );
        default: return _D( "BENE!" );
    }
}
//---------------------------------------------------------------------------

