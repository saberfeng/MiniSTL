//
// Created by Sabre Sever on 2017/7/24.
//

#ifndef MINISTL_UTIL_H
#define MINISTL_UTIL_H

#include "iterator.h"

namespace mini {


    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    _distance(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type count = 0;
        while (first != last) {
            ++first;
            ++count;
        }
        return count;
    }

    template<class RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    _distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
        return last - first;
    }

    template<class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        return _distance(first, last, typename iterator_traits<InputIterator>::iterator_category());
    }


}


#endif //MINISTL_UTIL_H
