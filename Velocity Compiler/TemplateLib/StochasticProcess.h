#pragma once

// generalize for multidim
class StochasticProcess
{
public:
	virtual ~StochasticProcess(void) {}

	virtual int dim() const = 0;
	virtual int factors()  const = 0;

	virtual double drift() const = 0;
	virtual double diffusion()  const = 0;
	virtual double stdDeviation() const = 0;
};
