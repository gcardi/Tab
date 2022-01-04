//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include "FrameNum.h"
#include <FMX.Objects.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.Edit.hpp>
#include <FMX.EditBox.hpp>
#include <FMX.NumberBox.hpp>
#include <FMX.ActnList.hpp>
#include <FMX.StdCtrls.hpp>
#include <System.Actions.hpp>
#include <FMX.Ani.hpp>

#include <array>
#include <memory>
#include <future>
#include <vector>
#include <atomic>

#include <anafestica/PersistFormWinFMX.h>
#include <anafestica/CfgRegistrySingleton.h>

//---------------------------------------------------------------------------

typedef Anafestica::TPersistFormWinFMX<Anafestica::TConfigRegistrySingleton>
		TConfigRegistryFormWinFMX;

class TfrmMain : public TConfigRegistryFormWinFMX
{
__published:	// IDE-managed Components
    TGridLayout *GridLayout1;
    TLayout *Layout1;
    TStyleBook *StyleBook1;
    TEdit *Edit1;
    TButton *Button1;
    TActionList *ActionList1;
    TAction *actTest;
    TLabel *Label1;
    TAction *actRicomincia;
    TButton *Button2;
	TRectangle *Rectangle1;
	TGradientAnimation *GradientAnimation1;
    TSwitch *Switch1;
    TLabel *Label2;
    TTimer *Timer1;
    TProgressBar *ProgressBar1;
    TTimer *Timer2;
    TLabel *Label3;
    void __fastcall actTestExecute(TObject *Sender);
    void __fastcall actTestUpdate(TObject *Sender);
    void __fastcall actRicominciaExecute(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Switch1Switch(TObject *Sender);
    void __fastcall Timer2Timer(TObject *Sender);
private:	// User declarations
    using NumFramePtr = std::unique_ptr<TfrmeNum>;
    using NumFrames = std::array<NumFramePtr,100>;
    using AutoNums = std::vector<NumFrames::size_type>;
    using CompletionFnType = std::function<void(void)>;

    NumFrames nums_;
    int idx_ { -1 };
    int punteggio_ {};
    int record_ { 100 };
    std::future<void> voiceFut_;
    AutoNums autoNums_;
    std::atomic_bool ready_ { true };
    static constexpr int MaxP = 5;
    static constexpr int MinP = 1;

    void Init();
    void ClearGame();
    void Destroy();
    //static String GetModuleFileName();
    //void SetupCaption();
    void RestoreProperties();
    void SaveProperties() const;
    void __fastcall NumClick(TObject *Sender);
    void NumClickInt( NumFrames::size_type Idx );
    void MostraPunteggio();
    void SayAsync( String Text, CompletionFnType OnCompletion = {} );
    void Shuffle();
    void ResetBar();
    void StartBar();
    void StopBar();
    void HideBar();
    bool GetAuto() const;
    bool AutoNumsEmpty() const { return autoNums_.empty(); }
    bool IsReady() const { return ready_; }
    bool VoiceStopped() const;
    void StopCountdown();
    void ClearAuto();
    int GetNumScore() const;
    String GetOkSentence( int Val ) const;
    void SetRecord( int Val );
    void MostraRecord();

    __property int Record = { read = record_, write = SetRecord };
public:		// User declarations
    __fastcall TfrmMain( TComponent* Owner );
	__fastcall TfrmMain( TComponent* Owner, StoreOpts StoreOptions,
					   Anafestica::TConfigNode* const RootNode );
    __fastcall ~TfrmMain();
	void StartCountdown();

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
