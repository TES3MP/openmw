#ifndef GAME_MWWORLD_PTR_H
#define GAME_MWWORLD_PTR_H

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>

#include "livecellref.hpp"

namespace MWWorld
{
    class ContainerStore;
    class CellStore;
    struct LiveCellRefBase;

    /// \brief Pointer to a LiveCellRef
<<<<<<< HEAD

    class Ptr
    {
        public:

            MWWorld::LiveCellRefBase *mRef;
            CellStore *mCell;
            ContainerStore *mContainerStore;

        public:
            Ptr(MWWorld::LiveCellRefBase *liveCellRef=nullptr, CellStore *cell=nullptr)
              : mRef(liveCellRef), mCell(cell), mContainerStore(nullptr)
            {
            }

            bool isEmpty() const
            {
                return mRef == nullptr;
            }

            const std::string& getTypeName() const;

            const Class& getClass() const
            {
                if(mRef != nullptr)
                    return *(mRef->mClass);
                throw std::runtime_error("Cannot get class of an empty object");
            }

            template<typename T>
            MWWorld::LiveCellRef<T> *get() const
            {
                MWWorld::LiveCellRef<T> *ref = dynamic_cast<MWWorld::LiveCellRef<T>*>(mRef);
                if(ref) return ref;

                std::stringstream str;
                str<< "Bad LiveCellRef cast to "<<typeid(T).name()<<" from ";
                /*
                    Start of tes3mp change (major)

                    Print additional information
                */
                if(mRef != nullptr) str<< getTypeName() << " " << mRef->mRef.getRefId().c_str() << " " << mRef->mRef.getRefNum().mIndex << "-" << mRef->mRef.getMpNum();
                /*
                    End of tes3mp change (major)
                */

                else str<< "an empty object";

                throw std::runtime_error(str.str());
            }

            MWWorld::LiveCellRefBase *getBase() const;

            MWWorld::CellRef& getCellRef() const;

            RefData& getRefData() const;

            CellStore *getCell() const
            {
                assert(mCell);
                return mCell;
            }

            bool isInCell() const
            {
                return (mContainerStore == nullptr) && (mCell != nullptr);
            }

            void setContainerStore (ContainerStore *store);
            ///< Must not be called on references that are in a cell.

            ContainerStore *getContainerStore() const;
            ///< May return a 0-pointer, if reference is not in a container.

            operator const void *();
            ///< Return a 0-pointer, if Ptr is empty; return a non-0-pointer, if Ptr is not empty
    };

    /// \brief Pointer to a const LiveCellRef
    /// @note a Ptr can be implicitely converted to a ConstPtr, but you can not convert a ConstPtr to a Ptr.
    class ConstPtr
=======
    /// @note PtrBase is never used directly and needed only to define Ptr and ConstPtr
    template <template <class> class TypeTransform>
    class PtrBase
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    {
    public:
        typedef TypeTransform<MWWorld::LiveCellRefBase> LiveCellRefBaseType;
        typedef TypeTransform<CellStore> CellStoreType;
        typedef TypeTransform<ContainerStore> ContainerStoreType;

        LiveCellRefBaseType* mRef;
        CellStoreType* mCell;
        ContainerStoreType* mContainerStore;

        bool isEmpty() const { return mRef == nullptr; }

        // Returns a 32-bit id of the ESM record this object is based on.
        // Specific values of ids are defined in ESM::RecNameInts.
        // Note 1: ids are not sequential. E.g. for a creature `getType` returns 0x41455243.
        // Note 2: Life is not easy and full of surprises. For example
        //         prison marker reuses ESM::Door record. Player is ESM::NPC.
        unsigned int getType() const
        {
            if (mRef != nullptr)
                return mRef->getType();
            throw std::runtime_error("Can't get type name from an empty object.");
        }

        std::string_view getTypeDescription() const { return mRef ? mRef->getTypeDescription() : "nullptr"; }

        const Class& getClass() const
        {
            if (mRef != nullptr)
                return *(mRef->mClass);
            throw std::runtime_error("Cannot get class of an empty object");
        }

        template <class T>
        auto* get() const
        {
<<<<<<< HEAD
            const MWWorld::LiveCellRef<T> *ref = dynamic_cast<const MWWorld::LiveCellRef<T>*>(mRef);
            if(ref) return ref;

            std::stringstream str;
            str<< "Bad LiveCellRef cast to "<<typeid(T).name()<<" from ";
            /*
                Start of tes3mp change (major)

                Print additional information
            */
            if(mRef != nullptr) str<< getTypeName() << " " << mRef->mRef.getRefId().c_str() << " " << mRef->mRef.getRefNum().mIndex << "-" << mRef->mRef.getMpNum();
            /*
                End of tes3mp change (major)
            */
            else str<< "an empty object";

            throw std::runtime_error(str.str());
=======
            return LiveCellRefBase::dynamicCast<T>(mRef);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        }

        LiveCellRefBaseType* getBase() const
        {
            if (!mRef)
                throw std::runtime_error("Can't access cell ref pointed to by null Ptr");
            return mRef;
        }

        TypeTransform<MWWorld::CellRef>& getCellRef() const
        {
            assert(mRef);
            return mRef->mRef;
        }

        TypeTransform<RefData>& getRefData() const
        {
            assert(mRef);
            return mRef->mData;
        }

        CellStoreType* getCell() const
        {
            assert(mCell);
            return mCell;
        }

        bool isInCell() const { return (mContainerStore == nullptr) && (mCell != nullptr); }

        void setContainerStore(ContainerStoreType* store)
        ///< Must not be called on references that are in a cell.
        {
            assert(store);
            assert(!mCell);
            mContainerStore = store;
        }

        ContainerStoreType* getContainerStore() const
        ///< May return a 0-pointer, if reference is not in a container.
        {
            return mContainerStore;
        }

        template <template <class> class TypeTransform2>
        bool operator==(const PtrBase<TypeTransform2>& other) const
        {
            return mRef == other.mRef;
        }

        template <template <class> class TypeTransform2>
        bool operator<(const PtrBase<TypeTransform2>& other) const
        {
            return mRef < other.mRef;
        }

    protected:
        PtrBase(LiveCellRefBaseType* liveCellRef, CellStoreType* cell, ContainerStoreType* containerStore)
            : mRef(liveCellRef)
            , mCell(cell)
            , mContainerStore(containerStore)
        {
        }
    };

    /// @note It is possible to get mutable values from const Ptr. So if a function accepts const Ptr&, the object is
    /// still mutable. To make it really const the argument should be const ConstPtr&.
    class Ptr : public PtrBase<std::remove_const_t>
    {
    public:
        Ptr(LiveCellRefBase* liveCellRef = nullptr, CellStoreType* cell = nullptr)
            : PtrBase(liveCellRef, cell, nullptr)
        {
        }

        std::string toString() const;
    };

    /// @note The difference between Ptr and ConstPtr is that the second one adds const to the underlying pointers.
    /// @note a Ptr can be implicitely converted to a ConstPtr, but you can not convert a ConstPtr to a Ptr.
    class ConstPtr : public PtrBase<std::add_const_t>
    {
    public:
        ConstPtr(const Ptr& ptr)
            : PtrBase(ptr.mRef, ptr.mCell, ptr.mContainerStore)
        {
        }
        ConstPtr(const LiveCellRefBase* liveCellRef = nullptr, const CellStoreType* cell = nullptr)
            : PtrBase(liveCellRef, cell, nullptr)
        {
        }
    };

    // SafePtr holds Ptr and automatically updates it via WorldModel if the Ptr becomes invalid.
    // Uses ESM::RefNum as an unique id. Can not be used for Ptrs without RefNum.
    // Note: WorldModel automatically assignes RefNum to all registered Ptrs.
    class SafePtr
    {
    public:
        using Id = ESM::RefNum;

        explicit SafePtr(Id id)
            : mId(id)
        {
        }
        explicit SafePtr(const Ptr& ptr);
        virtual ~SafePtr() = default;
        Id id() const { return mId; }

        std::string toString() const;

        const Ptr& ptrOrEmpty() const
        {
            update();
            return mPtr;
        }

    private:
        const Id mId;

        mutable Ptr mPtr;
        mutable size_t mLastUpdate = 0;

        void update() const;
    };
}

#endif
