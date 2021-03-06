#include "SONGKYO.h"
#include "tdrstyle.C"
#include "../CMS_lumi.C"
#include "../cutsAndBin.h"

void draw_CrossSection_rap_isArrow(int ppAA=1, bool isArrow=true) //1=pp, 2=AA
{
  setTDRStyle();
  writeExtraText = true;       // if extra text
  //int iPeriod; // 1: pp, 2: pPb, 3: PbPb, 100: RAA vs cent, 101: RAA vs pt or rap
  int iPos = 33;
  
  int nState = 3; // Y(1S), Y(2S), and Y(3S)
  if(ppAA==1) nState=3;
  double xmax = 2.4;
//  double relsys = 0.1;

  double exsys_1s[6] =  {0.2, 0.2, 0.2, 0.2, 0.2, 0.2};
  double exsys_2s[3] =  {0.4, 0.4, 0.4};
  double exsys_3s[2] =  {0.6, 0.6};

  TString sz_ppAA;
  if (ppAA==1) { sz_ppAA = "PP";}
  else if (ppAA==2) { sz_ppAA = "AA";}
  else { cout << "ERROR!! Select ppAA==1 or 2!!!"<< endl; return; }

  ////////////////////////////////////////////////////////////////
  //// read input file : value & stat.
  TFile* fIn[nState];
	TGraphErrors* gCrossSection[nState];
	TGraphErrors* gCrossSection_sys[nState];
  for (int is=0; is<nState; is++){
  	fIn[is] = new TFile(Form("Ups_%d_RAA.root",is+1),"READ");
    gCrossSection[is]=(TGraphErrors*)fIn[is]->Get(Form("gCrossSection_rap_%s",sz_ppAA.Data()));
    gCrossSection_sys[is]=(TGraphErrors*)fIn[is]->Get(Form("gCrossSection_rap_%s",sz_ppAA.Data()));
    cout << "gCrossSection["<<is<<"] = " <<gCrossSection[is] << endl;
  }
  //// read input file : syst.
  TFile* fInSys[nState];
  TH1D* hSys[nState];
  int npoint[nState];
  for (int is=0; is<nState; is++){
    fInSys[is] = new TFile(Form("../Systematic/mergedSys_ups%ds.root",is+1),"READ");
    hSys[is]=(TH1D*)fInSys[is]->Get(Form("hrap%s_merged",sz_ppAA.Data()));
    npoint[is] = hSys[is]->GetSize()-2;
    cout << "*** Y("<<is+1<<") : # of point = " << npoint[is] << endl;
  }

  //// set bin width and calculate systematic uncertainties 
  double pxtmp, pytmp, extmp, eytmp;
  double relsys;
  for (int is=0; is<nState; is++){
    cout << is+1 <<"th state***************" << endl;
    if (npoint[is] != gCrossSection[is]->GetN()) {cout << "Error!! data file and syst. file have different binnig!" << endl; return; }
    for (int ipt=0; ipt< npoint[is] ; ipt++) { //bin by bin
      pxtmp=0; pytmp=0; extmp=0; eytmp=0; relsys=0;
      gCrossSection[is]->GetPoint(ipt, pxtmp, pytmp);
      extmp=gCrossSection[is]->GetErrorX(ipt);
      eytmp=gCrossSection[is]->GetErrorY(ipt);
      relsys=hSys[is]->GetBinContent(ipt+1);
      cout << ipt <<"th bin CrossSection value = " << pytmp << endl;
      cout << ipt <<"th bin stat. = " << eytmp << endl;
      //cout << ipt <<"th bin rel. syst. = " << relsys << endl;
      cout << ipt <<"th bin syst. = " << pytmp*relsys << endl; 
      // 1) remove ex from gCrossSection
      gCrossSection[is]->SetPointError(ipt, 0, eytmp);
      // 2) set ey for gCrossSection_sys
      //gCrossSection_sys[is]->SetPointError(ipt, extmp, pytmp*relsys);
      if (is==0) gCrossSection_sys[is]->SetPointError(ipt, exsys_1s[ipt], pytmp*relsys);
      else if (is==1) gCrossSection_sys[is]->SetPointError(ipt, exsys_2s[ipt], pytmp*relsys);
      else if (is==2 && ppAA==2) gCrossSection_sys[is]->SetPointError(ipt, exsys_3s[ipt], pytmp*relsys);
      else if (is==2 && ppAA!=1) {
        gCrossSection_sys[is]->SetPointError(ipt, 0, 0);
        gCrossSection[is]->SetPoint(ipt,-10,-10);
      }
      else gCrossSection_sys[is]->SetPointError(ipt, exsys_3s[ipt], pytmp*relsys);
    }
  }
 
  ////////////////////////////////////////////////////////////////
  int ulstate = 2; //3S
  static const int n3s = 2;
  double boxw = 0.05; // for syst. box (vs cent)
  double lower68[n3s] = {lower68XS_y1,lower68XS_y2};
  double upper68[n3s] = {upper68XS_y1,upper68XS_y2};
  double lower95[n3s] = {lower95XS_y1,lower95XS_y2};
  double upper95[n3s] = {upper95XS_y1,upper95XS_y2};
  if (n3s != npoint[ulstate]) {cout<<"ERROR!! # of bins for UL is wrong!!"<<endl;return;}

  //// --- vs centrality
  TBox *box68per[n3s];
  TArrow *arr95per[n3s];
  for (int ipt=0; ipt< n3s ; ipt++) { //bin by bin
    pxtmp=0; pytmp=0; extmp=0; eytmp=0;
    //lower68=0; upper68=0; lower95=0; upper95=0;
    gCrossSection[ulstate]->GetPoint(ipt, pxtmp, pytmp);
    box68per[ipt] = new TBox(pxtmp-boxw,lower68[ipt],pxtmp+boxw,upper68[ipt]);
    arr95per[ipt] = new TArrow(pxtmp,lower95[ipt],pxtmp,upper95[ipt],0.027,"<-|"); //95%
    box68per[ipt]->SetLineColor(kGreen+3);
    box68per[ipt]->SetFillColorAlpha(kGreen-6,0.5);
    box68per[ipt]->SetLineWidth(1);
    arr95per[ipt]->SetLineColor(kGreen+2);
    arr95per[ipt]->SetLineWidth(2);
  }
   
  //// graph style 
  for (int is=0; is<nState; is++){
    SetGraphStyle(gCrossSection[is], is, is); 
    SetGraphStyleSys(gCrossSection_sys[is], is); 
	}
  
  //// latex for text
  TLatex* globtex = new TLatex();
  globtex->SetNDC();
  globtex->SetTextAlign(12); //left-center
  globtex->SetTextFont(42);
  globtex->SetTextSize(0.040);
  
  //// legend
//  TLegend *leg;
//  if (ppAA==1) {leg= new TLegend(0.68, 0.24, 0.85, 0.41); }//leg-> SetNColumns(3);}
//  else {
//    leg= new TLegend(0.68, 0.6, 0.90, 0.72);
//  }
//  //else leg= new TLegend(0.80, 0.42, 1.00, 0.59);
//  SetLegendStyle(leg);
//  for (int is=0; is<nState; is++){
//    leg -> AddEntry(gCrossSection[is],Form("#Upsilon(%dS)",is+1),"lp");
//  }

  
  //// axis et. al
  gCrossSection_sys[0]->GetXaxis()->SetTitle("|y^{#mu#mu}|");
  gCrossSection_sys[0]->GetXaxis()->CenterTitle();
  if (ppAA==1) gCrossSection_sys[0]->GetYaxis()->SetTitle("B #frac{d#sigma}{dy} (nb)");
  else gCrossSection_sys[0]->GetYaxis()->SetTitle("B #frac{1}{T_{AA}} #frac{dN}{dy} (nb)");
  gCrossSection_sys[0]->GetYaxis()->CenterTitle();
  gCrossSection_sys[0]->GetYaxis()->SetTitleOffset(2.0);
  gCrossSection_sys[0]->GetYaxis()->SetTitleSize(0.045);
  gCrossSection_sys[0]->GetXaxis()->SetTitleOffset(1.);
  gCrossSection_sys[0]->GetXaxis()->SetLimits(0.,xmax);
  gCrossSection_sys[0]->SetMinimum(0.004);
  gCrossSection_sys[0]->SetMaximum(100.);
  // for rap
  gCrossSection_sys[0]->GetXaxis()->SetNdivisions(505);

  if (isArrow == true){
        gCrossSection_sys[2]->SetPoint(0,-10,-10);
        gCrossSection_sys[2]->SetPointError(0,0,0);
        gCrossSection_sys[2]->SetPoint(1,-11,-11);
        gCrossSection_sys[2]->SetPointError(1,0,0);
        gCrossSection_sys[2]->SetPoint(2,-12,-12);
        gCrossSection_sys[2]->SetPointError(2,0,0);
        gCrossSection[2]->SetPoint(0,-10,-10);
        gCrossSection[2]->SetPointError(0,0,0);
        gCrossSection[2]->SetPoint(1,-11,-11);
        gCrossSection[2]->SetPointError(1,0,0);
        gCrossSection[2]->SetPoint(2,-12,-12);
        gCrossSection[2]->SetPointError(2,0,0);
        gCrossSection_sys[2]->GetHistogram()->GetXaxis()->SetLimits(0,2.4);
        gCrossSection_sys[2]->GetHistogram()->GetXaxis()->SetRangeUser(0,2.4);
        gCrossSection_sys[2]->SetMinimum(0.0);
        gCrossSection_sys[2]->SetMaximum(1.3);
        gCrossSection[2]->GetHistogram()->GetXaxis()->SetRangeUser(0,2.4);
        gCrossSection[2]->GetHistogram()->GetXaxis()->SetLimits(0,2.4);
        gCrossSection[2]->SetMinimum(0.0);
        gCrossSection[2]->SetMaximum(1.3);
      }

  //// draw  
  TCanvas* c1 = new TCanvas("c1","c1",600,600);
  c1->SetTicks(1,1); 
  gPad->SetLogy(1); // for cross section
  for (int is=0; is<nState; is++){
    if ( is==0) {gCrossSection_sys[is]->Draw("A5");}
    else if(is==ulstate && isArrow==true) {
      for(int ipt=0;ipt<n3s;ipt++){
        box68per[ipt]->Draw("l");
      }
      gCrossSection_sys[is]->Draw("5");
    }
    else {gCrossSection_sys[is]->Draw("5");}
  }
  for(int is=0;is<nState;is++){
    if(is==ulstate && isArrow==true) {
      for(int ipt=0;ipt<n3s;ipt++) {
        arr95per[ipt]->Draw();
      }
      gCrossSection[is]->Draw("P");
    }
    else {gCrossSection[is]->Draw("P");}
  }

  TLegend *leg= new TLegend(0.62, 0.63, 0.83, 0.72);
  SetLegendStyle(leg);
  TLegend *leg_up= new TLegend(0.62, 0.54, 0.83, 0.63);
  SetLegendStyle(leg_up);

  TArrow *arrLeg = new TArrow(1.39,0.8675,1.39,1.2,0.021,"<-|");
  arrLeg->SetLineColor(kGreen+2);
  arrLeg->SetLineWidth(2);

  if (isArrow==false) {
    for (int is=0; is<nState; is++){
      leg -> AddEntry(gCrossSection[is],Form(" #Upsilon(%dS)",is+1),"lp");
    }
  }
  else {
    leg -> AddEntry(gCrossSection[0]," #Upsilon(1S)","lp");
    leg -> AddEntry(gCrossSection[1]," #Upsilon(2S)","lp");
//    leg -> AddEntry(gCrossSection[2]," #Upsilon(3S)","lp");
    TLegendEntry *ent=leg_up->AddEntry("ent"," #Upsilon(3S) 68\% CL","f");
    ent->SetLineColor(kGreen+3);
    ent->SetFillColorAlpha(kGreen-6,0.5);
    ent->SetFillStyle(1001);
    ent=leg_up->AddEntry("ent"," #Upsilon(3S) 95\% CL","f");
    ent->SetLineColor(kWhite);
    leg->Draw("same");
    leg_up->Draw("same");
    arrLeg->Draw();
  }

  gPad->SetLeftMargin(0.23);
  gPad->SetBottomMargin(0.16);
  gPad->SetRightMargin(0.06);
  gPad->SetTopMargin(0.1);

  //// draw text
  double sz_init = 0.875; double sz_step = 0.0525;
  double sz_shift;
  if (ppAA==1) sz_shift=0.0;
  else sz_shift=0.0;
//  globtex->DrawLatex(0.27, sz_init-sz_shift, "p_{T}^{#mu} > 4 GeV/c");
  globtex->DrawLatex(0.27, sz_init-sz_shift-sz_step, "p_{T}^{#mu#mu} < 30 GeV/c");
//  globtex->DrawLatex(0.27, sz_init-sz_shift-sz_step, "|y|^{#mu#mu} < 2.4");
//  globtex->DrawLatex(0.27, sz_init-sz_shift-sz_step*2, "|#eta^{#mu}| < 2.4");
  if(ppAA==2) globtex->DrawLatex(0.27, sz_init-sz_shift-sz_step*2, "Cent. 0-100%");
  
  c1->Modified();
  c1->Update();
  CMS_lumi( c1, ppAA, iPos );

	c1->Update();
  c1->SaveAs(Form("CrossSection_vs_rap_%s.pdf",sz_ppAA.Data()));
  c1->SaveAs(Form("CrossSection_vs_rap_%s.png",sz_ppAA.Data()));

	///////////////////////////////////////////////////////////////////
	//// save as a root file
	TFile *outFile = new TFile("CrossSection_vs_rap.root", "RECREATE");
	outFile->cd();
	for (int is=0; is<nState; is++){
		gCrossSection_sys[is]->Write();	
		gCrossSection[is]->Write();	
	}
	outFile->Close();
	
	return;

} // end of main func.

