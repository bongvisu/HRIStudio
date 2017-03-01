#pragma once

#include <xscLevel.h>
#include <xscCommand.h>
#include <hsmoIModel.h>

namespace hsmo {

class Model;
typedef xsc::Pointer<Model> ModelPtr;
typedef std::list<Model*> ModelList;
typedef std::vector<Model*> ModelVec;
typedef std::unordered_set<Model*> ModelSet;
typedef std::unordered_map<Model*, xsc::ComponentSet> ModelToComponentSet;

class HSMO_API Model
	:
	public xsc::Level,
	public IModel
{
	XSC_DECLARE_DYNAMIC(Model)

public:
	static const wchar_t* FILE_EXTENSION;

public:
	Model(const std::wstring& path);
	virtual ~Model();

	const std::wstring& GetFilePath() const;
	bool Load();
	bool Save();

	bool Compile(std::wstring& code);

	static void SortOutPerContext(const xsc::ComponentSet&, ModelToComponentSet&);
	void ExecuteCommand(const xsc::CommandPtr& command);
	void EnrollCustomCommand(xsc::Command* command);

	void Undo();
	void Redo();

	virtual xsc::Component* GetModel() const override;
	virtual const std::wstring& GetDirectory() const override;
	virtual void GetHierarchicalPath(std::wstring& path) const override;

	// Stream ---------------------------------------------------------------------------------------------------------
	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	// ----------------------------------------------------------------------------------------------------------------

protected:
	std::wstring mFilePath;
	std::wstring mDirectory;

	std::deque<xsc::CommandPtr> mUndoStack;
	std::deque<xsc::CommandPtr> mRedoStack;
};

} // namespace hsmo
