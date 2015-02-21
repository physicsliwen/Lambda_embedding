{
   ifstream infile("weight_antilambda_fp.txt"); 
   float efficiency[9][10] = {0,};
   int centbin = 0;
   int ptbin = 0;
   double dumb = 0.;
   double eff = 0.;
   double efferr = 0.;
   float ptbd[11] = {0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.3,2.6};
   //Float_t ptx[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   float ptx[10] = {};
   for(int i = 0; i< 10; i++){
       ptx[i] = (ptbd[i] + ptbd[i+1])/2;
       printf("pt_x = %f\n", ptx[i]);
   }
   while(infile>>centbin){
        infile>>ptbin;
        infile>>dumb;
        infile>>dumb;
        infile>>dumb;
        infile>>dumb;
        infile>>eff;
        infile>>efferr;
        efficiency[centbin][ptbin-1] = eff;
        //printf("centbin %d ptbin %d efficiency is %f\n", centbin, ptbin, eff);
   } 

   //TCanvas* can = new TCanvas("c1", "c1", 800, 600);
   //can -> SetLogy();
   TGraph* gr = new TGraph(10, ptx, efficiency[1]);
   gr->Draw();

/*
   for(int i = 0; i < 9; i++){
       TGraph* gr = new TGraph(10, ptx, efficiency[1]);
       can -> cd(1);
       gr -> Draw();
    
   }
*/
}
