#include "hsprPrecompiled.h"
#include "hsprPortPresenter.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static ID2D1Bitmap1* gsPortILN;
static ID2D1Bitmap1* gsPortITN;
static ID2D1Bitmap1* gsPortIRN;
static ID2D1Bitmap1* gsPortIBN;
static ID2D1Bitmap1* gsPortILH;
static ID2D1Bitmap1* gsPortITH;
static ID2D1Bitmap1* gsPortIRH;
static ID2D1Bitmap1* gsPortIBH;
static ID2D1Bitmap1* gsPortILS;
static ID2D1Bitmap1* gsPortITS;
static ID2D1Bitmap1* gsPortIRS;
static ID2D1Bitmap1* gsPortIBS;

static ID2D1Bitmap1* gsPortOLN;
static ID2D1Bitmap1* gsPortORN;
static ID2D1Bitmap1* gsPortOTN;
static ID2D1Bitmap1* gsPortOBN;
static ID2D1Bitmap1* gsPortOLH;
static ID2D1Bitmap1* gsPortOTH;
static ID2D1Bitmap1* gsPortORH;
static ID2D1Bitmap1* gsPortOBH;
static ID2D1Bitmap1* gsPortOLS;
static ID2D1Bitmap1* gsPortOTS;
static ID2D1Bitmap1* gsPortORS;
static ID2D1Bitmap1* gsPortOBS;

static ID2D1Bitmap1* gsPortGhostIL;
static ID2D1Bitmap1* gsPortGhostIR;
static ID2D1Bitmap1* gsPortGhostIT;
static ID2D1Bitmap1* gsPortGhostIB;

static ID2D1Bitmap1* gsPortGhostOL;
static ID2D1Bitmap1* gsPortGhostOR;
static ID2D1Bitmap1* gsPortGhostOT;
static ID2D1Bitmap1* gsPortGhostOB;

void PortPresenter::InitPortPresenter()
{
	WicBitmap wicBitmap;

	// input ----------------------------------------------------------------------------------------------------------
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_L_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortILN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_T_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortITN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_R_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortIRN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_B_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortIBN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_L_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortILH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_T_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortITH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_R_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortIRH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_B_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortIBH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_L_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortILS);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_T_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortITS);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_R_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortIRS);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_I_B_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortIBS);
	// input ----------------------------------------------------------------------------------------------------------

	// output ---------------------------------------------------------------------------------------------------------
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_L_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOLN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_T_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOTN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_R_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortORN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_B_N);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOBN);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_L_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOLH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_T_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOTH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_R_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortORH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_B_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOBH);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_L_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOLS);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_T_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOTS);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_R_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortORS);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_O_B_S);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortOBS);
	// output ---------------------------------------------------------------------------------------------------------

	// ghosts ---------------------------------------------------------------------------------------------------------
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_I_L);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostIL);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_I_R);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostIR);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_I_T);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostIT);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_I_B);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostIB);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_O_L);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostOL);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_O_R);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostOR);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_O_T);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostOT);

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_PORT_GHOST_O_B);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsPortGhostOB);
	// ghosts ---------------------------------------------------------------------------------------------------------
}

void PortPresenter::TermPortPresenter()
{
	gsPortGhostIL->Release();
	gsPortGhostIR->Release();
	gsPortGhostIT->Release();
	gsPortGhostIB->Release();

	gsPortGhostOL->Release();
	gsPortGhostOR->Release();
	gsPortGhostOT->Release();
	gsPortGhostOB->Release();

	gsPortILN->Release();
	gsPortITN->Release();
	gsPortIRN->Release();
	gsPortIBN->Release();
	gsPortILH->Release();
	gsPortITH->Release();
	gsPortIRH->Release();
	gsPortIBH->Release();
	gsPortILS->Release();
	gsPortITS->Release();
	gsPortIRS->Release();
	gsPortIBS->Release();

	gsPortOLN->Release();
	gsPortOTN->Release();
	gsPortORN->Release();
	gsPortOBN->Release();
	gsPortOLH->Release();
	gsPortOTH->Release();
	gsPortORH->Release();
	gsPortOBH->Release();
	gsPortOLS->Release();
	gsPortOTS->Release();
	gsPortORS->Release();
	gsPortOBS->Release();
}
// ====================================================================================================================

static const int PORT_MAJOR_SIZE = 2 * Port::CONNECTION_MARGIN + 1;
static const int PORT_MINOR_SIZE = Port::CONNECTION_MARGIN + 1;

PortPresenter::PortPresenter(Port* port, Presenter* parent, ILevelView* levelView)
	:
	Presenter(port, parent, levelView),
	mHot(false)
{
}

PortPresenter::~PortPresenter()
{
}

PresenterType PortPresenter::GetPresenterType() const
{
	return PRESENTER_TYPE_PORT;
}

IVisual* PortPresenter::GetVisualInterface() const
{
	return static_cast<IVisual*>(const_cast<PortPresenter*>(this));
}

void PortPresenter::UpdateBound()
{
	Port* port = static_cast<Port*>(mComponent);
	MPRectangle::Edge attachSide = port->GetAttachSide();
	const MPVector& location = port->GetLocation();

	switch (attachSide)
	{
	case MPRectangle::EDGE_L:
		mBound.l = location.x - PORT_MAJOR_SIZE + 1;
		mBound.r = mBound.l + PORT_MAJOR_SIZE;
		mBound.t = location.y - PORT_MINOR_SIZE;
		mBound.b = mBound.t + PORT_MAJOR_SIZE;
		break;
	
	case MPRectangle::EDGE_R:
		mBound.l = location.x - 1;
		mBound.r = mBound.l + PORT_MAJOR_SIZE;
		mBound.t = location.y - PORT_MINOR_SIZE;
		mBound.b = mBound.t + PORT_MAJOR_SIZE;
		break;

	case MPRectangle::EDGE_T:
		mBound.l = location.x - PORT_MINOR_SIZE;
		mBound.r = mBound.l + PORT_MAJOR_SIZE;
		mBound.t = location.y - PORT_MAJOR_SIZE + 1;
		mBound.b = mBound.t + PORT_MAJOR_SIZE;
		break;

	case MPRectangle::EDGE_B:
		mBound.l = location.x - PORT_MINOR_SIZE;
		mBound.r = mBound.l + PORT_MAJOR_SIZE;
		mBound.t = location.y - 1;
		mBound.b = mBound.t + PORT_MAJOR_SIZE;;
		break;
	}
}

PortType PortPresenter::GetPortType() const
{
	Port* port = static_cast<Port*>(mComponent);
	return port->GetPortType();
}

void PortPresenter::SetHot(bool hot)
{
	mHot = hot;
}

bool PortPresenter::GetHot() const
{
	return mHot;
}

bool PortPresenter::IsOccupied() const
{
	Port* port = static_cast<Port*>(mComponent);
	if (port->GetPortType() == PORT_TYPE_OUTPUT)
	{
		if (port->GetConnectionJoints().size())
		{
			return true;
		}
	}
	return false;
}

bool PortPresenter::IsUnoccupied() const
{
	return !IsOccupied();
}

MPVector PortPresenter::GetConnectionPoint() const
{
	Port* port = static_cast<Port*>(mComponent);
	return port->GetConnectionPoint();
}

Presenter* PortPresenter::FindHit(const xsc::MPVector& point) const
{
	if (mBound.Contains(point))
	{
		return const_cast<PortPresenter*>(this);
	}
	return nullptr;
}

void PortPresenter::BuildSnapTargets(PresenterVec& pointTargets, PresenterVec& segmentTargets)
{
	Port* port = static_cast<Port*>(mComponent);

	mSnapPoint = port->GetConnectionPoint();
	mSnapBound.l = mSnapPoint.x - POINTER_ACTION_LATENCY;
	mSnapBound.t = mSnapPoint.y - POINTER_ACTION_LATENCY;
	mSnapBound.r = mSnapPoint.x + POINTER_ACTION_LATENCY;
	mSnapBound.b = mSnapPoint.y + POINTER_ACTION_LATENCY;

	pointTargets.push_back(this);
}

const MPVector& PortPresenter::GetSnapPoint() const
{
	return mSnapPoint;
}

const MPRectangle& PortPresenter::GetSnapBound() const
{
	return mSnapBound;
}

void PortPresenter::DrawMoveGhost(ID2D1DeviceContext* context)
{
	Port* port = static_cast<Port*>(mComponent);
	MPRectangle::Edge attachSide = port->GetAttachSide();
	const MPVector& location = port->GetLocation();

	D2D1_RECT_F rect;
	ID2D1Bitmap1* targetBitmap = nullptr;
	if (port->GetPortType() == PORT_TYPE_INPUT)
	{
		switch (attachSide)
		{
		case MPRectangle::EDGE_L:
			targetBitmap = gsPortGhostIL;
			rect.left = static_cast<FLOAT>(location.x - PORT_MINOR_SIZE);
			rect.right = rect.left + PORT_MINOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y - PORT_MINOR_SIZE);
			rect.bottom = rect.top + PORT_MAJOR_SIZE;
			break;

		case MPRectangle::EDGE_T:
			targetBitmap = gsPortGhostIT;
			rect.left = static_cast<FLOAT>(location.x - PORT_MINOR_SIZE);
			rect.right = rect.left + PORT_MAJOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y - PORT_MINOR_SIZE);
			rect.bottom = rect.top + PORT_MINOR_SIZE;
			break;

		case MPRectangle::EDGE_R:
			targetBitmap = gsPortGhostIR;
			rect.left = static_cast<FLOAT>(location.x);
			rect.right = rect.left + PORT_MINOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y - PORT_MINOR_SIZE);
			rect.bottom = rect.top + PORT_MAJOR_SIZE;
			break;

		case MPRectangle::EDGE_B:
			targetBitmap = gsPortGhostIB;
			rect.left = static_cast<FLOAT>(location.x - PORT_MINOR_SIZE);
			rect.right = rect.left + PORT_MAJOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y);
			rect.bottom = rect.top + PORT_MINOR_SIZE;
			break;
		}
	}
	else
	{
		switch (attachSide)
		{
		case MPRectangle::EDGE_L:
			targetBitmap = gsPortGhostOL;
			rect.left = static_cast<FLOAT>(location.x - PORT_MINOR_SIZE);
			rect.right = rect.left + PORT_MINOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y - PORT_MINOR_SIZE);
			rect.bottom = rect.top + PORT_MAJOR_SIZE;
			break;

		case MPRectangle::EDGE_T:
			targetBitmap = gsPortGhostOT;
			rect.left = static_cast<FLOAT>(location.x - PORT_MINOR_SIZE);
			rect.right = rect.left + PORT_MAJOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y - PORT_MINOR_SIZE);
			rect.bottom = rect.top + PORT_MINOR_SIZE;
			break;

		case MPRectangle::EDGE_R:
			targetBitmap = gsPortGhostOR;
			rect.left = static_cast<FLOAT>(location.x);
			rect.right = rect.left + PORT_MINOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y - PORT_MINOR_SIZE);
			rect.bottom = rect.top + PORT_MAJOR_SIZE;
			break;

		case MPRectangle::EDGE_B:
			targetBitmap = gsPortGhostOB;
			rect.left = static_cast<FLOAT>(location.x - PORT_MINOR_SIZE);
			rect.right = rect.left + PORT_MAJOR_SIZE;
			rect.top = static_cast<FLOAT>(location.y);
			rect.bottom = rect.top + PORT_MINOR_SIZE;
			break;
		}
	}
	context->DrawBitmap(targetBitmap, rect);
}

void PortPresenter::DrawSnapEffect(ID2D1DeviceContext* context)
{
	D2D1_RECT_F rect;
	rect.left = mSnapPoint.x - 8.0f;
	rect.top = mSnapPoint.y - 8.0f;
	rect.right = mSnapPoint.x + 7.0f;
	rect.bottom = mSnapPoint.y + 7.0f;
	context->DrawBitmap(msTargetImage, rect);
}

void PortPresenter::Render(ID2D1DeviceContext* context, double t)
{
	Port* port = static_cast<Port*>(mComponent);
	MPRectangle::Edge attachSide = port->GetAttachSide();
	const MPVector& location = port->GetLocation();

	auto selectable = mParent->GetSelectableInterface();
	bool selected = selectable->GetSelected();

	D2D1_RECT_F rect;
	ID2D1Bitmap1* targetBitmap;
	if (port->GetPortType() == PORT_TYPE_INPUT)
	{
		switch (attachSide)
		{
		case MPRectangle::EDGE_L:
			if (mHot)			targetBitmap = gsPortILH;
			else if (selected)	targetBitmap = gsPortILS;
			else				targetBitmap = gsPortILN;
			break;

		case MPRectangle::EDGE_T:
			if (mHot)			targetBitmap = gsPortITH;
			else if (selected)	targetBitmap = gsPortITS;
			else				targetBitmap = gsPortITN;
			break;

		case MPRectangle::EDGE_R:
			if (mHot)			targetBitmap = gsPortIRH;
			else if (selected)	targetBitmap = gsPortIRS;
			else				targetBitmap = gsPortIRN;
			break;

		case MPRectangle::EDGE_B:
			if (mHot)			targetBitmap = gsPortIBH;
			else if (selected)	targetBitmap = gsPortIBS;
			else				targetBitmap = gsPortIBN;
			break;
		}
	}
	else
	{
		switch (attachSide)
		{
		case MPRectangle::EDGE_L:
			if (mHot)			targetBitmap = gsPortOLH;
			else if (selected)	targetBitmap = gsPortOLS;
			else				targetBitmap = gsPortOLN;
			break;

		case MPRectangle::EDGE_T:
			if (mHot)			targetBitmap = gsPortOTH;
			else if (selected)	targetBitmap = gsPortOTS;
			else				targetBitmap = gsPortOTN;
			break;

		case MPRectangle::EDGE_R:
			if (mHot)			targetBitmap = gsPortORH;
			else if (selected)	targetBitmap = gsPortORS;
			else				targetBitmap = gsPortORN;
			break;

		case MPRectangle::EDGE_B:
			if (mHot)			targetBitmap = gsPortOBH;
			else if (selected)	targetBitmap = gsPortOBS;
			else				targetBitmap = gsPortOBN;
			break;
		}
	}

	rect = mBound.ToRectF();
	context->DrawBitmap(targetBitmap, rect);
}

} // namespace hspr
