#ifndef StMcXiMaker_def
#define StMcXiMaker_def

#include "StMaker.h"
#include "TString.h"

class StMcV0Maker	 ;
class StGlobalTrack;
class TFile        ;
class TTree        ;
class TH1F         ;

#include "StXiType.h"
#include "StMcV0Maker/StMcV0Dst.h"
#include "StMcXiDst.h"

class StMcXiMaker : public StMaker
{
  
 private:

  //constants
//  enum {
//     kMaxNumberOfTH1F = 10
//  };

  StMcV0Maker*    	mV0Maker ;
  
  //master switch for the analysis, Xi type
  StXiType  mXiType;

  //switch for rotating the coordinates and momenta (in transverse direction) of one daughter.
  //for background estimation.
  //bool	mRotate;
  bool	mDcaAlgoLong;

  //section of v0type related constants
  Float_t   mMass1;
  Int_t	mCharge1;
  Float_t	mMass2;
  Int_t	mCharge2;
  Float_t	mMassV0;
  Float_t	mMassBachelor;
  Int_t 	mChargeBachelor;
  Float_t	mMassXi;
  Int_t	mChargeXi;
  
  Int_t     mGeantIDXi;
  Int_t	mGeantIDBachelor;
  Int_t     mGeantIDV0;
  Int_t     mGeantID1;
  Int_t     mGeantID2;

  //section of parameters for Xi analysis
  float  cutAbsVertexZLeEq;
  int    cutTriggerIdEq;

  int    cutNHitsGr;
  float  cutPtGrEq;

  float  cutAbsNSigma1Le;
  float  cutAbsNSigma2Le;
  float  cutDca1GrEq;
  float  cutDca2GrEq;

  float  cutV0DcaGrEq;
  float  cutV0MassWidthLeEq;
  float  cutDca1to2LeEq;
  float  cutXiMassWidthLeEq;
  float  cutDauPtArmLeEq;
  float  cutAbsDausPtShoulderDiffLeEq;
  float  cutDau1DecAngGr;
  float  cutDau2DecAngGr;
  float  cutXirdotpGr;
  float  cutXircrosspLeEq;
  float  cutDcaXiLe;
  float  cutXiDecLenGrEq;

  //histograms (mostly for QA purpose)
  //TH1F*         histogram[kMaxNumberOfTH1F] ;       //  1D Histograms
  TH1F*		hVertexZ;
  TH1F*		hPt;
  TH1F*		hNPrimVertex;
  TH1F*		hNRefMult;
  TH1F*		hPtDiff;
  TH1F*		hOrDiff;
  TH1F*		hPDiff;
  TH1F*		hDcaDiff;
  TH1F*		hInvMass;

  //files related
  TString       mHistogramOutputFileName ;         //  Name of the histogram output file 
  TString       mXiTreeOutputFileName ;         //  Name of the xi tree output file 
  TFile*        histogram_output ;                 //  Histograms outputfile pointer
  TFile*        xitree_output ;                 //  Xi Tree outputfile pointer
  TTree*        mXiTree;                 // Xi Tree outputfile pointer
  StMcXiDst	    mXiDst ;		     // Xi event (picoDst), to fill the tree

  //statistic information
  UInt_t        mEventsProcessed ;       //  Number of Events read and processed

  //some diagnosing variables
  Float_t   mTestVZ;
  UInt_t    mTestNTrack;

  //temporary variables (put here for performance consideration)
  vector<StGlobalTrack *> mDauVec1;
  vector<StGlobalTrack *> mDauVec2;
 
  vector<double> mDauDcaVec1;
  vector<double> mDauDcaVec2;

  vector<int> mDauVerVec1;
  vector<int> mDauVerVec2;

  vector<int> mDauMcXiKeyVec1;
  vector<int> mDauMcXiKeyVec2;
  //vector<StV0Data> mV0Vec;
  
  //private member functions
  void  initParam ( ) ;		//initialize parameters
  void  initConst ( ) ;		//initialize constants for v0type
  void  initHisto ( ) ;		//book histograms
  void  initTree ( ) ;		//book tree

 protected:
  //I do not expect some class inherits this maker! 

 public:

  StMcXiMaker(StMcV0Maker* v0maker, const char* name) ;       //  Constructor
  virtual          ~StMcXiMaker( ) ;       //  Destructor

  Int_t Init    ( ) ;                    //  Initiliaze the analysis tools ... done once
  Int_t Make    ( ) ;                    //  The main analysis that is done on each event
  Int_t Finish  ( ) ;                    //  Finish the analysis, close files, and clean up.

  void setHistoFileName(TString name) {mHistogramOutputFileName = name;} //set the name of output file for histograms
  void setXiTreeFileName(TString name) {mXiTreeOutputFileName = name;} //set the name of output file for StMcXiDst
  void setXiType(StXiType xitype) {mXiType = xitype;}	//set the xi type: kXi,kAntiXi,kOmega,kAntiOmega
  //void setRotate(bool brot) {mRotate = brot;}		//set rotation option 

  const StMcXiDst& getXiDst() const {return mXiDst;} 
  
  ClassDef(StMcXiMaker,1)                  //  Macro for CINT compatability
    
};

#endif

