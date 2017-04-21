#pragma once
#include "StochasticProcess.h"

// generalize for multidim
class BlackScholesProcess : public StochasticProcess
{
public:

	BlackScholesProcess(double r, double q, double v, double T)
	{
		riskFree = r;
		dividend = q;
		sigma = v;
		tau = T;
	}

	virtual ~BlackScholesProcess(void) {}

	virtual int dim() const {return 1;}
	virtual int factors() const {return 1;}

	virtual double drift() const {return riskFree - dividend - 0.5 * sigma * sigma;}
	virtual double diffusion() const {return sigma;}
	virtual double stdDeviation() const {return sigma * sqrt(tau);}

private:
//	double s0;
	double riskFree;
	double dividend;
	double sigma;
	double tau;
};
