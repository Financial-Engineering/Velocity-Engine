/**
* 2009 (c) Razor Risk Technologies - all rights reserved. No part of this program to be reproduced without permission of Razor Risk Technologies.
*/
#pragma warning (disable: 4786)

#include <rq.h>
#include <stdlib.h>

// common includes
#include <MathUtils.h>


// local includes
#include "PricingCalculatorSPO.h"
#include <PricingHelper.h>
#include <ValidationHelper.h>
#include <FinmarkHelper.h>
#include <RazorPricingContext.h>


// DLL loader
#define _WIN32_WINNT 0x0502
#include "Windows.h"
#include "Mmsystem.h"

#include "Template.h"

// Boost
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scoped_array.hpp>

// STL
#include <set>
#include <list>
#include <fstream>
#include <strstream>
#include <streambuf>

typedef rzmlScriptedProduct ScriptedProduct;

//static
std::string PricingCalculatorSPO::s_yldCurveType = "Reval";
std::string PricingCalculatorSPO::s_fwdCurveType = "FX";

// public accessors
rq_date 
PricingCalculatorSPO::getFinalMaturityDate(const struct rq_pricing_request *pricing_request)
{
	const Trade *trade = (const Trade *)pricing_request->trade_details;
	const ScriptedProduct *spoption = trade->getProductScriptedProduct();
	if (spoption)
		return spoption->getExpirationDate()->getUnadjustedDate().julian();
	else
		return 0;
}

short 
PricingCalculatorSPO::validateProduct(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result)
{
	short valid = 1;

	const Trade *trade = (const Trade *)pricing_request->trade_details;
	rq_asset_mgr_t asset_mgr = rq_system_get_asset_mgr(pricing_request->system);

	if (!isValidTradeHeader(trade, pricing_result))
	{
		valid = 0;
	}

	const ScriptedProduct *spo = trade->getProductScriptedProduct();

	// Product exists
	if (!spo)
	{
		rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "No product information");
		valid = 0;
	}
	else
	{

		// ExpiryDate valid
		if (this->getFinalMaturityDate(pricing_request) == 0)
		{
			rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "No expiry date");
			valid = 0;
		}
		else if ((this->getFinalMaturityDate(pricing_request) - pricing_request->value_date) < 0)
		{
			rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "Trade has expired");
			valid = 0;	
		} 

		// Currency defined
		const fpmlAmount* fv=spo->getFaceValue(); 
		if (!fv->getCurrency() ||
			RQ_IS_NULL(rq_asset_ccy_find(asset_mgr, fv->getCurrency())))
		{
			if (!fv->getCurrency())
				rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "No currency specified");
			else
			{
				std::string ccy = "Option Currency ";
				ccy += fv->getCurrency();
				ccy += " is not found in the market";
				rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, ccy.c_str());
			}

			valid = 0;
		}

		// Amount valid
		if (ISNAN(fv->getAmount()) || fv->getAmount() <= 0.0)
		{
			rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "amount <= 0");
			valid = 0;
		}

		const rzmlScript* script = spo->getScript();

		if (script) {



			RazorPricingContext* ctx = (RazorPricingContext*)pricing_request->application_data;

			if (ctx) {
				const VelocityConfig* vc = ctx->getSystem()->getVelocityConfig();
				if (vc) {
					// recompile if script timestamp is newer than generated CPP file
					// move this to the compile wrapper
					std::string message = "";
					bool status = compile(vc,spo->getTemplateId(),trade->getTradeHeaderTradeId(),
						script->getScriptText(),script->getTimeStamp(), message);
					if (!status) {
						rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "Velocity Compiler Error");
						//
						// TODO: retrieve error log and inject into pricing_result
						//
						valid=0;
					}
				}
				else {
					rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "Velocity Config not found");
					valid=0;
				}
			}
			else {
				rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "Missing Pricing Context");
				valid=0;
			}
		}
		else {
			rq_pricing_result_add_message(pricing_result, RQ_PRICING_RESULTS_VALIDATE, RQ_ERROR_SEVERITY_CRITICAL, "Missing Razor Script");
			valid=0;
		}
	}

	pricing_result->valid = valid;

	return valid;
}

void 
PricingCalculatorSPO::getProductFaceValue(const struct rq_pricing_request *pricing_request, FaceValue &fv)
{
	const Trade *trade = (const Trade *)pricing_request->trade_details;

	const ScriptedProduct *spoption = trade->getProductScriptedProduct();

	fv.amt = spoption->getFaceValue()->getAmount();
	strcpy(fv.ccy, spoption->getFaceValue()->getCurrency());

	if (!RQ_IS_NULL(pricing_request->market) && pricing_request->pricing_currency)
	{
		rq_exchange_rate_mgr_t ermgr = rq_market_get_exchange_rate_mgr(pricing_request->market);
		double fxRate = rq_exchange_rate_mgr_get(
			ermgr,
			fv.ccy,
			pricing_request->pricing_currency
			);
		fv.amt *= fxRate;
	}
}

double 
PricingCalculatorSPO::getProductMtm(const struct rq_pricing_request *pricing_request,  struct rq_pricing_result *pricing_result)
{
	return calculatePricingResults(pricing_request, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

double 
PricingCalculatorSPO::calculatePricingResults(const struct rq_pricing_request *pricing_request,
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
											  double *iFwd)
{
	const Trade *trade = (const Trade *)pricing_request->trade_details;
	const ScriptedProduct *spoption = trade->getProductScriptedProduct();

	const fpmlAmount* fv = spoption->getFaceValue();

	const char *ccy = fv->getCurrency();

	double amount = fv->getAmount();

	rq_yield_curve_t ts = rq_termstruct_cache_find_yield_curve(
		pricing_request->termstruct_cache, 
		s_yldCurveType.c_str(),
		ccy
		);

	rq_exchange_rate_mgr_t ermgr = rq_market_get_exchange_rate_mgr(pricing_request->market);

	rq_date market_date = rq_market_get_market_date(pricing_request->market);
	rq_date expiry_date = spoption->getExpirationDate()->getUnadjustedDate().julian();
	double tau_e = (expiry_date - market_date) / 365.0;

	double df = rq_yield_curve_get_discount_factor(ts, expiry_date);

	// set to NaN to trigger server error handling
	double p = NaNConst;
	double mtm = NaNConst;

	double vol = 0.0;
	double spot_rate = 0.0;
	double strike_rate = 0.0;


	double error;

	RazorPricingContext* ctx = (RazorPricingContext*)pricing_request->application_data;

	if (ctx) {
		const VelocityConfig* vc = ctx->getSystem()->getVelocityConfig();
		if (vc)
			price(vc,spoption->getTemplateId(), trade->getTradeHeaderTradeId(), p, error);
	}

	mtm = p;

	if (!strcmp(trade->getTradeHeaderBuySell(),"SELL"))
		mtm = -mtm;

	double exchange_rate = rq_exchange_rate_mgr_get(
		ermgr,
		ccy,
		pricing_request->pricing_currency
		);

	mtm *= exchange_rate;

	if (iRepExRate) (*iRepExRate) = exchange_rate;
	if (iVol) (*iVol) = vol;

	if (iCcy) strcpy(iCcy, ccy);
	if (iSpot) (*iSpot) = spot_rate;
	if (iStrike) (*iStrike) = strike_rate;
	//may need to change the model so forward price uses df from spot->delivery
	if (iIR)
		(*iIR) = RQ_DOUBLE_EQUAL(tau_e, 0) ? 0.0 : -log(df) / tau_e;
	if (iTauE) (*iTauE) = tau_e;
	if (iModelPrice) (*iModelPrice) = p;
	//	if (iFwd) (*iFwd) = spot_rate * (for_df / dom_df);

	return mtm;
}

void 
PricingCalculatorSPO::getProductMarketRequirements(const struct rq_pricing_request *pricing_request, rq_market_requirements_t market_requirements)
{
	const Trade *trade = (const Trade *)pricing_request->trade_details;
	const ScriptedProduct *spoption = trade->getProductScriptedProduct();

	const fpmlAmount* fv = spoption->getFaceValue();

	const char *ccy = fv->getCurrency();

	double amount = fv->getAmount();

	rq_date expiry_date = spoption->getExpirationDate()->getUnadjustedDate().julian();

	rq_market_requirements_termstruct_add(
		market_requirements,
		RQ_TERMSTRUCT_TYPE_YIELD_CURVE,
		s_yldCurveType.c_str(),
		ccy,
		expiry_date
		);
}

void 
PricingCalculatorSPO::getProductNamedValues(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result, unsigned long all)
{
	PricingCalculator::getProductNamedValues(pricing_request, pricing_result, all);
}

const std::string
PricingCalculatorSPO::getATM(bool isSpot, bool isCall, double spotFwd, double strike, double maturityTau)  
{
	std::string optionStatus;
	double	spotOrFwd;

	if (isSpot)
		spotOrFwd = spotFwd;
	else
		spotOrFwd = spotFwd;

	if (maturityTau <= 0)
		optionStatus = "Expired";	
	else if ( RQ_DOUBLE_EQUAL(strike, spotOrFwd) )
		optionStatus = "At the money";

	if (isCall)
	{
		if (spotOrFwd < strike)
		{
			optionStatus = "Out of the money";
		}
		else if (spotOrFwd > strike)
		{
			optionStatus = "In the money";
		}
	}
	else // it's a Put
	{
		if (spotOrFwd < strike)
		{
			optionStatus = "In the money";
		}
		else if (spotOrFwd > strike)
		{
			optionStatus = "Out of the money";
		}
	}

	return optionStatus;
}

void 
PricingCalculatorSPO::getProductPricingValues(const struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result, unsigned long all, double mtm)
{
	const Trade *trade = (const Trade *)pricing_request->trade_details;
	const ScriptedProduct *spoption = trade->getProductScriptedProduct();

	double delta, gamma, vega, rho, theta;

	// Input Variables
	double iSpot, iStrike, iVol, iIR, iRepExRate;
	double iTauE, iModelPrice, iFwd;

	char* iCcy = new char[4];

	double price = calculatePricingResults(pricing_request, &delta, &gamma, &vega, &rho, &theta, &iSpot, &iStrike, &iVol, &iIR, iCcy, &iRepExRate, &iTauE, &iModelPrice, &iFwd);

	//	std::string optionStatusSpot = getATM(true, isCall, iSpot, iStrike, iTauE);
	//	std::string optionStatusFwd = getATM(false, isCall, iFwd, iStrike, iTauD);

	// Get Premium details
	const fpmlPremium* premium = spoption->getOptionPremium();

	// add requested named values to the result
	const char* nameStr;

	if (PricingCalculator::getIsNameRequested(PANV_DELTA, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, delta);
	}

	if (PricingCalculator::getIsNameRequested(PANV_GAMMA, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, gamma);
	}

	if (PricingCalculator::getIsNameRequested(PANV_VEGA, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, vega);
	}

	if (PricingCalculator::getIsNameRequested(PANV_RHO, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, rho);
	}

	if (PricingCalculator::getIsNameRequested(PANV_THETA, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, theta);
	}

	if (PricingCalculator::getIsNameRequested(PANV_DELTA_DOLLARAMOUNT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, mtRound(delta * 0.0001 * price,2));
	}

	if (PricingCalculator::getIsNameRequested(PANV_GAMMA_DOLLARAMOUNT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, mtRound(gamma * 0.0001 * price,2));
	}

	if (PricingCalculator::getIsNameRequested(PANV_VEGA_DOLLARAMOUNT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, mtRound(vega * 0.01 * price,2));
	}

	if (PricingCalculator::getIsNameRequested(PANV_RHO_DOMESTIC_DOLLARAMOUNT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, mtRound(rho * 0.0001 * price,2));
	}

	if (PricingCalculator::getIsNameRequested(PANV_THETA_DOLLARAMOUNT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, mtRound(theta * price,2));
	}

	// Tracing Values - need to add to getTracingValues later ... 

	if (PricingCalculator::getIsNameRequested(PANV_SPOT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iSpot);
	}
	if (PricingCalculator::getIsNameRequested(PANV_STRIKE, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iStrike);
	}
	if (PricingCalculator::getIsNameRequested(PANV_VOLATILITY, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iVol);
	}
	if (PricingCalculator::getIsNameRequested(PANV_DOMESTIC_INT, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iIR);
	}
	if (PricingCalculator::getIsNameRequested(PANV_DOMESTIC_CCY, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_string(pricing_result->pricing_value_results, nameStr, iCcy);
	}

	if (PricingCalculator::getIsNameRequested(PANV_REP_EXCHANGE_RATE, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iRepExRate);
	}
	if (PricingCalculator::getIsNameRequested(PANV_TAU_E, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iTauE);
	}
	if ((PricingCalculator::getIsNameRequested(PANV_PREMIUM, pricing_request, nameStr) || all) && premium)
	{
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, premium->getPaymentAmount()->getAmount());
	} 
	if ((PricingCalculator::getIsNameRequested(PANV_PREMIUM_CCY, pricing_request, nameStr) || all) && premium)
	{
		rq_named_variant_mgr_add_string(pricing_result->pricing_value_results, nameStr, premium->getPaymentAmount()->getCurrency());
	}
	if (PricingCalculator::getIsNameRequested(PANV_MODEL_PRICE, pricing_request, nameStr) || all)
	{ 
		rq_named_variant_mgr_add_double(pricing_result->pricing_value_results, nameStr, iModelPrice);
	}
	if (PricingCalculator::getIsNameRequested(PANV_FORWARD_RATE, pricing_request, nameStr) || all)
	{
		rq_named_variant_mgr_add_double(pricing_result->named_value_results, nameStr, iFwd);
	}

	//if (PricingCalculator::getIsNameRequested(PANV_CALL_PUT, pricing_request, nameStr) || all)
	//{
	//	rq_named_variant_mgr_add_string(pricing_result->pricing_value_results, nameStr, callPut.c_str());
	//}
	//if (PricingCalculator::getIsNameRequested(PANV_MONEYNESS_SPOT, pricing_request, nameStr) || all)
	//{
	//	rq_named_variant_mgr_add_string(pricing_result->pricing_value_results, nameStr, optionStatusSpot.c_str());
	//} 
	//if (PricingCalculator::getIsNameRequested(PANV_MONEYNESS_FWD, pricing_request, nameStr) || all)
	//{
	//	rq_named_variant_mgr_add_string(pricing_result->pricing_value_results, nameStr, optionStatusFwd.c_str());
	//} 
	//if (PricingCalculator::getIsNameRequested(PANV_MODEL_CALL_PUT, pricing_request, nameStr) || all)
	//{
	//	rq_named_variant_mgr_add_string(pricing_result->named_value_results, nameStr, (isCall ? "Call" : "Put") );
	//}

	PricingCalculator::getProductPricingValues(pricing_request, pricing_result, all, mtm);

	delete [] iCcy;

}

// returns true if file's last write time is before the supplied datetime string
bool PricingCalculatorSPO::isOlder(const std::string& fname, const std::string& date)
{
	typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;

	bool ret = true;

	try {
		boost::posix_time::ptime t1(boost::posix_time::time_from_string(date)); // time returned is in local time
		boost::filesystem::path file(fname);

		if (boost::filesystem::exists(file)) {
			std::time_t time = boost::filesystem::last_write_time(file); // time returned is in UTC
			boost::posix_time::ptime t2 = local_adj::utc_to_local(boost::posix_time::from_time_t(time)); 
			ret = t2 < t1;
		}
	}
	catch (...)
	{
		// ignore any out of range exceptions from bogus dates
	}

	return ret;
}

// if this is defined the process will suspend until a remote debugger is attached
#undef _JNI_DEBUG
#ifdef _JNI_DEBUG
#define _JNI_OPTIONS 7
#else
#define _JNI_OPTIONS 5
#endif

// Wraps the JAVA razor compiler through JNI
bool PricingCalculatorSPO::compile(const VelocityConfig* vc, const std::string& tmp, const std::string& tradeId, 
								   const std::string& script, const std::string& timeStamp, std::string& message)
{
	bool status = true;

	// if the timestamp is older than the generated CPP then don't compile
	// this is also done in the java compiler but this avoids starting up the JVM
	if (!isOlder(getFilePath(vc, tmp, "cpp"), timeStamp))
		return status;

	jint res;

	std::stringstream oss;

	// redirect cout and cerr to a string stream
	std::streambuf* old_cout = std::cout.rdbuf();
	std::streambuf* old_cerr = std::cerr.rdbuf();

	std::cout.rdbuf(oss.rdbuf());
	std::cerr.rdbuf(oss.rdbuf());

	// ***
	// *** Use goto instead of exceptions since C++ lacks a 'finally' block and are runtime expensive
	// ***

	if (!jvm) {
		JavaVMInitArgs vm_args; // JDK/JRE 5 VM initialization arguments


		std::string classpath = "-Djava.class.path="+vc->getCompilerDir()+"/bin/razorcompiler.jar";
		std::string boost     = "-Dboost.dir="+vc->getBoostDir();
		std::string intel     = "-Dintel.mkl="+vc->getIntelMKLDir();
		std::string compiler  = "-Drazor.compiler="+vc->getCompilerDir();

		// make sure all slashes are facing the same way...
		//boost::algorithm::replace_all(classpath,"\\","/");
		//boost::algorithm::replace_all(boost,"\\","/");
		//boost::algorithm::replace_all(intel,"\\","/");
		//boost::algorithm::replace_all(compiler,"\\","/");

		boost::scoped_array<JavaVMOption> options(new JavaVMOption[7]);
		//std::vector<JavaVMOption> options(7);

		options[0].optionString = "-Djava.compiler=JIT";
		options[1].optionString = const_cast<char*>(classpath.c_str());
		options[2].optionString = const_cast<char*>(boost.c_str());
		options[3].optionString = const_cast<char*>(intel.c_str());
		options[4].optionString = const_cast<char*>(compiler.c_str());

		// remote debugging options
		options[5].optionString = "-Xdebug";
		options[6].optionString = "-Xrunjdwp:transport=dt_socket,server=y,suspend=y,address=31415";

		vm_args.version = JNI_VERSION_1_6;

		// Set this to 5 for normal, 7 to enable remote debugging
		vm_args.nOptions = _JNI_OPTIONS;

		vm_args.options = options.get();

		vm_args.ignoreUnrecognized = JNI_TRUE;

		res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);

		if (res != JNI_OK) {
			if (env->ExceptionOccurred())
				env->ExceptionDescribe();

			status = false;
			goto exit;
		}
	}

	jsize size;
	res = JNI_GetCreatedJavaVMs(&jvm, 1, &size);
	res = jvm->AttachCurrentThread((void**)&env,NULL);

	if (res == JNI_OK) {
		/* invoke the Main.test method using the JNI */
		jclass cls = env->FindClass("RazorCompiler");
		if (env->ExceptionOccurred()) {
			env->ExceptionDescribe();
			status = false;
			goto exit;
		}

		jmethodID mid = env->GetStaticMethodID(cls,"compileString","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZZZ)Z");
		if (!mid || env->ExceptionOccurred()) {
			env->ExceptionDescribe();
			status = false;
			goto exit;
		}

		jstring tmpl = env->NewStringUTF(tmp.c_str());
		jstring name = env->NewStringUTF(tradeId.c_str());
		jstring str = env->NewStringUTF(script.c_str());

		// create DLL, don't force recompile, no debug, optimize
		jboolean ret = env->CallStaticBooleanMethod(cls, mid, tmpl, name, str, JNI_FALSE, JNI_FALSE, JNI_FALSE, JNI_TRUE);

		if (env->ExceptionOccurred())
		{
			env->ExceptionDescribe();
			status = false;
		}
	}

exit:

	std::cout.rdbuf(old_cout);
	std::cerr.rdbuf(old_cerr);

	jvm->DetachCurrentThread();

	message = oss.str();

	return status;
}

const std::string PricingCalculatorSPO::getFilePath(const VelocityConfig* vc, const std::string& tmplName, const std::string& ext, const std::string& tradeId)
{
	std::string tmplDir = vc->getTemplateDir(); 
	boost::algorithm::replace_all(tmplDir,"/","\\");

	return tmplDir+"\\"+tmplName+"\\"+tmplName+tradeId+"."+ext;
}

bool PricingCalculatorSPO::price(const VelocityConfig* vc, const std::string& tmplName, const std::string& tradeId, double& p, double& err)
{
	bool ret = false;

	p=0.0;
	err=0.0;

	HINSTANCE hinstLib; 
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 

	std::string tmplDir = vc->getTemplateDir(); 
	boost::algorithm::replace_all(tmplDir,"/","\\");

	std::string dllPath = getFilePath(vc, tmplName, "dll", tradeId);

	hinstLib = LoadLibraryEx(dllPath.c_str(),NULL,LOAD_WITH_ALTERED_SEARCH_PATH);

	// If the handle is valid, try to get the function address.

	if (hinstLib != NULL) 
	{ 
		ProcNew newTemplate = (ProcNew) GetProcAddress(hinstLib, "newTemplate"); 
		ProcDel deleteTemplate = (ProcDel) GetProcAddress(hinstLib, "deleteTemplate"); 

		//ProcGetKeys getKeys = (ProcGetKeys) GetProcAddress(hinstLib, "getKeys"); 
		//ProcSet setValue = (ProcSet) GetProcAddress(hinstLib, "setValue"); 
		//ProcGet getValue = (ProcGet) GetProcAddress(hinstLib, "getValue"); 

		Template* tmpl = newTemplate();

		//size_t size;
		//char** keys=getKeys(tmpl,size);

		p = tmpl->price();
		err = tmpl->stdError();
		deleteTemplate(tmpl);

		fFreeResult = FreeLibrary(hinstLib);
		ret = true;
	}

	return ret;
}

void* PricingCalculatorSPO::allocPricingAdapterCache()
{
	jvm = NULL;
	env = NULL;

	return jvm;
}

void PricingCalculatorSPO::freePricingAdapterCache(void* cache)
{
	if (jvm)
		jvm->DestroyJavaVM();

	jvm = NULL;
}

void* PricingCalculatorSPO::allocTradeTransitionCache(const struct rq_pricing_request* pricing_request)
{
	TradeTransitionCache* tradeTransitionCache = new PricingCalculatorSPO::TradeTransitionCache;

	tradeTransitionCache->compile = true;

	return tradeTransitionCache;
}

void PricingCalculatorSPO::freeTradeTransitionCache(void* cache)
{
	TradeTransitionCache* tradeTransitionCache = (TradeTransitionCache*)cache;
	delete tradeTransitionCache;
}

void 
PricingCalculatorSPO::clearTradeTransitionCache(void *cache)
{
	TradeTransitionCache* tradeTransitionCache = (TradeTransitionCache*)cache;
	tradeTransitionCache->compile = true;
}
