#include "EleCalibAna.h"
#include "TCanvas.h"

int FormatGraph(TGraph* g) {
	g->SetTitle("");
	g->GetXaxis()->SetTitleSize(0.08);
	g->GetXaxis()->SetLabelSize(0.06);
	g->GetXaxis()->CenterTitle();
	g->GetYaxis()->SetTitleSize(0.08);
	g->GetYaxis()->SetLabelSize(0.06);
	g->GetYaxis()->CenterTitle();
	return 1;
}

int FormatHist(TH1* h) {
	h->SetTitle("");
	h->GetXaxis()->SetTitleSize(0.08);
	h->GetXaxis()->SetLabelSize(0.04);
	h->GetXaxis()->CenterTitle();
	h->GetYaxis()->SetTitleSize(0.08);
	h->GetYaxis()->SetLabelSize(0.06);
	h->GetYaxis()->CenterTitle();
	return 1;
}

int main(int argc, char** argv) {
	if (argc != 7) {
		cout << "Wrong number of input arguments" << endl;
		return 0;
	}

	TString ef(argv[1]);
	TString cf(argv[2]);
	TString of(argv[3]);

	int rl, rh;
	sscanf(argv[4], "%d", &rl);
	sscanf(argv[5], "%d", &rh);

	TString wf(argv[6]);

	EleCalibAna* pro = new EleCalibAna(ef, cf, rl, rh, wf);

	if (!pro->DumpElecTruth()) {
		cout << "DumpElecTruth: Input error." << endl;
		return 0;
	}
	if (!pro->DumpCalib()) {
		cout << "DumpCalib: Input error." << endl;
		return 0;
	}
	if (!pro->DumpElecWave()) {
		cout << "DumpWave: Input error." << endl;
		return 0;
	}

	TGraph* PEs = pro->DrawPE();
	TGraph* Ts = pro->DrawT();
	TH1F* DPEs = pro->DrawDPE();
	TH1F* DTs = pro->DrawDT();
	// TH1F* wave = pro->DrawWave();
	double dnPE = pro->getDiffnPE();
	cout << "Difference of nPE: " << dnPE << endl;

	auto c = new TCanvas();
	c->SetRightMargin(0.1);
	c->SetLeftMargin(0.2);
	c->SetTopMargin(0.1);
	c->SetBottomMargin(0.2);
	c->Print(of + "[");

	c->cd();
	FormatGraph(PEs);
	PEs->SetTitle("");
	PEs->GetXaxis()->SetTitle("True nPE");
	PEs->GetYaxis()->SetTitle("Rec nPE");
	PEs->GetYaxis()->SetRangeUser(0, 15000);
	PEs->Draw("AP");
	c->Print(of);

	c->cd();
	FormatGraph(Ts);
	Ts->SetTitle("");
	Ts->GetXaxis()->SetTitle("True hit time / ns");
	Ts->GetYaxis()->SetTitle("Rec hit time / ns");
	Ts->Draw("AP");
	c->Print(of);

	c->cd();
	c->SetLogy();
	FormatHist(DPEs);
	DPEs->GetXaxis()->SetRangeUser(-5000, 750);
	DPEs->GetXaxis()->SetTitle("#Delta nPE (Truth - Rec.)");
	DPEs->Draw();
	c->Print(of);

	c->cd();
	FormatHist(DTs);
	DTs->GetXaxis()->SetRangeUser(-500, 500);
	DTs->GetXaxis()->SetTitle("#Delta Hit time (Truth - Rec.)");
	DTs->Draw();
	c->Print(of);

	// c->cd();
	// gStyle->SetOptStat(0000);
	// FormatHist(wave);
	// wave->GetXaxis()->SetTitle("Time / ns");
	// wave->GetYaxis()->SetTitle("Amplitude / (adc unit)");
	// wave->Draw();
	// c->Print(of);

	c->Print(of +"]");

	return 1;
}
