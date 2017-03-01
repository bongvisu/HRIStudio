#pragma once

#include <xscEntity.h>
#include <xscMemento.h>
#include <xscMPColor.h>
#include <xscMPRectangle.h>

namespace xsc {

class Component;
typedef Pointer<Component> ComponentPtr;
typedef std::list<Component*> ComponentList;
typedef std::vector<Component*> ComponentVec;
typedef std::unordered_set<Component*> ComponentSet;
typedef std::list<ComponentPtr> ComponentPtrList;
typedef std::vector<ComponentPtr> ComponentPtrVec;
typedef std::unordered_set<ComponentPtr, std::hash<Component*>> ComponentPtrSet;
typedef std::unordered_map<Component*, Component*> ComponentToComponent;
typedef std::unordered_map<Component*, ComponentSet> ComponentToComponentSet;

class XSC_API IComponentListener
{
public:
	virtual void OnChildAddition(const ComponentSet& components) = 0;
	virtual void OnChildRemoval(const ComponentSet& components) = 0;
	virtual void OnPropertyChange(Entity::PropID prop) = 0;
	virtual void OnNodalGeometryChange() = 0;

protected:
	virtual ~IComponentListener();
};

class XSC_API Component : public Entity
{
	XSC_DECLARE_DYNAMIC(Component)

public:
	void RegisterListener(IComponentListener* listener);
	void UnregisterListener(IComponentListener* listener);

	static const PropID PROP_NAME = Entity::LAST_PROP;
	static const PropID LAST_PROP = PROP_NAME + 1;

public:
	virtual ~Component();
	
	void SetParent(Component* parent);
	Component* GetParent() const;

	const wchar_t* FormatID() const;

	virtual void SetName(const std::wstring& name);
	const std::wstring& GetName() const;

	virtual void GetHierarchicalPath(std::wstring& path) const;
	virtual bool IsChild(const Component* component) const;
	virtual bool Contains(const Component* component) const;
	virtual void GetChildren(ComponentVec& children, bool deep) const;
	virtual void GetChildren(ComponentSet& children, bool deep) const;

	virtual Component* GetModel() const;
	virtual Component* GetLevel() const;

	class XSC_API ComponentMemento : public Memento
	{
	public:
		ComponentMemento(Component* component);
		virtual void Restore() override;

	protected:
		Component* mComponent;

		ComponentPtrSet mChildren;
		MementoPtrVec mChildMementos;
	};
	virtual MementoPtr SaveState() const;

	virtual Component* Clone(ComponentToComponent& remapper);
	virtual void Remap(const ComponentToComponent& remapper);

	class XSC_API Stream final
	{
	public:
		Stream();
		~Stream();

		void BeginOutputComponent(const Component* component);
		void EndOutputComponent();
		void BeginOutputSection(const wchar_t* name, bool newLine, bool indent);
		void EndOutputSection(bool newLine, bool indent);
		void SetAttribute(const wchar_t* name, const wchar_t* value);
		void SetSimpleValaue(const wchar_t* text);
		void SetCDataValue(const wchar_t* data);
		bool Save(const wchar_t* path);

		bool Load(const wchar_t* path);
		int BeginInputComponents();
		void EndInputComponents();
		int BeginInputSection(const std::wstring& name);
		void EndInputSection();
		void ActivateItem(int i);
		void DeactivateItem();
		Component* ToComponent();
		Component* FromID(ID id);
		bool GetAttribute(const wchar_t* name, std::wstring& value);
		bool GetTextValue(std::wstring& value);

	protected:
		IXMLDOMDocument* mDomDocument;
		std::stack<IXMLDOMNode*> mStack;

		int mIndentation;
		std::wstring mFormattedString;

		std::vector<IXMLDOMElement*> mTemporaryItems;
		std::unordered_map<ID, Component*> mID2Component;
		std::stack<std::vector<IXMLDOMElement*>> mItemStack;
	};
	virtual bool Serialize(Stream& stream);
	virtual bool Deserialize(Stream& stream);
	virtual bool DoPostloadProcessing(Stream& stream);

protected:
	Component();
		
	void NotifyChildAddition(const ComponentSet& components);
	void NotifyChildRemoval(const ComponentSet& components);
	void NotifyPropertyChange(PropID prop);
	void NotifyNodalGeometryChange();

protected:
	std::wstring mName;
	Component* mParent;
	ComponentPtrSet mChildren;

protected:
	std::unordered_set<IComponentListener*> mListeners;

protected:
	static const unsigned int GENERIC_TEXT_BUF_SIZE = 4096;
	static wchar_t msGenericTextBuf[GENERIC_TEXT_BUF_SIZE];
	static void TokenizeCsv(const std::wstring& source, std::vector<std::wstring>& tokens);
};

} // namespace xsc
