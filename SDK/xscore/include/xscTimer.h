#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API Timer
{
public:
    Timer();

    int64_t GetTicks() const;
    double GetSeconds() const;

    double GetSeconds(int64_t numTicks) const;
    int64_t GetTicks(double seconds) const;

    void Reset();

private:
    int64_t mFrequency;
    int64_t mInitialTicks;
    double mInvFrequency;
};

} // namespace xsc
