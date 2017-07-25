//
// Created by Sabre Sever on 2017/7/24.
//

#ifndef MINISTL_UTIL_H
#define MINISTL_UTIL_H

#include "iterator.h"

namespace mini {

    template<class T1, class T2>
    struct pair {
        typedef T1 first_type;
        typedef T2 second_type;
        first_type first;
        second_type second;

        pair() : first(first_type()), second(second_type()) {}

        pair(const first_type &a, const second_type &b) : first(a), second(b) {}

        template<class U1, class U2>
        pair(const pair<U1, U2> &p):first(p.first), second(p.second) {}
    };

    template<class T1, class T2>
    bool operator==(const pair<T1, T2> &p1, const pair<T1, T2> &p2) {
        return (p1.first == p2.first && p1.second == p2.second);
    }

    template<class T1, class T2>
    bool operator<(const pair<T1, T2> &p1, const pair<T1, T2> &p2) {
        return p1.first < p2.first || (!(p2.first < p1.first) && p1.second < p2.second);
    }

    template<class T1, class T2>
    bool operator!=(const pair<T1, T2> &p1, const pair<T1, T2> &p2) {
        return !(p1 == p2);
    };

    template<class T1, class T2>
    bool operator<=(const pair<T1, T2> &p1, const pair<T1, T2> &p2) {
        return !(p2 < p1);
    }

    template<class T1, class T2>
    bool operator>(const pair<T1, T2> &p1, const pair<T1, T2> &p2) {
        return p2 < p1;
    }

    template<class T1, class T2>
    bool operator>=(const pair<T1, T2> &p1, const pair<T1, T2> &p2) {
        return !(p1 < p2);
    }

    template <class T1,class T2>
    pair<T1,T2> make_pair(T1 first,T2 last){
        return pair<T1,T2>(first,last);
    };

}


#endif //MINISTL_UTIL_H
