#pragma once

#include <xscRtti.h>
#include <xscPointer.h>

namespace xsc {

class Object;
typedef Pointer<Object> ObjectPtr;
typedef std::list<Object*> ObjectList;
typedef std::vector<Object*> ObjectVec;
typedef std::unordered_set<Object*> ObjectSet;

class XSC_API Object
{
public:
	typedef Object* (* FactoryFunction) ();
	static void RegisterFactory(const Rtti& rtti, FactoryFunction factoryFunction);
	static Object* FromFactory(const std::wstring& rttiName);

public:
	virtual ~Object();

	static const Rtti RTTI_Object;
	virtual const Rtti& GetRtti() const;
	virtual bool IsExactly(const Rtti& rtti) const;
	virtual bool IsDerivedFrom(const Rtti& rtti) const;

protected:
	Object();
};

} // namespace xsc

#define XSC_DECLARE_DYNAMIC(class) \
	public: \
		static const xsc::Rtti RTTI_##class; \
		virtual const xsc::Rtti& GetRtti() const; \
		virtual bool IsExactly(const xsc::Rtti& rtti) const; \
		virtual bool IsDerivedFrom(const xsc::Rtti& rtti) const;
		
#define XSC_IMPLEMENT_DYNAMIC(nameSpace, class, baseClass) \
	const xsc::Rtti class::RTTI_##class{ L#nameSpace L"." L#class, &baseClass::RTTI_##baseClass }; \
	bool class::IsExactly(const xsc::Rtti& rtti) const \
	{ \
		return RTTI_##class.IsExactly(rtti); \
	} \
	bool class::IsDerivedFrom(const xsc::Rtti& rtti) const \
	{ \
		return RTTI_##class.IsDerivedFrom(rtti); \
	} \
	const xsc::Rtti& class::GetRtti() const \
	{ \
		return RTTI_##class; \
	}

#define XSC_DECLARE_DYNCREATE(class) \
	XSC_DECLARE_DYNAMIC(class)	 \
	static xsc::Object* Dyncreate##class();

#define XSC_IMPLEMENT_DYNCREATE(nameSpace, class, baseClass) \
	XSC_IMPLEMENT_DYNAMIC(nameSpace, class, baseClass) \
	xsc::Object* class::Dyncreate##class() \
	{ \
		return new class; \
	}

#define XSC_RTTI(class) \
	class::RTTI_##class

#define XSC_REGISTER_FACTORY(namespace, class) \
	xsc::Object::RegisterFactory(namespace::class::RTTI_##class, namespace::class::Dyncreate##class);
