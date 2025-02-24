#include <cstdlib>
#include <iostream>
#include <chrono>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TDatabasePDG.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TH1.h>
#include <TH2.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TBenchmark.h>
#include <bitset>
#include "clas12reader.h"
#include "jsonFileMerger.h"
#include "clas12databases.h"
#include "QADB.h"

// this file create a root file with ttree with all the information for QA DB from hipo files
const int nToProcess = -1;
void processHipo(TString inputFile);



void NickSkim(){
  TString inputFile;
  int isHipo = -1;
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    if((opt.Contains(".dat") || opt.Contains(".txt"))){
      inputFile=opt(5,opt.Sizeof());
      isHipo = 1;
    }
    else if(opt.Contains(".root")){
      inputFile=opt(5,opt.Sizeof());
      isHipo = 0;
    } 
  }
  cout << "isHipo: "<< isHipo << endl;
  if(isHipo < 0)  {
    std::cout << " *** please provide a root or text input file name..." << std::endl;
    exit(0);
  }
  processHipo(inputFile);
}



void processHipo(TString inputFile){
    
  double Ebeam = 10.6041;

  double M_p = 0.93827;
    
  bool isData = 0;
  bool isEmptyTarget = 0;
  bool isKinemAndFidCut = 0;


    

  vector<double> px_prot_gen;
  vector<double> py_prot_gen;
  vector<double> pz_prot_gen;

  vector<double> px_prot_rec;
  vector<double> py_prot_rec;
  vector<double> pz_prot_rec;


  vector<double> vx_prot;
  vector<double> vy_prot;
  vector<double> vz_prot;



  TFile outFile(Form("../../data_test/%s_.root",inputFile.Data()), "recreate");
  TTree out_tree("out_tree","out_tree");
                                                                                                                                                                                                   


  out_tree.Branch("px_prot_gen", &px_prot_gen);
  out_tree.Branch("py_prot_gen", &py_prot_gen);
  out_tree.Branch("pz_prot_gen", &pz_prot_gen);

  out_tree.Branch("px_prot_rec", &px_prot_rec);
  out_tree.Branch("py_prot_rec", &py_prot_rec);
  out_tree.Branch("pz_prot_rec", &pz_prot_rec);

  out_tree.Branch("vx_prot", &vx_prot);
  out_tree.Branch("vy_prot", &vy_prot);
  out_tree.Branch("vz_prot", &vz_prot);
 


  HipoChain chain;
  TString nextFile;
  ifstream chainIn(inputFile);
  while (chainIn >> nextFile){
    chain.Add(nextFile);
  }
  int counter = 0;
  long long counter_generated = 0;
  auto config_c12=chain.GetC12Reader(); //in case you want to configure, use this

  config_c12->applyQA("pass1");//GETPASSSTRINGHERE="latest", "pass1, "pass2",...
  config_c12->db()->qadb_addQARequirement("MarginalOutlier");
  config_c12->db()->qadb_addQARequirement("TotalOutlier");
  config_c12->db()->qadb_addQARequirement("TerminalOutlier");
  config_c12->db()->qadb_addQARequirement("MarginalOutlier");
  config_c12->db()->qadb_addQARequirement("SectorLoss");
  config_c12->db()->qadb_addQARequirement("LowLiveTime");

  
  /*
   
  if(config_c12->qadb()!=nullptr && isEmptyTarget != 1){
    config_c12->db()->qadb_requireGolden(true);
    config_c12->applyQA();
  }
  */

  if (isData) config_c12->addAtLeastPid(11,1);
    
    
  gBenchmark->Start("db");
  auto& c12=chain.C12ref();
  while(chain.Next()){
    counter++;
    if (counter%1000000 == 0) cout << "processed "<< counter/1000000 << "M events" << endl;
    if (counter == nToProcess) break;

    
    px_prot_gen.clear();
    py_prot_gen.clear();
    pz_prot_gen.clear();

    px_prot_rec.clear();
    py_prot_rec.clear();
    pz_prot_rec.clear();

    vx_prot.clear();
    vy_prot.clear();
    vz_prot.clear();





    auto eventbank = c12->event();
    auto runconfigbank = c12->runconfig();
    if (isData){
        int nEvent = runconfigbank->getEvent();
        int runNumber = runconfigbank->getRun();
    }
    // auto particles = c12->getDetParticles();
    auto proton=c12->getByID(2212);
    
    if (proton.size()==0) continue; // discard events with no protons
    for (int u = 0; u < proton.size(); u++){
        
      
      
 	      double p_px = proton[u]->par()->getPx();
 	      double p_py = proton[u]->par()->getPy();
      	double p_pz = proton[u]->par()->getPz();

        double p_E = sqrt(proton[u]->par()->getPx()*proton[u]->par()->getPx() + proton[u]->par()->getPy()*proton[u]->par()->getPy() + proton[u]->par()->getPz()*proton[u]->par()->getPz() + M_p*M_p);
        if (proton[u]->getRegion() == FD){
      

	        vx_prot.push_back(proton[u]->par()->getVx());
	        vy_prot.push_back(proton[u]->par()->getVy());
	        vz_prot.push_back(proton[u]->par()->getVz());

	        p4_ele_px.push_back(e_px);
	        p4_ele_py.push_back(e_py);
	        p4_ele_pz.push_back(e_pz);
	        p4_ele_E.push_back(e_E);
        
        
	        pcalHX.push_back(electron[u]->cal(PCAL)->getHx());
	        pcalHY.push_back(electron[u]->cal(PCAL)->getHy());
	        pcalHZ.push_back(electron[u]->cal(PCAL)->getHz());
	        pcalLu.push_back(electron[u]->cal(PCAL)->getLu());
	        pcalLv.push_back(electron[u]->cal(PCAL)->getLv());
	        pcalLw.push_back(electron[u]->cal(PCAL)->getLw());  
	        ecinHX.push_back(electron[u]->cal(ECIN)->getHx());
	        ecinHY.push_back(electron[u]->cal(ECIN)->getHy());
	        ecinHZ.push_back(electron[u]->cal(ECIN)->getHz());
	        ecoutHX.push_back(electron[u]->cal(ECOUT)->getHx());
	        ecoutHY.push_back(electron[u]->cal(ECOUT)->getHy());
	        ecoutHZ.push_back(electron[u]->cal(ECOUT)->getHz());
	        pcalE.push_back(electron[u]->cal(PCAL)->getEnergy());
	        ecinE.push_back(electron[u]->cal(ECIN)->getEnergy());
	        ecoutE.push_back(electron[u]->cal(ECOUT)->getEnergy());
	        ele_chi2.push_back(electron[u]->par()->getChi2Pid());
        
          ftofHX.push_back(electron[u]->sci(FTOF1B)->getHX());
          ftofHY.push_back(electron[u]->sci(FTOF1B)->getHY());
        
          sectorSci.push_back(electron[u]->sci(FTOF1B)->getSector());
          layerSci.push_back(electron[u]->sci(FTOF1B)->getLayer());
          compSci.push_back(electron[u]->sci(FTOF1B)->getComponent());
        }  
      }
    auto mcpbank=c12->mcparts();
    for(Int_t i=0;i<mcpbank->getRows();i++){
      mcpbank->setEntry(i);
      if ( mcpbank->getPid() == 11){
    	vMC_px.push_back(mcpbank->getPx());
    	vMC_py.push_back(mcpbank->getPy());
    	vMC_pz.push_back(mcpbank->getPz());
        cout<<mcpbank->getPx()<<endl;
      }
    }
    counter_generated++;
    out_tree.Fill();
  }
  
  cout <<"Accumulated charge post QA 2: " << chain.TotalBeamCharge()<<" nC"<<endl;
  cout << "Accumulated charge good 2: " << chain.db()->qa()->getAccCharge() << " nC" << endl;

    
  outFile.cd();
  outFile.Write();
  outFile.Close();
}
int PCALFidXY(float x, float y, int cutLevel){
  int fidCut = 0;
  float cutLimit = 0;
  if (cutLevel == 0) cutLimit = 260;
  if (cutLevel == 1) cutLimit = 250;
  if (y > -0.5*(x + cutLimit) && y < +0.5*(x + cutLimit)) fidCut = 1;
  return fidCut;
}