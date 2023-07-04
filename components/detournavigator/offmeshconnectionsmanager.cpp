#include "offmeshconnectionsmanager.hpp"
<<<<<<< HEAD
#include "settings.hpp"
#include "settingsutils.hpp"
#include "tileposition.hpp"
#include "objectid.hpp"
#include "offmeshconnection.hpp"

#include <algorithm>
#include <vector>
#include <set>

namespace DetourNavigator
{
    OffMeshConnectionsManager::OffMeshConnectionsManager(const Settings& settings)
        : mSettings(settings)
    {}
=======
#include "objectid.hpp"
#include "offmeshconnection.hpp"
#include "settings.hpp"
#include "settingsutils.hpp"
#include "tileposition.hpp"

#include <algorithm>
#include <set>
#include <vector>

namespace DetourNavigator
{
    OffMeshConnectionsManager::OffMeshConnectionsManager(const RecastSettings& settings)
        : mSettings(settings)
    {
    }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

    void OffMeshConnectionsManager::add(const ObjectId id, const OffMeshConnection& value)
    {
        const auto values = mValues.lock();

        values->mById.insert(std::make_pair(id, value));

        const auto startTilePosition = getTilePosition(mSettings, value.mStart);
        const auto endTilePosition = getTilePosition(mSettings, value.mEnd);

        values->mByTilePosition[startTilePosition].insert(id);

        if (startTilePosition != endTilePosition)
            values->mByTilePosition[endTilePosition].insert(id);
    }

    std::set<TilePosition> OffMeshConnectionsManager::remove(const ObjectId id)
    {
        const auto values = mValues.lock();

        const auto byId = values->mById.equal_range(id);

        if (byId.first == byId.second)
            return {};

        std::set<TilePosition> removed;

<<<<<<< HEAD
        std::for_each(byId.first, byId.second, [&] (const auto& v) {
=======
        std::for_each(byId.first, byId.second, [&](const auto& v) {
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
            const auto startTilePosition = getTilePosition(mSettings, v.second.mStart);
            const auto endTilePosition = getTilePosition(mSettings, v.second.mEnd);

            removed.emplace(startTilePosition);
            if (startTilePosition != endTilePosition)
                removed.emplace(endTilePosition);
        });

        for (const TilePosition& tilePosition : removed)
        {
            const auto it = values->mByTilePosition.find(tilePosition);
            if (it == values->mByTilePosition.end())
                continue;
            it->second.erase(id);
            if (it->second.empty())
                values->mByTilePosition.erase(it);
        }

        values->mById.erase(byId.first, byId.second);

        return removed;
    }

<<<<<<< HEAD
    std::vector<OffMeshConnection> OffMeshConnectionsManager::get(const TilePosition& tilePosition)
    {
        std::vector<OffMeshConnection> result;

        const auto values = mValues.lock();
=======
    std::vector<OffMeshConnection> OffMeshConnectionsManager::get(const TilePosition& tilePosition) const
    {
        std::vector<OffMeshConnection> result;

        const auto values = mValues.lockConst();
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        const auto itByTilePosition = values->mByTilePosition.find(tilePosition);

        if (itByTilePosition == values->mByTilePosition.end())
            return result;

<<<<<<< HEAD
        std::for_each(itByTilePosition->second.begin(), itByTilePosition->second.end(),
            [&] (const ObjectId v)
            {
                const auto byId = values->mById.equal_range(v);
                std::for_each(byId.first, byId.second, [&] (const auto& v)
                {
                    if (getTilePosition(mSettings, v.second.mStart) == tilePosition
                            || getTilePosition(mSettings, v.second.mEnd) == tilePosition)
                        result.push_back(v.second);
                });
            });
=======
        std::for_each(itByTilePosition->second.begin(), itByTilePosition->second.end(), [&](const ObjectId v) {
            const auto byId = values->mById.equal_range(v);
            std::for_each(byId.first, byId.second, [&](const auto& v) {
                if (getTilePosition(mSettings, v.second.mStart) == tilePosition
                    || getTilePosition(mSettings, v.second.mEnd) == tilePosition)
                    result.push_back(v.second);
            });
        });
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        std::sort(result.begin(), result.end());

        return result;
    }
}
