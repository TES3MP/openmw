#include "settings.hpp"
#include "parser.hpp"
#include "values.hpp"

#include <charconv>
#include <filesystem>
#include <sstream>
#include <system_error>

#if !(defined(_MSC_VER) && (_MSC_VER >= 1924)) && !(defined(__GNUC__) && __GNUC__ >= 11) || defined(__clang__)         \
    || defined(__apple_build_version__)

#include <cerrno>
#include <ios>
#include <locale>

#endif

#include <components/files/configurationmanager.hpp>
#include <components/misc/strings/algorithm.hpp>
#include <components/misc/strings/conversion.hpp>

namespace Settings
{
<<<<<<< HEAD

CategorySettingValueMap Manager::mDefaultSettings = CategorySettingValueMap();
CategorySettingValueMap Manager::mUserSettings = CategorySettingValueMap();
CategorySettingVector Manager::mChangedSettings = CategorySettingVector();

void Manager::clear()
{
    mDefaultSettings.clear();
    mUserSettings.clear();
    mChangedSettings.clear();
}

/*
    Start of tes3mp change (major)

    Add a base64encoded argument to this function to allow unencoded files to still be opened
*/
void Manager::loadDefault(const std::string &file, bool base64encoded)
{
    SettingsFileParser parser;
    parser.loadSettingsFile(file, mDefaultSettings, base64encoded);
}
/*
    End of tes3mp change (major)
*/

void Manager::loadUser(const std::string &file)
{
    SettingsFileParser parser;
    parser.loadSettingsFile(file, mUserSettings);
}

void Manager::saveUser(const std::string &file)
{
    SettingsFileParser parser;
    parser.saveSettingsFile(file, mUserSettings);
}

std::string Manager::getString(const std::string &setting, const std::string &category)
{
    CategorySettingValueMap::key_type key = std::make_pair(category, setting);
    CategorySettingValueMap::iterator it = mUserSettings.find(key);
    if (it != mUserSettings.end())
        return it->second;

    it = mDefaultSettings.find(key);
    if (it != mDefaultSettings.end())
        return it->second;

    throw std::runtime_error(std::string("Trying to retrieve a non-existing setting: ") + setting
                             + ".\nMake sure the defaults.bin file was properly installed.");
}

float Manager::getFloat (const std::string& setting, const std::string& category)
{
    const std::string& value = getString(setting, category);
    std::stringstream stream(value);
    float number = 0.f;
    stream >> number;
    return number;
}

double Manager::getDouble (const std::string& setting, const std::string& category)
{
    const std::string& value = getString(setting, category);
    std::stringstream stream(value);
    double number = 0.0;
    stream >> number;
    return number;
}

int Manager::getInt (const std::string& setting, const std::string& category)
{
    const std::string& value = getString(setting, category);
    std::stringstream stream(value);
    int number = 0;
    stream >> number;
    return number;
}

bool Manager::getBool (const std::string& setting, const std::string& category)
{
    const std::string& string = getString(setting, category);
    return Misc::StringUtils::ciEqual(string, "true");
}

osg::Vec2f Manager::getVector2 (const std::string& setting, const std::string& category)
{
    const std::string& value = getString(setting, category);
    std::stringstream stream(value);
    float x, y;
    stream >> x >> y;
    if (stream.fail())
        throw std::runtime_error(std::string("Can't parse 2d vector: " + value));
    return osg::Vec2f(x, y);
}

osg::Vec3f Manager::getVector3 (const std::string& setting, const std::string& category)
{
    const std::string& value = getString(setting, category);
    std::stringstream stream(value);
    float x, y, z;
    stream >> x >> y >> z;
    if (stream.fail())
        throw std::runtime_error(std::string("Can't parse 3d vector: " + value));
    return osg::Vec3f(x, y, z);
}

void Manager::setString(const std::string &setting, const std::string &category, const std::string &value)
{
    CategorySettingValueMap::key_type key = std::make_pair(category, setting);

    CategorySettingValueMap::iterator found = mUserSettings.find(key);
    if (found != mUserSettings.end())
=======
    namespace
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    {
        template <class T>
        T parseNumberFromSetting(const std::string& value, std::string_view setting, std::string_view category)
        {
            T number{};

            const auto result = std::from_chars(value.data(), value.data() + value.size(), number);
            if (result.ec != std::errc())
            {
                throw std::system_error(std::make_error_code(result.ec),
                    "Failed to parse number from setting [" + std::string(category) + "] " + std::string(setting)
                        + " value \"" + value + "\"");
            }

            return number;
        }

#if !(defined(_MSC_VER) && (_MSC_VER >= 1924)) && !(defined(__GNUC__) && __GNUC__ >= 11) || defined(__clang__)         \
    || defined(__apple_build_version__)
        template <>
        float parseNumberFromSetting<float>(
            const std::string& value, std::string_view setting, std::string_view category)
        {
            std::istringstream iss(value);
            iss.imbue(std::locale::classic());

            float floatValue = 0.0f;

            if (!(iss >> floatValue))
            {
                throw std::system_error(errno, std::generic_category(),
                    "Failed to parse number from setting [" + std::string(category) + "] " + std::string(setting)
                        + " value \"" + value + "\"");
            }

            return floatValue;
        }

        template <>
        double parseNumberFromSetting<double>(
            const std::string& value, std::string_view setting, std::string_view category)
        {
            std::istringstream iss(value);
            iss.imbue(std::locale::classic());

            double doubleValue = 0.0;

            if (!(iss >> doubleValue))
            {
                throw std::system_error(errno, std::generic_category(),
                    "Failed to parse number from setting [" + std::string(category) + "] " + std::string(setting)
                        + " value \"" + value + "\"");
            }

            return doubleValue;
        }
#endif
        template <class T>
        std::string serialize(const T& value)
        {
            std::ostringstream stream;
            stream << value;
            return stream.str();
        }
    }

    CategorySettingValueMap Manager::mDefaultSettings = CategorySettingValueMap();
    CategorySettingValueMap Manager::mUserSettings = CategorySettingValueMap();
    CategorySettingVector Manager::mChangedSettings = CategorySettingVector();
    std::set<std::pair<std::string_view, std::string_view>> Manager::sInitialized;

    void Manager::clear()
    {
        mDefaultSettings.clear();
        mUserSettings.clear();
        mChangedSettings.clear();
    }

    std::filesystem::path Manager::load(const Files::ConfigurationManager& cfgMgr, bool loadEditorSettings)
    {
        SettingsFileParser parser;
        const std::vector<std::filesystem::path>& paths = cfgMgr.getActiveConfigPaths();
        if (paths.empty())
            throw std::runtime_error("No config dirs! ConfigurationManager::readConfiguration must be called first.");

        // Create file name strings for either the engine or the editor.
        std::string defaultSettingsFile;
        std::string userSettingsFile;

<<<<<<< HEAD
void Manager::setDouble (const std::string &setting, const std::string &category, const double value)
{
    std::ostringstream stream;
    stream << value;
    setString(setting, category, stream.str());
}

void Manager::setBool(const std::string &setting, const std::string &category, const bool value)
{
    setString(setting, category, value ? "true" : "false");
}
=======
        if (!loadEditorSettings)
        {
            defaultSettingsFile = "defaults.bin";
            userSettingsFile = "settings.cfg";
        }
        else
        {
            defaultSettingsFile = "defaults-cs.bin";
            userSettingsFile = "openmw-cs.cfg";
        }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        // Create the settings manager and load default settings file.
        const auto defaultsBin = paths.front() / defaultSettingsFile;
        if (!std::filesystem::exists(defaultsBin))
            throw std::runtime_error("No default settings file found! Make sure the file \"" + defaultSettingsFile
                + "\" was properly installed.");
        parser.loadSettingsFile(defaultsBin, mDefaultSettings, true, false);

        const CategorySettingValueMap originalDefaultSettings = mDefaultSettings;

        // Load "settings.cfg" or "openmw-cs.cfg" from every config dir except the last one as additional default
        // settings.
        for (int i = 0; i < static_cast<int>(paths.size()) - 1; ++i)
        {
            const auto additionalDefaults = paths[i] / userSettingsFile;
            if (std::filesystem::exists(additionalDefaults))
                parser.loadSettingsFile(additionalDefaults, mDefaultSettings, false, true);
        }

        if (!loadEditorSettings)
            Settings::StaticValues::initDefaults();

        // Load "settings.cfg" or "openmw-cs.cfg" from the last config dir as user settings. This path will be used to
        // save modified settings.
        auto settingspath = paths.back() / userSettingsFile;
        if (std::filesystem::exists(settingspath))
            parser.loadSettingsFile(settingspath, mUserSettings, false, false);

        if (!loadEditorSettings)
            Settings::StaticValues::init();

        for (const auto& [key, value] : originalDefaultSettings)
            if (!sInitialized.contains(key))
                throw std::runtime_error("Default setting [" + key.first + "] " + key.second + " is not initialized");

        return settingspath;
    }

    void Manager::saveUser(const std::filesystem::path& file)
    {
        SettingsFileParser parser;
        parser.saveSettingsFile(file, mUserSettings);
    }

    const std::string& Manager::getString(std::string_view setting, std::string_view category)
    {
        const auto key = std::make_pair(category, setting);
        CategorySettingValueMap::iterator it = mUserSettings.find(key);
        if (it != mUserSettings.end())
            return it->second;

        it = mDefaultSettings.find(key);
        if (it != mDefaultSettings.end())
            return it->second;

        throw std::runtime_error("Trying to retrieve a non-existing setting: [" + std::string(category) + "] "
            + std::string(setting) + ".\nMake sure the defaults.bin file was properly installed.");
    }

    std::vector<std::string> Manager::getStringArray(std::string_view setting, std::string_view category)
    {
        // TODO: it is unclear how to handle empty value -
        // there is no difference between empty serialized array
        // and a serialized array which has one empty value
        std::vector<std::string> values;
        const std::string& value = getString(setting, category);
        if (value.empty())
            return values;

        Misc::StringUtils::split(value, values, ",");
        for (auto& item : values)
            Misc::StringUtils::trim(item);
        return values;
    }

    float Manager::getFloat(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<float>(getString(setting, category), setting, category);
    }

    double Manager::getDouble(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<double>(getString(setting, category), setting, category);
    }

    int Manager::getInt(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<int>(getString(setting, category), setting, category);
    }

    std::uint64_t Manager::getUInt64(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<uint64_t>(getString(setting, category), setting, category);
    }

    std::size_t Manager::getSize(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<size_t>(getString(setting, category), setting, category);
    }

    unsigned Manager::getUnsigned(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<unsigned>(getString(setting, category), setting, category);
    }

    unsigned long Manager::getUnsignedLong(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<unsigned long>(getString(setting, category), setting, category);
    }

    unsigned long long Manager::getUnsignedLongLong(std::string_view setting, std::string_view category)
    {
        return parseNumberFromSetting<unsigned long long>(getString(setting, category), setting, category);
    }

    bool Manager::getBool(std::string_view setting, std::string_view category)
    {
        const std::string& string = getString(setting, category);
        return Misc::StringUtils::ciEqual(string, "true");
    }

    osg::Vec2f Manager::getVector2(std::string_view setting, std::string_view category)
    {
        const std::string& value = getString(setting, category);

        std::vector<std::string> components;
        Misc::StringUtils::split(value, components);

        if (components.size() == 2)
        {
            auto x = Misc::StringUtils::toNumeric<float>(components[0]);
            auto y = Misc::StringUtils::toNumeric<float>(components[1]);

            if (x && y)
            {
                return { x.value(), y.value() };
            }
        }

        throw std::runtime_error(std::string("Can't parse 2d vector: " + value));
    }

    osg::Vec3f Manager::getVector3(std::string_view setting, std::string_view category)
    {
        const std::string& value = getString(setting, category);

        std::vector<std::string> components;
        Misc::StringUtils::split(value, components);

        if (components.size() == 3)
        {
            auto x = Misc::StringUtils::toNumeric<float>(components[0]);
            auto y = Misc::StringUtils::toNumeric<float>(components[1]);
            auto z = Misc::StringUtils::toNumeric<float>(components[2]);

            if (x && y && z)
            {
                return { x.value(), y.value(), z.value() };
            }
        }

        throw std::runtime_error(std::string("Can't parse 3d vector: " + value));
    }

    void Manager::setString(std::string_view setting, std::string_view category, const std::string& value)
    {
        auto found = mUserSettings.find(std::make_pair(category, setting));
        if (found != mUserSettings.end())
        {
            if (found->second == value)
                return;
        }

        CategorySettingValueMap::key_type key(category, setting);

        mUserSettings[key] = value;

        mChangedSettings.insert(std::move(key));
    }

    void Manager::setStringArray(
        std::string_view setting, std::string_view category, const std::vector<std::string>& value)
    {
        std::stringstream stream;

        // TODO: escape delimeters, new line characters, etc.
        for (size_t i = 0; i < value.size(); ++i)
        {
            std::string item = value[i];
            Misc::StringUtils::trim(item);
            stream << item;

            if (i < value.size() - 1)
                stream << ",";
        }

        setString(setting, category, stream.str());
    }

    void Manager::setInt(std::string_view setting, std::string_view category, const int value)
    {
        std::ostringstream stream;
        stream << value;
        setString(setting, category, stream.str());
    }

    void Manager::setUInt64(std::string_view setting, std::string_view category, const std::uint64_t value)
    {
        std::ostringstream stream;
        stream << value;
        setString(setting, category, stream.str());
    }

    void Manager::setFloat(std::string_view setting, std::string_view category, const float value)
    {
        std::ostringstream stream;
        stream << value;
        setString(setting, category, stream.str());
    }

    void Manager::setDouble(std::string_view setting, std::string_view category, const double value)
    {
        std::ostringstream stream;
        stream << value;
        setString(setting, category, stream.str());
    }

    void Manager::setBool(std::string_view setting, std::string_view category, const bool value)
    {
        setString(setting, category, value ? "true" : "false");
    }

    void Manager::setVector2(std::string_view setting, std::string_view category, const osg::Vec2f value)
    {
        std::ostringstream stream;
        stream << value.x() << " " << value.y();
        setString(setting, category, stream.str());
    }

    void Manager::setVector3(std::string_view setting, std::string_view category, const osg::Vec3f value)
    {
        std::ostringstream stream;
        stream << value.x() << ' ' << value.y() << ' ' << value.z();
        setString(setting, category, stream.str());
    }

    CategorySettingVector Manager::getPendingChanges()
    {
        return mChangedSettings;
    }

    CategorySettingVector Manager::getPendingChanges(const CategorySettingVector& filter)
    {
        CategorySettingVector intersection;
        std::set_intersection(mChangedSettings.begin(), mChangedSettings.end(), filter.begin(), filter.end(),
            std::inserter(intersection, intersection.begin()));
        return intersection;
    }

    void Manager::resetPendingChanges()
    {
        mChangedSettings.clear();
    }

    void Manager::resetPendingChanges(const CategorySettingVector& filter)
    {
        for (const auto& key : filter)
        {
            mChangedSettings.erase(key);
        }
    }

    void Manager::set(std::string_view setting, std::string_view category, int value)
    {
        setInt(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, unsigned value)
    {
        setString(setting, category, serialize(value));
    }

    void Manager::set(std::string_view setting, std::string_view category, unsigned long value)
    {
        setString(setting, category, serialize(value));
    }

    void Manager::set(std::string_view setting, std::string_view category, unsigned long long value)
    {
        setString(setting, category, serialize(value));
    }

    void Manager::set(std::string_view setting, std::string_view category, float value)
    {
        setFloat(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, double value)
    {
        setDouble(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, const std::string& value)
    {
        setString(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, bool value)
    {
        setBool(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, const osg::Vec2f& value)
    {
        setVector2(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, const osg::Vec3f& value)
    {
        setVector3(setting, category, value);
    }

    void Manager::set(std::string_view setting, std::string_view category, DetourNavigator::CollisionShapeType value)
    {
        setInt(setting, category, static_cast<int>(value));
    }

    void Manager::recordInit(std::string_view setting, std::string_view category)
    {
        sInitialized.emplace(category, setting);
    }

}
