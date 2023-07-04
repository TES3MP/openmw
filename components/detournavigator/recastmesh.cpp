#include "recastmesh.hpp"
#include "exceptions.hpp"

namespace DetourNavigator
{
    Mesh::Mesh(std::vector<int>&& indices, std::vector<float>&& vertices, std::vector<AreaType>&& areaTypes)
    {
        if (indices.size() / 3 != areaTypes.size())
            throw InvalidArgument("Number of flags doesn't match number of triangles: triangles="
<<<<<<< HEAD
                + std::to_string(getTrianglesCount()) + ", areaTypes=" + std::to_string(mAreaTypes.size()));
        if (getVerticesCount())
            rcCalcBounds(mVertices.data(), static_cast<int>(getVerticesCount()), mBounds.mMin.ptr(), mBounds.mMax.ptr());
        mIndices.shrink_to_fit();
        mVertices.shrink_to_fit();
        mAreaTypes.shrink_to_fit();
        mWater.shrink_to_fit();
=======
                + std::to_string(indices.size() / 3) + ", areaTypes=" + std::to_string(areaTypes.size()));
        indices.shrink_to_fit();
        vertices.shrink_to_fit();
        areaTypes.shrink_to_fit();
        mIndices = std::move(indices);
        mVertices = std::move(vertices);
        mAreaTypes = std::move(areaTypes);
    }

    RecastMesh::RecastMesh(const Version& version, Mesh mesh, std::vector<CellWater> water,
        std::vector<Heightfield> heightfields, std::vector<FlatHeightfield> flatHeightfields,
        std::vector<MeshSource> meshSources)
        : mVersion(version)
        , mMesh(std::move(mesh))
        , mWater(std::move(water))
        , mHeightfields(std::move(heightfields))
        , mFlatHeightfields(std::move(flatHeightfields))
        , mMeshSources(std::move(meshSources))
    {
        mWater.shrink_to_fit();
        mHeightfields.shrink_to_fit();
        for (Heightfield& v : mHeightfields)
            v.mHeights.shrink_to_fit();
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    }
}
