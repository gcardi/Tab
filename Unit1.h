//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Layouts.hpp>
#include <FMX.Types.hpp>
#include "Unit2.h"
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

//---------------------------------------------------------------------------
class TForm1 : public TForm
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
    void __fastcall actTestExecute(TObject *Sender);
    void __fastcall actTestUpdate(TObject *Sender);
    void __fastcall actRicominciaExecute(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Switch1Switch(TObject *Sender);
private:	// User declarations
    using NumFramePtr = std::unique_ptr<TFrame2>;
    using NumFrames = std::array<NumFramePtr,100>;
    using AutoNums = std::vector<NumFrames::size_type>;

    NumFrames nums_;
    int idx_ { -1 };
    int punteggio_ {};
    std::future<void> voiceFut_;
    AutoNums autoNums_;
    std::atomic_bool ready_ { true };

    void Init();
    void __fastcall NumClick(TObject *Sender);
    void NumClickInt( NumFrames::size_type Idx );
    void MostraPunteggio();
    bool VoiceStopped() const;
    void SayAsync( String Text );
    bool GetAuto() const;
    void Shuffle();
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
