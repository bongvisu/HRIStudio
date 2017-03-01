#pragma once

#include <xscModule.h>

#ifdef HSMO_EXPORTS
#define HSMO_API __declspec(dllexport)
#else
#define HSMO_API __declspec(dllimport)
#endif // HSMO_EXPORTS

namespace hsmo {

class HSMO_API Module
{
public:
    static HMODULE handle;

	static void Init();
	static void Term();
};

} // namespace hsmo
