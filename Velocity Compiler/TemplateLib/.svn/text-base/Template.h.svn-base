#pragma once

#define _UBLAS_BINDING

#include "TemplateDefs.h"

#include <string>
#include <map>

// Boost Includes
#include <boost/numeric/bindings/blas/blas.hpp>
#include <boost/numeric/bindings/traits/ublas_vector.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/shared_ptr.hpp>

namespace ublas = boost::numeric::ublas;
#ifdef _UBLAS_BINDING
namespace blas = boost::numeric::bindings::blas;
#else
namespace blas = boost::numeric::ublas::blas_1;
#endif

typedef ublas::array_adaptor<double> array_adaptor;
typedef ublas::vector<double, array_adaptor> VDouble;
typedef ublas::scalar_vector<double,array_adaptor> SVDouble;

typedef boost::shared_ptr<VDouble> pVDouble;

typedef std::map<std::string, VDouble > VarTable;

class Template {

friend __declspec(dllexport) void setValue(Template*, const char*, const double[], const size_t);
friend __declspec(dllexport) double* getValue(Template*, const char*);
friend __declspec(dllexport) char** getKeys(Template*, size_t&);
friend __declspec(dllexport) double getPrice(Template*);
friend __declspec(dllexport) double getError(Template*);

protected:

	VarTable vars;
    double _price;
    double _stdErr;

	virtual void bindVars() =0;

public:
	virtual const double price() {return _price;}
	virtual const double stdError() {return _stdErr;}

	virtual VarTable& getVarTable() {return vars;}

	virtual ~Template() {
//		VarTable::iterator it;
//		for ( it=vars.begin() ; it != vars.end(); it++ )
//			((*it).second).empty();
	}
};

