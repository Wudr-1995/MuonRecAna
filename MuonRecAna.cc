#include "MuonRecAna.h"
#include "Event/LpmtElecTruthEvent.h"

MuonRecAna::MuonRecAna(TString sf = "simEvent.root",
					   TString wf = "WpEvent.root",
					   TString lf = "LsEvent.root",
					   TString spmtf = "SpmtEvent.root",
					   TString of = "RecAna.root") {
	SimName = sf;
	WpName = wf;
	LsName = lf;
	SpmtName = spmtf;
	outPath = of;
	mLSRadius = 17700.;
	mMinuit = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
	mMinuit->SetMaxFunctionCalls(1000000);
	mMinuit->SetMaxIterations(100000);
	mMinuit->SetTolerance(0.001);
}

MuonRecAna::~MuonRecAna() {
	delete mMinuit;
}

bool MuonRecAna::Fit() {

	// Load files
	TFile* simFile = TFile::Open(SimName);
	if (!simFile) {
		std::cerr << "Failed to load file " << SimName << std::endl;
		return false;
	}

	TFile* wpFile = TFile::Open(WpName);
	if (!wpFile) {
		std::cerr << "Failed to load file " << WpName << std::endl;
		return false;
	}

	TFile* lsFile = TFile::Open(LsName);
	if (!wpFile) {
		std::cerr << "Failed to load file " << LsName << std::endl;
		return false;
	}

	TFile* spmtFile = TFile::Open(SpmtName);
	if (!wpFile) {
		std::cerr << "Failed to load file " << SpmtName << std::endl;
		return false;
	}

	// Load trees
	TString simTreeName = "Event/Sim/SimEvent";
	TTree* simTree = (TTree*)(simFile->Get(simTreeName));
	if (!simTree) {
		std::cerr << "Failed to load tree " << simTreeName << " in the sim rec. file" << std::endl;
		return false;
	}

	TString wpTreeName = "Event/Rec/WPRecEvent";
	TTree* wpTree = (TTree*)(wpFile->Get(wpTreeName));
	if (!wpTree) {
		std::cerr << "Failed to load tree " << wpTreeName << " in the WP rec. file" << std::endl;
		return false;
	}

	TString lsTreeName = "Event/Rec/CDTrackRecEvent";
	TTree* lsTree = (TTree*)(lsFile->Get(lsTreeName));
	if (!lsTree) {
		std::cerr << "Failed to load tree " << lsTreeName << " in the FHT rec. file" << std::endl;
		return false;
	}

	TString spmtTreeName = "Event/Rec/CDTrackRecEvent";
	TTree* spmtTree = (TTree*)(spmtFile->Get(spmtTreeName));
	if (!spmtTree) {
		std::cerr << "Failed to load tree " << spmtTreeName << " in the SPMT rec. file" << std::endl;
		return false;
	}

	// Load branches
	JM::SimEvent* simObj = 0;
	JM::WPRecEvent* wpObj = 0;
	JM::CDTrackRecEvent* lsObj = 0;
	JM::CDTrackRecEvent* spmtObj = 0;

	simTree->SetBranchAddress("SimEvent", &simObj);
	wpTree->SetBranchAddress("WPRecEvent", &wpObj);
	lsTree->SetBranchAddress("CDTrackRecEvent", &lsObj);
	spmtTree->SetBranchAddress("CDTrackRecEvent", &spmtObj);

	simTree->GetBranch("SimEvent")->SetAutoDelete(true);
	wpTree->GetBranch("WPRecEvent")->SetAutoDelete(true);
	lsTree->GetBranch("CDTrackRecEvent")->SetAutoDelete(true);
	spmtTree->GetBranch("CDTrackRecEvent")->SetAutoDelete(true);

	// Analysis
	int nEvts = simTree->GetEntries();
	for (int i = 0; i < nEvts; i ++) {
		clog << "Processing " << i << "th events." << endl;
		simTree->GetEntry(i);
		wpTree->GetEntry(i);
		lsTree->GetEntry(i);
		spmtTree->GetEntry(i);
		const vector<JM::SimTrack*>& simTrks = simObj->getTracksVec();
		const vector<JM::RecTrack*>& wpTrks = wpObj->wpTracks();
		const vector<JM::RecTrack*>& lsTrks = lsObj->cdTracks();
		const vector<JM::RecTrack*>& spmtTrks = spmtObj->cdTracks();
		const vector<JM::SimPMTHit*>& simHits = simObj->getCDHitsVec();

		int nMuonTrks = 0;
		int size = simTrks.size();
		for (int i = 0; i < size; i ++) {
			if (simTrks[i]->getPDGID() == 13 || simTrks[i]->getPDGID() == - 13)
				nMuonTrks ++;
		}
		if (nMuonTrks > 1) {
			clog << "More than one track, skip." << endl;
			continue;
		}

		double npe = 0;
		for (JM::SimPMTHit* hit : simHits) {
			npe += hit->getNPE();
		}
		nPEtmp.push_back(npe);

		clog << "nSimTracks: " << simTrks.size() << endl
			 << "nWpTracks: " << wpTrks.size() << endl
			 << "nFHTTracks: " << lsTrks.size() << endl
			 << "nSPMTTracks: " << spmtTrks.size() << endl;
		if (simTrks.size() && wpTrks.size() && lsTrks.size() && spmtTrks.size()) {
			vector<double> X, Y, Z;

			clog << "The id of track is " << simTrks[0]->getTrackID() << "." << endl;

			TVector3 simInit(simTrks[0]->getInitX(), simTrks[0]->getInitY(), simTrks[0]->getInitZ());
			TVector3 simExit(simTrks[0]->getExitX(), simTrks[0]->getExitY(), simTrks[0]->getExitZ());
			TVector3 simDir(simTrks[0]->getInitPx(), simTrks[0]->getInitPy(), simTrks[0]->getInitPz());

			const CLHEP::HepLorentzVector& wpHInit = wpTrks[0]->start();
			const CLHEP::HepLorentzVector& wpHExit = wpTrks[0]->end();
			TVector3 wpInit(wpHInit.x(), wpHInit.y(), wpHInit.z());
			TVector3 wpExit(wpHExit.z(), wpHExit.y(), wpHExit.z());
			X.push_back(wpInit.X());
			Y.push_back(wpInit.Y());
			Z.push_back(wpInit.Z());
			X.push_back(wpExit.X());
			Y.push_back(wpExit.Y());
			Z.push_back(wpExit.Z());

			const CLHEP::HepLorentzVector& lsHInit = lsTrks[0]->start();
			const CLHEP::HepLorentzVector& lsHExit = lsTrks[0]->end();
			TVector3 lsInit(lsHInit.x(), lsHInit.y(), lsHInit.z());
			TVector3 lsExit(lsHExit.x(), lsHExit.y(), lsHExit.z());
			TVector3 lsDir = lsExit - lsInit;
			lsDir = lsDir.Unit();
			X.push_back(lsInit.X());
			Y.push_back(lsInit.Y());
			Z.push_back(lsInit.Z());
			X.push_back(lsExit.X());
			Y.push_back(lsExit.Y());
			Z.push_back(lsExit.Z());

			const CLHEP::HepLorentzVector& spmtHInit = spmtTrks[0]->start();
			const CLHEP::HepLorentzVector& spmtHExit = spmtTrks[0]->end();
			TVector3 spmtInit(spmtHInit.x(), spmtHInit.y(), spmtHInit.z());
			TVector3 spmtExit(spmtHExit.z(), spmtHExit.y(), spmtHExit.z());
			X.push_back(spmtInit.X());
			Y.push_back(spmtInit.Y());
			Z.push_back(spmtInit.Z());
			X.push_back(spmtExit.X());
			Y.push_back(spmtExit.Y());
			Z.push_back(spmtExit.Z());

			mChi2Func = new LineChi2(X, Y, Z);
			ROOT::Math::Functor* func = new ROOT::Math::Functor(*mChi2Func, 4);
			mMinuit->SetFunction(*func);

			mMinuit->SetVariable(0, "thei", lsInit.Theta(), UNIT);
			mMinuit->SetVariable(1, "phii", lsInit.Phi(), UNIT);
			mMinuit->SetVariable(2, "thed", lsDir.Theta(), UNIT);
			mMinuit->SetVariable(3, "phid", lsDir.Phi(), UNIT);
			mMinuit->SetPrintLevel(0);
			mMinuit->Minimize();

			const double* xx;
			xx = mMinuit->X();
			double chi2ndf = mMinuit->MinValue();

			TVector3 recInit, recDir;
			recInit.SetMagThetaPhi(mLSRadius, xx[0], xx[1]);
			recDir.SetMagThetaPhi(1., xx[2], xx[3]);

			SimInits.push_back(simInit);
			SimDirs.push_back(simDir.Unit());
			RecInits.push_back(recInit);
			RecDirs.push_back(recDir);

			mMinuit->Clear();

			delete mChi2Func;
			mChi2Func = NULL;
		}
	}
	return true;
}

bool MuonRecAna::Analysis() {

	int size = SimInits.size();
	for (int i = 0; i < size; i ++) {
		double a = TMath::ACos((SimDirs[i] * RecDirs[i]) / (SimDirs[i].Mag() * RecDirs[i].Mag()));
		TVector3 simMid = SimInits[i] - SimInits[i] * SimDirs[i] * SimDirs[i];
		TVector3 recMid = RecInits[i] - RecInits[i] * RecDirs[i] * RecDirs[i];
		if (simMid.Mag() > mLSRadius) {
			clog << "The track is out of the range of LS ball, skip. Distance is " << simMid.Mag() << endl;
			continue;
		}
		double d = (simMid - recMid).Mag();
		double len = 2 * TMath::Sqrt(mLSRadius * mLSRadius - simMid.Mag() * simMid.Mag());
		if (len < 8000 || nPEtmp[i] / len > 5500) {
			clog << "The event is stoped in the CD, or it is a shower event, skip." << endl;
			continue;
		}
		as.push_back(a);
		ds.push_back(d);
		sds.push_back(simMid.Mag());
		rds.push_back(recMid.Mag());
		lens.push_back(len);
		nPEs.push_back(nPEtmp[i]);
	}

	return true;
}

bool MuonRecAna::DumpResult() {

	ofstream outFile(outPath, std::ios::app);
	int size = as.size();
	for (int i = 0; i < size; i ++) {
		outFile << as[i] << "\t"
				<< ds[i] << "\t"
				<< sds[i] << "\t"
				<< rds[i] << "\t"
				<< lens[i] << "\t"
				<< nPEs[i] << endl;
	}
	outFile.close();

	return true;
}
