#pragma once

#include <hsmoModule.h>

#ifdef HSPR_EXPORTS
#define HSPR_API __declspec(dllexport)
#else
#define HSPR_API __declspec(dllimport)
#endif // HSPR_EXPORTS

namespace hspr {

class HSPR_API Module
{
public:
    static HMODULE handle;

	static void Init();
	static void Term();
};

} // namespace hspr
