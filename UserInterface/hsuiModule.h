#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include <afxwin.h>
#include <afxext.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>
#include <afxodlgs.h>
#include <afxdisp.h>
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>

#include <xscModule.h>
#include <hsmoModule.h>

#ifdef HSUI_EXPORTS
#define HSUI_API __declspec(dllexport)
#else
#define HSUI_API __declspec(dllimport)
#endif // HSUI_EXPORTS

namespace hsui {

class HSUI_API Module
{
public:
	static HMODULE handle;

	static void Init();
	static void Term();
};

} // namespace hsui
