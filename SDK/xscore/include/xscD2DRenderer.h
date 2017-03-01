#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API D2DRenderer
{
public:
	static ID2D1Factory1* factory;
	static ID2D1Device*	device;
	static ID2D1DeviceContext* resourcingContext;
	static ID2D1DCRenderTarget* dcRenderTarget;

public:
	D2DRenderer();
	~D2DRenderer();

	bool Create(HWND handle, UINT width, UINT height);
	void Destroy();

	ID2D1DeviceContext* GetContext() const;

	void Resize(UINT width, UINT height);
	void Display();

	void BeginDraw();
	void EndDraw();

	void Clear(const D2D1_COLOR_F& color);
	
	void FillRectangle(const D2D1_RECT_F& rect, ID2D1Brush* brush);
	void FillRoundedRectangle(const D2D1_ROUNDED_RECT& roundedRect, ID2D1Brush* brush);

	void DrawLine(D2D1_POINT_2F point0, D2D1_POINT_2F point1, ID2D1Brush* brush,
		FLOAT strokeWidth = 1.0f, ID2D1StrokeStyle* strokeStyle = nullptr);

	void DrawRectangle(const D2D1_RECT_F& rect, ID2D1Brush* brush, FLOAT strokeWidth = 1.0f);
	
	void DrawBitmap(ID2D1Bitmap* bitmap, const D2D1_RECT_F* destinationRectangle = nullptr);
	void DrawBitmap(ID2D1Bitmap* bitmap, const D2D1_RECT_F& destinationRectangle);

	void DrawText(const WCHAR* string, UINT32 stringLength,
		IDWriteTextFormat* textFormat, const D2D1_RECT_F& layoutRect,
		ID2D1Brush* defaultForegroundBrush, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE);

	void SetAntialiasMode(D2D1_ANTIALIAS_MODE antialiasMode);
	D2D1_ANTIALIAS_MODE GetAntialiasMode() const;

	bool CreateBitmapFromWicBitmap(IWICBitmapSource* wicBitmapSource, ID2D1Bitmap** bitmap);
	bool CreateBitmapFromWicBitmap(IWICBitmapSource* wicBitmapSource, ID2D1Bitmap1** bitmap);
	bool CreateSolidColorBrush(const D2D1_COLOR_F& color, ID2D1SolidColorBrush** solidColorBrush);

	bool CreateGradientStopCollection(const D2D1_GRADIENT_STOP* gradientStops,
		UINT32 gradientStopsCount, D2D1_GAMMA colorInterpolationGamma,
		D2D1_EXTEND_MODE extendMode, ID2D1GradientStopCollection** gradientStopCollection);
	
	bool CreateLinearGradientBrush(const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES& linearGradientBrushProperties,
        ID2D1GradientStopCollection* gradientStopCollection, ID2D1LinearGradientBrush** linearGradientBrush);

protected:
	void CreateRenderTarget();
	void DestroyRenderTarget();

	ID2D1DeviceContext* mContext;
	IDXGISwapChain1* mSwapChain;

	D2D1_SIZE_U mTargetSize;

public:
	static void Init_D2DRenderer();
	static void Term_D2DRenderer();
};

} // namespace xsc
