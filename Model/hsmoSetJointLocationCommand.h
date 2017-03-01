#pragma once

#include <xscCommand.h>
#include <xscLinkJoint.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API SetJointLocationCommand : public xsc::Command
{
public:
	SetJointLocationCommand(xsc::LinkJointSet&& joints, xsc::MPVector::Dimension dim, int value);
	virtual ~SetJointLocationCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::LinkJointSet mJoints;
	xsc::MPVector::Dimension mDim;
	int mValue;
};

} // namespace hsmo
