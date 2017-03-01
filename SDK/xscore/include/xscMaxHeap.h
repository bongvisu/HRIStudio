#pragma once

#include <xscModule.h>

namespace xsc {

template <typename KeyType, typename ValueType>
class MaxHeap
{
public:
    struct Record
    {
        KeyType key;
        ValueType value;
        int index;
    };

    MaxHeap(MaxHeap const& maxHeap);
    MaxHeap(int maxElements = 0);
    MaxHeap& operator=(MaxHeap const& maxHeap);

    void Reset(int maxElements);
    inline int GetNumElements() const;
    inline bool GetMaximum(KeyType& key, ValueType& value) const;
    Record* Insert(KeyType const& key, ValueType const& value);
    bool Remove(KeyType& key, ValueType& value);
    void Update(Record* record, ValueType const& value);
    bool IsValid() const;

private:
    int mNumElements;
    std::vector<Record> mRecords;
    std::vector<Record*> mPointers;
};

template <typename KeyType, typename ValueType>
MaxHeap<KeyType, ValueType>::MaxHeap(int maxElements)
{
    Reset(maxElements);
}

template <typename KeyType, typename ValueType>
MaxHeap<KeyType, ValueType>::MaxHeap(MaxHeap const& maxHeap)
{
    *this = maxHeap;
}

template <typename KeyType, typename ValueType>
MaxHeap<KeyType, ValueType>& MaxHeap<KeyType, ValueType>::operator=(MaxHeap const& maxHeap)
{
    mNumElements = maxHeap.mNumElements;
    mRecords = maxHeap.mRecords;
    mPointers.resize(maxHeap.mPointers.size());
    for (auto& record : mRecords)
    {
        mPointers[record.index] = &record;
    }
    return *this;
}

template <typename KeyType, typename ValueType>
void MaxHeap<KeyType, ValueType>::Reset(int maxElements)
{
    mNumElements = 0;
    if (maxElements > 0)
    {
        mRecords.resize(maxElements);
        mPointers.resize(maxElements);
        for (int i = 0; i < maxElements; ++i)
        {
            mPointers[i] = &mRecords[i];
            mPointers[i]->index = i;
        }
    }
    else
    {
        mRecords.clear();
        mPointers.clear();
    }
}

template <typename KeyType, typename ValueType>
inline int MaxHeap<KeyType, ValueType>::GetNumElements() const
{
    return mNumElements;
}

template <typename KeyType, typename ValueType>
inline bool MaxHeap<KeyType, ValueType>::GetMaximum(KeyType& key, ValueType& value) const
{
    if (mNumElements > 0)
    {
        key = mPointers[0]->key;
        value = mPointers[0]->value;
        return true;
    }
    else
    {
        return false;
    }
}

template <typename KeyType, typename ValueType>
typename MaxHeap<KeyType, ValueType>::Record* MaxHeap<KeyType, ValueType>::Insert(
	KeyType const& key,
	ValueType const& value)
{
    // Return immediately when the heap is full.
    if (mNumElements == static_cast<int>(mRecords.size()))
    {
        return nullptr;
    }

    int child = mNumElements++;
    Record* record = mPointers[child];
    record->key = key;
    record->value = value;

	while (child > 0)
    {
        int parent = (child - 1) / 2;
        if (mPointers[parent]->value >= value)
        {
            break;
        }

        // Move the parent into the child's slot.
        mPointers[child] = mPointers[parent];
        mPointers[child]->index = child;

        // Move the child into the parent's slot.
        mPointers[parent] = record;
        mPointers[parent]->index = parent;

        child = parent;
    }

    return mPointers[child];
}

template <typename KeyType, typename ValueType>
bool MaxHeap<KeyType, ValueType>::Remove(KeyType& key, ValueType& value)
{
    if (mNumElements == 0)
    {
        return false;
    }

    Record* root = mPointers[0];
    key = root->key;
    value = root->value;

    int last = --mNumElements;
    Record* record = mPointers[last];
    int parent = 0, child = 1;
    while (child <= last)
    {
        if (child < last)
        {
            int childP1 = child + 1;
            if (mPointers[childP1]->value > mPointers[child]->value)
            {
                child = childP1;
            }
        }

        if (record->value >= mPointers[child]->value)
        {
            break;
        }

        mPointers[parent] = mPointers[child];
        mPointers[parent]->index = parent;

        parent = child;
        child = 2 * child + 1;
    }

    mPointers[parent] = record;
    mPointers[parent]->index = parent;

    mPointers[last] = root;
    mPointers[last]->index = last;
    return true;
}

template <typename KeyType, typename ValueType>
void MaxHeap<KeyType, ValueType>::Update(Record* record, ValueType const& value)
{
    // Return immediately on invalid record.
    if (!record)
    {
        return;
    }

    int parent, child, childP1, maxChild;

    if (record->value > value)
    {
        record->value = value;

        parent = record->index;
        child = 2 * parent + 1;
        while (child < mNumElements)
        {
            childP1 = child + 1;
            if (childP1 < mNumElements)
            {
                // Two children exist.
                if (mPointers[child]->value >= mPointers[childP1]->value)
                {
                    maxChild = child;
                }
                else
                {
                    maxChild = childP1;
                }
            }
            else
            {
                // One child exists.
                maxChild = child;
            }

            if (value >= mPointers[maxChild]->value)
            {
                break;
            }

            mPointers[parent] = mPointers[maxChild];
            mPointers[parent]->index = parent;

            mPointers[maxChild] = record;
            mPointers[maxChild]->index = maxChild;

            parent = maxChild;
            child = 2 * parent + 1;
        }
    }
    else if (value > record->value)
    {
        record->value = value;

        child = record->index;
        while (child > 0)
        {
            // A parent exists.
            parent = (child - 1) / 2;

            if (mPointers[parent]->value >= value)
            {
                break;
            }

            mPointers[child] = mPointers[parent];
            mPointers[child]->index = child;

            mPointers[parent] = record;
            mPointers[parent]->index = parent;

            child = parent;
        }
    }
}

template <typename KeyType, typename ValueType>
bool MaxHeap<KeyType, ValueType>::IsValid() const
{
    for (int child = 0; child < mNumElements; ++child)
    {
        int parent = (child - 1) / 2;
        if (parent > 0)
        {
            if (mPointers[child]->value > mPointers[parent]->value)
            {
                return false;
            }

            if (mPointers[parent]->index != parent)
            {
                return false;
            }
        }
    }

    return true;
}

} // namespace xsc
