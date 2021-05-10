#ifndef OleUtilsH
#define OleUtilsH
//---------------------------------------------------------------------------

#include <System.Win.ComObj.hpp>

class TOleSession
{
public:
    TOleSession() {
        OleCheck( ::CoInitialize( 0 ) );
        doUninitialize_ = true;
    }
    ~TOleSession() {
        if ( doUninitialize_ ) {
            ::CoUninitialize();
        }
    }
    TOleSession( TOleSession const & ) = delete;
    TOleSession& operator=( TOleSession const & ) = delete;
    void Release() { doUninitialize_ = false; }
private:
    bool doUninitialize_ {};
};

//---------------------------------------------------------------------------
#endif


