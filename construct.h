#ifndef MINISTL_CONSTRUCT_H
#define MINISTL_CONSTRUCT_H

#include <new>

template<typename T1, typename T2>
void construct(T1 *pointer, const T2 &value) {
    new(pointer) T2(value);
}

template<typename T>
void destroy(T *pointer){
    ~T();
}



#endif //MINISTL_CONSTRUCT_H
