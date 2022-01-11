#include "EleCalibAna.h"
#include "Event/LpmtElecTruthEvent.h"

EleCalibAna::EleCalibAna(TString ef, TString cf, int l, int h, TString wf) {
	EleName = ef;
	CalibName = cf;
	rl = l;
	rh = h;
	tnPE = 0;
	cnPE = 0;
	outFile = wf;
}

EleCalibAna::~EleCalibAna() {}

bool EleCalibAna::DumpElecTruth() {
	TFile* f = TFile::Open(EleName);
	if (!f) {
		std::cerr << "Failed to load file " << f << std::endl;
		return false;
	}
	TString treename = "Event/Sim/Truth/LpmtElecTruthEvent";
	TTree* lpmttruth_tree = (TTree*)(f->Get(treename));
	if (!lpmttruth_tree) {
		std::cerr << "Failed to load tree " << treename << std::endl;
		return false;
	}

	JM::LpmtElecTruthEvent* truthobj = 0;
	lpmttruth_tree->SetBranchAddress("LpmtElecTruthEvent", &truthobj);
	lpmttruth_tree->GetBranch("LpmtElecTruthEvent")->SetAutoDelete(true);

	for (int evti = rl; evti < lpmttruth_tree->GetEntries() && evti < rh; ++evti) {
		lpmttruth_tree->GetEntry(evti);
		cout << "ElecTruth Event: " << evti << endl;
		const std::vector<JM::LpmtElecTruth>& truths = truthobj->truths();

		for (int i = 0; i < truths.size(); ++i) {
			unsigned int id = truths[i].pmtId();
			truthPEs[id] += truths[i].npe();
			tnPE += truths[i].npe();
			if (truthTs[id])
				truthTs[id] = truths[i].hitTime() < truthTs[id] ? truths[i].hitTime() : truthTs[id];
			else
				truthTs[id] = truths[i].hitTime();
		}
	}
	return true;
}

bool EleCalibAna::DumpCalib() {
	TFile* f = TFile::Open(CalibName);
	if (!f) {
		cerr << "Filed to load file " << f << endl;
		return false;
	}
	TString treeName = "Event/Calib/CalibEvent";
	TTree* evtsTree = (TTree*)(f->Get(treeName));
	if (!evtsTree) {
		cerr << "Filed to load tree " << treeName << endl;
		return false;
	}

	JM::CalibEvent* calibs = 0;
	evtsTree->SetBranchAddress("CalibEvent", &calibs);
	evtsTree->GetBranch("CalibEvent")->SetAutoDelete(true);

	for (int i = rl; i < evtsTree->GetEntries() && i < rh; i ++) {
		evtsTree->GetEntry(i);
		cout << "Rec Event: " << i << endl;
		const list<JM::CalibPMTChannel*>& calib = calibs->calibPMTCol();
		list<JM::CalibPMTChannel*>::const_iterator it;

		for (it = calib.begin(); it != calib.end(); it ++) {
			Identifier id = Identifier((*it)->pmtId());
			unsigned pid = CdID::module(id);
			Identifier::value_type value = id.getValue();
			if (not ((value & 0xFF000000) >> 24 == 0x10))
				continue;
			calibPEs[pid] += (*it)->nPE();
			cnPE += (*it)->nPE();
			if (calibTs[pid])
				calibTs[pid] = (*it)->firstHitTime() < calibTs[pid] ? (*it)->firstHitTime() : calibTs[pid];
			else
				calibTs[pid] = (*it)->firstHitTime();
		}
	}
	return true;
}

bool EleCalibAna::DumpElecWave() {
	TFile* f = TFile::Open(EleName);
	if (!f) {
		cerr << "Failed to load file " << f << endl;
		return false;
	}
	TString treename = "Event/Elec/ElecEvent";
	TTree* elecEvent_tree = (TTree*)(f->Get(treename));
	if (!elecEvent_tree) {
		cerr << "Failed to load tree " << treename << endl;
		return false;
	}

	JM::ElecEvent* elecObj = 0;
	elecEvent_tree->SetBranchAddress("ElecEvent", &elecObj);
	elecEvent_tree->GetBranch("ElecEvent")->SetAutoDelete(true);

	for (int i = rl; i < elecEvent_tree->GetEntries() && i < rh; i ++) {
		elecEvent_tree->GetEntry(i);
		const JM::ElecFeeCrate &efc = elecObj->elecFeeCrate();
		JM::ElecFeeCrate* EFC = const_cast<JM::ElecFeeCrate*>(&efc);

		map<int, JM::ElecFeeChannel>& chs = EFC->channelData();
		map<int, JM::ElecFeeChannel>::iterator it;

		TCanvas* c = new TCanvas("Waveforms", "", 2000, 2000);
		c->Divide(5, 5);
		c->Print(outFile + "[");
		int nTh = 1;
		int pages = 0;
		for (it = chs.begin(); it != chs.end(); it ++) {
			JM::ElecFeeChannel &ch = (it->second);
			vector<unsigned> &adcInt = ch.adc();
			if (WaveExa == NULL) {
				WaveExa = new TH1F("waveform", "", adcInt.size(), 0, adcInt.size());
				WaveExa->GetXaxis()->SetTitle("Time / ns");
				WaveExa->GetYaxis()->SetTitle("Amplitude / (adc unit)");
			}

			size = adcInt.size();

			double baseline = 0;
			for (int j = 0; j < 50; j ++)
				baseline += adcInt.at(j);
			baseline /= 50;

			double amp = 0;
			for (int j = 0; j < adcInt.size(); j ++) {
				WaveExa->SetBinContent(j + 1, adcInt.at(j) - baseline);
				// WaveExa->SetBinContent(j + 1, adcInt.at(j) < 1E7 ? adcInt.at(j) - baseline : 0);
				if (amp < WaveExa->GetBinContent(j))
					amp = WaveExa->GetBinContent(j);
			}
			if (WaveExa) {
				c->cd(nTh);
				gPad->SetRightMargin(0.1);
				gPad->SetLeftMargin(0.2);
				gPad->SetTopMargin(0.1);
				gPad->SetBottomMargin(0.2);
				// cout << nTh << " Amplitude: " << WaveExa->GetMaximumBin() << endl;
				TString tmp("Waveform");
				WaveExa->Clone(tmp + nTh)->Draw();
				nTh ++;
				if (nTh == 26) {
					c->Print(outFile);
					pages ++;
					nTh = 1;
				}
			}
			// delete WaveExa;
			// WaveExa = NULL;
			if (pages > 10)
				break;
			// if (WaveExa && it != chs.begin() && amp > 1500)
			// 	break;
		}
		c->Print(outFile + "]");
	}
	return true;
}

TGraph* EleCalibAna::DrawPE() {
	vector<double> tPE;
	vector<double> recPE;

	for (map<unsigned, double>::iterator it = truthPEs.begin(); it != truthPEs.end(); it ++) {
		tPE.push_back(it->second);
		recPE.push_back(calibPEs[it->first]);
	}

	TGraph* ret = new TGraph(tPE.size(), &tPE[0], &recPE[0]);
	return ret;
}

TGraph* EleCalibAna::DrawT() {
	vector<double> tT;
	vector<double> recT;
	for (map<unsigned, double>::iterator it = truthTs.begin(); it != truthTs.end(); it ++) {
		tT.push_back(it->second);
		recT.push_back(calibTs[it->first]);
	}

	TGraph* ret = new TGraph(tT.size(), &tT[0], &recT[0]);
	return ret;
}

TH1F* EleCalibAna::DrawDPE() {
	TH1F* ret = new TH1F("DeltaPE", "", 10000, -5000, 5000);

	for (map<unsigned, double>::iterator it = truthPEs.begin(); it != truthPEs.end(); it ++) {
		ret->Fill(it->second - calibPEs[it->first]);
	}

	return ret;
}

TH1F* EleCalibAna::DrawDT() {
	TH1F* ret = new TH1F("DeltaT", "", 1000, -5000, 5000);

	for (map<unsigned, double>::iterator it = truthTs.begin(); it != truthTs.end(); it ++) {
		ret->Fill(it->second - calibTs[it->first]);
	}

	return ret;
}

TH1F* EleCalibAna::DrawWave() {
	TH1F* ret = new TH1F("ret", "", size, 0, size);
	for (int i = 1; i <= size; i ++)
		ret->SetBinContent(i, WaveExa->GetBinContent(i));
	return ret;
}

double EleCalibAna::getDiffnPE() {
	cout << "True nPE: " << tnPE << endl
		 << "Calib nPE: " << cnPE << endl;
	return tnPE - cnPE;
}
