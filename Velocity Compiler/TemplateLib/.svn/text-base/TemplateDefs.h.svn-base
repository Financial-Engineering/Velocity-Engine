#pragma once

class Template;

typedef Template* (__cdecl *ProcNew)(void); 
typedef void (__cdecl *ProcDel)(Template*);
typedef double* (__cdecl *ProcGet)(Template*, const char*); 
typedef void (__cdecl *ProcSet)(Template*, const char*, const double*, const size_t);
typedef char** (__cdecl *ProcGetKeys)(Template*, size_t&);
typedef double (__cdecl *ProcGetResult)(Template*);

extern "C" {
	__declspec(dllexport) void setValue(Template*, const char*, const double*, const int);
	__declspec(dllexport) double* getValue(Template*, const char*);
	__declspec(dllexport) char** getKeys(Template*, size_t&);
	__declspec(dllexport) double getPrice(Template*);
	__declspec(dllexport) double getError(Template*);
}

