#pragma once

#include "TemplateDefs.h"

namespace ublas = boost::numeric::ublas;
namespace blas = boost::numeric::bindings::blas;

//typedef ublas::array_adaptor<double> array_adaptor;
//typedef ublas::vector<double, array_adaptor> VDouble;
//typedef ublas::scalar_vector<double,array_adaptor> SVDouble;

typedef ublas::unbounded_array<double> array_adaptor;
typedef ublas::vector<double, array_adaptor> VDouble;
typedef ublas::scalar_vector<double, array_adaptor> SVDouble;

typedef boost::shared_ptr<VDouble> pVDouble;
typedef boost::shared_ptr<SVDouble> pSVDouble;

typedef std::map<std::string, pVDouble > VarTable;

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

