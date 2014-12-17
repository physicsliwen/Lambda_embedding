#ifndef StMcXiDst_def
#define StMcXiDst_def

#define MAX_NUM_McXi  1000
struct StMcXiDst{ 
   //event information, copy StV0Dst
   int runnumber;
   int evtnumber;
   int nrefmult; 
   int nrefmultftpceast;
   float primvertexX;
   float primvertexY;
   float primvertexZ;
   float magn;
 
   int nmcxi;
   int nxi;

   float mcxipt[MAX_NUM_McXi];
   float mcxipx[MAX_NUM_McXi];
   float mcxipy[MAX_NUM_McXi];
   float mcxipz[MAX_NUM_McXi];
   float mcxirapidity[MAX_NUM_McXi];
   int   mcxiid[MAX_NUM_McXi];
   float mcxix[MAX_NUM_McXi];
   float mcxiy[MAX_NUM_McXi];
   float mcxiz[MAX_NUM_McXi];

   //v0 information, copy StXiDst
   float v0mass[MAX_NUM_McXi];
   float v0pt[MAX_NUM_McXi];
   float v0rapidity[MAX_NUM_McXi];
   float v0eta[MAX_NUM_McXi];
   float v0x[MAX_NUM_McXi];
   float v0y[MAX_NUM_McXi];
   float v0z[MAX_NUM_McXi];
   float v0px[MAX_NUM_McXi];
   float v0py[MAX_NUM_McXi];
   float v0pz[MAX_NUM_McXi];
   float v0declen[MAX_NUM_McXi];
   float v0dca[MAX_NUM_McXi];
   float v0dca2d[MAX_NUM_McXi];

   int   dau1id[MAX_NUM_McXi];
   float dau1dca[MAX_NUM_McXi];
   float dau1dca2d[MAX_NUM_McXi];
   int dau1nhits[MAX_NUM_McXi];
   float dau1dedx[MAX_NUM_McXi];
   float dau1nsigma[MAX_NUM_McXi];
   float dau1pt[MAX_NUM_McXi];
   float dau1px[MAX_NUM_McXi];
   float dau1py[MAX_NUM_McXi];
   float dau1pz[MAX_NUM_McXi];
   int dau1tpc[MAX_NUM_McXi];
   int dau1ssd[MAX_NUM_McXi];
   int dau1svt[MAX_NUM_McXi];

   int   dau2id[MAX_NUM_McXi];
   float dau2dca[MAX_NUM_McXi];
   float dau2dca2d[MAX_NUM_McXi];
   int dau2nhits[MAX_NUM_McXi];
   float dau2dedx[MAX_NUM_McXi];
   float dau2nsigma[MAX_NUM_McXi];
   float dau2pt[MAX_NUM_McXi];
   float dau2px[MAX_NUM_McXi];
   float dau2py[MAX_NUM_McXi];
   float dau2pz[MAX_NUM_McXi];
   int dau2tpc[MAX_NUM_McXi];
   int dau2ssd[MAX_NUM_McXi];
   int dau2svt[MAX_NUM_McXi];

   float dca1to2[MAX_NUM_McXi];

   //bachelor information
   int   bachid[MAX_NUM_McXi];
   float bachdca[MAX_NUM_McXi];
   float bachdca2d[MAX_NUM_McXi];
   int bachnhits[MAX_NUM_McXi];
   float bachdedx[MAX_NUM_McXi];
   float bachnsigma[MAX_NUM_McXi];
   float bachpt[MAX_NUM_McXi];
   float bachpx[MAX_NUM_McXi];
   float bachpy[MAX_NUM_McXi];
   float bachpz[MAX_NUM_McXi];
   int bachtpc[MAX_NUM_McXi];
   int bachssd[MAX_NUM_McXi];
   int bachsvt[MAX_NUM_McXi];
   
   float dcav0tobach[MAX_NUM_McXi];
   //float stdcav0tobach[MAX_NUM_McXi];

   //xi information
   int   ximcid[MAX_NUM_McXi];
   float ximass[MAX_NUM_McXi];
   float xipt[MAX_NUM_McXi];
   float xirapidity[MAX_NUM_McXi];
   float xieta[MAX_NUM_McXi];
   float xix[MAX_NUM_McXi];
   float xiy[MAX_NUM_McXi];
   float xiz[MAX_NUM_McXi];
   float xipx[MAX_NUM_McXi];
   float xipy[MAX_NUM_McXi];
   float xipz[MAX_NUM_McXi];
   float xideclen[MAX_NUM_McXi];
   float xidca[MAX_NUM_McXi];
   float xidca2d[MAX_NUM_McXi];
   float xisinth[MAX_NUM_McXi];
   
};

#endif
