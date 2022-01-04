//---------------------------------------------------------------------------

#ifndef FrameNumH
#define FrameNumH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls.Presentation.hpp>
#include <FMX.StdCtrls.hpp>
#include <FMX.Types.hpp>
#include <FMX.Objects.hpp>
#include <FMX.Ani.hpp>
//---------------------------------------------------------------------------
class TfrmeNum : public TFrame
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TRectangle *Rectangle1;
    TColorAnimation *ColorAnimation1;
	void __fastcall Click(TObject *Sender);
public:		// User declarations
    enum class State { Enabled, Ok, Fail };
private:	// User declarations
    //bool enabled_ { true };
    State state_ { State::Enabled };
public:		// User declarations
    __fastcall TfrmeNum(TComponent* Owner);
    void Fail();
    void Ok();
    //bool IsEnabled() const { return enabled_; }
    State GetState() const { return state_; }
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmeNum *frmeNum;
//---------------------------------------------------------------------------
#endif
