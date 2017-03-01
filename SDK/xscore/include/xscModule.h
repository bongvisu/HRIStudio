#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>
#include <stack>
#include <array>
#include <vector>
#include <string>
#include <limits>
#include <memory>
#include <random>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <mutex>
#include <thread>
#include <iomanip>
#include <atomic>
#include <iterator>
#include <initializer_list>
#include <regex>

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include <Shlwapi.h>
#include <comdef.h>
#include <msxml6.h>

#pragma warning(disable:4251)

#ifdef XSC_EXPORTS
#define XSC_API __declspec(dllexport)
#else
#define XSC_API __declspec(dllimport)
#endif // XSC_EXPORTS

namespace xsc {

extern void XSC_API StartupModule();
extern void XSC_API CleanupModule();

template <typename T>
inline void SafeRelease(T*& object)
{
    if (object)
    {
        object->Release();
        object = nullptr;
    }
}

template <typename T>
inline void SafeDelete(T*& object)
{
    if (object)
    {
        delete object;
        object = nullptr;
    }
}

} // namespace xsc

#ifdef _DEBUG
#define XSC_DEBUG_OUTPUT(msg) OutputDebugStringA(msg "\n")
#else
#define XSC_DEBUG_OUTPUT(msg)
#endif // _DEBUG
