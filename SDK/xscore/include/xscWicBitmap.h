#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API WicBitmap
{
public:
	WicBitmap(const WicBitmap& wicBitmap) = delete;
	WicBitmap& operator=(const WicBitmap& wicBitmap) = delete;

	WicBitmap();
	explicit WicBitmap(const std::wstring& filePath, bool premultipliedAlpha = true);
	explicit WicBitmap(HINSTANCE module, UINT pngId, bool premultipliedAlpha = true);
	explicit WicBitmap(BYTE* memory, DWORD size, bool premultipliedAlpha = true);

	~WicBitmap();
	void Release();

	bool LoadFromMemory(BYTE* memory, DWORD size, bool premultiplied = true);
	bool LoadFromFile(const std::wstring& filename, bool premultiplied = true);
	bool LoadFromResource(HINSTANCE module, UINT pngId, bool premultiplied = true);
	
	void Dimension(UINT& w, UINT& h) const;

	HBITMAP CreateHBitmap() const;
	IWICBitmap* CreateWicBitmap() const;

	IWICBitmapSource* GetSource() const;

protected:
	IWICBitmapSource* mSource;

public:
	static void Init_WicBitmap();
	static void Term_WicBitmap();
};

} // namespace xsc
