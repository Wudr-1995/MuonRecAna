#ifndef LineChi2_h
#define LineChi2_h 1
#include "TVector3.h"
#include <vector>

using namespace std;

class LineChi2 {
	public:
		LineChi2(vector<double>&, vector<double>&, vector<double>&);
		double operator()(const double* xx);

	private:
		vector<double>& X;
		vector<double>& Y;
		vector<double>& Z;
		double mLSR;
};

#endif
