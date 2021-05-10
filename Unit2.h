//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
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
class TFrame2 : public TFrame
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TRectangle *Rectangle1;
    TColorAnimation *ColorAnimation1;
	void __fastcall Click(TObject *Sender);
private:	// User declarations
    bool enabled_ { true };
public:		// User declarations
    __fastcall TFrame2(TComponent* Owner);
    void Fail();
    void Ok();
    bool IsEnabled() const { return enabled_; }
};
//---------------------------------------------------------------------------
extern PACKAGE TFrame2 *Frame2;
//---------------------------------------------------------------------------
#endif
