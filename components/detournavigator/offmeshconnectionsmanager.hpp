#ifndef OPENMW_COMPONENTS_DETOURNAVIGATOR_OFFMESHCONNECTIONSMANAGER_H
#define OPENMW_COMPONENTS_DETOURNAVIGATOR_OFFMESHCONNECTIONSMANAGER_H

<<<<<<< HEAD
#include "settings.hpp"
#include "tileposition.hpp"
=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#include "objectid.hpp"
#include "offmeshconnection.hpp"
#include "settings.hpp"
#include "tileposition.hpp"

#include <components/misc/guarded.hpp>

#include <map>
#include <set>
#include <unordered_set>
#include <vector>

namespace DetourNavigator
{
    class OffMeshConnectionsManager
    {
    public:
<<<<<<< HEAD
        OffMeshConnectionsManager(const Settings& settings);
=======
        explicit OffMeshConnectionsManager(const RecastSettings& settings);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        void add(const ObjectId id, const OffMeshConnection& value);

        std::set<TilePosition> remove(const ObjectId id);

<<<<<<< HEAD
        std::vector<OffMeshConnection> get(const TilePosition& tilePosition);
=======
        std::vector<OffMeshConnection> get(const TilePosition& tilePosition) const;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

    private:
        struct Values
        {
            std::multimap<ObjectId, OffMeshConnection> mById;
            std::map<TilePosition, std::unordered_set<ObjectId>> mByTilePosition;
        };

        const RecastSettings& mSettings;
        Misc::ScopeGuarded<Values> mValues;
    };
}

#endif
