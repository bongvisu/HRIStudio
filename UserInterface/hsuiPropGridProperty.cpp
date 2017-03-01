#include "hsuiPrecompiled.h"
#include "hsuiPropGridProperty.h"
#include "hsuiPropGridCtrl.h"
#include "hsuiVisualManager.h"

#include <xscD2DRenderer.h>

#undef max
#undef min

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

IMPLEMENT_DYNAMIC(PropGridProperty, CObject)

#define PGP_FORMAT_CHAR   _T("%c")
#define PGP_FORMAT_SHORT  _T("%d")
#define PGP_FORMAT_LONG   _T("%ld")
#define PGB_FORMAT_USHORT _T("%u")
#define PGP_FORMAT_ULONG  _T("%u")
#define PGP_FORMAT_FLOAT  _T("%f")
#define PGP_FORMAT_DOUBLE _T("%lf")

const DWORD PropGridProperty::PROP_HAS_LIST	= 0x0001;
const DWORD PropGridProperty::PROP_HAS_BUTTON = 0x0002;
const DWORD PropGridProperty::PROP_HAS_SPIN	= 0x0004;

PropGridProperty::PropGridProperty(const CString& name, const COleVariant& value,
	LPCTSTR descr, DWORD_PTR dwData, LPCTSTR editMask, LPCTSTR editTemplate, LPCTSTR validChars)
	:
	mName(name),
	mValue(value),
	mOriginalValue(value),
	mDescr(descr == nullptr ? _T("") : descr),
	mEditMask(editMask == nullptr ? _T("") : editMask),
	mEditTemplate(editTemplate == nullptr ? _T("") : editTemplate),
	mValidChars(validChars == nullptr ? _T("") : validChars),
	mData(dwData)
{
	mIsGroup = FALSE;
	mIsValueList = FALSE;

	Init();
	SetFlags();

	if (mValue.vt == VT_BOOL)
	{
		mAllowEdit = FALSE;
	}
}

PropGridProperty::PropGridProperty(const CString& groupName, DWORD_PTR data, BOOL isValueList)
	:
	mName(groupName),
	mData(data),
	mIsValueList(isValueList)
{
	mIsGroup = TRUE;

	Init();
	SetFlags();
}

void PropGridProperty::SetFlags()
{
	mFlags = 0;

	switch (mValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_I4:
	case VT_INT:
	case VT_UINT:
	case VT_UI2:
	case VT_UI4:
		break;

	case VT_DATE:
		break;

	case VT_BOOL:
		mFlags = PROP_HAS_LIST;
		break;

	default:
		break;
	}
}

void PropGridProperty::Init()
{
	mWndList = nullptr;
	mExpanded = !mIsValueList;
	mEnabled = TRUE;
	mParent = nullptr;
	mWndInPlace = nullptr;
	mWndCombo = nullptr;
	mWndSpin = nullptr;
	mInPlaceEditing = FALSE;
	mButtonIsFocused = FALSE;
	mButtonIsDown = FALSE;
	mAllowEdit = TRUE;
	mNameIsTruncated = FALSE;
	mValueIsTruncated = FALSE;

	mRect.SetRectEmpty();
	mRectButton.SetRectEmpty();

	mMinValue = 0;
	mMaxValue = 0;

	mIsModified = FALSE;
	mIsVisible = TRUE;
}

PropGridProperty::~PropGridProperty()
{
	while (!mSubItems.IsEmpty())
	{
		delete mSubItems.RemoveHead();
	}

	OnDestroyWindow();
}

void PropGridProperty::OnDestroyWindow()
{
	if (mWndCombo != nullptr)
	{
		mWndCombo->DestroyWindow();
		delete mWndCombo;
		mWndCombo = nullptr;
	}

	if (mWndInPlace != nullptr)
	{
		mWndInPlace->DestroyWindow();
		delete mWndInPlace;
		mWndInPlace = nullptr;
	}

	if (mWndSpin != nullptr)
	{
		mWndSpin->DestroyWindow();
		delete mWndSpin;
		mWndSpin = nullptr;
	}

	if (mValue.vt == VT_BOOL)
	{
		mOptions.RemoveAll();
	}
}

BOOL PropGridProperty::HasButton() const
{
	return (mFlags & PROP_HAS_LIST) || (mFlags & PROP_HAS_BUTTON);
}

BOOL PropGridProperty::AddSubItem(PropGridProperty* prop)
{
	ASSERT_VALID(this);
	ASSERT_VALID(prop);

	if (!IsGroup())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (prop->mWndList != nullptr)
	{
		for (POSITION pos = prop->mWndList->mProps.GetHeadPosition(); pos != nullptr; /**/)
		{
			PropGridProperty* listProp = prop->mWndList->mProps.GetNext(pos);
			ASSERT_VALID(listProp);

			if (listProp == prop || listProp->IsSubItem(prop))
			{
				// Can't ad the same property twice
				ASSERT(FALSE);
				return FALSE;
			}
		}
	}

	prop->mParent = this;

	mSubItems.AddTail(prop);
	prop->mWndList = mWndList;

	return TRUE;
}

BOOL PropGridProperty::RemoveSubItem(PropGridProperty*& prop, BOOL deleteProp)
{
	ASSERT_VALID(this);
	ASSERT_VALID(prop);

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		POSITION posSaved = pos;

		PropGridProperty* pListProp = mSubItems.GetNext(pos);
		ASSERT_VALID(pListProp);

		if (pListProp == prop)
		{
			mSubItems.RemoveAt(posSaved);

			if (mWndList != nullptr && mWndList->mSelection == prop)
			{
				mWndList->mSelection = nullptr;
			}

			if (deleteProp)
			{
				delete prop;
				prop = nullptr;
			}

			return TRUE;
		}

		if (pListProp->RemoveSubItem(prop, deleteProp))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PropGridProperty::AddOption(LPCTSTR option, BOOL insertUnique)
{
	ASSERT_VALID(this);
	ENSURE(option != nullptr);

	if (insertUnique)
	{
		if (mOptions.Find(option) != nullptr)
		{
			return FALSE;
		}
	}

	mOptions.AddTail(option);
	mFlags = PROP_HAS_LIST;

	return TRUE;
}

BOOL PropGridCtrl::DeleteProperty(PropGridProperty*& prop, BOOL redraw, BOOL adjustLayout)
{
	ASSERT_VALID(this);
	ASSERT_VALID(prop);

	BOOL found = FALSE;

	for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; /**/)
	{
		POSITION posSaved = pos;

		PropGridProperty* listProp = mProps.GetNext(pos);
		ASSERT_VALID(listProp);

		if (listProp == prop) // Top level property
		{
			found = TRUE;

			mProps.RemoveAt(posSaved);
			break;
		}

		if (listProp->RemoveSubItem(prop, FALSE))
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
	{
		return FALSE;
	}

	if (mSelection == prop)
	{
		mSelection = nullptr;
	}

	if (mSelection != nullptr)
	{
		for (PropGridProperty* parent = mSelection; parent != nullptr; parent = parent->GetParent())
		{
			if (parent == prop)
			{
				mSelection = nullptr;
				break;
			}
		}
	}

	delete prop;
	prop = nullptr;

	if (adjustLayout)
	{
		AdjustLayout();
		return TRUE;
	}

	if (redraw && GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}

	return TRUE;
}

void PropGridProperty::RemoveAllOptions()
{
	ASSERT_VALID(this);

	mOptions.RemoveAll();
	mFlags = 0;
}

int PropGridProperty::GetOptionCount() const
{
	ASSERT_VALID(this);
	return(int) mOptions.GetCount();
}

LPCTSTR PropGridProperty::GetOption(int nIndex) const
{
	ASSERT_VALID(this);

	if (nIndex < 0 || nIndex >= mOptions.GetCount())
	{
		ASSERT(FALSE);
		return nullptr;
	}

	POSITION pos = mOptions.FindIndex(nIndex);
	if (pos == nullptr)
	{
		ASSERT(FALSE);
		return nullptr;
	}

	return mOptions.GetAt(pos);
}

int PropGridProperty::GetExpandedSubItems(BOOL includeHidden) const
{
	ASSERT_VALID(this);

	if (!mExpanded)
	{
		return 0;
	}

	int count = 0;

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		if (includeHidden || prop->IsVisible())
		{
			count += prop->GetExpandedSubItems(includeHidden) + 1;
		}
	}

	return count;
}

PropGridProperty* PropGridProperty::HitTest(CPoint point, ClickArea* clickArea)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (mRect.PtInRect(point))
	{
		if (clickArea != nullptr)
		{
			if (IsGroup() && point.x < mRect.left + mRect.Height())
			{
				*clickArea = ClickExpandBox;
			}
			else if (HasValueField() && point.x > mWndList->mRectList.left + mWndList->mLeftColumnWidth)
			{
				*clickArea = ClickValue;
			}
			else
			{
				*clickArea = ClickName;
			}
		}

		return this;
	}

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		PropGridProperty* hit = prop->HitTest(point, clickArea);
		if (hit != nullptr)
		{
			return hit;
		}
	}

	return nullptr;
}

void PropGridProperty::Expand(BOOL expand)
{
	ASSERT_VALID(this);
	ASSERT(IsGroup());

	if (mExpanded == expand || mSubItems.IsEmpty())
	{
		return;
	}

	mExpanded = expand;

	if (mWndList != nullptr && mWndList->GetSafeHwnd() != nullptr)
	{
		ASSERT_VALID(mWndList);
		mWndList->AdjustLayout();

		CRect rectRedraw = mWndList->mRectList;
		rectRedraw.top = mRect.top;

		mWndList->RedrawWindow(rectRedraw);
	}
}

void PropGridProperty::ExpandDeep(BOOL expand)
{
	ASSERT_VALID(this);

	mExpanded = expand;

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		prop->ExpandDeep(expand);
	}
}

void PropGridProperty::ResetOriginalValue()
{
	ASSERT_VALID(this);

	mIsModified = FALSE;

	SetValue(mOriginalValue);

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		prop->ResetOriginalValue();
	}
}

void PropGridProperty::Redraw()
{
	ASSERT_VALID(this);

	if (mWndList != nullptr)
	{
		ASSERT_VALID(mWndList);
		mWndList->InvalidateRect(mRect);

		if (mParent != nullptr && mParent->mIsValueList)
		{
			mWndList->InvalidateRect(mParent->mRect);
		}

		if (mIsValueList)
		{
			for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
			{
				PropGridProperty* prop = mSubItems.GetNext(pos);
				ASSERT_VALID(prop);

				mWndList->InvalidateRect(prop->mRect);
			}
		}

		mWndList->UpdateWindow();
	}
}

void PropGridProperty::EnableSpinControl(BOOL enable, int minValue, int maxValue)
{
	ASSERT_VALID(this);

	switch (mValue.vt)
	{
	case VT_INT:
	case VT_UINT:
	case VT_I2:
	case VT_I4:
	case VT_UI2:
	case VT_UI4:
		break;

	default:
		ASSERT(FALSE);
		return;
	}

	mMinValue = minValue;
	mMaxValue = maxValue;

	if (enable)
	{
		mFlags |= PROP_HAS_SPIN;
	}
	else
	{
		mFlags &= ~PROP_HAS_SPIN;
	}
}

BOOL PropGridProperty::IsSelected() const
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	return mWndList->mSelection == this;
}

void PropGridProperty::SetName(LPCTSTR name, BOOL redraw)
{
	ASSERT_VALID(this);

	if (name == nullptr)
	{
		ASSERT(FALSE);
		return;
	}

	mName = name;

	if (redraw)
	{
		Redraw();
	}
}

void PropGridProperty::SetValue(const COleVariant& value)
{
	ASSERT_VALID(this);

	if (mValue.vt != VT_EMPTY && mValue.vt != value.vt)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bInPlaceEdit = mInPlaceEditing;
	if (bInPlaceEdit)
	{
		OnEndEdit();
	}

	mValue = value;
	Redraw();

	if (bInPlaceEdit)
	{
		ASSERT_VALID(mWndList);
		mWndList->EditItem(this);
	}
}

void PropGridProperty::SetOriginalValue(const COleVariant& value)
{
	ASSERT_VALID(this);

	if (mOriginalValue.vt != VT_EMPTY && mOriginalValue.vt != value.vt)
	{
		ASSERT(FALSE);
		return;
	}

	mOriginalValue = value;
}

BOOL PropGridProperty::IsParentExpanded() const
{
	ASSERT_VALID(this);

	for (PropGridProperty* prop = mParent; prop != nullptr; /**/)
	{
		ASSERT_VALID(prop);

		if (!prop->IsExpanded())
		{
			return FALSE;
		}

		prop = prop->mParent;
	}

	return TRUE;
}

int PropGridProperty::GetHierarchyLevel() const
{
	ASSERT_VALID(this);

	int level = 0;
	for (PropGridProperty* parent = mParent; parent != nullptr; parent = parent->mParent)
	{
		level++;
	}

	return level;
}

PropGridProperty* PropGridProperty::GetSubItem(int nIndex) const
{
	ASSERT_VALID(this);

	if (nIndex < 0 || nIndex >= mSubItems.GetCount())
	{
		ASSERT(FALSE);
		return nullptr;
	}

	return mSubItems.GetAt(mSubItems.FindIndex(nIndex));
}

void PropGridProperty::Enable(BOOL enable)
{
	ASSERT_VALID(this);

	if (mEnabled != enable)
	{
		mEnabled = enable;

		if (mWndList->GetSafeHwnd() != nullptr)
		{
			ASSERT_VALID(mWndList);
			mWndList->InvalidateRect(mRect);
		}
	}
}

void PropGridProperty::SetOwnerList(PropGridCtrl* wndList)
{
	ASSERT_VALID(this);
	ASSERT_VALID(wndList);

	mWndList = wndList;

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* pProp = mSubItems.GetNext(pos);
		ASSERT_VALID(pProp);

		pProp->SetOwnerList(mWndList);
	}
}

void PropGridProperty::Reposition(int& y)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	CRect rectOld = mRect;

	BOOL bShowProperty = mIsVisible &&(IsParentExpanded() || mWndList->mAlphabeticMode);

	if (mWndList->mAlphabeticMode &&
		mParent != nullptr &&
		mParent->mIsValueList &&
		!IsParentExpanded())
	{
		bShowProperty = FALSE;
	}

	if (bShowProperty)
	{
		int dx = mWndList->mAlphabeticMode ? mWndList->mRowHeight : GetHierarchyLevel() * mWndList->mRowHeight;

		if (mWndList->mAlphabeticMode && mIsValueList)
		{
			dx = 0;
		}

		mRect = CRect(mWndList->mRectList.left + dx, y, mWndList->mRectList.right, y + mWndList->mRowHeight);

		if (!mRectButton.IsRectEmpty())
		{
			mRectButton.top = mRect.top + 1;
			mRectButton.bottom = mRect.bottom;
		}

		y += mWndList->mRowHeight;

		CRect rectName = mRect;
		rectName.right = mWndList->mRectList.left + mWndList->mLeftColumnWidth;

		if (IsWindow(mWndList->mToolTip.GetSafeHwnd()))
		{
			mWndList->mToolTip.AddTool(mWndList,LPSTR_TEXTCALLBACK, rectName, mWndList->mTooltipsCount + 1);
			mWndList->mTooltipsCount++;

			if (!IsGroup())
			{
				CRect rectValue = mRect;
				rectValue.left = rectName.right + 1;
				mWndList->mToolTip.AddTool(mWndList, LPSTR_TEXTCALLBACK, rectValue, mWndList->mTooltipsCount + 1);
				mWndList->mTooltipsCount++;
			}
		}
	}
	else
	{
		mRect.SetRectEmpty();
		mRectButton.SetRectEmpty();
	}

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		prop->Reposition(y);
	}

	OnPosSizeChanged(rectOld);
}

void PropGridProperty::AddTerminalProp(CList<PropGridProperty*, PropGridProperty*>& props)
{
	ASSERT_VALID(this);

	if (!mIsGroup || mIsValueList)
	{
		// Insert sorted:
		BOOL bInserted = FALSE;
		for (POSITION pos = props.GetHeadPosition(); !bInserted && pos != nullptr; /**/)
		{
			POSITION posSave = pos;

			PropGridProperty* pProp = props.GetNext(pos);

			if (mWndList->CompareProps(pProp, this) > 0)
			{
				props.InsertBefore(posSave, this);
				bInserted = TRUE;
			}
		}

		if (!bInserted)
		{
			props.AddTail(this);
		}
		return;
	}

	mRect.SetRectEmpty();

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		prop->AddTerminalProp(props);
	}
}

BOOL PropGridProperty::IsSubItem(PropGridProperty* subProp) const
{
	ASSERT_VALID(this);
	ASSERT_VALID(subProp);

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		if (subProp == prop || prop->IsSubItem(subProp))
		{
			return TRUE;
		}
	}

	return FALSE;
}

PropGridProperty* PropGridProperty::FindSubItemByData(DWORD_PTR data) const
{
	ASSERT_VALID(this);

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* prop = mSubItems.GetNext(pos);
		ASSERT_VALID(prop);

		if (prop->mData == data)
		{
			return prop;
		}

		prop = prop->FindSubItemByData(data);

		if (prop != nullptr)
		{
			return prop;
		}
	}

	return nullptr;
}

CString PropGridProperty::FormatProperty()
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	COleVariant& var = mValue;

	CString strVal;

	if (mIsValueList)
	{
		for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
		{
			PropGridProperty* pProp = mSubItems.GetNext(pos);
			ASSERT_VALID(pProp);

			strVal += pProp->FormatProperty();

			if (pos != nullptr)
			{
				strVal += mWndList->mListDelimeter;
				strVal += _T(' ');
			}
		}

		return strVal;
	}

	switch (var.vt)
	{
	case VT_BSTR:
		strVal = var.bstrVal;
		break;

	case VT_I2:
		strVal.Format(PGP_FORMAT_SHORT, (short)var.iVal);
		break;

	case VT_I4:
	case VT_INT:
		strVal.Format(PGP_FORMAT_LONG, (long)var.lVal);
		break;

	case VT_UI1:
		if ((BYTE)var.bVal != 0)
		{
			strVal.Format(PGP_FORMAT_CHAR, (TCHAR)(BYTE)var.bVal);
		}
		break;

	case VT_UI2:
		strVal.Format(PGB_FORMAT_USHORT, var.uiVal);
		break;

	case VT_UINT:
	case VT_UI4:
		strVal.Format(PGP_FORMAT_ULONG, var.ulVal);
		break;

	case VT_R4:
		strVal.Format(PGP_FORMAT_FLOAT, (float)var.fltVal);
		break;

	case VT_R8:
		strVal.Format(PGP_FORMAT_DOUBLE, (double)var.dblVal);
		break;

	case VT_BOOL:
		strVal = var.boolVal == VARIANT_TRUE ? mWndList->mTrueString : mWndList->mFalseString;
		break;

	default:
		// Unsupported type
		strVal = _T("*** error ***");
	}

	return strVal;
}

void PropGridProperty::OnDrawName(CDC* dc, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(dc);
	ASSERT_VALID(mWndList);

	COLORREF clrTextOld = (COLORREF)-1;

	if (IsSelected() && (!mWndList->mVSDotNetLook || !IsGroup() || mIsValueList))
	{
		CRect rectFill = rect;
		rectFill.top++;

		if (!mWndList->mFocused)
		{
			clrTextOld = dc->SetTextColor(VisualManager::ToGdiColor(VisualManager::CLR_LEVEL1));
			dc->FillRect(&rectFill, theVisualManager->GetBaseBrush());
		}
		else
		{
			clrTextOld = dc->SetTextColor(VisualManager::ToGdiColor(VisualManager::CLR_WHITE));
			dc->FillRect(&rectFill, theVisualManager->GetBlueBrush());
		}
	}

	if (mWndList->mVSDotNetLook && IsGroup() && !mIsValueList)
	{
		if (mWndList->mGroupTextColor != (COLORREF)-1)
		{
			clrTextOld = dc->SetTextColor(mWndList->mGroupTextColor);
		}
		else
		{
			clrTextOld = dc->SetTextColor(VisualManager::ToGdiColor(VisualManager::CLR_LEVEL1));
		}
	}

	if (mParent != nullptr && mParent->mIsValueList)
	{
		rect.left += rect.Height();
	}

	rect.DeflateRect(AFX_TEXT_MARGIN, 0);

	int nTextHeight = dc->DrawText(mName, rect,
		DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

	mNameIsTruncated = dc->GetTextExtent(mName).cx > rect.Width();

	if (IsSelected() && mWndList->mVSDotNetLook && IsGroup() && !mIsValueList)
	{
		CRect rectFocus = rect;
		rectFocus.top = rectFocus.CenterPoint().y - nTextHeight / 2;
		rectFocus.bottom = rectFocus.top + nTextHeight;
		rectFocus.right = std::min(rect.right, rectFocus.left + dc->GetTextExtent(mName).cx);
		rectFocus.InflateRect(2, 0);

		COLORREF clrShadow = mWndList->mUseControlBarColors ?
			GetGlobalData()->clrBarShadow : GetGlobalData()->clrBtnShadow;

		dc->Draw3dRect(rectFocus, clrShadow, clrShadow);
	}

	if (clrTextOld != (COLORREF)-1)
	{
		dc->SetTextColor(clrTextOld);
	}
}

void PropGridProperty::OnDrawValue(CDC* dc, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(dc);
	ASSERT_VALID(mWndList);

	if ((IsGroup() && !mIsValueList) || !HasValueField())
	{
		return;
	}

	CFont* oldFont = nullptr;
	if (IsModified() && mWndList->mMarkModifiedProperties)
	{
		oldFont = dc->SelectObject(&mWndList->mFontBold);
	}

	CString strVal = FormatProperty();

	rect.DeflateRect(AFX_TEXT_MARGIN, 0);

	dc->DrawText(strVal, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

	mValueIsTruncated = dc->GetTextExtent(strVal).cx > rect.Width();

	if (oldFont != nullptr)
	{
		dc->SelectObject(oldFont);
	}
}

void PropGridProperty::OnDrawButton(CDC* dc, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(dc);
	ASSERT_VALID(mWndList);

	CMFCToolBarComboBoxButton button;
	
	dc->FillRect(&rect, theVisualManager->GetLevel3Brush());

	if (mFlags & PROP_HAS_LIST)
	{
		ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;
		dcRenderTarget->BindDC(dc->m_hDC, &rect);
		dcRenderTarget->BeginDraw();
		D2D1_RECT_F imageRect;
		imageRect.left = 0;
		imageRect.top = 0;
		imageRect.right = static_cast<FLOAT>(rect.Width());
		imageRect.bottom = static_cast<FLOAT>(rect.Height());
		if (mButtonIsDown)
		{
			dcRenderTarget->DrawBitmap(mWndList->comboDropdownButtonImageP, imageRect);
		}
		else
		{
			dcRenderTarget->DrawBitmap(mWndList->comboDropdownButtonImageN, imageRect);
		}
		dcRenderTarget->EndDraw();
		return; /* done */
	}

	CString str = _T("...");
	dc->DrawText(str, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	CMFCVisualManager::AFX_BUTTON_STATE state = mButtonIsDown ?
		CMFCVisualManager::ButtonsIsPressed : CMFCVisualManager::ButtonsIsRegular;
	theVisualManager->OnDrawButtonBorder(dc, &button, rect, state);

	if (mButtonIsFocused)
	{
		CRect rectFocus = rect;
		rectFocus.DeflateRect(2, 2);

		dc->DrawFocusRect(rectFocus);
	}
}

void PropGridProperty::OnDrawExpandBox(CDC* dc, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(dc);
	ASSERT_VALID(mWndList);
	ASSERT(IsGroup());

	CPoint ptCenter = rect.CenterPoint();

	int nMaxBoxSize = 9;
	if (GetGlobalData()->GetRibbonImageScale() != 1.)
	{
		nMaxBoxSize = (int)(.5 + nMaxBoxSize * GetGlobalData()->GetRibbonImageScale());
	}

	int nBoxSize = std::min(nMaxBoxSize, rect.Width ());

	rect = CRect(ptCenter, CSize(1, 1));
	rect.InflateRect(nBoxSize / 2, nBoxSize / 2);

	if (mIsValueList)
	{
		if (mWndList->mBackgroundBrush.GetSafeHandle() != nullptr)
		{
			dc->FillRect(rect, &mWndList->mBackgroundBrush);
		}
		else
		{
			dc->FillRect(rect, &(GetGlobalData()->brWindow));
		}
	}

	COLORREF clrShadow = mWndList->mUseControlBarColors ?
		GetGlobalData()->clrBarShadow : GetGlobalData()->clrBtnShadow;
	COLORREF clrText = mWndList->mUseControlBarColors ?
		GetGlobalData()->clrBarText : GetGlobalData()->clrBtnText;

	theVisualManager->OnDrawExpandingBox(dc, rect,
		mExpanded && !mSubItems.IsEmpty(),
		mWndList->mVSDotNetLook ? clrText : clrShadow);
}

void PropGridProperty::OnDrawDescription(CDC* dc, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(dc);
	ASSERT_VALID(mWndList);

	HFONT hOldFont = static_cast<HFONT>(::SelectObject(dc->GetSafeHdc(), mWndList->mFontBold.GetSafeHandle()));
	int nHeight = dc->DrawText(mName, rect, DT_SINGLELINE | DT_NOPREFIX);

	::SelectObject(dc->GetSafeHdc(), hOldFont);

	rect.top += nHeight + 2;

	dc->DrawText(mDescr, rect, DT_WORDBREAK | DT_NOPREFIX | DT_END_ELLIPSIS);
}

BOOL PropGridProperty::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndInPlace);
	ASSERT_VALID(mWndList);
	ASSERT(::IsWindow(mWndInPlace->GetSafeHwnd()));

	CString text;
	mWndInPlace->GetWindowText(text);

	BOOL result = FALSE;
	BOOL isChanged = FormatProperty() != text;

	if (mIsValueList)
	{
		CString delimeter(mWndList->mListDelimeter);

		for (int i = 0; !text.IsEmpty() && i < GetSubItemsCount(); i++)
		{
			CString strItem = text.SpanExcluding(delimeter);

			if (strItem.GetLength() + 1 > text.GetLength())
			{
				text.Empty();
			}
			else
			{
				text = text.Mid(strItem.GetLength() + 1);
			}

			strItem.TrimLeft();
			strItem.TrimRight();

			PropGridProperty* subItem = GetSubItem(i);
			ASSERT_VALID(subItem);

			subItem->TextToVar(strItem);
		}

		result = TRUE;
	}
	else
	{
		result = TextToVar(text);
	}

	if (result && isChanged)
	{
		mWndList->OnPropertyChanged(this);
	}

	return result;
}

BOOL PropGridProperty::TextToVar(const CString& text)
{
	CString strVal = text;

	switch (mValue.vt)
	{
	case VT_BSTR:
		mValue = (LPCTSTR) strVal;
		return TRUE;

	case VT_UI1:
		mValue = strVal.IsEmpty() ?(BYTE) 0 :(BYTE) strVal[0];
		return TRUE;

	case VT_I2:
		mValue = (short) _ttoi(strVal);
		return TRUE;

	case VT_INT:
	case VT_I4:
		mValue = _ttol(strVal);
		return TRUE;

	case VT_UI2:
		mValue.uiVal = unsigned short(_ttoi(strVal));
		return TRUE;

	case VT_UINT:
	case VT_UI4:
		mValue.ulVal = unsigned long(_ttol(strVal));
		return TRUE;

	case VT_R4:
		{
			float fVal = 0.;

			strVal.TrimLeft();
			strVal.TrimRight();

			if (!strVal.IsEmpty())
			{
				_stscanf_s(strVal, PGP_FORMAT_FLOAT, &fVal);
			}

			mValue = fVal;
		}
		return TRUE;

	case VT_R8:
		{
			double dblVal = 0.;

			strVal.TrimLeft();
			strVal.TrimRight();

			if (!strVal.IsEmpty())
			{
				_stscanf_s(strVal, PGP_FORMAT_DOUBLE, &dblVal);
			}

			mValue = dblVal;
		}
		return TRUE;

	case VT_BOOL:
		strVal.TrimRight();
		mValue = (VARIANT_BOOL)(strVal == mWndList->mTrueString);
		return TRUE;
	}

	return FALSE;
}

BOOL PropGridProperty::IsValueChanged() const
{
	ASSERT_VALID(this);

	if (mOriginalValue.vt != mValue.vt)
	{
		return FALSE;
	}

	const COleVariant& var = mValue;
	const COleVariant& var1 = mOriginalValue;

	switch (mValue.vt)
	{
	case VT_BSTR:
		{
			CString str1 = var.bstrVal;
			CString str2 = var1.bstrVal;

			return str1 != str2;
		}
		break;

	case VT_I2:
		return(short)var.iVal != (short)var1.iVal;

	case VT_I4:
	case VT_INT:
		return(long)var.lVal != (long)var1.lVal;

	case VT_UI1:
		return(BYTE)var.bVal != (BYTE)var1.bVal;

	case VT_UI2:
		return var.uiVal != var1.uiVal;

	case VT_UINT:
	case VT_UI4:
		return var.ulVal != var1.ulVal;

	case VT_R4:
		return(float)var.fltVal != (float)var1.fltVal;

	case VT_R8:
		return(double)var.dblVal != (double)var1.dblVal;

	case VT_BOOL:
		return(VARIANT_BOOL)var.boolVal != (VARIANT_BOOL)var1.boolVal;
	}

	return FALSE;
}

BOOL PropGridProperty::OnEdit(LPPOINT)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (!HasValueField())
	{
		return FALSE;
	}

	mWndInPlace = nullptr;

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect(rectEdit, rectSpin);

	BOOL bDefaultFormat = FALSE;
	mWndInPlace = CreateInPlaceEdit(rectEdit, bDefaultFormat);

	if (mWndInPlace != nullptr)
	{
		if (bDefaultFormat)
		{
			mWndInPlace->SetWindowTextW(FormatProperty());
		}

		if (mFlags & PROP_HAS_LIST)
		{
			CRect rectCombo = mRect;
			rectCombo.left = rectEdit.left - 4;

			mWndCombo = CreateCombo(mWndList, rectCombo);
			ASSERT_VALID(mWndCombo);

			mWndCombo->SetFont(mWndList->GetFont());

			// Synchronize bottom edge of the combobox with the property bottom edge:
			mWndCombo->GetWindowRect(rectCombo);
			mWndList->ScreenToClient(&rectCombo);

			int dy = rectCombo.Height() - mRect.Height();

			mWndCombo->SetWindowPos(nullptr,
				rectCombo.left,
				rectCombo.top - dy + 1,
				-1,
				-1,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			if (mValue.vt == VT_BOOL)
			{
				mOptions.AddTail(mWndList->mTrueString);
				mOptions.AddTail(mWndList->mFalseString);
			}

			for (POSITION pos = mOptions.GetHeadPosition(); pos != nullptr; /**/)
			{
				mWndCombo->AddString(mOptions.GetNext(pos));
			}
		}

		if (mFlags & PROP_HAS_SPIN)
		{
			mWndSpin = CreateSpinControl(rectSpin);
		}

		mWndInPlace->SetFont(IsModified() && mWndList->mMarkModifiedProperties ?
			&mWndList->mFontBold : mWndList->GetFont());
		mWndInPlace->SetFocus();

		if (!mAllowEdit)
		{
			mWndInPlace->HideCaret();
		}

		mInPlaceEditing = TRUE;
		return TRUE;
	}

	return FALSE;
}

void PropGridProperty::AdjustButtonRect()
{
	ASSERT_VALID(this);

	mRectButton = mRect;
	mRectButton.left = mRectButton.right - mRectButton.Height() + 3;
	mRectButton.top ++;
}

void PropGridProperty::AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	rectEdit = mRect;
	//rectEdit.DeflateRect(0, 2);
	rectEdit.top += 3; // XS-NOTE: to look the same as in the display mode

	int nMargin = mWndList->mMarkModifiedProperties && mIsModified ?
		mWndList->mBoldEditLeftMargin : mWndList->mEditLeftMargin;

	rectEdit.left = mWndList->mRectList.left +
		mWndList->mLeftColumnWidth + AFX_TEXT_MARGIN - nMargin + 1;

	if (HasButton())
	{
		AdjustButtonRect();
		rectEdit.right = mRectButton.left;
	}

	if (mFlags & PROP_HAS_SPIN)
	{
		rectSpin = mRect;
		rectSpin.right = rectEdit.right;
		rectSpin.left = rectSpin.right - rectSpin.Height();
		rectSpin.top ++;
		rectEdit.right = rectSpin.left;
	}
	else
	{
		rectSpin.SetRectEmpty();
	}
}

CWnd* PropGridProperty::CreateInPlaceEdit(CRect rectEdit, BOOL& defaultFormat)
{
	switch (mValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI2:
	case VT_UI4:
	case VT_BOOL:
		break;

	default:
		if (!mIsValueList)
		{
			return nullptr;
		}
	}

	CEdit* wndEdit = nullptr;

	if (!mEditMask.IsEmpty() || !mEditTemplate.IsEmpty() || !mValidChars.IsEmpty())
	{
		CMFCMaskedEdit* pWndEditMask = new CMFCMaskedEdit;
		pWndEditMask->EnableSetMaskedCharsOnly(FALSE);
		pWndEditMask->EnableGetMaskedCharsOnly(FALSE);

		if (!mEditMask.IsEmpty() && !mEditTemplate.IsEmpty())
		{
			pWndEditMask->EnableMask(mEditMask, mEditTemplate, _T(' '));
		}

		if (!mValidChars.IsEmpty())
		{
			pWndEditMask->SetValidChars(mValidChars);
		}

		wndEdit = pWndEditMask;
	}
	else
	{
		wndEdit = new CEdit;
	}

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL;

	if (!mEnabled || !mAllowEdit)
	{
		dwStyle |= ES_READONLY;
	}

	wndEdit->Create(dwStyle, rectEdit, mWndList, PropGridCtrl::WND_ID_INPLACE);

	defaultFormat = TRUE;
	return wndEdit;
}

CSpinButtonCtrl* PropGridProperty::CreateSpinControl(CRect rectSpin)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	CSpinButtonCtrl* wndSpin = new CMFCSpinButtonCtrl;

	if (!wndSpin->Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS,
		rectSpin, mWndList, PropGridCtrl::WND_ID_INPLACE))
	{
		return nullptr;
	}

	wndSpin->SetBuddy(mWndInPlace);

	if (mMinValue != 0 || mMaxValue != 0)
	{
		wndSpin->SetRange32(mMinValue, mMaxValue);
	}

	return wndSpin;
}

BOOL PropGridProperty::OnEndEdit()
{
	ASSERT_VALID(this);

	mInPlaceEditing = FALSE;
	mButtonIsFocused = FALSE;

	OnDestroyWindow();
	return TRUE;
}

CComboBox* PropGridProperty::CreateCombo(CWnd* wndParent, CRect rect)
{
	ASSERT_VALID(this);

	rect.bottom = rect.top + 400;
	rect.left += 2; // XS-NOTE

	CComboBox* wndCombo = new CComboBox;
	if (!wndCombo->Create(WS_CHILD | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | WS_VSCROLL,
		rect, wndParent, PropGridCtrl::WND_ID_INPLACE))
	{
		delete wndCombo;
		return nullptr;
	}

	return wndCombo;
}

void PropGridProperty::OnClickButton(CPoint)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (mWndCombo != nullptr)
	{
		mButtonIsDown = TRUE;
		Redraw();

		CString str;
		mWndInPlace->GetWindowTextW(str);

		mWndCombo->SetCurSel(mWndCombo->FindStringExact(-1, str));

		mWndCombo->SetFocus();
		mWndCombo->ShowDropDown();
	}
}

BOOL PropGridProperty::OnClickValue(UINT msg, CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (mWndInPlace == nullptr)
	{
		return FALSE;
	}

	ASSERT_VALID(mWndInPlace);

	if (mWndSpin != nullptr)
	{
		ASSERT_VALID(mWndSpin);
		ASSERT(mWndSpin->GetSafeHwnd() != nullptr);

		CRect rectSpin;
		mWndSpin->GetClientRect(rectSpin);
		mWndSpin->MapWindowPoints(mWndList, rectSpin);

		if (rectSpin.PtInRect(point))
		{
			mWndList->MapWindowPoints(mWndSpin, &point, 1);

			mWndSpin->SendMessageW(msg, 0, MAKELPARAM(point.x, point.y));
			return TRUE;
		}
	}

	CPoint ptEdit = point;
	MapWindowPoints( mWndList->GetSafeHwnd(), mWndInPlace->GetSafeHwnd(), &ptEdit, 1);

	mWndInPlace->SendMessageW(msg, 0, MAKELPARAM(ptEdit.x, ptEdit.y));
	return TRUE;
}

BOOL PropGridProperty::OnDblClk(CPoint)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (mWndInPlace == nullptr)
	{
		return FALSE;
	}

	ASSERT(::IsWindow(mWndInPlace->GetSafeHwnd()));

	if (mOptions.GetCount() > 1)
	{
		CString strText;
		mWndInPlace->GetWindowText(strText);

		POSITION pos = mOptions.Find(strText);
		if (pos == nullptr)
		{
			return FALSE;
		}

		mOptions.GetNext(pos);
		if (pos == nullptr)
		{
			pos = mOptions.GetHeadPosition();
		}

		ENSURE(pos != nullptr);
		strText = mOptions.GetAt(pos);

		mWndInPlace->SetWindowText(strText);
		OnUpdateValue();

		return TRUE;
	}

	if (mFlags & PROP_HAS_BUTTON)
	{
		CWaitCursor wait;

		CString strPrevVal = FormatProperty();

		OnClickButton(CPoint(-1, -1));

		if (strPrevVal != FormatProperty())
		{
			mWndList->OnPropertyChanged(this);
		}

		return TRUE;
	}

	return FALSE;
}

void PropGridProperty::OnSelectCombo()
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndCombo);
	ASSERT_VALID(mWndInPlace);

	int selIndex = mWndCombo->GetCurSel();
	if (selIndex >= 0)
	{
		CString str;
		mWndCombo->GetLBText(selIndex, str);
		mWndInPlace->SetWindowText(str);
		OnUpdateValue();
	}
}

void PropGridProperty::OnCloseCombo()
{
	ASSERT_VALID(this);

	mButtonIsDown = FALSE;
	Redraw();

	ASSERT_VALID(mWndInPlace);
	mWndInPlace->SetFocus();
}

BOOL PropGridProperty::OnSetCursor() const
{
	if (mInPlaceEditing)
	{
		return FALSE;
	}

	if (mIsValueList)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
		return TRUE;
	}

	switch (mValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI2:
	case VT_UI4:
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
		return TRUE;
	}

	return FALSE;
}

BOOL PropGridProperty::PushChar(UINT nChar)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);
	ASSERT(mWndList->mSelection == this);
	ASSERT_VALID(mWndInPlace);

	if (mIsValueList)
	{
		if (mEnabled && mAllowEdit && nChar != _T('+') && nChar != _T('-'))
		{
			mWndInPlace->SetWindowTextW(_T(""));
			mWndInPlace->SendMessageW(WM_CHAR, (WPARAM) nChar);
			return TRUE;
		}
	}

	switch (mValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_INT:
	case VT_UINT:
	case VT_I4:
	case VT_UI2:
	case VT_UI4:
		if (mEnabled && mAllowEdit)
		{
			mWndInPlace->SetWindowTextW(_T(""));
			mWndInPlace->SendMessageW(WM_CHAR, (WPARAM) nChar);
			return TRUE;
		}
	}

	if (!mAllowEdit)
	{
		if (nChar == VK_SPACE)
		{
			OnDblClk(CPoint(-1, -1));
		}
		else if (mOptions.GetCount() > 1)
		{
			CString strText;
			mWndInPlace->GetWindowTextW(strText);

			POSITION pos = mOptions.Find(strText);
			if (pos == nullptr)
			{
				return FALSE;
			}

			POSITION posSave = pos;
			CString strChar((TCHAR) nChar);
			strChar.MakeUpper();

			mOptions.GetNext(pos);

			while (pos != posSave)
			{
				if (pos == nullptr)
				{
					pos = mOptions.GetHeadPosition();
				}

				if (pos == posSave)
				{
					break;
				}

				strText = mOptions.GetAt(pos);

				CString strUpper = strText;
				strUpper.MakeUpper();

				if (strUpper.Left(1) == strChar)
				{
					mWndInPlace->SetWindowTextW(strText);
					OnUpdateValue();
					break;
				}

				mOptions.GetNext(pos);
			}
		}
	}

	OnEndEdit();

	if (::GetCapture() == mWndList->GetSafeHwnd())
	{
		ReleaseCapture();
	}

	return FALSE;
}

CString PropGridProperty::GetNameTooltip()
{
	ASSERT_VALID(this);
	return _T("");
}

CString PropGridProperty::GetValueTooltip()
{
	ASSERT_VALID(this);
	return _T("");
}

HBRUSH PropGridProperty::OnCtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (mWndList->mTextColor != (COLORREF)-1)
	{
		pDC->SetTextColor(mWndList->mTextColor);
	}

	if (mWndList->mBackgroundBrush.GetSafeHandle() != nullptr)
	{
		if (mWndList->mBackgroundColor != -1)
		{
			pDC->SetBkColor(mWndList->mBackgroundColor);
		}

		return(HBRUSH) mWndList->mBackgroundBrush.GetSafeHandle();
	}

	switch (mValue.vt)
	{
	case VT_BSTR:
	case VT_R4:
	case VT_R8:
	case VT_UI1:
	case VT_I2:
	case VT_I4:
	case VT_INT:
	case VT_UINT:
	case VT_UI2:
	case VT_UI4:
	case VT_BOOL:
		if (!mEnabled || !mAllowEdit)
		{
			pDC->SetBkColor(GetGlobalData()->clrWindow);
			return(HBRUSH) GetGlobalData()->brWindow.GetSafeHandle();
		}
	}

	return nullptr;
}

void PropGridProperty::SetModifiedFlag()
{
	ASSERT_VALID(mWndList);

	BOOL isModified = IsValueChanged();

	if (mIsModified == isModified && !mIsValueList)
	{
		return;
	}

	mIsModified = isModified;

	if (mParent != nullptr && mParent->mIsValueList)
	{
		if (isModified)
		{
			mParent->mIsModified = TRUE;
		}
		else
		{
			mParent->mIsModified = FALSE;

			for (POSITION pos = mParent->mSubItems.GetHeadPosition(); pos != nullptr; /**/)
			{
				PropGridProperty* pSubItem = (PropGridProperty*)mParent->mSubItems.GetNext(pos);
				ASSERT_VALID(pSubItem);

				if (pSubItem->mIsModified)
				{
					mParent->mIsModified = TRUE;
					break;
				}
			}
		}
	}

	if (mIsValueList)
	{
		for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
		{
			PropGridProperty* pSubItem = (PropGridProperty*)mSubItems.GetNext(pos);
			ASSERT_VALID(pSubItem);

			if (pSubItem->mIsModified)
			{
				mIsModified = TRUE;
				break;
			}
		}
	}

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* pSubItem = (PropGridProperty*) mSubItems.GetNext(pos);
		ASSERT_VALID(pSubItem);

		pSubItem->SetModifiedFlag();
	}

	if (mWndList->mMarkModifiedProperties)
	{
		OnPosSizeChanged(mRect);

		if (mWndInPlace->GetSafeHwnd() != nullptr)
		{
			if (mIsModified)
			{
				mWndInPlace->SetFont(&mWndList->mFontBold);
			}
			else
			{
				mWndInPlace->SetFont(mWndList->GetFont());
			}

			CRect rectInPlace;
			mWndInPlace->GetWindowRect(&rectInPlace);
			mWndList->ScreenToClient(&rectInPlace);

			int nXOffset = mWndList->mBoldEditLeftMargin - mWndList->mEditLeftMargin;

			if (mIsModified)
			{
				nXOffset = -nXOffset;
			}

			if (HasButton())
			{
				AdjustButtonRect();
				rectInPlace.right = mRectButton.left;
			}

			mWndInPlace->SetWindowPos(nullptr,
				rectInPlace.left + nXOffset,
				rectInPlace.top,
				rectInPlace.Width() - nXOffset,
				rectInPlace.Height(),
				SWP_NOZORDER | SWP_NOACTIVATE);
		}

		Redraw();
	}
}

void PropGridProperty::Show(BOOL show, BOOL adjustLayout)
{
	ASSERT_VALID(this);

	if (mIsVisible == show)
	{
		return;
	}
	mIsVisible = show;

	for (POSITION pos = mSubItems.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* pProp = mSubItems.GetNext(pos);
		ASSERT_VALID(pProp);

		pProp->Show(show, FALSE);
	}

	if (adjustLayout && mWndList != nullptr && mWndList->GetSafeHwnd() != nullptr)
	{
		ASSERT_VALID(mWndList);
		mWndList->AdjustLayout();
	}
}

BOOL PropGridProperty::OnActivateByTab()
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	if (mInPlaceEditing &&(mFlags & PROP_HAS_BUTTON))
	{
		mButtonIsFocused = !mButtonIsFocused;
		mWndList->RedrawWindow(mRectButton);
		return TRUE;
	}

	if (!mInPlaceEditing && mEnabled && mWndList->EditItem(this))
	{
		CEdit* pEdit = DYNAMIC_DOWNCAST(CEdit, mWndInPlace);
		if (::IsWindow(pEdit->GetSafeHwnd()))
		{
			pEdit->SetSel(0, -1);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL PropGridProperty::OnRotateListValue(BOOL forward)
{
	if (mWndInPlace == nullptr)
	{
		return FALSE;
	}

	CString text;
	mWndInPlace->GetWindowTextW(text);

	POSITION pos = mOptions.Find(text);
	if (pos == nullptr)
	{
		return FALSE;
	}

	if (forward)
	{
		mOptions.GetNext(pos);
		if (pos == nullptr)
		{
			pos = mOptions.GetHeadPosition();
		}
	}
	else
	{
		mOptions.GetPrev(pos);
		if (pos == nullptr)
		{
			pos = mOptions.GetTailPosition();
		}
	}

	if (pos == nullptr)
	{
		return FALSE;
	}

	text = mOptions.GetAt(pos);

	mWndInPlace->SetWindowTextW(text);
	OnUpdateValue();

	CEdit* edit = DYNAMIC_DOWNCAST(CEdit, mWndInPlace);
	if (::IsWindow(edit->GetSafeHwnd()))
	{
		edit->SetSel(0, -1);
	}
	return TRUE;
}

} // namespace hsui
