// DLLDriver.cpp : Defines the entry point for the console application.
//
#undef INIT
#undef CVS

#include "stdafx.h"
#include "WinDef.h"
#include "Mmsystem.h"

#include "Template.h"

#include <iostream>
#include <map>
#include <vector>


using namespace std;

typedef std::basic_string< _TCHAR > tstring;

int _tmain(int argc, _TCHAR* argv[])
{
    HINSTANCE hinstLib; 
    BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 
 
    // Get a handle to the DLL module.
	//_TCHAR* dllName = argv[1];

	tstring x = argv[1];
#ifdef INIT
	tstring s = argv[2];
	double N = atof(s.c_str());
#endif
	SetDllDirectory(x.c_str());
	hinstLib = LoadLibrary(x.c_str()); 
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib != NULL) 
    { 
			ProcNew newTemplate = (ProcNew) GetProcAddress(hinstLib, "newTemplate"); 
			ProcDel deleteTemplate = (ProcDel) GetProcAddress(hinstLib, "deleteTemplate"); 
			ProcGetKeys getKeys = (ProcGetKeys) GetProcAddress(hinstLib, "getKeys"); 
			ProcSet setValue = (ProcSet) GetProcAddress(hinstLib, "setValue"); 
			ProcGet getValue = (ProcGet) GetProcAddress(hinstLib, "getValue"); 

			Template* tmpl = newTemplate();
			char** keys=getKeys(tmpl);

			double val[] = {.40};
			setValue(tmpl, "sigma", val, ELEMENTS(val));

//			VarTable& vars = tmpl->getVarTable();
//			vars["STRIKE"]=new double(105);
#ifdef INIT			
			vars["S0"]=VDouble(1,100);
			vars["STRIKE"]=VDouble(1,100);
			vars["sigma"]=VDouble(1,.20);
			vars["R"]=VDouble(1,0.05);
			vars["T"]=VDouble(1,1);
			vars["N"]=VDouble(1,N);
			vars["SEED"]=VDouble(1,2);
			vars["x"]=1.4;
#endif
			DWORD start=0;
			DWORD finish=0;
			
			double p,serr;
#ifdef CSV
			for (int j=1024;j<65356;j+=512) {
				vars["N"]=j;
				for (int i=0;i<10;i++) {
#endif
					start = timeGetTime();		
					p = tmpl->price();
					finish = timeGetTime();
					serr = tmpl->stdError();
#ifdef CSV
				}
				cout << j << ',' << p << ',' << serr << ',' <<(finish-start) << endl;
			}
#else
				cout << "Price: " << p << " Error: " << serr << endl
				<< " Execution Time: "<< (finish-start)/10.0 << "ms" << endl;
#endif
			// Free the DLL module.
		deleteTemplate(tmpl);

        fFreeResult = FreeLibrary(hinstLib);
    }
	return 0;
}

