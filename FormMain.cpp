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
#include <algorithm>

#include <anafestica/PersistFormWinFMX.h>

#include "FormMain.h"
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
using std::max;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TfrmMain *frmMain;
//---------------------------------------------------------------------------

__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TfrmMain( Owner, StoreOpts::All, nullptr )
{
}
//---------------------------------------------------------------------------

__fastcall TfrmMain::TfrmMain( TComponent* Owner, StoreOpts StoreOptions,
						   Anafestica::TConfigNode* const RootNode )
	: TConfigRegistryFormWinFMX( Owner, StoreOptions, RootNode )
{
	Init();
}
//---------------------------------------------------------------------------

__fastcall TfrmMain::~TfrmMain()
{
    try {
        Destroy();
    }
    catch ( ... ) {
    }
}
//---------------------------------------------------------------------------

void TfrmMain::Init()
{
    ClearGame();
    RestoreProperties();
}
//---------------------------------------------------------------------------

void TfrmMain::ClearGame()
{
    int v {};
    for ( auto& n : nums_ ) {
        n = make_unique<TfrmeNum>( nullptr );
        n->Parent = GridLayout1;
        n->Label1->Text = Format( _D( "%d✕%d" ), v / 10 + 1, v % 10 + 1 );
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

void TfrmMain::Destroy()
{
    SaveProperties();
}
//---------------------------------------------------------------------------

void TfrmMain::RestoreProperties()
{
    RESTORE_LOCAL_PROPERTY( Record );
}
//---------------------------------------------------------------------------

void TfrmMain::SaveProperties() const
{
    SAVE_LOCAL_PROPERTY( Record );
}
//---------------------------------------------------------------------------

void TfrmMain::SayAsync( String Text, std::function<void(void)> OnCompletion )
{

    if ( voiceFut_.valid() ) {
        voiceFut_.wait();
    }
    voiceFut_ = async(
        launch::async,
        [this]( auto Txt, auto Completion ) {
            {
                TOleSession OleSession;
                //::CoInitialize( 0 );

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
                //::CoUninitialize();
            }
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

void __fastcall TfrmMain::NumClick(TObject *Sender)
{
    if ( !GetAuto() ) {
        decltype( auto ) Num = static_cast<TfrmeNum&>( *Sender );
        NumClickInt( static_cast<NumFrames::size_type>( Num.Tag ) );
    }
}
//---------------------------------------------------------------------------

bool TfrmMain::VoiceStopped() const
{
    return !voiceFut_.valid() ||
           voiceFut_.wait_for( milliseconds( 10 ) ) == future_status::ready;
}
//---------------------------------------------------------------------------

class TRestartBarNotify : public TIdNotify
{
protected:
    TfrmMain* FForm;

    void __fastcall DoNotify()
    {
        FForm->StartCountdown();
    }

public:
    __fastcall TRestartBarNotify( TfrmMain* Form )
        : TIdNotify(), FForm( Form )
    {
    }
};

void TfrmMain::StartCountdown()
{
    if ( GetAuto() ) {
        ResetBar();
        StartBar();
    }
}
//---------------------------------------------------------------------------

void TfrmMain::StopCountdown()
{
    StopBar();
}
//---------------------------------------------------------------------------

void TfrmMain::NumClickInt( NumFrames::size_type Idx )
{
    if ( VoiceStopped() ) {
        if ( idx_ >= 0 ) {
            nums_[idx_]->ColorAnimation1->Enabled = false;
            nums_[idx_]->Rectangle1->Fill->Color = 0xFFE0E0E0;
        }
        idx_ = Idx;

        SayAsync(
			Format(_D("%d per %d" ), idx_ / 10 + 1, idx_ % 10 + 1 ),
            [this]() {
                ( new TRestartBarNotify( this ) )->Notify();
            }
        );
        nums_[idx_]->ColorAnimation1->Enabled = true;
    }
}
//---------------------------------------------------------------------------

void TfrmMain::MostraPunteggio()
{
    Label1->Text = Format( _D( "Punteggio: %d" ), punteggio_ );
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actTestExecute(TObject *Sender)
{
    auto const N = Edit1->Text.ToIntDef( -1 );
    if ( N >= 0 ) {
        StopCountdown();
        auto Match = ( idx_ / 10 + 1 ) * ( idx_ % 10 + 1 );
        if ( N == Match ) {
            auto Pnt = GetNumScore();
            SayAsync( N );
            SayAsync( GetOkSentence( Pnt ) );

            punteggio_ += Pnt;
            MostraPunteggio();
            nums_[idx_]->Ok();
        }
        else {
            SayAsync( Format( _D( "ERRORE!!! Hai scritto %d. Il risultato corretto è %d" ), N, Match ) );
            nums_[idx_]->Fail();
        }

        idx_ = -1;
        Edit1->Text = String();

        if ( GetAuto() ) {
            if ( !autoNums_.empty() ) {
                ready_ = true;
            }
            else {
                ClearAuto();
                record_ = max( record_, punteggio_ );
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actTestUpdate(TObject *Sender)
{
    TAction& Act = static_cast<TAction&>( *Sender );
    auto const N = Edit1->Text.ToIntDef( -1 );
	Act.Enabled = idx_ >= 0 && N >= 0 && N <= 100;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actRicominciaExecute(TObject *Sender)
{
    ClearGame();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if ( voiceFut_.valid() ) {
        voiceFut_.wait();
    }
}
//---------------------------------------------------------------------------

bool TfrmMain::GetAuto() const
{
    return Switch1->IsChecked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Timer1Timer(TObject *Sender)
{
    if ( GetAuto() && !autoNums_.empty() && ready_ && VoiceStopped() ) {
        ready_ = false;
        NumClickInt( autoNums_.back() );
        autoNums_.pop_back();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Switch1Switch(TObject *Sender)
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

template<typename T>
static inline bool IsEnabled( T const & Btn )
{
    return Btn.GetState() == TfrmeNum::State::Enabled;
}
//---------------------------------------------------------------------------

void TfrmMain::Shuffle()
{
    AutoNums Cnt;
    for ( NumFrames::size_type i = 0 ; i < nums_.size() ; ++i ) {
//        if ( nums_[i]->IsEnabled() ) {
        if ( IsEnabled( *nums_[i] ) ) {
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

void __fastcall TfrmMain::Timer2Timer(TObject *Sender)
{
    ProgressBar1->Value =
        std::max( ProgressBar1->Value - 1, ProgressBar1->Min );
    if ( !ProgressBar1->Value ) {
        Timer2->Enabled = false;
    }
    //Caption = GetNumScore();
}
//---------------------------------------------------------------------------

void TfrmMain::ResetBar()
{
    ProgressBar1->Value = ProgressBar1->Max;
    ProgressBar1->Visible = true;
    //Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmMain::StartBar()
{
    Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmMain::StopBar()
{
    Timer2->Enabled = false;
}
//---------------------------------------------------------------------------

void TfrmMain::HideBar()
{
    Timer2->Enabled = false;
    ProgressBar1->Visible = false;
}
//---------------------------------------------------------------------------

void TfrmMain::ClearAuto()
{
    Switch1->IsChecked = false;
}
//---------------------------------------------------------------------------

int TfrmMain::GetNumScore() const
{
    return
        ( static_cast<double>( MaxP ) + 0.75 - MinP ) *
        ( ProgressBar1->Value - ProgressBar1->Min ) /
        ProgressBar1->Max - ProgressBar1->Min + MinP;
}
//---------------------------------------------------------------------------

String TfrmMain::GetOkSentence( int Val ) const
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

void TfrmMain::SetRecord( int Val )
{
    record_ = Val;
    MostraRecord();
}
//---------------------------------------------------------------------------

void TfrmMain::MostraRecord()
{
    Label3->Text = Format( _D( "Record: %d" ), record_ );
}
//---------------------------------------------------------------------------

