#include "hsuiPrecompiled.h"
#include "hsuiPWLink.h"
#include "hsuiVisualManager.h"
#include "hsuiEditCtrl.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWLink(Component* component)
{
	return new PWLink(component);
}

void PWLink::InitPWLink()
{
	msFactory[XSC_RTTI(Link).GetName()] = CreatePWLink;
}

void PWLink::TermPWLink()
{
}
// ====================================================================================================================

PWLink::PWLink(Component* component)
	:
	ParamWindow(component)
{
	mIcon = msLinkIcon;
}

PWLink::~PWLink()
{
}

void PWLink::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"LINK");
}

void PWLink::GetDimension(int& w, int& h) const
{
	w = 200;
	h = 109;
}

void PWLink::OnInitWnd()
{
	auto link = static_cast<Link*>(mComponent);
	ComboCtrl::Item item;

	int x = CONTENT_MARGIN;
	int y = TOP_MARGIN;

	static const int LABEL_WIDTH = 80;
	static const int COMBO_WIDTH = 103;

	mWidthLabel.CreateWnd(this, L"Line Width", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mWidthLabel.SetDrawBorder(true);
	x += LABEL_WIDTH + INTERCONTROL_GAP;
	mWidthCombo.CreateWnd(this, x, y, COMBO_WIDTH);
	item.text = L"Thin";
	mWidthCombo.AddItem(item);
	item.text = L"Thick";
	mWidthCombo.AddItem(item);
	mWidthCombo.SetCurrentItem(link->GetLineWidth());

	x = CONTENT_MARGIN;
	y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
	mStyleLabel.CreateWnd(this, L"Line Style", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mStyleLabel.SetDrawBorder(true);
	x += LABEL_WIDTH + INTERCONTROL_GAP;
	mStyleCombo.CreateWnd(this, x, y, COMBO_WIDTH);
	item.text = L"Solid";
	mStyleCombo.AddItem(item);
	item.text = L"Dashed";
	mStyleCombo.AddItem(item);
	mStyleCombo.SetCurrentItem(link->GetLineStyle());

	x = CONTENT_MARGIN;
	y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
	mColorLabel.CreateWnd(this, L"Line Color", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mColorLabel.SetDrawBorder(true);
	x += LABEL_WIDTH + INTERCONTROL_GAP;
	mColorCombo.CreateWnd(this, x, y, COMBO_WIDTH);
	item.text = L"Black";
	mColorCombo.AddItem(item);
	item.text = L"Teal";
	mColorCombo.AddItem(item);
	item.text = L"Purple";
	mColorCombo.AddItem(item);
	mColorCombo.SetCurrentItem(link->GetLineColor());
}

void PWLink::OnApply()
{
	auto link = static_cast<Link*>(mComponent);
	link->SetLineWidth(static_cast<LinkLineWidth>(mWidthCombo.GetCurrentItem()));
	link->SetLineStyle(static_cast<LinkLineStyle>(mStyleCombo.GetCurrentItem()));
	link->SetLineColor(static_cast<LinkLineColor>(mColorCombo.GetCurrentItem()));
}

} // namespace hsui
