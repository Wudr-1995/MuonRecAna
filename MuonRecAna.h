#ifndef MuonRecAna_h
#define MuonRecAna_h

#include <iostream>
#include <map>
#include "TFile.h"
#include "TTree.h"
#include "Event/CalibEvent.h"
#include "Event/ElecEvent.h"
#include "Event/SimEvent.h"
#include "Event/WPRecEvent.h"
#include "Event/CDTrackRecEvent.h"
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
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "LineChi2.h"
#include <fstream>

#define UNIT TMath::Pi() / 180
#define PI TMath::Pi()

using namespace std;

class MuonRecAna {
	public:
		MuonRecAna(TString, TString, TString, TString, TString);
		~MuonRecAna();

		bool Fit();
		bool Analysis();
		bool DumpResult();

	private:
		TString SimName;
		TString WpName;
		TString LsName;
		TString SpmtName;
		TString outPath;

		double mLSRadius;

		vector<TVector3> RecInits;
		vector<TVector3> RecDirs;
		vector<TVector3> SimInits;
		vector<TVector3> SimDirs;
		vector<double> nPEtmp;

		vector<double> as;
		vector<double> ds;
		vector<double> sds;
		vector<double> rds;
		vector<double> lens;
		vector<double> nPEs;

		vector<double> aBias;
		vector<double> aRes;
		vector<double> dBias;
		vector<double> dRes;

		ROOT::Math::Minimizer* mMinuit;
		LineChi2* mChi2Func;
};

#endif
