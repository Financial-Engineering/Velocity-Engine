#include "PricingCalculatorSPO.h"

/* -- code -------------------------------------------------------- */

PricingCalculatorSPO calcSPO;

extern "C" const char *
SPO_get_pricing_adapter_id()
{
	return calcSPO.getId();
}

void
SPO_get_market_requirements(struct rq_pricing_request *pricing_request, rq_market_requirements_t market_requirements)
{
	getMarketRequirements(pricing_request, market_requirements, &calcSPO);
}


extern "C" short 
SPO_get_pricing_results(struct rq_pricing_request *pricing_request, struct rq_pricing_result *pricing_result)
{
	return getPricingResults(pricing_request, pricing_result, &calcSPO);
}

void*
SPO_alloc_pricing_adapter_cache()
{
    return calcSPO.allocPricingAdapterCache();
}

void
SPO_free_pricing_adapter_cache(void* cache)
{
    return calcSPO.freePricingAdapterCache(cache);
}


void *
SPO_alloc_trade_transition_cache(struct rq_pricing_request *pricing_request)
{
    return calcSPO.allocTradeTransitionCache(pricing_request);
}

void
SPO_clear_trade_transition_cache(void *cache)
{
	calcSPO.clearTradeTransitionCache(cache);
}

void
SPO_free_trade_transition_cache(void *cache)
{
	calcSPO.freeTradeTransitionCache(cache);
}

extern "C"int __declspec(dllexport)
register_spo_adapter(struct rq_pricing_adapter *pa)
{
	pa->get_pricing_adapter_id = SPO_get_pricing_adapter_id;

	pa->alloc_pricing_adapter_cache = SPO_alloc_pricing_adapter_cache;
	pa->free_pricing_adapter_cache = SPO_free_pricing_adapter_cache;

	pa->alloc_trade_transition_cache = SPO_alloc_trade_transition_cache;
	pa->clear_trade_transition_cache = SPO_clear_trade_transition_cache;
	pa->free_trade_transition_cache = SPO_free_trade_transition_cache;

	pa->alloc_market_transition_cache = NULL;
    pa->clear_market_transition_cache = NULL;
	pa->free_market_transition_cache = NULL;

	pa->get_pricing_results = SPO_get_pricing_results;
    pa->get_market_requirements = SPO_get_market_requirements;

    pa->free_pricing_result_data = free_pricing_result_data;

	return 1;
}
