#include "hsprPrecompiled.h"
#include "hsprIPointSnapTarget.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;

namespace hspr {

ID2D1Bitmap1* IPointSnapTarget::msTargetImage;

void IPointSnapTarget::InitIPointSnapTarget()
{
	WicBitmap wicBitmap(hspr::Module::handle, HSPR_IDR_POINT_SNAP);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &msTargetImage);
}

void IPointSnapTarget::TermIPointSnapTarget()
{
	msTargetImage->Release();
}

IPointSnapTarget::~IPointSnapTarget()
{
}

} // namespace hspr
