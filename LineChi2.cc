#include "LineChi2.h"

LineChi2::LineChi2(vector<double>& x, vector<double>& y, vector<double>& z)
	: X(x),
	  Y(y),
	  Z(z),
	  mLSR(17700)
{}

double LineChi2::operator()(const double* xx) {
	TVector3 inci, dir;
	inci.SetMagThetaPhi(mLSR, xx[0], xx[1]);
	dir.SetMagThetaPhi(1., xx[2], xx[3]);

	int size = X.size();
	double ndf = 0;
	double chi2 = 0;
	for (int i = 0; i < size; i ++) {
		chi2 += (TVector3(X[i], Y[i], Z[i]) - inci).Cross(dir).Mag();
		ndf ++;
	}
	ndf -= 4;
	return chi2 / ndf;
}
