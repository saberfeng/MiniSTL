#ifndef MINISTL_CONSTRUCT_H
#define MINISTL_CONSTRUCT_H

#include <new>

template<typename T1, typename T2>
inline void construct(T1 *pointer, const T2 &value) {
    new(pointer) T2(value);
}

template<typename T>
inline void destroy(T *pointer){
    pointer->~T();
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first,ForwardIterator last){
    for(;first!=last;++first)
        destroy(&*first);
}

inline void destroy(char*,char*){}
inline void destroy(int*,int*){}
inline void destroy(long*,long*){}
inline void destroy(float*,float*){}
inline void destroy(double*,double*){}


#endif //MINISTL_CONSTRUCT_H
