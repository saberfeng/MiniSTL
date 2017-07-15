
#ifndef MINISTL_ALLOCATOR_H
#define MINISTL_ALLOCATOR_H

#include <cstddef>
#include <new>

namespace mini{

    template <typename T>
    struct allocator{

        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        //pointer allocate(size_type n ,mini::allocator<void>::const_pointer hint=0);

        T* allocate(size_type num){
            return static_cast<T*>(::operator new(num*sizeof(T)));
        }
        //TODO: use n?
        void deallocate(pointer ptr,size_type num){
            ::operator delete(static_cast<void*>(ptr));
        }
        void construct(pointer ptr, const_reference value){
            new (static_cast<void*>(ptr)) T (value);
        }
        void destroy(pointer ptr){
            ptr->~T();
        }
    };

    //TODO: throw()?
    template <typename T ,typename U>
    bool operator==(const allocator<T>& ,const allocator<U>&){
        return true;
    }

    //TODO: throw()?
    template <typename T,typename U>
    bool operator!=(const allocator<T>& ,const allocator<U>&){
        return false;
    }


}
#endif //MINISTL_ALLOCATOR_H
