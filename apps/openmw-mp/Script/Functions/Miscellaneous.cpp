#include "Miscellaneous.hpp"

#include <components/openmw-mp/TimedLog.hpp>

#include <apps/openmw-mp/Networking.hpp>

#include <extern/PicoSHA2/picosha2.h>

#include <iostream>
#include <random>

using namespace std;

const char *MiscellaneousFunctions::GenerateRandomString(unsigned int length) noexcept
{
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(0, characters.size() - 1);

    /* WARNING: this function is no longer reentrant with a static variable */
    static std::string randomString;

    for (std::size_t i = 0; i < length; ++i)
    {
        randomString += characters[distribution(generator)];
    }

    return randomString.c_str();
}

const char *MiscellaneousFunctions::GetSHA256Hash(const char* inputString) noexcept
{
    /* WARNING: this function is no longer reentrant with a static variable */
    static std::string hashString = picosha2::hash256_hex_string(std::string{inputString});

    return hashString.c_str();
}

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
