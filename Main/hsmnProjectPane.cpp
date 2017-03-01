#include "hsmnPrecompiled.h"
#include "hsmnProjectPane.h"
#include "hsmnApplication.h"
#include "hsmnMainFrame.h"
#include <hsmoModuleBlock.h>
#include <hsmoModelManager.h>
#include <hsuiPropManager.h>
#include <hsuiVisualManager.h>

#undef max
#undef min

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(ProjectPane, DockablePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_COMMAND(HSMN_IDC_SET_AS_ACTIVE_MODEL, OnSetAsActiveModel)
	ON_COMMAND(HSMN_IDC_CLOSE_MODEL, OnCloseModel)
END_MESSAGE_MAP()

ProjectPane::ProjectPane()
{
	mActiveModel = nullptr;
}

ProjectPane::~ProjectPane()
{
}

void ProjectPane::AddComponent(Component* component)
{
	ComponentToHandle::iterator iter;

	iter = mComponentToHandle.find(component);
	if (iter != mComponentToHandle.end())
	{
		return;
	}

	TreeCtrl::Item item;

	if (component->IsDerivedFrom(XSC_RTTI(Model)))
	{
		Model* model = static_cast<Model*>(component);

		item.image = 0;
		item.data = model;
		item.text = model->GetName();
		int handle = mView.InsertItem(item);
		mComponentToHandle[model] = handle;

		const BlockList& blocks = model->GetBlocks();
		for (auto block : blocks)
		{
			AddComponent(block);
		}
	}
	else if (component->IsDerivedFrom(XSC_RTTI(ModuleBlock)))
	{
		auto moduleBlock = static_cast<ModuleBlock*>(component);

		Component* nodalParent;
		auto level = moduleBlock->GetLevel();
		if (level->IsDerivedFrom(XSC_RTTI(Model)))
		{
			nodalParent = level;
		}
		else
		{
			assert(level->IsDerivedFrom(XSC_RTTI(ModuleLevel)));
			nodalParent = level->GetParent();
		}
		iter = mComponentToHandle.find(nodalParent);
		assert(iter != mComponentToHandle.end());
		int parent = iter->second;

		item.image = 1;
		item.data = moduleBlock;
		item.text = moduleBlock->GetName();
		int handle = mView.InsertItem(item, parent);
		mComponentToHandle[moduleBlock] = handle;

		auto moduleLevel = moduleBlock->GetModuleLevel();
		auto& blocks = moduleLevel->GetBlocks();
		for (auto block : blocks)
		{
			AddComponent(block);
		}
	}
	else
	{
		auto block = static_cast<Block*>(component);
		
		Component* nodalParent;
		auto level = block->GetLevel();
		if (level->IsDerivedFrom(XSC_RTTI(Model)))
		{
			nodalParent = level;
		}
		else
		{
			assert(level->IsDerivedFrom(XSC_RTTI(ModuleLevel)));
			nodalParent = level->GetParent();
		}
		iter = mComponentToHandle.find(nodalParent);
		assert(iter != mComponentToHandle.end());
		int parent = iter->second;

		item.image = 2;
		item.data = block;
		item.text = block->GetName();
		int handle = mView.InsertItem(item, parent);
		mComponentToHandle[block] = handle;
	}
}

void ProjectPane::OnNotiChildAddition(Component*, const ComponentSet& components)
{
	LevelSet reorder;
	int maxBlocks = 0;
	bool refreshNeeded = false;
	for (auto component : components)
	{
		if (component->IsDerivedFrom(XSC_RTTI(Model)))
		{
			auto model = static_cast<Model*>(component);
			AddComponent(model);

			refreshNeeded = true;
		}
		else if (component->IsDerivedFrom(XSC_RTTI(Block)))
		{
			auto block = static_cast<Block*>(component);
			auto level = static_cast<Level*>(block->GetLevel());
			reorder.insert(level);
			int numBlocks = (int)(level->GetBlocks().size());
			maxBlocks = std::max(maxBlocks, numBlocks);
			AddComponent(block);

			refreshNeeded = true;
		}
	}

	vector<int> children;
	children.reserve(maxBlocks);
	ComponentToHandle::iterator levelIter;
	ComponentToHandle::iterator blockIter;
	for (auto level : reorder)
	{
		children.clear();
		auto& blocks = level->GetBlocks();
		for (auto block : blocks)
		{
			blockIter = mComponentToHandle.find(block);
			assert(blockIter != mComponentToHandle.end());
			children.push_back(blockIter->second);
		}

		Component* nodalParent;
		if (level->IsDerivedFrom(XSC_RTTI(Model)))
		{
			nodalParent = level;
		}
		else
		{
			assert(level->IsDerivedFrom(XSC_RTTI(ModuleLevel)));
			nodalParent = level->GetParent();
		}

		levelIter = mComponentToHandle.find(nodalParent);
		assert(levelIter != mComponentToHandle.end());
		mView.Reorder(levelIter->second, children);
	}

	if (refreshNeeded)
	{
		mView.Refresh();
	}
}

void ProjectPane::OnNotiChildRemoval(Component*, const ComponentSet& components)
{
	mTemporaries.clear();
	for (auto item : mComponentToHandle)
	{
		for (auto component : components)
		{
			if (component->Contains(item.first))
			{
				mTemporaries.push_back(item.first);
				break;
			}
		}
	}

	ComponentToHandle::const_iterator iter;
	for (auto component : mTemporaries)
	{
		iter = mComponentToHandle.find(component);
		mView.RemoveItem(iter->second);
		mComponentToHandle.erase(iter);
	}

	if (mTemporaries.size())
	{
		mView.Refresh();
	}
}

void ProjectPane::OnNotiPropertyChange(Component* sender, Entity::PropID prop)
{
	if (sender == theModelManager)
	{
		if (prop == ModelManager::PROP_ACTIVE_MODEL)
		{
			ComponentToHandle::iterator iter;

			iter = mComponentToHandle.find(mActiveModel);
			if (iter != mComponentToHandle.end())
			{
				mView.SetItemTextBoldness(iter->second, false);
			}
		
			mActiveModel = theModelManager->GetActiveModel();
			iter = mComponentToHandle.find(mActiveModel);
			if (iter != mComponentToHandle.end())
			{
				mView.SetItemTextBoldness(iter->second, true);
			}

			mView.Refresh();
		}
	}
	else
	{
		if (prop == Component::PROP_NAME)
		{
			ComponentToHandle::const_iterator iter;
			iter = mComponentToHandle.find(sender);
			if (iter != mComponentToHandle.end())
			{
				mView.SetItemText(iter->second, sender->GetName());
				mView.Refresh();
			}
		}
	}
}

void ProjectPane::OnLButtonDownOnItem(TreeCtrl* treeCtrl)
{
	int selectedItem = mView.GetSelectedItem();
	void* itemData = mView.GetItemData(selectedItem);
	Component* component = reinterpret_cast<Component*>(itemData);
	ComponentToHandle::const_iterator iter;
	iter = mComponentToHandle.find(component);
	if (iter == mComponentToHandle.end())
	{
		return;
	}

	ComponentSet targets;
	targets.insert(component);
	thePropManager->SetTargets(std::move(targets));
}

void ProjectPane::OnSelectionChanged(TreeCtrl* treeCtrl)
{
}

void ProjectPane::OnItemDoubleClicked(TreeCtrl*)
{
	int selectedItem = mView.GetSelectedItem();
	Component* component = reinterpret_cast<Component*>(mView.GetItemData(selectedItem));
	ComponentToHandle::const_iterator iter;
	iter = mComponentToHandle.find(component);
	if (iter != mComponentToHandle.end())
	{
		Level* level;
		if (component->IsDerivedFrom(XSC_RTTI(ModuleBlock)))
		{
			level = static_cast<ModuleBlock*>(component)->GetModuleLevel();
		}
		else
		{
			level = static_cast<Level*>(component->GetLevel());
		}
		MainFrame* mainFrame = static_cast<MainFrame*>(theApp.m_pMainWnd);
		mainFrame->OpenLevelView(level);
	}
}

void ProjectPane::OnSetAsActiveModel()
{
	int handle = mView.GetSelectedItem();
	if (handle)
	{
		ComponentToHandle::iterator iter;
		Component* component = reinterpret_cast<Component*>(mView.GetItemData(handle));
		iter = mComponentToHandle.find(component);
		if (iter != mComponentToHandle.end())
		{
			if (component->IsDerivedFrom(XSC_RTTI(Model)))
			{
				Model* oldActiveModel = theModelManager->GetActiveModel();
				Model* newActiveModel = static_cast<Model*>(component);
				if (oldActiveModel != newActiveModel)
				{
					theModelManager->SetActiveModel(newActiveModel);
				}
			}
		}
	}
}

void ProjectPane::OnCloseModel()
{
	int handle = mView.GetSelectedItem();
	if (handle)
	{
		ComponentToHandle::iterator iter;
		Component* component = reinterpret_cast<Component*>(mView.GetItemData(handle));
		iter = mComponentToHandle.find(component);
		if (iter != mComponentToHandle.end())
		{
			if (component->IsDerivedFrom(XSC_RTTI(Model)))
			{
				theModelManager->CloseModel(static_cast<Model*>(component));
			}
		}
	}
}

void ProjectPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int handle = mView.GetSelectedItem();
	if (handle)
	{
		ComponentToHandle::iterator iter;
		Component* component = reinterpret_cast<Component*>(mView.GetItemData(handle));
		iter = mComponentToHandle.find(component);
		if (iter != mComponentToHandle.end())
		{
			if (component->IsDerivedFrom(XSC_RTTI(Model)))
			{
				theApp.GetContextMenuManager()->ShowPopupMenu(HSMN_IDR_PROJECT_POPUP, point.x, point.y, this, TRUE);
			}
		}
	}
}

int ProjectPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DockablePane::OnCreate(lpCreateStruct);

	mView.CreateWnd(this, 1, 1, 100, 100);
	mView.ShowHorzScrollBar(false);

	const ModelList& models = theModelManager->GetModels();
	for (auto model : models)
	{
		AddComponent(model);
	}

	mActiveModel = theModelManager->GetActiveModel();
	ComponentToHandle::iterator iter = mComponentToHandle.find(mActiveModel);
	if (iter != mComponentToHandle.end())
	{
		mView.SetItemTextBoldness(iter->second, true);
	}

	UINT imageList[] =
	{
		HSMN_IDR_PROJECT_TREE_ICON_MODEL,
		HSMN_IDR_PROJECT_TREE_ICON_MODULE,
		HSMN_IDR_PROJECT_TREE_ICON_BLOCK
	};
	mView.BuildImageList(theApp.m_hInstance, 3, imageList);

	mView.Refresh();
	mView.SetListener(static_cast<TreeCtrl::IListener*>(this));

	theNotiCenter->RegisterListener(static_cast<INotiListener*>(this));

	return 0;
}

void ProjectPane::OnDestroy()
{
	theNotiCenter->UnregisterListener(static_cast<INotiListener*>(this));
	
	mView.ResetListener();

	DockablePane::OnDestroy();
}

void ProjectPane::OnSize(UINT nType, int cx, int cy)
{
	DockablePane::OnSize(nType, cx, cy);

	mView.ResizeWnd(cx - 2, cy - 2);
}

void ProjectPane::OnSetFocus(CWnd* pOldWnd)
{
	DockablePane::OnSetFocus(pOldWnd);

	mView.SetFocus();
}

void ProjectPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	CPen* originalPen = dc.SelectObject(theVisualManager->GetLevel3Pen());

	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.right - 1, rect.top);
	dc.LineTo(rect.right - 1, rect.bottom - 1);
	dc.LineTo(rect.left, rect.bottom - 1);
	dc.LineTo(rect.left, rect.top);

	dc.SelectObject(originalPen);
}

} // namespace hsmn
