#ifndef PricingCalculatorSPO_h
#define PricingCalculatorSPO_h
/**
 * @file
 * 
 * This implements a base model for supporting the pricing adapter interface.
 *
 * @author Richard Lewis
 *
 * 2009 (c) Razor Risk Technologies - all rights reserved. No part of this program to be reproduced without permission of Razor Risk Technologies.
 */

#pragma warning (disable: 4786)

#include "PricingCalculator.h"

// metatype includes
#include <MetatypeSupport.h>
#include <FinMark.h>

class VelocityConfig;

// standard includes
#include <string>

// JNI 1.5
#include <jni.h>

class PricingCalculatorSPO : public PricingCalculator {
  public:

	//types
	struct TradeTransitionCache
	{
		bool compile;
	};

	//inherited methods
	virtual const char * getId() { return "SPO"; }
	virtual rq_date getFinalMaturityDate(const struct rq_pricing_request *pricing_request);

	virtual void getProductMarketRequirements(const struct rq_pricing_request *pricing_request, rq_market_requirements_t market_requirements);

	virtual void* allocPricingAdapterCache();
	virtual void freePricingAdapterCache(void* cache);
	virtual void* allocTradeTransitionCache(const struct rq_pricing_request *pricing_request);
	virtual void freeTradeTransitionCache(void *cache);
	virtual void clearTradeTransitionCache(void *cache);

	static std::string s_yldCurveType;
	static std::string s_fwdCurveType;
	//static std::string s_volSurfaceType;

  protected:

	//product specific interface
	virtual short validateProduct(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result);
	virtual void getProductFaceValue(const struct rq_pricing_request *pricing_request, FaceValue &fv);
	virtual double getProductMtm(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result = NULL);
	virtual void getProductNamedValues(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result, unsigned long all);
	virtual void getProductPricingValues(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result, unsigned long all, double mtm);

  private:
	//hidden methods
	double calculatePricingResults(const struct rq_pricing_request *pricing_request,
								   double *delta,
								   double *gamma,
								   double *vega,
								   double *rho,
								   double *theta,
								   double *iSpot,
								   double *iStrike,
								   double *iVol,
								   double *iIR,
								   char* iCcy,
								   double *iTauE,
								   double *iRepExRate,
								   double *iModelPrice,
								   double *iFwd);

	const std::string getATM(bool isSpot, bool isCall, double spotFwd, double strike, double maturityTau);  

	// returns true if file's last write time is greater than the supplied datetime
	bool isOlder(const std::string& fname, const std::string& date);
	// 
	const std::string getFilePath(const VelocityConfig* vc, const std::string& tmplName, 
								  const std::string& ext, const std::string& tradeId="");

	bool compile(const VelocityConfig* vc, const std::string& tmp, const std::string& tradeId,
				 const std::string& script, const std::string& timeStamp, std::string& message);

	bool price(const VelocityConfig* vc, const std::string& tmpl, const std::string& tradeId, double& p, double& err);

	JavaVM* jvm;       /* denotes a Java VM */
	JNIEnv* env;
};

#endif


