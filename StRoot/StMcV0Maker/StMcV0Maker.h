#ifndef StMcV0Maker_def
#define StMcV0Maker_def

#include "StMaker.h"
#include "TString.h"

class StGlobalTrack  ;
class TFile        ;
class TTree        ;
class TH1F         ;

#include "StV0Type.h"
#include "StMcV0Dst.h"

class StMcV0Maker : public StMaker
{
  
 private:

  //constants
//  enum {
//     kMaxNumberOfTH1F = 10
//  };

  //master switch for the analysis, v0 type
  StV0Type  mV0Type;

  //switch for rotating the coordinates and momenta (in transverse direction) of one daughter.
  //for background estimation.
  //bool	mRotate;
  //bool	mSameSignPlus;
  //bool	mSameSignMinus;
  bool	mDcaAlgoLong;

  //section of v0type related constants
  Float_t   mMass1;
  Float_t	mMass2;
  Float_t	mMassV0;
  Int_t	mCharge1;
  Int_t	mCharge2;
  Int_t	mGeantIDV0;
  Int_t	mGeantID1;
  Int_t	mGeantID2;
  
  //section of parameters for v0 analysis
  float  cutAbsVertexZLeEq;
  int    cutTriggerIdEq;

  int    cutNHitsGr;
  float  cutPtGrEq;

  float  cutAbsNSigma1Le;
  float  cutAbsNSigma2Le;
  float  cutDca1GrEq;
  float  cutDca2GrEq;
  float  cutDca1LeEq;
  float  cutDca2LeEq;

  float  cutDca1to2LeEq;
  float  cutV0MassWidthLeEq;
  float  cutDauPtArmLeEq;
  float  cutAbsDausPtShoulderDiffLeEq;
  float  cutDau1DecAngGr;
  float  cutDau2DecAngGr;
  float  cutV0rdotpGr;
  float  cutDcaV0Le;
  float  cutV0DecLenGrEq;
  float  cutDau1Dau2Ang3DLe;
  float  cutDau1Dau2DipAngDiffLe;

  //histograms (mostly for QA purpose)
  //TH1F*         histogram[kMaxNumberOfTH1F] ;       //  1D Histograms
  TH1F*		hVertexZ;
  TH1F*		hPt;
  TH1F*		hNPrimVertex;
  TH1F*		hNRefMult;
  TH1F*		hSelectNRefMult;
  TH1F*		hPtDiff;
  TH1F*		hOrDiff;
  TH1F*		hPDiff;
  TH1F*		hDcaDiff;
  TH1F*		hInvMass;

  //files related
  TString       mHistogramOutputFileName ;         //  Name of the histogram output file 
  TString       mV0TreeOutputFileName ;         //  Name of the v0 tree output file 
  TFile*        histogram_output ;                 //  Histograms outputfile pointer
  TFile*        v0tree_output ;                 //  V0 Tree outputfile pointer
  TTree*        mV0Tree;                 // V0 Tree outputfile pointer
  StMcV0Dst	    mV0Dst ;		     // V0 event (picoDst), to fill the tree

  //statistic information
  UInt_t        mEventsProcessed ;                 //  Number of Events read and processed

  //some diagnosing variables
  Float_t   mTestVZ;
  UInt_t    mTestNTrack;
  bool      mPassEventCut;

  //temporary variables (put here for performance consideration)
  vector<StGlobalTrack *> mDauVec1;
  vector<StGlobalTrack *> mDauVec2;
 
  vector<double> mDauDcaVec1;
  vector<double> mDauDcaVec2;

  //private member functions
  void  initParam ( ) ;		//initialize parameters
  void  initConst ( ) ;		//initialize constants for v0type
  void  initHisto ( ) ;		//book histograms
  void  initTree ( ) ;		//book tree

 protected:
  //I do not expect some class inherits this maker! 

 public:

  StMcV0Maker(const char* name) ;       //  Constructor
  virtual          ~StMcV0Maker( ) ;       //  Destructor

  Int_t Init    ( ) ;                    //  Initiliaze the analysis tools ... done once
  Int_t Make    ( ) ;                    //  The main analysis that is done on each event
  Int_t Finish  ( ) ;                    //  Finish the analysis, close files, and clean up.

  void setHistoFileName(TString name) {mHistogramOutputFileName = name;} //set the name of output file for histograms
  void setV0TreeFileName(TString name) {mV0TreeOutputFileName = name;} //set the name of output file for StV0Dst
  void setV0Type(StV0Type v0type) {mV0Type = v0type;}	//set the v0 type: kLambda,kAntiLambda,kKs
  //void setRotate(bool brot) {mRotate = brot;}		//set rotation option 
  //void setSameSignPlus(bool brot) {mSameSignPlus = brot;}		//set same sign plus
  //void setSameSignMinus(bool brot) {mSameSignMinus = brot;}		//set same sign minus

  const StMcV0Dst& getV0Dst() const {return mV0Dst;}
  bool passEventCut() const {return mPassEventCut;}
  
  ClassDef(StMcV0Maker,1)                  //  Macro for CINT compatability
    
};

#endif

