//
//  trim.hpp
//  JubSDK
//
//  Created by panmin on 2019/9/26.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef trim_h
#define trim_h

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <functional>

namespace jub {
namespace algorithm {

namespace detail {
//    template< typename ForwardIteratorT, typename PredicateT >
//    inline ForwardIteratorT trim_end_iter_select(
//                                                 ForwardIteratorT InBegin,
//                                                 ForwardIteratorT InEnd,
//                                                 PredicateT IsSpace,
//                                                 std::forward_iterator_tag )
//    {
//        ForwardIteratorT TrimIt=InBegin;
//
//        for( ForwardIteratorT It=InBegin; It!=InEnd; ++It )
//        {
//            if ( !IsSpace(*It) )
//            {
//                TrimIt=It;
//                ++TrimIt;
//            }
//        }
//
//        return TrimIt;
//    }
//
//    template< typename ForwardIteratorT, typename PredicateT >
//    inline ForwardIteratorT trim_end_iter_select(
//                                                 ForwardIteratorT InBegin,
//                                                 ForwardIteratorT InEnd,
//                                                 PredicateT IsSpace,
//                                                 std::bidirectional_iterator_tag )
//    {
//        for( ForwardIteratorT It=InEnd; It!=InBegin;  )
//        {
//            if ( !IsSpace(*(--It)) )
//                return ++It;
//        }
//
//        return InBegin;
//    }

template< typename ForwardIteratorT, typename PredicateT >
inline ForwardIteratorT eTrim_iter_select(ForwardIteratorT InBegin,
                                          ForwardIteratorT InEnd,
                                          PredicateT IsSpace,
                                          std::forward_iterator_tag ) {
    ForwardIteratorT TrimIt=InBegin;
    
    for( ForwardIteratorT It=InBegin; It!=InEnd; ++It )
    {
        if ( !IsSpace(*It) )
        {
            TrimIt=It;
            ++TrimIt;
        }
    }
    
    return TrimIt;
}

//    // Search for first non matching character from the beginning of the sequence
//    template< typename ForwardIteratorT, typename PredicateT >
//    inline ForwardIteratorT trim_begin(
//                                       ForwardIteratorT InBegin,
//                                       ForwardIteratorT InEnd,
//                                       PredicateT IsSpace )
//    {
//        ForwardIteratorT It=InBegin;
//        for(; It!=InEnd; ++It )
//        {
//            if (!IsSpace(*It))
//                return It;
//        }
//
//        return It;
//    }
template< typename ForwardIteratorT, typename PredicateT >
inline ForwardIteratorT eTrim(ForwardIteratorT InBegin,
                              ForwardIteratorT InEnd,
                              PredicateT IsSpace ) {
    typename std::iterator_traits<ForwardIteratorT>::iterator_category category;
    return eTrim_iter_select(InBegin, InEnd, IsSpace, category);
}

} // namespace detail end

template<typename SequenceT>
inline SequenceT& lTrim(SequenceT &Input) {
    auto p = find_if(Input.begin(), Input.end(), std::not1(std::function<bool(char)>(isspace)));
    Input.erase(Input.begin(), p);
    return Input;
}

//template<typename SequenceT, typename PredicateT>
//inline void trim_right_if(SequenceT& Input, PredicateT IsSpace)
//{
//    Input.erase(
//                ::boost::algorithm::detail::trim_end(
//                                                     ::boost::begin(Input),
//                                                     ::boost::end(Input),
//                                                     IsSpace ),
//                ::boost::end(Input)
//                );
//}
template<typename SequenceT, typename PredicateT>
inline void rTrim_if(SequenceT& Input, PredicateT IsSpace) {
    Input.erase(detail::eTrim(Input.begin(),
                              Input.end(),
                              IsSpace),
                Input.end());
}

//boost::algorithm::trim_right
//template<typename SequenceT>
//inline void trim_right(SequenceT& Input, const std::locale& Loc=std::locale())
//{
//    ::boost::algorithm::trim_right_if(
//                                      Input,
//                                      is_space(Loc) );
//}
template<typename SequenceT>
inline SequenceT& rTrim(SequenceT &Input, const std::locale& Loc=std::locale()) {

    auto p = find_if(Input.rbegin(), Input.rend(), std::not1(std::function<bool(char)>(isspace)));
    Input.erase(p.base(), Input.end());
    return Input;
}

template<typename SequenceT>
inline SequenceT& trim(SequenceT &st) {
    lTrim(rTrim(st));
    return st;
}

} // namespace algorithm end
} // namespace jub end
#endif /* trim_h */
