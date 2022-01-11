#ifndef EleCalibAna_h
#define EleCalibAna_h

#include <iostream>
#include <map>
#include "TFile.h"
#include "TTree.h"
#include "Event/CalibEvent.h"
#include "Event/ElecEvent.h"
#include "TString.h"
#include "Identifier/Identifier.h"
#include "Identifier/CdID.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TMath.h"
#include "TH2F.h"
#include <stdio.h>
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"

using namespace std;

class EleCalibAna {
	public:
		EleCalibAna(TString, TString, int, int, TString);
		~EleCalibAna();

		bool DumpElecTruth();
		bool DumpCalib();
		bool DumpElecWave();

		TGraph* DrawPE();
		TGraph* DrawT();
		TH1F* DrawDPE();
		TH1F* DrawDT();
		TH1F* DrawWave();
		double getDiffnPE();

	private:
		map<unsigned int, double> truthPEs;
		map<unsigned int, double> truthTs;
		map<unsigned int, double> calibPEs;
		map<unsigned int, double> calibTs;

		TString EleName;
		TString CalibName;
		TString outFile;

		TH1F* WaveExa;

		int size;
		int rl;
		int rh;

		double tnPE;
		double cnPE;
};

#endif
