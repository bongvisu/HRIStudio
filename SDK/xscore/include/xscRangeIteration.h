#pragma once

#include <xscModule.h>

namespace xsc {

template <typename Iterator>
class ReversalObject
{
public:
    ReversalObject(Iterator begin, Iterator end)
        :
        mBegin(begin),
        mEnd(end)
    {
    }

    Iterator begin() const { return mBegin; }
    Iterator end() const { return mEnd; }

private:
    Iterator mBegin, mEnd;
};

template
<
    typename Iterable,
    typename Iterator = decltype(std::begin(std::declval<Iterable>())),
    typename ReverseIterator = std::reverse_iterator<Iterator>
>
ReversalObject<ReverseIterator> reverse(Iterable&& range)
{
    return ReversalObject<ReverseIterator>(ReverseIterator(std::end(range)), ReverseIterator(std::begin(range)));
}

} // namespace xsc
