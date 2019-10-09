#include "Miscellaneous.hpp"

#include <components/openmw-mp/MWMPLog.hpp>

#include <apps/openmw-mp/Networking.hpp>

#include <iostream>
using namespace std;

unsigned int MiscellaneousFunctions::GetLastPlayerId() noexcept
{
    return Players::getLastPlayerId();
}

int MiscellaneousFunctions::GetCurrentMpNum() noexcept
{
    return mwmp::Networking::getPtr()->getCurrentMpNum();
}

void MiscellaneousFunctions::SetCurrentMpNum(int mpNum) noexcept
{
    mwmp::Networking::getPtr()->setCurrentMpNum(mpNum);
}
