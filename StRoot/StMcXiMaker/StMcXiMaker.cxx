#include "StMcXiMaker.h"
#include "StarClassLibrary/SystemOfUnits.h"

#include <iostream>

#include "StMessMgr.h"
#include "StEvent/StGlobalTrack.h"
#include "StEvent/StEvent.h"

#include "StAssociationMaker/StAssociationMaker.h"
#include "StAssociationMaker/StTrackPairInfo.hh"
#include "StMcEvent/StMcEvent.hh"
#include "StEventUtilities/StuRefMult.hh"
#include "StMcEvent/StMcTrack.hh"
#include "StMcEvent/StMcVertex.hh"

#include "StMcV0Maker/StMcV0Maker.h"
#include "StMcV0Maker/StDcaService.h"

#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

ClassImp(StMcXiMaker)                   // Macro for CINT compatibility

StMcXiMaker::StMcXiMaker(StMcV0Maker* v0maker, const char * name) : StMaker(name)
{ // Initialize and/or zero all public/private data members here.

  //for ( Int_t i = 0 ; i < kMaxNumberOfTH1F ; i++ )  // Zero the histogram pointers, not necessary. it is NULL naturaly.
  //  {
  //    histogram[i] = NULL ;
  //  }

  mV0Maker      	 = v0maker ;                  // Pass V0Maker pointer to DstAnlysisMaker Class member functions

  mXiType = kXi;	//Lambda as default!

  mDcaAlgoLong = true; //use standard StHelix dca method as default

  histogram_output = NULL  ;                    // Zero the Pointer to histogram output file
  xitree_output = NULL  ;                    // Zero the Pointer to v0 tree output file
  mHistogramOutputFileName = "" ;               // Histogram Output File Name will be set inside the "analysis".C macro
  mXiTreeOutputFileName = "" ;               // Xi Output File Name will be set inside the "analysis".C macro

  mXiTree = NULL ;

  mEventsProcessed = 0     ;                    // Zero the Number of Events processed by the maker 
  mTestNTrack = 0;
  mTestVZ = 0;

}

StMcXiMaker::~StMcXiMaker() 
{ // Destroy and/or zero out all public/private data members here.
}

void StMcXiMaker::initConst(){
   //initialize the constant for different Xi types.
   
   if(mXiType == kXi || mXiType == kAntiXi){
	// for Xi or AntiXi
	mMass1      = 0.93827; // mass of proton or pbar
	mMass2      = 0.13957; // mass of pion- or pion+
	mMassV0     = 1.115684;// mass of Lambda or AntiLambda
	mMassBachelor = 0.13957; //mass of pion- or pion+
	mMassXi	= 1.32131;   //mass of Xi or AntiXi

	mGeantIDXi  = (mXiType == kXi)? 22:31; //22 is andrew's typo. 23 in geant
	mGeantIDBachelor = (mXiType == kXi)? 9:8;
	mGeantIDV0  = (mXiType == kXi)? 18:26;
	mGeantID1  = (mXiType == kXi)? 14:15;
	mGeantID2  = (mXiType == kXi)? 9:8;

	if(mXiType == kXi) { 
	   mCharge1	= 1;
	   mCharge2	= -1;
	   mChargeBachelor = -1;
	   mChargeXi = -1;
	}
	else {
	   mCharge1	= -1;
	   mCharge2	= 1;
	   mChargeBachelor = 1;
	   mChargeXi = 1;
	}
	//do not setup the cut values here. those are parameters
	
      //parameters for StDcaService.cxx
      kShiftConnect = 0.3;
      kShiftContain = 0.3;
   }
   else {
	assert(mXiType == kOmega || mXiType == kAntiOmega); 
	// for Omega or AntiOmega 
	mMass1      = 0.93827; // mass of proton or pbar
	mMass2      = 0.13957; // mass of pion- or pion+
	mMassV0     = 1.115684;// mass of Lambda or AntiLambda
	mMassBachelor = 0.493677; //mass of K- or K+
	mMassXi	= 1.67245;   //mass of Omega or AntiOmega
	
	mGeantIDXi  = (mXiType == kOmega)? 24:32;
	mGeantIDBachelor = (mXiType == kOmega)? 12:11;
	mGeantIDV0  = (mXiType == kOmega)? 18:26;
	mGeantID1  = (mXiType == kOmega)? 14:15;
	mGeantID2  = (mXiType == kOmega)? 9:8;

	if(mXiType == kOmega) { 
	   mCharge1	= 1;
	   mCharge2	= -1;
	   mChargeBachelor = -1;
	   mChargeXi = -1;
	}
	else {
	   mCharge1	= -1;
	   mCharge2	= 1;
	   mChargeBachelor = 1;
	   mChargeXi = 1;
	}
	//do not setup the cut values here. those are parameters

      //parameters for StDcaService.cxx
      kShiftConnect = 0.3;
      kShiftContain = 0.3;
   }

   return;
}

void StMcXiMaker::initParam(){
   //setup the cut values here. do not hard-code them in ::Make()

   cutNHitsGr = 15;
   cutPtGrEq = 0.15;

   cutAbsNSigma1Le = 4.;
   cutDca1GrEq  = 0;

   cutV0DcaGrEq = 0;
   cutV0MassWidthLeEq = 0.2;

   cutDca1to2LeEq = 3.0;
   cutXiMassWidthLeEq = 0.05;
   //cutDauPtArmLeEq = 0.3;
   //cutAbsDausPtShoulderDiffLeEq = 1.2;
   //cutDau1DecAngGr = 0.;
   cutXirdotpGr  = 0;
   cutXircrosspLeEq = 0.9;
   //cutDcaXiLe    = 3.3;
   cutXiDecLenGrEq = 0;

   return;
}

void StMcXiMaker::initHisto()
{
   // Create Histograms
   // there is no better way to set QA histograms. do not use histogram arrays or vectors.
   // it is not useful. there are no need to operate all the histograms at the same time.

   const Int_t    nbins    =  100   ;

   hVertexZ  = new TH1F( "Vertex", "Event Vertex Z Position", nbins, -25.0, 25.0 ) ; 
   hPt  = new TH1F( "Pt", "Transverse Momentum for all particles", nbins, 0.0, 10.0 ) ;
   hNPrimVertex  = new TH1F( "PrimVertex", "Number of Primary Vertex", 10, 0.0, 10.0 ) ;
   hNRefMult  = new TH1F( "RefMult", "Reference Multiplicity", 200, 0.0, 1000.0 ) ;
   hPtDiff  = new TH1F( "ptdiff", "Reference Multiplicity", 100, 0.0, 0.02 ) ;
   hOrDiff  = new TH1F( "ordiff", "Reference Multiplicity", 100, 0.0, 0.2 ) ;
   hPDiff  = new TH1F( "pdiff", "Reference Multiplicity", 100, 0.0, 0.4 ) ;
   hDcaDiff  = new TH1F( "dcadiff", "Reference Multiplicity", 100, -0.4, 0.4 ) ;
   hInvMass  = new TH1F( "invmass", "Reference Multiplicity", 100, mMassXi-0.08, mMassXi+0.08 ) ;

   return;
}

void StMcXiMaker::initTree()
{
   //initialize the TTree for StXiDst
   mXiTree = new TTree("McXiPicoDst","McXiPicoDst from StMcXiMaker");

   mXiTree->SetDirectory(xitree_output);

   mXiTree->Branch("runnumber",&mXiDst.runnumber,"runnumber/I");
   mXiTree->Branch("evtnumber",&mXiDst.evtnumber,"evtnumber/I");
   mXiTree->Branch("nrefmult",&mXiDst.nrefmult,"nrefmult/I");
   mXiTree->Branch("nrefmultftpceast",&mXiDst.nrefmultftpceast,"nrefmultftpceast/I");
   mXiTree->Branch("primvertexX",&mXiDst.primvertexX,"primvertexX/F");
   mXiTree->Branch("primvertexY",&mXiDst.primvertexY,"primvertexY/F");
   mXiTree->Branch("primvertexZ",&mXiDst.primvertexZ,"primvertexZ/F");   
   mXiTree->Branch("magn",&mXiDst.magn,"magn/F");
   mXiTree->Branch("nmcxi",&mXiDst.nmcxi,"nmcxi/I");
   mXiTree->Branch("nxi",&mXiDst.nxi,"nxi/I");

   mXiTree->Branch("mcxipt",mXiDst.mcxipt,"mcxipt[nmcxi]/F");
   mXiTree->Branch("mcxipx",mXiDst.mcxipx,"mcxipx[nmcxi]/F");
   mXiTree->Branch("mcxipy",mXiDst.mcxipy,"mcxipy[nmcxi]/F");
   mXiTree->Branch("mcxipz",mXiDst.mcxipz,"mcxipz[nmcxi]/F");
   mXiTree->Branch("mcxirapidity",mXiDst.mcxirapidity,"mcxirapidity[nmcxi]/F");
   mXiTree->Branch("mcxiid",mXiDst.mcxiid,"mcxiid[nmcxi]/I");
   mXiTree->Branch("mcxix",mXiDst.mcxix,"mcxix[nmcxi]/F");
   mXiTree->Branch("mcxiy",mXiDst.mcxiy,"mcxiy[nmcxi]/F");
   mXiTree->Branch("mcxiz",mXiDst.mcxiz,"mcxiz[nmcxi]/F");
   
   mXiTree->Branch("v0mass",mXiDst.v0mass,"v0mass[nxi]/F");
   mXiTree->Branch("v0pt",mXiDst.v0pt,"v0pt[nxi]/F");
   mXiTree->Branch("v0rapidity",mXiDst.v0rapidity,"v0rapidity[nxi]/F");
   mXiTree->Branch("v0eta",mXiDst.v0eta,"v0eta[nxi]/F");
   mXiTree->Branch("v0x",mXiDst.v0x,"v0x[nxi]/F");
   mXiTree->Branch("v0y",mXiDst.v0y,"v0y[nxi]/F");
   mXiTree->Branch("v0z",mXiDst.v0z,"v0z[nxi]/F");
   mXiTree->Branch("v0px",mXiDst.v0px,"v0px[nxi]/F");
   mXiTree->Branch("v0py",mXiDst.v0py,"v0py[nxi]/F");
   mXiTree->Branch("v0pz",mXiDst.v0pz,"v0pz[nxi]/F");
   mXiTree->Branch("v0declen",mXiDst.v0declen,"v0declen[nxi]/F");
   mXiTree->Branch("v0dca",mXiDst.v0dca,"v0dca[nxi]/F");
   mXiTree->Branch("v0dca2d",mXiDst.v0dca2d,"v0dca2d[nxi]/F");

   mXiTree->Branch("dau1id",mXiDst.dau1id,"dau1id[nxi]/I");
   mXiTree->Branch("dau1dca",mXiDst.dau1dca,"dau1dca[nxi]/F");
   mXiTree->Branch("dau1dca2d",mXiDst.dau1dca2d,"dau1dca2d[nxi]/F");
   mXiTree->Branch("dau1nhits",mXiDst.dau1nhits,"dau1nhits[nxi]/I");
   mXiTree->Branch("dau1dedx",mXiDst.dau1dedx,"dau1dedx[nxi]/F");
   mXiTree->Branch("dau1nsigma",mXiDst.dau1nsigma,"dau1nsigma[nxi]/F");
   mXiTree->Branch("dau1pt",mXiDst.dau1pt,"dau1pt[nxi]/F");
   mXiTree->Branch("dau1px",mXiDst.dau1px,"dau1px[nxi]/F");
   mXiTree->Branch("dau1py",mXiDst.dau1py,"dau1py[nxi]/F");
   mXiTree->Branch("dau1pz",mXiDst.dau1pz,"dau1pz[nxi]/F");
   mXiTree->Branch("dau1tpc",mXiDst.dau1tpc,"dau1tpc[nxi]/I");
   mXiTree->Branch("dau1ssd",mXiDst.dau1ssd,"dau1ssd[nxi]/I");
   mXiTree->Branch("dau1svt",mXiDst.dau1svt,"dau1svt[nxi]/I");
 
   mXiTree->Branch("dau2id",mXiDst.dau2id,"dau2id[nxi]/I");
   mXiTree->Branch("dau2dca",mXiDst.dau2dca,"dau2dca[nxi]/F");
   mXiTree->Branch("dau2dca2d",mXiDst.dau2dca2d,"dau2dca2d[nxi]/F");
   mXiTree->Branch("dau2nhits",mXiDst.dau2nhits,"dau2nhits[nxi]/I");
   mXiTree->Branch("dau2dedx",mXiDst.dau2dedx,"dau2dedx[nxi]/F");
   mXiTree->Branch("dau2nsigma",mXiDst.dau2nsigma,"dau2nsigma[nxi]/F");
   mXiTree->Branch("dau2pt",mXiDst.dau2pt,"dau2pt[nxi]/F");
   mXiTree->Branch("dau2px",mXiDst.dau2px,"dau2px[nxi]/F");
   mXiTree->Branch("dau2py",mXiDst.dau2py,"dau2py[nxi]/F");
   mXiTree->Branch("dau2pz",mXiDst.dau2pz,"dau2pz[nxi]/F");
   mXiTree->Branch("dau2tpc",mXiDst.dau2tpc,"dau2tpc[nxi]/I");
   mXiTree->Branch("dau2ssd",mXiDst.dau2ssd,"dau2ssd[nxi]/I");
   mXiTree->Branch("dau2svt",mXiDst.dau2svt,"dau2svt[nxi]/I");

   mXiTree->Branch("dca1to2",mXiDst.dca1to2,"dca1to2[nxi]/F");

   mXiTree->Branch("bachid",mXiDst.bachid,"bachid[nxi]/I");
   mXiTree->Branch("bachdca",mXiDst.bachdca,"bachdca[nxi]/F");
   mXiTree->Branch("bachdca2d",mXiDst.bachdca2d,"bachdca2d[nxi]/F");
   mXiTree->Branch("bachnhits",mXiDst.bachnhits,"bachnhits[nxi]/I");
   mXiTree->Branch("bachdedx",mXiDst.bachdedx,"bachdedx[nxi]/F");
   mXiTree->Branch("bachnsigma",mXiDst.bachnsigma,"bachnsigma[nxi]/F");
   mXiTree->Branch("bachpt",mXiDst.bachpt,"bachpt[nxi]/F");
   mXiTree->Branch("bachpx",mXiDst.bachpx,"bachpx[nxi]/F");
   mXiTree->Branch("bachpy",mXiDst.bachpy,"bachpy[nxi]/F");
   mXiTree->Branch("bachpz",mXiDst.bachpz,"bachpz[nxi]/F");
   mXiTree->Branch("bachtpc",mXiDst.bachtpc,"bachtpc[nxi]/I");
   mXiTree->Branch("bachssd",mXiDst.bachssd,"bachssd[nxi]/I");
   mXiTree->Branch("bachsvt",mXiDst.bachsvt,"bachsvt[nxi]/I");
   
   mXiTree->Branch("dcav0tobach",mXiDst.dcav0tobach,"dcav0tobach[nxi]/F");
   
   mXiTree->Branch("ximcid",mXiDst.ximcid,"ximcid[nxi]/I");
   mXiTree->Branch("ximass",mXiDst.ximass,"ximass[nxi]/F");
   mXiTree->Branch("xipt",mXiDst.xipt,"xipt[nxi]/F");
   mXiTree->Branch("xirapidity",mXiDst.xirapidity,"xirapidity[nxi]/F");
   mXiTree->Branch("xieta",mXiDst.xieta,"xieta[nxi]/F");
   mXiTree->Branch("xix",mXiDst.xix,"xix[nxi]/F");
   mXiTree->Branch("xiy",mXiDst.xiy,"xiy[nxi]/F");
   mXiTree->Branch("xiz",mXiDst.xiz,"xiz[nxi]/F");
   mXiTree->Branch("xipx",mXiDst.xipx,"xipx[nxi]/F");
   mXiTree->Branch("xipy",mXiDst.xipy,"xipy[nxi]/F");
   mXiTree->Branch("xipz",mXiDst.xipz,"xipz[nxi]/F");
   mXiTree->Branch("xideclen",mXiDst.xideclen,"xideclen[nxi]/F");
   mXiTree->Branch("xidca",mXiDst.xidca,"xidca[nxi]/F");
   mXiTree->Branch("xidca2d",mXiDst.xidca2d,"xidca2d[nxi]/F");
   mXiTree->Branch("xisinth",mXiDst.xisinth,"xisinth[nxi]/F");

   return;
}

Int_t StMcXiMaker::Init( )
{
  // setup the constants according to mXiType
  initConst();

  // initialize parameters (cuts)
  initParam();
  
  // Create Histogram output file
  if(mHistogramOutputFileName == "") { 
     //CAUTION: ALWAYS USE { } HERE!!! LOG_XXX is a if()xxx macro!!!
     LOG_ERROR << "StMcXiMaker: Please specify the histrogram output file" <<endm;
     exit(-1);
  }
  else {
     histogram_output = new TFile( mHistogramOutputFileName, "recreate" ) ;  
  }
  // Book histograms
  initHisto();
  
  // Create Xi Tree output file
  if(mXiTreeOutputFileName == "") {
     LOG_WARN << "StMcXiMaker: The Xi tree output file is not specified! output is smeared!" <<endm;
  }
  else {
     xitree_output = new TFile( mXiTreeOutputFileName, "recreate" ) ;
     // Create Xi Tree
     initTree();
  }

  // Clear daughter vectors.
  mDauDcaVec1.clear();
  mDauDcaVec2.clear();
  mDauVerVec1.clear();
  mDauVerVec2.clear();
  mDauMcXiKeyVec1.clear();
  mDauMcXiKeyVec2.clear();
  mDauVec1.clear();
  mDauVec2.clear();
  //mXiVec.clear();
  return kStOK ; 
}

Int_t StMcXiMaker::Make( )
{ // Do each event
   
  if(GetDebug()) LOG_QA<<"in StMcXiMaker::Make"<<endm;
  // Do some cleaning here
  mXiDst.nmcxi = 0;
  mXiDst.nxi = 0;

  //skip the event if it did not passed the event cut in StMcV0Maker
  if(!mV0Maker->passEventCut()) return kStOK;

  // Get 'event' data 
  //StMuEvent* muEvent      =  mMuDstMaker->muDst()->event() ;
  StEvent* event = (StEvent*) GetInputDS("StEvent");
  StMcEvent* mcEvent = (StMcEvent*) GetDataSet("StMcEvent");
  StAssociationMaker* assocMaker = (StAssociationMaker*) GetMaker("StAssociationMaker");

  rcTrackMapType*   theRcTrackMap = assocMaker->rcTrackMap();
  StPrimaryVertex* primaryVertex = event->primaryVertex();

  // Get 'v0' data 
  const StMcV0Dst& v0Dst    =  mV0Maker->getV0Dst();

  //if no MC v0 candidates were found in StMcV0Maker, do not search Xi.(not necessary)
  //if(v0Dst.nmcv0==0) return kStOK;
  
  /* skip the event cut in StMcXiMaker, do it in StMcV0Maker
  hNPrimVertex -> Fill( mMuDstMaker->muDst()->numberOfPrimaryVertices());

  // Cut on the number of vertices in the event.  On old tapes, no-vertex gets reported as VtxPosition=(0,0,0).
  // Skip events that do not have a primary vertex. the small value '1e-5' is ok for hard coding.
  if ( fabs(muEvent->primaryVertexPosition().x()) < 1e-5 && fabs(muEvent->primaryVertexPosition().y()) < 1e-5 && fabs(muEvent->primaryVertexPosition().z()) < 1e-5 )  return kStOK ;  
  //if(GetDebug()) LOG_QA<<"in StMcXiMaker::Make : has pv"<<endm;

  // further selection on events. find qualified events!!
  // triggerid, skip events that do not comply with the required trigger id.
  if ( !muEvent->triggerIdCollection().nominal().isTrigger(cutTriggerIdEq) ) return kStOK ; 

  // cut on vertexZ
  if (fabs(muEvent->primaryVertexPosition().z()) > cutAbsVertexZLeEq ) return kStOK ;
  // possible duplicate events.
  if ( mMuDstMaker->muDst()->numberOfPrimaryTracks() == mTestNTrack && mEventsProcessed !=0 && mTestVZ !=0 &&  muEvent->primaryVertexPosition().z() == mTestVZ ) {
     LOG_WARN << mEventsProcessed <<" "<<"seems a duplicated event!"<<endm;
     return kStOK ;
  }
  mTestVZ = muEvent->primaryVertexPosition().z();
  mTestNTrack = mMuDstMaker->muDst()->numberOfPrimaryTracks();

   // cut on centrality or reference multiplicity.
  if ( muEvent->refMult() ) {}   //TODO: need to check whether this is the same as in old code. the old code might ignore the case of pile-up.

 
  // Fill some QA plots
  hVertexZ -> Fill( muEvent->primaryVertexPosition().z() ) ; // Make histogram of the vertex Z distribution
  hNRefMult -> Fill( muEvent->refMult() );		
  */

  mXiDst.runnumber = v0Dst.runnumber;
  mXiDst.evtnumber = v0Dst.evtnumber;
  mXiDst.nrefmult = v0Dst.nrefmult;
  mXiDst.nrefmultftpceast = v0Dst.nrefmultftpceast;
  mXiDst.primvertexX = v0Dst.primvertexX;
  mXiDst.primvertexY = v0Dst.primvertexY;
  mXiDst.primvertexZ = v0Dst.primvertexZ;
  // Do 'event' analysis based on event data

  // Record the MC track information here
  StSPtrVecMcTrack& McTrack = mcEvent->tracks();
  int Mctrackcounter = 0;
  for(unsigned int nMc=0; nMc < McTrack.size(); nMc++){
     if(McTrack[nMc]->geantId()!=mGeantIDXi)continue;
     /*
     if(!McTrack[nMc]->stopVertex())cout<<"Haha Xi no stpvtx "<<McTrack[nMc]->pt()<<endl;
     if(!McTrack[nMc]->stopVertex())continue;
     StMcVertex * stpvtx = McTrack[nMc]->stopVertex();
     //cout<<stpvtx->numberOfDaughters()<<endl;
     cout<<"xi: "<<McTrack[nMc]->key()<<endl;
     const StPtrVecMcTrack& daughters = stpvtx->daughters();
     for(unsigned int ndau = 0;ndau<daughters.size();ndau++)
	  if(daughters[ndau]->geantId() == mGeantIDV0){
	     if(!daughters[ndau]->stopVertex())cout<<"Haha v0 no stpvtx "<<daughters[ndau]->pt()<<endl;
	     if(!daughters[ndau]->stopVertex())continue;
	     StMcVertex *v0stpvtx = daughters[ndau]->stopVertex();
	     cout<<"v0: "<<daughters[ndau]->key()<<endl;
	     const StPtrVecMcTrack& v0daughters = v0stpvtx->daughters();
	     cout<<"v0 daughters: "<<v0daughters.size()<<endl;
	     for(unsigned int nv0dau = 0;nv0dau<v0daughters.size();nv0dau++)
		  cout<<"v0dau: "<<v0daughters[nv0dau]->geantId()<<" "<<v0daughters[nv0dau]->key()<<endl;
	  }
     //cout<<"dau: "<<daughters[ndau]->geantId()<<endl;
     */
     mXiDst.mcxipt[Mctrackcounter] = McTrack[nMc]->pt();
     mXiDst.mcxipx[Mctrackcounter] = McTrack[nMc]->momentum().x();
     mXiDst.mcxipy[Mctrackcounter] = McTrack[nMc]->momentum().y();
     mXiDst.mcxipz[Mctrackcounter] = McTrack[nMc]->momentum().z();
     mXiDst.mcxirapidity[Mctrackcounter] = McTrack[nMc]->rapidity();
     mXiDst.mcxiid[Mctrackcounter] = McTrack[nMc]->key();
     //CAUTION: some xi (or v0) mc tracks do not have stopVertex for some reason!
     mXiDst.mcxix[Mctrackcounter] = McTrack[nMc]->startVertex()->position().x();
     mXiDst.mcxiy[Mctrackcounter] = McTrack[nMc]->startVertex()->position().y();
     mXiDst.mcxiz[Mctrackcounter] = McTrack[nMc]->startVertex()->position().z();
     Mctrackcounter++;
  }
  mXiDst.nmcxi = Mctrackcounter;

  // Record some information...
  Double_t magn = event->runInfo()->magneticField();
  mXiDst.magn = magn;
  
  // Select Bachelor tracks ...
  // Get 'track' data, make cuts on tracks, do physics analysis, histogram results.
  StSPtrVecTrackNode& rcTrackNodes = event->trackNodes();
  unsigned int Loopsize =  rcTrackNodes.size();

  for(unsigned int i = 0; i < Loopsize; i++){             // Main loop for Iterating over tracks

     StTrackNode* trkNode = rcTrackNodes[i];
     StGlobalTrack* rcTrack = dynamic_cast<StGlobalTrack*>(trkNode->track(global));
     
     int MC=0;
     int McXiKey=0;
     if(theRcTrackMap){
	  pair<rcTrackMapIter,rcTrackMapIter> rcTrackBounds = theRcTrackMap->equal_range(rcTrack);
	  for (rcTrackMapIter rctrkIter1 =  rcTrackBounds.first; rctrkIter1!= rcTrackBounds.second; rctrkIter1++){
	     StMcTrack *mcSave=(StMcTrack *)(*rctrkIter1).second->partnerMcTrack();
	     if(!mcSave)continue;
	     StMcVertex *ver=mcSave->startVertex();
	     if(!ver||ver->key()==1)continue;
	     StMcTrack *Tr=(StMcTrack *)ver->parent();
	     if(!Tr)continue;
	     if(Tr->geantId()==mGeantIDXi) {
		  MC=ver->key()+1;
		  McXiKey=Tr->key();
	     }
	  }
     }
     if(MC==0)continue; //this rc track if not the bachelor mc track.

     hPt -> Fill( rcTrack->geometry()->momentum().perp() ) ; //at dca to PV, for a global rcTrack, this value is useless. anyway, the pt value is supposed to be the same anywhere.
     //double eta = rcTrack->eta();		  //at dca to PV
     //double phi = rcTrack->phi();		  //at dca to PV
     short flag = rcTrack->flag();
     //unsigned short nHits = rcTrack->detectorInfo()->numberOfPoints(kTpcId);
     unsigned short nHits = rcTrack->detectorInfo()->numberOfPoints();
     unsigned short nHitsFit = rcTrack->fitTraits().numberOfFitPoints(kTpcId);
     short charge = rcTrack->geometry()->charge();
     //StThreeVectorF p = rcTrack->p();	  //at dca to PV
     //StThreeVectorF origin = rcTrack->firstPoint();  //? NOT SURE whether firstPoint from detectorinfo is the same as helix.orgin()!!!
     //double nsigmapion = rcTrack->nSigmaPion();
     //double nsigmaproton = rcTrack->nSigmaProton();
     //double nsigmakaon = rcTrack->nSigmaKaon();
     //double dedx = rcTrack->dEdx();

     StPhysicalHelixD helix = rcTrack->geometry()->helix(); //inner helix. good for dca to PV.
     StThreeVectorD p = helix.momentum(magn*kilogauss);    //momentum at origin
     StThreeVectorD origin = helix.origin();  //origin of helix
     double pt = p.perp();

     //some checks.
     hPtDiff -> Fill( rcTrack->geometry()->momentum().perp() - p.perp() ) ; 
     hOrDiff -> Fill( (rcTrack->detectorInfo()->firstPoint() - origin).mag() ) ; 
     hPDiff -> Fill( fabs(rcTrack->geometry()->momentum().mag() - p.mag()) ) ; 
     //comments: there are difference between the values above. But they seem to be acceptably small!

     /*
     int hrot;		//helicity of helix, sign of -charge*magn
     if (-charge*magn > 0) hrot = 1;
     else hrot = -1;

     double nsigma;
     if(mXiType == kXi || mXiType == kAntiXi) nsigma = nsigmapion;
     else nsigma = nsigmakaon;
     */

     //if(rcTrack->vertexIndex()!=StMuDst::currentVertexIndex())continue;
     //if you want to use rcTrack->dca(), turn this on. if it is not turned on, that function crashes.
     //OK. let's cut rcTracks
     if(nHitsFit<=cutNHitsGr)continue;
     if(flag <=0  )continue; //or <=0 ?
     //if(rcTrack->bad() )continue; 
     if(abs(charge)!=1) continue;
     if(pt<cutPtGrEq)continue; //should be larger. like 0.15 or 0.2

     if(charge == mChargeBachelor){
	  //record the bachelor 
	  //fill the vector
	  StPhysicalHelixD helix = rcTrack->geometry()->helix(); //inner helix. good for dca to PV.

	  StThreeVectorF pv = primaryVertex->position();
	  /*
	  //rotate transverse coordinates and momenta for background estimation
	  if(mRotate){
	     StThreeVectorD p1 = helix.momentum(magn*kilogauss);    //momentum at origin
	     StThreeVectorD x1 = helix.origin();    //origin
	     p1.setX(-p1.x());
	     p1.setY(-p1.y());
	     x1.setX(-(x1.x()-pv.x())+pv.x());
	     x1.setY(-(x1.y()-pv.y())+pv.y());
	     StPhysicalHelixD helixtmp(p1, x1, magn*kilogauss, rcTrack->charge());
	     helix = helixtmp;
	  }
	  */
	  
	  double pathlength = helix.pathLength(primaryVertex->position(), false); // do scan periods. NOTE: the default is false. this is not necessary for rcTracks with pt>0.15GeV/c
	  StThreeVectorF dca = helix.at(pathlength)-primaryVertex->position();
	  
	  //some tests on dca functions.
	  //StThreeVectorF dca1 = rcTrack->dca(muEvent->primaryVertexPosition()); //it simply crash!
	  //StThreeVectorF dca11 = rcTrack->dcaGlobal(); //it doesn't crash, but give some zero values
	  //hDcaDiff -> Fill((dca1-dca11).mag());
	  //LOG_QA<<"KK "<<(dca1-dca11).mag()<<" "<<dca1.mag()<<endm;
	  
	  //if(getDcaToPV(rcTrack, muEvent->primaryVertexPosition()) - dca.mag() > 0.4)
	  //LOG_QA << getDcaToPV(rcTrack, muEvent->primaryVertexPosition()) << " "<< dca.mag()<<endm;
	  //if(getDcaToPV(rcTrack, muEvent->primaryVertexPosition()) < 2)
	  //hDcaDiff -> Fill(getDcaToPV(rcTrack, muEvent->primaryVertexPosition())-dca.mag());
	  
	  //double dca = getDcaToPV(rcTrack, muEvent->primaryVertexPosition());
	  
	  //CONCLUSION: StHelix method seems more strict, always use it instead of getDcaToPV!!!
	  if(dca.mag()<cutDca1GrEq)continue;
	  mDauDcaVec1.push_back(dca.mag());
	  mDauVerVec1.push_back(MC);
	  mDauMcXiKeyVec1.push_back(McXiKey);
	  mDauVec1.push_back(rcTrack);
     }
  }

  assert(mDauVec1.size() == mDauDcaVec1.size());
  
  //reconstruct Xi
  int nXi = 0;
  
  for(int i=0;i<v0Dst.nv0;i++){
     //get v0 info here
     //cut them before in StMcV0Maker

     if(v0Dst.v0dca[i]<cutV0DcaGrEq)continue;
     if(fabs(v0Dst.v0mass[i]-mMassV0)>cutV0MassWidthLeEq)continue;
     
     StThreeVectorF xv0(v0Dst.v0x[i],v0Dst.v0y[i],v0Dst.v0z[i]), 
			  pv0(v0Dst.v0px[i],v0Dst.v0py[i],v0Dst.v0pz[i]);

     
     for(unsigned int j=0;j<mDauVec1.size();j++){
	  //get pion track info here
	  StGlobalTrack * track1 = mDauVec1[j];
	  if(track1->key() == v0Dst.dau2id[i])continue;  
	  StPhysicalHelixD helix1 = track1->geometry()->helix(); //inner helix. good for dca to PV.

	  if(v0Dst.v0mcstverid[i] != mDauVerVec1[j])continue; //not from the same mc xi
	  
	  StThreeVectorF pv = primaryVertex->position();

	  /*
	  if(mRotate){
	     StThreeVectorD tp1 = helix1.momentum(magn*kilogauss);    //momentum at origin
	     StThreeVectorD tx1 = helix1.origin();    //origin
	     tp1.setX(-tp1.x());
	     tp1.setY(-tp1.y());
	     tx1.setX(-(tx1.x()-pv.x())+pv.x());
	     tx1.setY(-(tx1.y()-pv.y())+pv.y());
	     StPhysicalHelixD helixtmp(tp1, tx1, magn*kilogauss, track1->charge());
	     helix1 = helixtmp;
	  }
	  */
	
	  double dca1 = mDauDcaVec1[j];
	  StThreeVectorD p1 = helix1.momentum(magn*kilogauss);    //momentum at origin
	  //cut them before in track selection
	  
	  StThreeVectorF xxi, op1;
	  
	  //StHelix dca algorithm is FAST in case of helix to staight line. 
	  //We will use it as default algorithm, 
	  //if you want to try out Long's method, switch on 
	  //mDcaAlgoLong in the constructor.
	  double dca1tov0;
	  if(!mDcaAlgoLong){
	     //v0 is a straight line. to simulate it, we boost its transverse momentum to 100GeV/c.
	     //about 700 meters transverse radii. 
	     //ALWAYS call helix1's pathLengths method, treat helixv0 as a parameter.
	     //Otherwise, bachelor helix1's periods will be scaned, the background will be HUGE!
	     StPhysicalHelixD helixv0(pv0/pv0.perp()*100, xv0, magn*kilogauss, 1);
	     //LOG_QA << 1.0/helixv0.curvature()<<endm;
	     pair<double,double> tmps = helix1.pathLengths(helixv0); 
	     StThreeVectorD ox1 = helix1.at(tmps.first);
	     StThreeVectorD ox2 = helixv0.at(tmps.second);
	     
	     dca1tov0 = (ox1-ox2).mag();
	     xxi = (ox1+ox2)/2.;
	     op1 = helix1.momentumAt(tmps.first, magn*kilogauss);
	  }

	  //LONG's two helices dca method, should also work. also need to construct v0 helix first.
	  //this method work well for smaller v0 pt (less than 100GeV). but has some difference 
	  //in some rare cases. should not be used for serious Xi analysis. 
	  //the following lines are for debug only!
	  if(mDcaAlgoLong){
	     StThreeVectorF op2;
	     StPhysicalHelixD helixv0(pv0/pv0.perp()*100, xv0, magn*kilogauss, 1);
	     dca1tov0 = closestDistance(helix1, helixv0, magn, pv, xxi, op1, op2);
	  }
	  
	  //if(mDcaAlgoLong) dca1tov0 = closestDistance(xv0, pv0, helix1, magn, xxi, op1);
	  //cut on dca1to2
	  if(dca1tov0 > cutDca1to2LeEq) continue;
	  //LOG_QA<<stdca1tov0<<" "<<longdca1tov0<<" "<<dca1tov0<<endm;
	  
	  double oe1 = sqrt(op1.mag2() + mMassBachelor*mMassBachelor);
	  double oev0 = sqrt(pv0.mag2() + mMassV0*mMassV0);
	  double ximass = sqrt(mMassBachelor*mMassBachelor + mMassV0*mMassV0 + 2.*oe1*oev0 - 2.*op1.dot(pv0));
	  //cut on ximass
	  if(fabs(ximass-mMassXi) > cutXiMassWidthLeEq)continue;

	  //StThreeVectorD xv0 = (ox1 + ox2)/2.;
	  StThreeVectorD pxi = op1 + pv0;
	  StThreeVectorD xxitoPV = xxi - pv;

	  /*
	  //pthead, ptarm cut
	  double pthead1 = op1.dot(pv0)/pv0.mag();
	  double pthead2 = op2.dot(pv0)/pv0.mag();
	  double ptarm   = sqrt(op1.mag2()-pthead1*pthead1);
	  if(mXiType!=kKs){
	     if(ptarm> cutDauPtArmLeEq || fabs((pthead1-pthead2)/(pthead1+pthead2))> cutAbsDausPtShoulderDiffLeEq )continue;
	  }
	  */

	  //forward decay cut
	  double ang1 = (pxi.x()*xxitoPV.x() + pxi.y()*xxitoPV.y())/pxi.perp()/xxitoPV.perp();
	  //if(ang1<=cutDau1DecAngGr)continue;
	  
	  //r dot p for xi. cut on it. should be larger than 0. 
	  double rdotp = xxitoPV.dot(pxi);
	  if(rdotp/xxitoPV.mag()/pxi.mag() <= cutXirdotpGr)continue;
	  if(pv0.dot(xv0-xxi) <= cutXirdotpGr)continue;
	  
	  //calculate xi to PV dca. cut on dca
	  StPhysicalHelixD helixxi(pxi, xxi, magn*kilogauss, mChargeXi);
	  double pathlength = helixxi.pathLength(pv, false); // do scan periods. NOTE: the default is false. this is not necessary for tracks with pt>0.15GeV/c
	  StThreeVectorF dca = helixxi.at(pathlength)-pv;
	  double dcaxitoPV = dca.mag();
	  //if(dcaxitoPV>=cutDcaXiLe)continue;

	  //cut on decay length
	  double xidecaylength = xxitoPV.mag();
	  //if(xidecaylength > v0Dst.v0declen[i] )continue;
	  if(xidecaylength < cutXiDecLenGrEq) continue;

	  //cut on sinth, or theta
	  double sinth = (xxitoPV.cross(pxi)).mag()/xxitoPV.mag()/pxi.mag();
	  double theta = atan2(sinth, rdotp/xxitoPV.mag()/pxi.mag()); //theta range: from 0 to pi
	  if(sinth > cutXircrosspLeEq) continue;

	  //save the xi information (including cut variables) into StXiDst. 
	  mXiDst.v0mass[nXi] = v0Dst.v0mass[i];
	  mXiDst.v0pt[nXi]   = v0Dst.v0pt[i];
	  mXiDst.v0rapidity[nXi]    = v0Dst.v0rapidity[i];
	  mXiDst.v0eta[nXi]    = v0Dst.v0eta[i];
	  mXiDst.v0x[nXi]    = v0Dst.v0x[i];
	  mXiDst.v0y[nXi]    = v0Dst.v0y[i];
	  mXiDst.v0z[nXi]    = v0Dst.v0z[i];
	  mXiDst.v0px[nXi]    = v0Dst.v0px[i];
	  mXiDst.v0py[nXi]    = v0Dst.v0py[i];
	  mXiDst.v0pz[nXi]    = v0Dst.v0pz[i];
	  mXiDst.v0declen[nXi] = v0Dst.v0declen[i];
	  mXiDst.v0dca[nXi]    = v0Dst.v0dca[i];
	  mXiDst.v0dca2d[nXi]    = v0Dst.v0dca2d[i];

	  mXiDst.dau1id[nXi]    = v0Dst.dau1id[i];
	  mXiDst.dau1dca[nXi]    = v0Dst.dau1dca[i];
	  mXiDst.dau1dca2d[nXi]    = v0Dst.dau1dca2d[i];
	  mXiDst.dau1nhits[nXi]    = v0Dst.dau1nhits[i];
	  mXiDst.dau1dedx[nXi]    = v0Dst.dau1dedx[i];
	  mXiDst.dau1nsigma[nXi]    = v0Dst.dau1nsigma[i];
	  mXiDst.dau1pt[nXi]    = v0Dst.dau1pt[i];
	  mXiDst.dau1px[nXi]    = v0Dst.dau1px[i];
	  mXiDst.dau1py[nXi]    = v0Dst.dau1py[i];
	  mXiDst.dau1pz[nXi]    = v0Dst.dau1pz[i];
	  mXiDst.dau1tpc[nXi]    = v0Dst.dau1tpc[i];
	  mXiDst.dau1ssd[nXi]    = v0Dst.dau1ssd[i];
	  mXiDst.dau1svt[nXi]    = v0Dst.dau1svt[i];

	  mXiDst.dau2id[nXi]    = v0Dst.dau2id[i];
	  mXiDst.dau2dca[nXi]    = v0Dst.dau2dca[i];
	  mXiDst.dau2dca2d[nXi]    = v0Dst.dau2dca2d[i];
	  mXiDst.dau2nhits[nXi]    = v0Dst.dau2nhits[i];
	  mXiDst.dau2dedx[nXi]    = v0Dst.dau2dedx[i];
	  mXiDst.dau2nsigma[nXi]    = v0Dst.dau2nsigma[i];
	  mXiDst.dau2pt[nXi]    = v0Dst.dau2pt[i];
	  mXiDst.dau2px[nXi]    = v0Dst.dau2px[i];
	  mXiDst.dau2py[nXi]    = v0Dst.dau2py[i];
	  mXiDst.dau2pz[nXi]    = v0Dst.dau2pz[i];
	  mXiDst.dau2tpc[nXi]    = v0Dst.dau2tpc[i];
	  mXiDst.dau2ssd[nXi]    = v0Dst.dau2ssd[i];
	  mXiDst.dau2svt[nXi]    = v0Dst.dau2svt[i];
	  
	  mXiDst.dca1to2[nXi]  = v0Dst.dca1to2[i];

	  mXiDst.bachid[nXi] = track1->key();
        mXiDst.bachdca[nXi]  = dca1;
	  mXiDst.bachdca2d[nXi]  = helix1.geometricSignedDistance(pv.x(),pv.y());
        //mXiDst.bachnhits[nXi]= track1->detectorInfo()->numberOfPoints(kTpcId);
        mXiDst.bachnhits[nXi]= track1->detectorInfo()->numberOfPoints();
        mXiDst.bachdedx[nXi] = 0;
        //mXiDst.bachnsigma[nXi] = (mXiType==kXi||mXiType==kAntiXi)? track1->nSigmaPion() : track1->nSigmaProton();
	  mXiDst.bachnsigma[nXi] = 0;
        mXiDst.bachpt[nXi]   = op1.perp();
        mXiDst.bachpx[nXi]   = op1.x();
        mXiDst.bachpy[nXi]   = op1.y();
        mXiDst.bachpz[nXi]   = op1.z();
	  mXiDst.bachtpc[nXi]  = track1->fitTraits().numberOfFitPoints(kTpcId);
        mXiDst.bachssd[nXi]  = track1->fitTraits().numberOfFitPoints(kSsdId);
        mXiDst.bachsvt[nXi]  = track1->fitTraits().numberOfFitPoints(kSvtId);
	  
	  mXiDst.dcav0tobach[nXi] = dca1tov0;
	  //mXiDst.stdcav0tobach[nXi] = stdca1tov0;
	  
	  mXiDst.ximcid[nXi]   = mDauMcXiKeyVec1[j];
	  mXiDst.ximass[nXi]   = ximass;
        mXiDst.xidca[nXi]    = dcaxitoPV;
	  mXiDst.xidca2d[nXi]    = helixxi.geometricSignedDistance(pv.x(),pv.y());
        mXiDst.xipt[nXi]   = pxi.perp();
        mXiDst.xirapidity[nXi]    = log( (sqrt(ximass*ximass+pxi.mag2()) + pxi.z())/sqrt(ximass*ximass+pxi.perp2()));
        mXiDst.xieta[nXi]    = 0.5*log( (pxi.mag() + pxi.z())/(pxi.mag() - pxi.z()) );
        mXiDst.xix[nXi]      = xxi.x();
        mXiDst.xiy[nXi]      = xxi.y();
        mXiDst.xiz[nXi]      = xxi.z();
        mXiDst.xipx[nXi]     = pxi.x();
        mXiDst.xipy[nXi]     = pxi.y();
        mXiDst.xipz[nXi]     = pxi.z();
        mXiDst.xideclen[nXi] = xidecaylength;
	  mXiDst.xisinth[nXi]  = sinth;

	  nXi ++;

	  
	  hInvMass->Fill(ximass);
	  //mXiVec.push_back(v0data);

     }
  }

  mXiDst.nxi = nXi;

  if(GetDebug()) {
     cout<<"in StMcV0Maker::Make : has "<<v0Dst.nv0<<" V0"<<endl;
     cout<<"in StMcXiMaker::Make : has "<<nXi<<" RC Xi "<<mXiDst.nmcxi<<" MC Xi"<<endl;
     cout<<".."<<mEventsProcessed<<endl;
  }
  if(mXiDst.nmcxi > 0 && mXiTree) mXiTree->Fill();
  //dump Xi vector into a TTree, record those events with Xi candidate only.
  //refer to hSelectNRefMult generated by StMcV0Maker for total number of events
  
  mEventsProcessed++ ;
  //LOG_QA << mDauVec1.size() <<" "<< mDauVec2.size() <<" "<< mXiVec.size()<<endm;
  mDauDcaVec1.clear();
  mDauDcaVec2.clear();
  mDauVerVec1.clear();
  mDauVerVec2.clear();
  mDauMcXiKeyVec1.clear();
  mDauMcXiKeyVec2.clear();
  mDauVec1.clear();
  mDauVec2.clear();
  //mXiVec.clear();
  return kStOK ;
  
}

Int_t StMcXiMaker::Finish( )
{ // Do once at the end the analysis

  // Write histograms to disk, output miscellaneous other information
  if(histogram_output!=NULL) histogram_output -> Write() ;   // Write all histograms to disk 
  if(xitree_output!=NULL) xitree_output -> Write() ;   // Write all histograms to disk 

  cout << "Total Events Processed in StMcXiMaker " << mEventsProcessed << endl ;

  return kStOk ;  

}
