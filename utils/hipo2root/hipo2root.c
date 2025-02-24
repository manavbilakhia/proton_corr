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
#include "clas12reader.h"
#include <TLine.h>
#include <TNtuple.h>

#include <cmath>
#include <iomanip>  
#include <fstream>  // Include for file reading


using namespace clas12;

void ProcessHipo(TString inputFile);

void hipo2root(){
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
  ProcessHipo(inputFile);
}



void ProcessHipo(TString inputFile) {

  auto db = TDatabasePDG::Instance();

  // Variables for TTree branches
  float px_prot_gen, py_prot_gen, pz_prot_gen, p_proton_gen;
  float px_prot_rec, py_prot_rec, pz_prot_rec, p_proton_rec;
  float vx_prot, vy_prot, vz_prot;
  int pid_proton, status_proton;

  TFile outFile(Form("../../data_test/%s.root", inputFile.Data()), "recreate");
  TTree out_tree("out_tree", "out_tree");

  out_tree.Branch("px_prot_gen", &px_prot_gen);
  out_tree.Branch("py_prot_gen", &py_prot_gen);
  out_tree.Branch("pz_prot_gen", &pz_prot_gen);
  out_tree.Branch("px_prot_rec", &px_prot_rec);
  out_tree.Branch("py_prot_rec", &py_prot_rec);
  out_tree.Branch("pz_prot_rec", &pz_prot_rec);
  out_tree.Branch("p_proton_gen", &p_proton_gen);
  out_tree.Branch("p_proton_rec", &p_proton_rec);
  out_tree.Branch("vx_prot", &vx_prot);
  out_tree.Branch("vy_prot", &vy_prot);
  out_tree.Branch("vz_prot", &vz_prot);
  out_tree.Branch("pid_proton", &pid_proton);
  out_tree.Branch("status_proton", &status_proton);

  // Start timing
  auto start = std::chrono::high_resolution_clock::now();
  gBenchmark->Start("timer");
  int counter = 0;

  std::cout << "Reading file example program (HIPO) " << __cplusplus << std::endl;

  // Read file paths from input file
  std::ifstream file(inputFile.Data());
  if (!file.is_open()) {
      std::cerr << "Error opening file: " << inputFile << std::endl;
      exit(1);
  }

  std::vector<std::string> data;
  std::string line;
  while (std::getline(file, line)) {
      if (!line.empty()) data.push_back(line);
  }
  file.close();

  // Process each HIPO file
  for (const auto& filePath : data) {
      hipo::reader reader;
      reader.open(filePath.c_str());

      hipo::dictionary factory;
      reader.readDictionary(factory);

      hipo::event event;
      hipo::bank REC_particle(factory.getSchema("REC::Particle"));
      hipo::bank MC_event(factory.getSchema("MC::Event"));
      hipo::bank MC_particle(factory.getSchema("MC::Particle"));

      while (reader.next() == true) {
          counter++;
          if (counter == 1) continue;

          reader.read(event);
          event.getStructure(REC_particle);
          event.getStructure(MC_event);
          event.getStructure(MC_particle);

          int N = REC_particle.getRows();
          std::vector<int> pid(N), status(N);
          for (int i = 0; i < N; i++) {
              pid[i] = REC_particle.getInt("pid", i);
              status[i] = REC_particle.getInt("status", i);
          }

          auto it_proton = std::find(pid.begin(), pid.end(), 2212);
          if (it_proton == pid.end()) continue; // If no proton found

          int index = std::distance(pid.begin(), it_proton);
          px_prot_gen = MC_particle.getFloat("px", index);
          py_prot_gen = MC_particle.getFloat("py", index);
          pz_prot_gen = MC_particle.getFloat("pz", index);

          px_prot_rec = REC_particle.getFloat("px", index);
          py_prot_rec = REC_particle.getFloat("py", index);
          pz_prot_rec = REC_particle.getFloat("pz", index);

          p_proton_rec = std::sqrt(px_prot_rec * px_prot_rec + py_prot_rec * py_prot_rec + pz_prot_rec * pz_prot_rec);
          p_proton_gen = std::sqrt(px_prot_gen * px_prot_gen + py_prot_gen * py_prot_gen + pz_prot_gen * pz_prot_gen);

          vx_prot = REC_particle.getFloat("vx", index);
          vy_prot = REC_particle.getFloat("vy", index);
          vz_prot = REC_particle.getFloat("vz", index);

          status_proton = status[index];
          pid_proton = pid[index];

          out_tree.Fill();
      }
  }

  outFile.Write();
  outFile.Close();
  std::cout << "Wrote data into a ROOT file." << std::endl;
}




