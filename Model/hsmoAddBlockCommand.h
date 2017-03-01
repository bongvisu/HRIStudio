#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API AddBlockCommand : public xsc::Command
{
public:
	AddBlockCommand(xsc::Level* level, const wchar_t* rttiName, xsc::MPVector center);
	virtual ~AddBlockCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::Level* mSuperLevel;
	const wchar_t* mRttiName;
	xsc::MPVector mCenter;
};

} // namespace hsmo
