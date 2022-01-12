#include "MuonRecAna.h"
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
	if (argc != 6) {
		cout << "Wrong number of input arguments" << endl;
		return 0;
	}

	TString sf(argv[1]);
	TString wf(argv[2]);
	TString lf(argv[3]);
	TString spmtf(argv[4]);
	TString of(argv[5]);

	MuonRecAna* recTool = new MuonRecAna(sf, wf, lf, spmtf, of);
	clog << "Fitting......" << endl;
	recTool->Fit();
	clog << "Analysising......" << endl;
	recTool->Analysis();
	clog << "Dumping......" << endl;
	recTool->DumpResult();

	return 1;
}
