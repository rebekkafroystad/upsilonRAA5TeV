#include <iostream>
#include "../rootFitHeaders.h"
#include "../commonUtility.h"
#include <RooGaussian.h>
#include <RooCBShape.h>
#include <RooWorkspace.h>
#include <RooChebychev.h>
#include <RooPolynomial.h>
#include "RooPlot.h"
#include "TText.h"
#include "TStyle.h"
#include "TPad.h"
#include "TArrow.h"
#include "TFile.h"
#include "TVirtualPad.h"
#include "../cutsAndBin.h"
#include "../PsetCollection.h"
#include "CMS_lumi_overlay.C"
#include "../tdrstyle.C"
#include "../SONGKYO.h"

using namespace std;
using namespace RooFit;
void doFitUpsilon_Data_pas_overlay_private(
       int collId = kAADATA,  
       float ptLow=0, float ptHigh=30,
       float yLow=0, float yHigh=2.4,
       int cLow=0, int cHigh=200,
       float muPtCut=4.0,
       bool fixParameters=1  )
{
  float dphiEp2Low = 0 ;
  float dphiEp2High = 100 ;
  

  using namespace RooFit;
  gStyle->SetEndErrorSize(0);
 
  TString SignalCB = "Double";

  float massLow = 8; 
  float massHigh = 14;

  float massLowForPlot = massLow;    
  float massHighForPlot = massHigh;

  int   nMassBin  = 60;
  //int   nMassBin  = (massHigh-massLow)*10;
  TFile* f1;
  if      ( collId == kPPDATA) f1 = new TFile("../skimmedFiles/yskimPP_L1DoubleMu0PD_Trig-L1DoubleMu0_OpSign_20164251755_3964bbec2f15f2cf9baa0676644690f40cee27c4.root");
  else if ( collId == kAADATA) f1 = new TFile("../skimmedFiles/yskimPbPb_L1DoubleMu0PD_Trig-L1DoubleMu0_OpSign_EP-OppositeHF_20164272229_95c28a5bdf107c32b9e54843b8c85939ffe1aa23.root");
  else if ( collId == kAADATAPeri) f1 = new TFile("../skimmedFiles/yskimPbPb_PeripheralPD_Trig-L1DoubleMu0Peripheral_OpSign_EP-OppositeHF_20164272252_95c28a5bdf107c32b9e54843b8c85939ffe1aa23.root");
  else if ( collId == kPPMCUps1S) f1 = new TFile("../skimmedFiles/yskimPP_MC_Ups1S_Trig-L1DoubleMu0_OpSign_EP-OppositeHF_20163251233_2b58ba03c4751c9d10cb9d60303271ddd6e1ba3a.root");
  else if ( collId == kAAMCUps1S) f1 = new TFile("../skimmedFiles/yskimPP_MC_Ups1S_Trig-L1DoubleMu0_OpSign_EP-OppositeHF_20163251233_2b58ba03c4751c9d10cb9d60303271ddd6e1ba3a.root");
 
  if(collId == kAADATAPeri) collId =2; 
  TString kineLabel = getKineLabel (collId, ptLow, ptHigh, yLow, yHigh, muPtCut, cLow, cHigh, dphiEp2Low, dphiEp2High) ;
  TString kineCut = Form("pt>%.2f && pt<%.2f && abs(y)>%.2f && abs(y)<%.2f",ptLow, ptHigh, yLow, yHigh);
  if (muPtCut>0) kineCut = kineCut + Form(" && (pt1>%.2f) && (pt2>%.2f)", (float)muPtCut, (float)muPtCut );
  if ( (collId == kAADATA) || (collId == kPADATA) || (collId == kAAMC) || (collId == kPAMC) || (collId == kAADATACentL3) || (collId==kAADATAPeri) )
    kineCut = kineCut + Form(" && (cBin>=%d && cBin<%d) && ( abs(abs(dphiEp2/3.141592)-0.5)>%.3f && abs(abs(dphiEp2/3.141592)-0.5)<%.3f )",cLow, cHigh, dphiEp2Low, dphiEp2High);
  
  
  TTree* tree = (TTree*) f1->Get("mm");
  RooDataSet *dataset = (RooDataSet*)f1->Get("dataset");
  RooWorkspace *ws = new RooWorkspace("workspace");
  //RooWorkspace *ws = new RooWorkspace(Form("workspace_%s",kineLabel.Data()));
  ws->import(*dataset);
  ws->data("dataset")->Print();
  cout << "####################################" << endl;
  RooDataSet *reducedDS = (RooDataSet*)dataset->reduce(RooArgSet(*(ws->var("mass")), *(ws->var("pt")), *(ws->var("y"))), kineCut.Data() );
  reducedDS->SetName("reducedDS");
  ws->import(*reducedDS);
  ws->var("mass")->setRange(massLow, massHigh);
  ws->var("mass")->Print();

  TCanvas* c1 =  new TCanvas("canvas2","My plots",600,600);
  //TCanvas* c1 =  new TCanvas("canvas2","My plots",4,45,550,520);
  c1->cd();
  TPad *pad1 = new TPad("pad1", "pad1", 0, 0, 1, 1.0);
  pad1->SetTicks(1,1);
  pad1->Draw(); pad1->cd();
//  c1->Update();


  RooPlot* myPlot = ws->var("mass")->frame(nMassBin); // bins
  //ws->data("reducedDS")->plotOn(myPlot,Name("dataHist"), Layout(0,1,0.95));
  ws->data("reducedDS")->plotOn(myPlot,Name("dataHist"));
  RooRealVar mean1s("m_{#Upsilon(1S)}","mean of the signal gaussian mass PDF",pdgMass.Y1S, pdgMass.Y1S -0.2, pdgMass.Y1S + 0.2 ) ;
  RooRealVar mRatio21("mRatio21","mRatio21",pdgMass.Y2S / pdgMass.Y1S );
  RooRealVar mRatio31("mRatio31","mRatio31",pdgMass.Y3S / pdgMass.Y1S );
  RooFormulaVar mean2s("mean2s","m_{#Upsilon(1S)}*mRatio21", RooArgSet(mean1s,mRatio21) );
  RooFormulaVar mean3s("mean3s","m_{#Upsilon(1S)}*mRatio31", RooArgSet(mean1s,mRatio31) );
          
  PSetUpsAndBkg initPset = getUpsilonPsets( collId, ptLow, ptHigh, yLow, yHigh, cLow, cHigh, muPtCut) ; 
  initPset.SetMCSgl();

  RooRealVar    sigma1s_1("sigma1s_1","width/sigma of the signal gaussian mass PDF",0.05, 0.02, 0.3);
  RooFormulaVar sigma2s_1("sigma2s_1","@0*@1",RooArgList(sigma1s_1,mRatio21) );
  RooFormulaVar sigma3s_1("sigma3s_1","@0*@1",RooArgList(sigma1s_1,mRatio31) );

  RooRealVar *x1s = new RooRealVar("x1s","sigma ratio ", 0.5, 0, 2.8);

  RooFormulaVar sigma1s_2("sigma1s_2","@0*@1",RooArgList(sigma1s_1, *x1s) );
  RooFormulaVar sigma2s_2("sigma2s_2","@0*@1",RooArgList(sigma1s_2,mRatio21) );
  RooFormulaVar sigma3s_2("sigma3s_2","@0*@1",RooArgList(sigma1s_2,mRatio31) );
  
  RooRealVar alpha1s_1("alpha1s_1","tail shift", 1.38 , 1.107, 3.593);
  RooFormulaVar alpha2s_1("alpha2s_1","1.0*@0",RooArgList(alpha1s_1) );
  RooFormulaVar alpha3s_1("alpha3s_1","1.0*@0",RooArgList(alpha1s_1) );
  RooFormulaVar alpha1s_2("alpha1s_2","1.0*@0",RooArgList(alpha1s_1) );
  RooFormulaVar alpha2s_2("alpha2s_2","1.0*@0",RooArgList(alpha1s_1) );
  RooFormulaVar alpha3s_2("alpha3s_2","1.0*@0",RooArgList(alpha1s_1) );

  RooRealVar n1s_1("n1s_1","power order", 2.156 , 1.050, 3.729);
  RooFormulaVar n2s_1("n2s_1","1.0*@0",RooArgList(n1s_1) );
  RooFormulaVar n3s_1("n3s_1","1.0*@0",RooArgList(n1s_1) );
  RooFormulaVar n1s_2("n1s_2","1.0*@0",RooArgList(n1s_1) );
  RooFormulaVar n2s_2("n2s_2","1.0*@0",RooArgList(n1s_1) );
  RooFormulaVar n3s_2("n3s_2","1.0*@0",RooArgList(n1s_1) );

  RooRealVar *f1s = new RooRealVar("f1s","1S CB fraction", 0.5, 0, 1);
  RooFormulaVar f2s("f2s","1.0*@0",RooArgList(*f1s) );
  RooFormulaVar f3s("f3s","1.0*@0",RooArgList(*f1s) );


  // Set initial parameters
  if ( initPset.n1s_1 == -1 )
    {
      cout << endl << endl << endl << "#########################  ERROR!!!! ##################" << endl;
      cout << "No Param. set for " << kineLabel << ","<<endl;
      cout << "Fitting macro is stopped!" << endl << endl << endl;
      return;
    }
  else { 
    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cout << endl << "Setting the initial  parameters..." << endl << endl;
    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cout << "initPset.n1s_1 = " << initPset.n1s_1 << endl;
    n1s_1.setVal(initPset.n1s_1);  
    cout << "initPset.alpha1s_1 = " << initPset.alpha1s_1 << endl;
    alpha1s_1.setVal(initPset.alpha1s_1);
    cout << "initPset.sigma1s_1 = " << initPset.sigma1s_1 << endl;
    sigma1s_1.setVal(initPset.sigma1s_1);
    cout << "initPset.f1s = " << initPset.f1s << endl;
    f1s->setVal(initPset.f1s); 
    cout << "initPset.x1s = " << initPset.x1s << endl;
    x1s->setVal(initPset.x1s);
  } 
  // Fix? 
  if (fixParameters)   {
    //pt<2
    //n1s_1.setVal(2.797209e+00);
    //alpha1s_1.setVal(3.819574e+00);
    //sigma1s_1.setVal(5.272972e-02);
    //f1s->setVal(3.734300e-01);
    //x1s->setVal(2.165312e+00);
    //pt>0
    n1s_1.setVal(3.563950e+00);
    alpha1s_1.setVal(1.535405e+00);
    sigma1s_1.setVal(8.527187e-02);
    f1s->setVal(9.939548e-01);
    x1s->setVal(2.688412e+00);

    n1s_1.setConstant();
    alpha1s_1.setConstant();
    sigma1s_1.setConstant();
    f1s->setConstant();
    x1s->setConstant();
  }

  
  RooCBShape* cb1s_1 = new RooCBShape("cball1s_1", "cystal Ball", *(ws->var("mass")), mean1s, sigma1s_1, alpha1s_1, n1s_1);
  cout << " n1s_1.getVal() = " << n1s_1.getVal() << endl;
  RooCBShape* cb2s_1 = new RooCBShape("cball2s_1", "cystal Ball", *(ws->var("mass")), mean2s, sigma2s_1, alpha2s_1, n2s_1);
  RooCBShape* cb3s_1 = new RooCBShape("cball3s_1", "cystal Ball", *(ws->var("mass")), mean3s, sigma3s_1, alpha3s_1, n3s_1);
  RooCBShape* cb1s_2 = new RooCBShape("cball1s_2", "cystal Ball", *(ws->var("mass")), mean1s, sigma1s_2, alpha1s_2, n1s_2);
  RooCBShape* cb2s_2 = new RooCBShape("cball2s_2", "cystal Ball", *(ws->var("mass")), mean2s, sigma2s_2, alpha2s_2, n2s_2);
  RooCBShape* cb3s_2 = new RooCBShape("cball3s_2", "cystal Ball", *(ws->var("mass")), mean3s, sigma3s_2, alpha3s_2, n3s_2);

  RooAddPdf*  cb1s = new RooAddPdf("cb1s","Signal 1S",RooArgList(*cb1s_1,*cb1s_2), RooArgList(*f1s) );
  RooAddPdf*  cb2s = new RooAddPdf("cb2s","Signal 2S",RooArgList(*cb2s_1,*cb2s_2), RooArgList(*f1s) );
  RooAddPdf*  cb3s = new RooAddPdf("cb3s","Signal 3S",RooArgList(*cb3s_1,*cb3s_2), RooArgList(*f1s) );

  RooRealVar *nSig1s= new RooRealVar("nSig1s"," 1S signals",0,100000);
  RooRealVar *nSig2s= new RooRealVar("nSig2s"," 2S signals",-100,36000);
  RooRealVar *nSig3s= new RooRealVar("nSig3s"," 3S signals",-100,26000);
  
  // background : 
  initPset.SetMCBkg();
  double init_mu = initPset.bkg_mu ;
  double init_sigma = initPset.bkg_sigma ;
  double init_lambda = initPset.bkg_lambda ;

  //  double init_mu_min = init_mu - 5; double init_mu_max = init_mu + 5;
  //  double init_sigma_min = init_sigma - 2.; double init_sigma_max = init_sigma + 2;
  //  double init_lambda_min = init_lambda - 10; double init_lambda_max = init_lambda + 10;
  double init_mu_min = init_mu - 10; double init_mu_max = init_mu + 10;
  double init_sigma_min = init_sigma - 10.; double init_sigma_max = init_sigma + 10;
  double init_lambda_min = init_lambda - 10; double init_lambda_max = init_lambda + 10;
  if(init_mu_min <0) init_mu_min = 0;
  if(init_sigma_min <0) init_sigma_min = 0;
  if(init_lambda_min <0) init_lambda_min = 0;
 
  RooRealVar err_mu("#mu","err_mu",init_mu,  0, 25) ;
  RooRealVar err_sigma("#sigma","err_sigma", init_sigma, 0,25);
  //RooRealVar m_lambda("#lambda","m_lambda",  5, 0,50.123);
  RooRealVar m_lambda("#lambda","m_lambda",  init_lambda, 0,25);

  if (fixParameters)   {
    //err_mu.setVal(7.9715); err_mu.setConstant();
    //err_sigma.setVal(1.0699); err_sigma.setConstant();
    //m_lambda.setVal(5.9808); m_lambda.setConstant();

    err_mu.setVal(7.971486e+00); err_mu.setConstant();
    err_sigma.setVal(1.069888e+00); err_sigma.setConstant();
    m_lambda.setVal(5.980788e+00); m_lambda.setConstant();
  }


 /* 
  RooRealVar err_mu("#mu","err_mu",init_mu,  0, 25) ;
  RooRealVar err_sigma("#sigma","err_sigma", init_sigma, 0,25);
  RooRealVar m_lambda("#lambda","m_lambda",  5, 0,23);
*/  //RooRealVar m_lambda("#lambda","m_lambda",  init_lambda, 0,25);
   
  /*
  RooRealVar err_mu("#mu","err_mu",init_mu,  init_mu_min, init_mu_max ) ;
  RooRealVar err_sigma("#sigma","err_sigma", init_sigma, init_sigma_min, init_sigma_max);
  RooRealVar m_lambda("#lambda","m_lambda",  init_lambda, init_lambda_min, init_lambda_max);
  */
  RooGenericPdf *bkg;
  RooGenericPdf *bkgLowPt = new RooGenericPdf("bkgLowPt","Background","TMath::Exp(-@0/@1)*(TMath::Erf((@0-@2)/(TMath::Sqrt(2)*@3))+1)*0.5",RooArgList( *(ws->var("mass")), m_lambda, err_mu, err_sigma) );
  RooGenericPdf *bkgHighPt = new RooGenericPdf("bkgHighPt","Background","TMath::Exp(-@0/@1)",RooArgList(*(ws->var("mass")),m_lambda));
  
  if  (ptLow >= 6)        bkg = bkgHighPt ;
  else bkg = bkgLowPt;

  RooRealVar *nBkg = new RooRealVar("nBkg","fraction of component 1 in bkg",0,1000000);  
  nBkg->setVal(1.046182e+05); nBkg->setConstant();

  RooAddPdf* model = new RooAddPdf();
  model = new RooAddPdf("model","1S+2S+3S + Bkg",RooArgList(*cb1s, *cb2s, *cb3s, *bkg),RooArgList(*nSig1s,*nSig2s,*nSig3s,*nBkg));

  ws->import(*model);


  RooPlot* myPlot2 = (RooPlot*)myPlot->Clone();
  ws->data("reducedDS")->plotOn(myPlot2,Name("dataOS_FIT"),MarkerSize(.8));
 
  
  RooFitResult* fitRes2 = ws->pdf("model")->fitTo(*reducedDS,Save(), Hesse(kTRUE),Range(massLow, massHigh),Timer(kTRUE),Extended(kTRUE));
  ws->pdf("model")->plotOn(myPlot2,Name("modelHist"),LineColor(kBlue));
  //ws->pdf("model")->plotOn(myPlot2,Name("Sig1S"),Components(RooArgSet(*cb1s)),LineColor(kGray+2),LineWidth(2),LineStyle(2));
  //ws->pdf("model")->plotOn(myPlot2,Components(RooArgSet(*cb2s)),LineColor(kGray+2),LineWidth(2),LineStyle(2));
  //ws->pdf("model")->plotOn(myPlot2,Components(RooArgSet(*cb3s)),LineColor(kGray+2),LineWidth(2),LineStyle(2));
  ws->pdf("model")->plotOn(myPlot2,Name("bkgPDF"),Components(RooArgSet(*bkg)),LineColor(kBlue),LineStyle(kDashed),LineWidth(2));

  RooRealVar *f1s_pp = new RooRealVar("f1s_pp","1S CB fraction", 0.5, 0, 1);
  RooRealVar *x1s_pp = new RooRealVar("x1s_pp","1S CB fraction", 0.5, 0, 2.5);
  //f1s_pp->setVal(ws->var("f1s")->getVal()); f1s_pp->setConstant();
  //x1s_pp->setVal(ws->var("x1s")->getVal()); x1s_pp->setConstant();
  //f1s_pp->setVal(2.927674e-01); f1s_pp->setConstant();//for pt<2
  //x1s_pp->setVal(2.131210e+00); x1s_pp->setConstant();//for pt<2
  f1s_pp->setVal(4.438662e-01); f1s_pp->setConstant();//for pt>0
  x1s_pp->setVal(1.959417e+00); x1s_pp->setConstant();//for pt>0
  RooRealVar sigma1s_1_pp("sigma1s_1_pp","width/sigma of the signal gaussian mass PDF",0.05, 0.05, 0.3);
  //sigma1s_1_pp.setVal(ws->var("sigma1s_1")->getVal()); sigma1s_1_pp.setConstant();
  //sigma1s_1_pp.setVal(5.252993e-02); sigma1s_1_pp.setConstant(); //for pt<2
  sigma1s_1_pp.setVal(6.296324e-02); sigma1s_1_pp.setConstant(); //for pt>0

  RooFormulaVar sigma1s_2_pp("sigma1s_2_pp","sigma1s_1_pp*x1s_pp",RooArgSet(sigma1s_1_pp,x1s_pp) );
  RooFormulaVar sigma2s_1_pp("sigma2s_1_pp","sigma1s_1_pp*mRatio21",RooArgSet(sigma1s_1_pp,mRatio21) );
  RooFormulaVar sigma2s_2_pp("sigma2s_2_pp","sigma2s_1_pp*x1s_pp",RooArgSet(sigma2s_1_pp,x1s_pp) );
  RooFormulaVar sigma3s_1_pp("sigma3s_1_pp","sigma1s_1_pp*mRatio31",RooArgSet(sigma1s_1_pp,mRatio31) );
  RooFormulaVar sigma3s_2_pp("sigma3s_2_pp","sigma3s_1_pp*x1s_pp",RooArgSet(sigma3s_1_pp,x1s_pp) );
  
  RooRealVar alpha1s_1_pp("alpha1s_1_pp","tail shift", 5. , 1.2, 9.8);
  RooRealVar n1s_1_pp("n1s_1_pp","power order", 5. , 1.2, 9.8);
  //alpha1s_1_pp.setVal(ws->var("alpha1s_1")->getVal()); alpha1s_1_pp.setConstant();
  //n1s_1_pp.setVal(ws->var("n1s_1")->getVal()); n1s_1_pp.setConstant();
  //alpha1s_1_pp.setVal(3.280915e+00); alpha1s_1_pp.setConstant(); //for pt<2
  //n1s_1_pp.setVal(3.584124e+00); n1s_1_pp.setConstant(); //for pt<2
  alpha1s_1_pp.setVal(1.477313e+00); alpha1s_1_pp.setConstant(); //for pt>0
  n1s_1_pp.setVal(3.761997e+00); n1s_1_pp.setConstant(); //for pt>0
  
  RooRealVar err_mu_pp("#mu","err_mu_pp",5,  0, 30) ;
  RooRealVar err_sigma_pp("#sigma","err_sigma_pp", 5, 0,30);
  RooRealVar m_lambda_pp("#lambda","m_lambda_pp",  5, 0,30);
  RooRealVar mean1s_pp("m_{#Upsilon(1S)}_pp","mean of the signal gaussian mass PDF",pdgMass.Y1S, pdgMass.Y1S -0.1, pdgMass.Y1S + 0.1 ) ;
  mean1s_pp.setVal(ws->var("m_{#Upsilon(1S)}")->getVal()); mean1s_pp.setConstant();
  
  RooFormulaVar mean2s_pp("mean2s_pp","m_{#Upsilon(1S)}_pp*mRatio21", RooArgSet(mean1s_pp,mRatio21) );
  RooFormulaVar mean3s_pp("mean3s_pp","m_{#Upsilon(1S)}_pp*mRatio31", RooArgSet(mean1s_pp,mRatio31) );
  
  err_mu_pp.setVal(ws->var("#mu")->getVal()); err_mu_pp.setConstant();
  err_sigma_pp.setVal(ws->var("#sigma")->getVal()); err_sigma_pp.setConstant();
  m_lambda_pp.setVal(ws->var("#lambda")->getVal()); m_lambda_pp.setConstant();

  RooRealVar *nSig1s_pp = new RooRealVar("nSig1s_pp"," 1S signals",4000,0,100000);
  RooRealVar *nSig2s_pp = new RooRealVar("nSig2s_pp"," 2S signals",1000,0,100000);
  RooRealVar *nSig3s_pp = new RooRealVar("nSig3s_pp"," 3S signals",100, 0,10000);
  RooRealVar *nBkg_pp = new RooRealVar("nBkg_pp","fraction of component 1 in bkg",10000,0,5000000);  

  //with out correction
  //root [2] fitResults->GetBinContent(1)
  //(Double_t) 3.484141e+04
  //root [3] fitResults->GetBinContent(2)
  //(Double_t) 1.134067e+04
  //root [4] fitResults->GetBinContent(3)
  //(Double_t) 5.970829e+03
  //double ppSig1S_over = 5.531557e+03 /(5.531557e+03/3.484141e+04 * 2.8e+10 / (NumberOfMBColl*TAA1s[nCentBins1s]) * 0.8879415347137637) ;
  //double ppSig2S_over = 5.136925e+02 /(5.136925e+02/1.134067e+04 * 2.8e+10 / (NumberOfMBColl*TAA1s[nCentBins1s]) * 0.8848920863309353);
  //double ppSig3S_over = 2.207336e+01 /(2.207336e+01/5.970829e+03 * 2.8e+10 / (NumberOfMBColl*TAA1s[nCentBins1s]) * 1 );

  //Emilien suggestion
  //double ppSig1S_over = 5.531557e+03 / 0.341 ;
  //double ppSig2S_over = 5.136925e+02 / 0.105 ;
  //double ppSig3S_over = 2.207336e+01 / (2.207336e+01/5.970829e+03 * 2.8e+10 / (NumberOfMBColl*TAA1s[nCentBins1s]) * 0.8890214797136038);

  //with correction
  double ppSig1S_over =  3.484141e+04 * 0.935751938 * ( TAA1s[nCentBins1s] * NumberOfMBColl / (2.8e+10) ) ;
  double ppSig2S_over =  1.134067e+04 * 0.935751938 * ( TAA1s[nCentBins1s] * NumberOfMBColl / (2.8e+10) ) ;
  double ppSig3S_over =  5.970829e+03 * 0.935751938 * ( TAA1s[nCentBins1s] * NumberOfMBColl / (2.8e+10) ) ;
  //
  //double ppSig1S_over = 877.036/3.106144e-01;  
  //double ppSig2S_over = ppSig1S_over/(6.688938e+03/2.070320e+03) ;
  //double ppSig3S_over = ppSig1S_over/(6.688938e+03/9.484567e+02) ;
  //nSig1s_pp->setVal(ws->var("nSig1s")->getVal()/(3.106144e-01)); nSig1s_pp->setConstant();
  //nSig2s_pp->setVal(ws->var("nSig2s")->getVal()/(1.027922e-01)); nSig2s_pp->setConstant();
  //nSig3s_pp->setVal(ws->var("nSig3s")->getVal()/(1.241043e-02)); nSig3s_pp->setConstant();
  
  //nSig1s_pp->setVal(ws->var("nSig1s")->getVal()/(3.518450e-01)); nSig1s_pp->setConstant();
  //nSig2s_pp->setVal(ws->var("nSig2s")->getVal()/(1.031954e-01)); nSig2s_pp->setConstant();
  //nSig3s_pp->setVal(ws->var("nSig3s")->getVal()/(8.431885e-03)); nSig3s_pp->setConstant();

  cout<<"$$$TAA numbers : "<<TAA1s[nCentBins1s]<<endl;
  cout<<"$$$NumberOfMBColl : "<<NumberOfMBColl<<endl;
    
  nSig1s_pp->setVal(ppSig1S_over); nSig1s_pp->setConstant();
  nSig2s_pp->setVal(ppSig2S_over); nSig2s_pp->setConstant();
  nSig3s_pp->setVal(ppSig3S_over); nSig3s_pp->setConstant();

  cout << "$$$PbPbSig1S : "<<ws->var("nSig1s")->getVal()<< " ppSig1S_over : " << nSig1s_pp->getVal() <<" ratio : "<< ws->var("nSig1s")->getVal()/nSig1s_pp->getVal()<<endl;
  cout << "$$$PbPbSig2S : "<<ws->var("nSig2s")->getVal()<< "  ppSig2S_over : " << nSig2s_pp->getVal() <<" ratio : "<<ws->var("nSig2s")->getVal()/nSig2s_pp->getVal()<<endl;
  cout << "$$$PbPbSig3S : "<<ws->var("nSig3s")->getVal()<< " ppSig3S_over : " << nSig3s_pp->getVal() <<" ratio : "<< ws->var("nSig3s")->getVal()/nSig3s_pp->getVal()<<endl;

  //nSig1s_pp->setVal(6.688938e+03 * 5.61 * 2.454e+9 / 2.8e+10); nSig1s_pp->setConstant();
  //nSig2s_pp->setVal(2.070320e+03 * 5.61 * 2.454e+9 / 2.8e+10); nSig2s_pp->setConstant();
  //nSig3s_pp->setVal(9.484567e+02 * 5.61 * 2.454e+9 / 2.8e+10); nSig3s_pp->setConstant();


  //nSig1s_pp->setVal(ws->var("nSig1s")->getVal()); nSig1s_pp->setConstant();
  //nSig2s_pp->setVal(0.3199*(ws->var("nSig1s")->getVal())); nSig2s_pp->setConstant();
  //nSig3s_pp->setVal(0.1677*(ws->var("nSig1s")->getVal())); nSig3s_pp->setConstant();
  nBkg_pp->setVal(ws->var("nBkg")->getVal()); nBkg_pp->setConstant();


  RooCBShape* cb1s_1_pp = new RooCBShape("cball1s_1_pp", "cystal Ball", *(ws->var("mass")), mean1s_pp, sigma1s_1_pp, alpha1s_1_pp, n1s_1_pp);
  RooCBShape* cb1s_2_pp = new RooCBShape("cball1s_2_pp", "cystal Ball", *(ws->var("mass")), mean1s_pp, sigma1s_2_pp, alpha1s_1_pp, n1s_1_pp);
  RooCBShape* cb2s_1_pp = new RooCBShape("cball2s_1_pp", "cystal Ball", *(ws->var("mass")), mean2s_pp, sigma2s_1_pp, alpha1s_1_pp, n1s_1_pp);
  RooCBShape* cb2s_2_pp = new RooCBShape("cball2s_2_pp", "cystal Ball", *(ws->var("mass")), mean2s_pp, sigma2s_2_pp, alpha1s_1_pp, n1s_1_pp);
  RooCBShape* cb3s_1_pp = new RooCBShape("cball3s_1_pp", "cystal Ball", *(ws->var("mass")), mean3s_pp, sigma3s_1_pp, alpha1s_1_pp, n1s_1_pp);
  RooCBShape* cb3s_2_pp = new RooCBShape("cball3s_2_pp", "cystal Ball", *(ws->var("mass")), mean3s_pp, sigma3s_2_pp, alpha1s_1_pp, n1s_1_pp);

  RooAddPdf*  cb1s_pp = new RooAddPdf("cb1s_pp","Signal 1S",RooArgList(*cb1s_1_pp,*cb1s_2_pp), RooArgList(*f1s_pp) );
  RooAddPdf*  cb2s_pp = new RooAddPdf("cb2s_pp","Signal 2S",RooArgList(*cb2s_1_pp,*cb2s_2_pp), RooArgList(*f1s_pp) );
  RooAddPdf*  cb3s_pp = new RooAddPdf("cb3s_pp","Signal 3S",RooArgList(*cb3s_1_pp,*cb3s_2_pp), RooArgList(*f1s_pp) );


  double numtotev1 = nSig1s_pp->getVal();
  double numtotev2 = nSig2s_pp->getVal();
  double numtotev3 = nSig3s_pp->getVal();
  double numtotev = nSig1s_pp->getVal()+nSig2s_pp->getVal()+nSig3s_pp->getVal()+nBkg_pp->getVal();
  RooGenericPdf *bkgLowPt_pp = new RooGenericPdf("bkgLowPt_pp","Background","TMath::Exp(-@0/@1)*(TMath::Erf((@0-@2)/(TMath::Sqrt(2)*@3))+1)*0.5",RooArgList( *(ws->var("mass")), m_lambda_pp, err_mu_pp, err_sigma_pp) );
  RooAddPdf* model_pp = new RooAddPdf();
  model_pp = new RooAddPdf("model_pp","1S+2S+3S + Bkg",RooArgList(*cb1s_pp, *cb2s_pp, *cb3s_pp, *bkgLowPt_pp),RooArgList(*nSig1s_pp,*nSig2s_pp,*nSig3s_pp,*nBkg_pp));
  ws->import(*model_pp);
  //model_pp->plotOn(myPlot2,LineColor(kRed),LineStyle(kDashed),LineWidth(2.3));
  model_pp->plotOn(myPlot2,Normalization(numtotev,RooAbsReal::NumEvent),Name("ppOverlay"),LineColor(kRed),LineStyle(kDashed),LineWidth(2.));

  TGaxis::SetMaxDigits(2);
  myPlot2->SetFillStyle(4000);
  myPlot2->SetAxisRange(massLowForPlot, massHighForPlot,"X");
  myPlot2->GetYaxis()->SetTitleOffset(1.77);
  myPlot2->GetYaxis()->CenterTitle();
  myPlot2->GetYaxis()->SetTitleSize(0.05);
  myPlot2->GetYaxis()->SetLabelSize(0.045);
  myPlot2->GetXaxis()->SetRangeUser(8,14);
  myPlot2->GetXaxis()->SetTitle("m_{#mu^{+}#mu^{-}} (GeV/c^{2})");
  myPlot2->GetXaxis()->SetTitleOffset(1.22) ;
  myPlot2->GetXaxis()->SetLabelOffset(0.018) ;
  myPlot2->GetXaxis()->SetLabelSize(0.045) ;
  myPlot2->GetXaxis()->SetTitleSize(0.057) ;
  myPlot2->GetXaxis()->CenterTitle();
  myPlot2->Draw();
  fitRes2->Print("v");
  Double_t theNLL = fitRes2->minNll();
  cout << " *** NLL : " << theNLL << endl;
  TString perc = "%";

  float pos_text_x = 0.41;
  float pos_text_y = 0.852;
  float pos_y_diff = 0.052;
  float text_size = 17;
  int text_color = 1;
  if(ptLow==0) drawText(Form("p_{T}^{#mu#mu} < %.f GeV/c",ptHigh ),pos_text_x,pos_text_y,text_color,text_size);
  else if(ptLow == 2.5 && ptHigh==5) drawText(Form("%.1f < p_{T}^{#mu#mu} < %.f GeV/c",ptLow,ptHigh ),pos_text_x,pos_text_y,text_color,text_size);
  else drawText(Form("%.f < p_{T}^{#mu#mu} < %.f GeV/c",ptLow,ptHigh ),pos_text_x,pos_text_y,text_color,text_size);
  if(yLow==0) drawText(Form("|y^{#mu#mu}| < %.1f",yHigh ), pos_text_x,pos_text_y-pos_y_diff,text_color,text_size);
  else if(yLow!=0) drawText(Form("%.1f < |y^{#mu#mu}| < %.1f",yLow,yHigh ), pos_text_x,pos_text_y-pos_y_diff,text_color,text_size);
  if(collId != kPPDATA && collId != kPPMCUps1S && collId != kPPMCUps2S) 
  {
      drawText(Form("p_{T}^{#mu} > %.f GeV/c", muPtCut ), pos_text_x,pos_text_y-pos_y_diff*2,text_color,text_size);
      drawText("|#eta^{#mu}| < 2.4 GeV/c", pos_text_x,pos_text_y-pos_y_diff*3,text_color,text_size);
      drawText(Form("Centrality %d-%d%s",cLow/2,cHigh/2,perc.Data()),pos_text_x,pos_text_y-pos_y_diff*4,text_color,text_size);
  }
  else {
    drawText(Form("p_{T}^{#mu} > %.f GeV/c", muPtCut ), pos_text_x,pos_text_y-pos_y_diff*2,text_color,text_size);
    drawText("|#eta^{#mu}| < 2.4 GeV/c", pos_text_x,pos_text_y-pos_y_diff*3,text_color,text_size);
  }  

  TLegend* fitleg = new TLegend(0.68,0.42,0.88,0.7); fitleg->SetTextSize(17);
  fitleg->SetTextFont(43);
  fitleg->SetBorderSize(0);
  fitleg->AddEntry(myPlot2->findObject("dataOS_FIT"),"PbPb Data","pe");
  fitleg->AddEntry(myPlot2->findObject("modelHist"),"Total fit","l");
  //fitleg->AddEntry(myPlot2->findObject("Sig1S"),"Signal","l");
  fitleg->AddEntry(myPlot2->findObject("bkgPDF"),"Background","l");
  fitleg->AddEntry(myPlot2->findObject("ppOverlay"),"pp overlaid","l");
  fitleg->AddEntry(myPlot2->findObject("ppOverlay"),"with T_{AA} scaling","");
  fitleg->Draw("same");

  // PULL 

  TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 0.98, 0.23);
  pad2->SetTicks(1,1);
  pad1->SetLeftMargin(0.18);
  pad1->SetBottomMargin(0.17);
  pad1->SetTopMargin(0.067);
  pad1->SetRightMargin(0.05);
  
  RooHist* hpull = myPlot2->pullHist("dataHist","modelHist");
  hpull->SetMarkerSize(0.8);
  RooPlot* pullFrame = ws->var("mass")->frame(Title("Pull Distribution")) ;
  pullFrame->addPlotable(hpull,"P") ;
  pullFrame->SetTitleSize(0);
  pullFrame->GetYaxis()->SetTitleOffset(0.33) ;
  pullFrame->GetYaxis()->SetTitle("Pull") ;
  pullFrame->GetYaxis()->SetTitleSize(0.16) ;
  pullFrame->GetYaxis()->SetLabelSize(0.12) ;
  pullFrame->GetYaxis()->SetRangeUser(-4.5,4.5) ;
//  pullFrame->GetYaxis()->SetLimits(-6,6) ;
  pullFrame->GetYaxis()->CenterTitle();

  pullFrame->GetXaxis()->SetTitle("m_{#mu^{+}#mu^{-}} (GeV/c^{2})");
  pullFrame->GetXaxis()->SetTitleOffset(1.27) ;
  pullFrame->GetXaxis()->SetLabelOffset(0.03) ;
  pullFrame->GetXaxis()->SetLabelSize(0.177) ;
  pullFrame->GetXaxis()->SetTitleSize(0.24) ;
  pullFrame->GetXaxis()->CenterTitle();
 // pullFrame->GetXaxis()->SetTitleFont(43);
 // pullFrame->GetYaxis()->SetTitleFont(43);
  
  pullFrame->GetYaxis()->SetTickSize(0.02);
  pullFrame->GetYaxis()->SetNdivisions(505);
  pullFrame->GetXaxis()->SetTickSize(0.03);
//  pullFrame->Draw() ;

  
  double chisq = 0;
  int nFullBinsPull = 0;
  int nBins = nMassBin; 
  double *ypull = hpull->GetY();
  for(int i=0;i<nBins;i++)
  {
    if(ypull[i] == 0) continue;
    chisq += TMath::Power(ypull[i],2);
    nFullBinsPull++;
  }

  int numFitPar = fitRes2->floatParsFinal().getSize();
  int ndf = nFullBinsPull - numFitPar;

  TLine *l1 = new TLine(massLow,0,massHigh,0);
  l1->SetLineStyle(9);
//  l1->Draw("same");
//  drawText(Form("chi^{2}/ndf : %.3f / %d ",chisq,ndf ),0.12,0.87,1,10);
/*
  TPad *pad3 = new TPad("pad3", "pad3", 0.685, 0.42, 0.885, 0.77);
  pad3->SetBottomMargin(0);
  pad3->SetTopMargin(0);
  pad3->SetLeftMargin(0);
  pad3->SetRightMargin(0);
  pad3->SetFillColorAlpha(kWhite,1.00);
  pad3->cd();
*/
  pad1->Update();
  //legFrame->findObject(Form("%s_paramBox",ws->pdf("model")->GetName()))->Draw();
              
  
  TH1D* outh = new TH1D("fitResults","fit result",20,0,20);

  outh->GetXaxis()->SetBinLabel(1,"Upsilon1S");
  outh->GetXaxis()->SetBinLabel(2,"Upsilon2S");
  outh->GetXaxis()->SetBinLabel(3,"Upsilon3S");
  //  outh->GetXaxis()->SetBinLabel(4,"2S/1S");
  //  outh->GetXaxis()->SetBinLabel(5,"3S/1S");
  
  float temp1 = ws->var("nSig1s")->getVal();  
  float temp1err = ws->var("nSig1s")->getError();  
  float temp2 = ws->var("nSig2s")->getVal();  
  float temp2err = ws->var("nSig2s")->getError();  
  float temp3 = ws->var("nSig3s")->getVal();  
  float temp3err = ws->var("nSig3s")->getError();  
  
  outh->SetBinContent(1,  temp1 ) ;
  outh->SetBinError  (1,  temp1err ) ;
  outh->SetBinContent(2,  temp2 ) ;
  outh->SetBinError  (2,  temp2err ) ;
  outh->SetBinContent(3,  temp3 ) ;
  outh->SetBinError  (3,  temp3err ) ;

  cout << "1S signal    =  " << outh->GetBinContent(1) << " +/- " << outh->GetBinError(1) << endl;
  cout << "2S signal    =  " << outh->GetBinContent(2) << " +/- " << outh->GetBinError(2) << endl;
  cout << "3S signal    =  " << outh->GetBinContent(3) << " +/- " << outh->GetBinError(3) << endl;

  setTDRStyle();
  writeExtraText = false;
  extraText = "Preliminary";

  TString label;
  label="";
  if(collId == kPPDATA) CMS_lumi_overlay(pad1, 1 ,33);
  else if(collId == kAADATA && cLow < 60) CMS_lumi_overlay(pad1, 2 ,33);
  else if(collId == kAADATA && cLow>=60) CMS_lumi_overlay(pad1, 21 ,33);

 

  pad1->Update();
  c1->cd();
  pad1->Draw();
  pad1->Update();
  c1->Modified();

  TFile* outf = new TFile(Form("PAS_overlay_RAANorm_fitresults_upsilon_DoubleCB_%s.root",kineLabel.Data()),"recreate");
  outh->Write();
  c1->SaveAs(Form("PAS_overlay_RAANorm_woSig_fitresults_upsilon_DoubleCB_%s.pdf",kineLabel.Data()));
  c1->Write();
  ws->Write();
  cout << "N, alpha, sigma1s, M0, f, X double CB for data " << endl;
  //  void setSignalParMC(float MCn_, float MCalpha_, float MCsigma1S_, float MCm0_, float MCf_, float MCx_)
  cout << Form(" else if ( binMatched( %.f, %.f, %.f, %.1f, %.1f) ) {setSignalParMC(",muPtCut, ptLow, ptHigh, yLow, yHigh);
  cout <<  ws->var("n1s_1")->getVal() << ", " <<  ws->var("alpha1s_1")->getVal() << ", "<<  ws->var("sigma1s_1")->getVal() << ", " <<  ws->var("m_{#Upsilon(1S)}")->getVal() << ", " <<  ws->var("f1s")->getVal() << ", "<<  ws->var("x1s")->getVal() << " );} " << endl;
//  outf->Close();


  ///  cout parameters :
  /*
  cout << "N, alpha, sigma1s, M0, f, X double CB for data " << endl;
  cout << "if ( (muPtCut==(float)"<< muPtCut<<") &&  ( ptLow == (float)"<< ptLow <<" ) && (ptHigh == (float)"<<ptHigh<<" ) && (yLow == (float)"<<yLow<<" ) && (yHigh == (float)"<<yHigh<<" ) )" << endl;

  //  void setSignalParMC(float MCn_, float MCalpha_, float MCsigma1S_, float MCm0_, float MCf_, float MCx_)
  cout << " {ret.setParMC( " ;
  cout <<  ws->var("n1s_1")->getVal() << ", " <<  ws->var("alpha1s_1")->getVal() << ", "<<  ws->var("sigma1s_1")->getVal() << ", " << endl;
  cout <<  ws->var("m_{#Upsilon(1S)}")->getVal() << ", " <<  ws->var("f1s")->getVal() << ", "<<  ws->var("x1s")->getVal() << " );} " << endl;
  */

} 
