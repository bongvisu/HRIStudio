#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API Rtti final
{
public:
	Rtti(const wchar_t* name, const Rtti* base);
	Rtti& operator=(const Rtti& rtti);

	const wchar_t* GetName() const;
	const Rtti* GetBase() const;

	bool IsExactly(const Rtti& rtti) const;
	bool IsDerivedFrom(const Rtti& rtti) const;

protected:
	const wchar_t* mName;
	const Rtti* mBase;
};

} // namespace xsc
