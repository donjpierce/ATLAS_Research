
#ifndef userInfo_h
#define userInfo_h

#include "TObject.h"
#include "TString.h"
#include "TDatime.h"


class userInfo : public TObject {
private:
    TString algAName;
    TString algBName;
    Int_t nbins;
    Float_t metmin;
    Float_t metmax;
    Float_t metl1thresh;
    Float_t frac;
    TString zbFileName;
    TString muonFileName;
    Int_t numzbRndm;
    Float_t algAThresh;
    Float_t algBThresh;
    Int_t muonNentries;
    Int_t zbNentries;
    Int_t numMuonKeptCombined;
    Int_t numMuonPassNumeratorAlgA;
    Int_t numMuonPassNumeratorAlgB;
    Int_t numMuonPassNumeratorAlgC;
    Int_t numMuonDenominator;
    Int_t epsilon;
    Float_t actintCut;
    TDatime now;
public:
    //default constructor
    userInfo();
    userInfo(TString algA , TString algB);
    //destructor
    void Print(Option_t *option = "") const;
    //getters
    TString get_zbFileName() const;
    TString get_muonFileName() const;
    Float_t getMetL1Thresh() const;
    Float_t getActintCut() const;
    Float_t getFrac() const;
    Float_t getMetMin() const;
    Float_t getMetMax() const;
    Int_t getNbins() const;
    Int_t getEpsilon() const;
    TDatime getDate() const;
    //setters
    void setAlgAName( const TString algA );
    void setAlgBName( const TString algB );
    void setNum_zbRndm( const Float_t num );
    void setAlgAThresh( const Float_t thresh );
    void setAlgBThresh( const Float_t thresh );
    void setMuonNentries( const Int_t num );
    void set_zbNentries( const Int_t num );
    void setNumMuonKeptCombined( const Int_t num );
    void setNumPassA( const Int_t num );
    void setNumPassB( const Int_t num );
    void setNumPassCombined( const Int_t num );
    void setNumTotal( const Int_t num );
    void setActintCut( const Float_t thresh );
    void setEpsilon( const Int_t epsilon );

    //this macro from Rtypes.h facilitates root I/O
    ClassDef( userInfo, 1)
};

    extern userInfo* logFileParams;


#endif
