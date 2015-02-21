//Xianglei Zhu, created on Apr. 9, 2008
//topologically reconstruct V0 (Ks or Lambda) from the globaltracks in MuDst.
//
#include "StRoot/StMcV0Maker/StV0Type.h"
#include "StRoot/StMcXiMaker/StXiType.h"

void reconstruct_mcv0(TString InputFileList, Int_t Nlist, Int_t Block = 50, Int_t nEvents = 0, TString OutputDir = "output_fp/");

void reconstruct_mcv0(TString InputFileList, Int_t Nlist, Int_t Block, Int_t nEvents, TString OutputDir) 
{
   
  // Load libraries
//  gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
//  loadSharedLibraries();
  gROOT->Macro("loadMuDst.C");
  gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
  loadSharedLibraries();
  gROOT->Macro("LoadLogger.C");
  /*
  gSystem->Load("St_base");
  gSystem->Load("StChain");
  */ 
  gSystem->Load("St_Tables");
  gSystem->Load("StUtilities");
  gSystem->Load("StTreeMaker");
  gSystem->Load("StIOMaker");
  gSystem->Load("StarClassLibrary");
  gSystem->Load("StBichsel");
  gSystem->Load("StEvent");

  gSystem->Load("StEventUtilities");
  gSystem->Load("StEmcUtil");
  gSystem->Load("StEEmcUtil");
  gSystem->Load("StTofUtil");
  gSystem->Load("StPmdUtil");
  gSystem->Load("StPreEclMaker");
  gSystem->Load("StStrangeMuDstMaker");
  gSystem->Load("StMuDSTMaker");

  gSystem->Load("StMcEvent");
  gSystem->Load("StMcEventMaker");
  gSystem->Load("StAssociationMaker");
  //gSystem->Load("StMcAnalysisMaker");

  gSystem->Load("StTpcDb");
  gSystem->Load("StDetectorDbMaker");
  gSystem->Load("StEventMaker");
  gSystem->Load("StFlowMaker");
  gSystem->Load("libgen_Tables");
  gSystem->Load("libsim_Tables");
  gSystem->Load("libglobal_Tables");
  gSystem->Load("libtpc_Tables");
  
  gSystem->Load("StMcV0Maker.so") ;
  gSystem->Load("StMcXiMaker.so") ;

  TDirectory* dir = gDirectory;
  //prepare the files
  std::ifstream inlist(InputFileList);
  if(!inlist)cout<<" can't open file "<<endl;
  StFile *files = new StFile();
  char  *fileName=new char[200];
  int count=Block*Nlist;
  int Ncount=0;
  int fileGet=0;

  while(!inlist.eof()&&Ncount<count){
     inlist.getline(fileName,200,'\n');
     cout<<fileName<<endl;
     Ncount++;
  }
  if(Ncount<count)return;
  int gdfile=0;
  while(!inlist.eof() && fileGet<Block){
     inlist.getline(fileName,200,'\n');
     fileGet++;
     TFile test(fileName);
     cout<<fileName<<endl;
     if(test.GetEND()>250) {
	  files->AddFile(fileName);
	  cout<<fileGet<<" ok "<<endl;
	  gdfile++;
     } else {
	  cout<<fileGet<<" bad "<<endl;
     }
     test.Close();
  }
  inlist.close();
  cout<<"Good file = "<<gdfile<<endl;
  if(gdfile==0)return;

  int Mctrack_count=0;
  dir->cd();
  //Chain
  StChain*                    chain  =  new StChain ;
  chain->SetDebug(0);
  StIOMaker IOMaker("IO","r",files,"bfcTree");
  IOMaker.SetDebug(1);
  IOMaker.SetBranch("*",0,"0");
  IOMaker.SetBranch("geantBranch",0,"r");
  IOMaker.SetBranch("eventBranch",0,"r");
  
  //StEventMaker*       eventReader   = new StEventMaker();
  //eventReader->doPrintMemoryInfo = kFALSE;
  //eventReader->SetDebug(1);

  StMcEventMaker*     mcEventReader = new StMcEventMaker;
  StAssociationMaker* associator    = new StAssociationMaker;
  associator->useInTracker();      ///lin
  associator->SetDebug(1);
  //associator->useDistanceAssoc();  ///lin
  StMcParameterDB* parameterDB = StMcParameterDB::instance();
  // TPC
  parameterDB->setXCutTpc(.5); // 5 mm  2cm
  parameterDB->setYCutTpc(.5); // 5 mm  2cm
  parameterDB->setZCutTpc(.5); // 2 mm 2cm 05/04/2000 xzb
  parameterDB->setReqCommonHitsTpc(3); // Require 5 hits in common for

  //StMcAnalysisMaker* mcana = new StMcAnalysisMaker;

  //first pass to get the raw signal of Lambda
  StMcV0Maker* mcsig  =  new StMcV0Maker("mcv0makerfirst") ;
  TString Name(InputFileList);
  char *fileIndex=new char[200];
  sprintf(fileIndex,".%d",Nlist);
  Name.ReplaceAll(".list","");
  Name.Append(fileIndex);
  mcsig -> setHistoFileName(OutputDir+Name+".la.histo.root") ; // Name the output file for histograms
  mcsig -> setV0TreeFileName(OutputDir+Name+".la.picodst.root"); // V0 candidate tree file for further cuts. 
  //mcsig -> setV0Type(kLambda);   //set V0 type. kKs, or kLambda, kAntiLambda. once a time! 
  
//============================================================
//          Change Particle Type
//============================================================

  mcsig -> setV0Type(kAntiLambda);   //set V0 type. kKs, or kLambda, kAntiLambda. once a time! 

//============================================================
//
//============================================================

  mcsig -> SetDebug(0);
 
  if ( nEvents == 0 )  nEvents = 10000000 ;       // Take all events in nFiles if nEvents = 0

  // Loop over the links in the chain
  Int_t iInit = chain -> Init() ;
  if (iInit) chain->Fatal(iInit,"on init");
  
  // chain -> EventLoop(1,nEvents) ;  //will output lots of useless debugging info.
  Int_t istat = 0, i = 1;
  while (i <= nEvents && istat != 2) {
     cout << endl << "== Event " << i << " start ==" << endl;
     chain->Clear();
     istat = chain->Make(i);
     cout << endl << "== Event " << i << " finish =="<< endl;
     if (istat == 2)
	  cout << "Last  event processed. Status = " << istat << endl;
     if (istat == 3)
	  cout << "Error event processed. Status = " << istat << endl;
     i++;
  }

  if (nEvents > 1) chain -> Finish() ;

  // Cleanup
}
