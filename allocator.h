
#ifndef MINISTL_ALLOCATOR_H
#define MINISTL_ALLOCATOR_H

#include <cstddef>
#include <new>


namespace mini{

    template <typename T>
    class allocator{

    public:

        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        //pointer allocate(size_type n ,mini::allocator<void>::const_pointer hint=0);

        T* _allocate(size_type num){
            return static_cast<T*>(::operator new(num*sizeof(T)));
        }
        //TODO: use num?
        void _deallocate(pointer ptr){
            ::operator delete(static_cast<void*>(ptr));
        }
        void construct(pointer ptr, const_reference value){
            new (static_cast<void*>(ptr)) T (value);
        }
        void destroy(pointer ptr){
            ptr->~T();
        }

        T* allocate(size_type num=1){
            size_type request_bytes=num*sizeof(T);
            if(request_bytes>128)
                return _allocate(num);

            obj **free_list_item=free_list+get_list_index(request_bytes);
            obj *result=*free_list_item;

            if(result==nullptr){
                result=refill_lists(round_up(request_bytes));
            }else{
                //*free_list_item=static_cast<char*>(*result);
                *free_list_item=result->next_block;
            }
            return (T*)result;

        }


        void deallocate(pointer ptr,size_type num){
            if(ptr==nullptr)
                return;
            size_type request_bytes=num*sizeof(T);
            if(request_bytes>128)
                return _deallocate(ptr);

            obj **free_list_item=free_list+get_list_index(request_bytes);
            ((obj*)ptr)->next_block=*free_list_item;
            *free_list_item=(obj*)ptr;
        }

    private:

        union obj{
            union obj *next_block;
        };

        static obj* free_list[16];
        static char* start_free;
        static char* end_free;



        size_type round_up(size_type bytes){
            return (bytes+7)&(~7);
        }

        size_type get_list_index(size_type bytes){
            return (bytes-1)/8;
        }

        obj* refill_lists(size_type block_size){
            size_type num_blocks=20;
            obj *chunk=(obj*)chunk_alloc(block_size,num_blocks);

            //TODO:handle out of memory
            if(num_blocks==0)
                return nullptr;
            else{
                obj *current=chunk;
                obj *next=(obj*)((char*)current+block_size);

                for(size_type i=0;i<num_blocks-1;++i){
                    current->next_block=next;
                    current=next;
                    next=(obj*)((char*)next+block_size);
                }
                current->next_block=nullptr;

                obj **free_list_item=free_list;
                free_list_item+=get_list_index(block_size);
                *free_list_item=chunk->next_block;
                return chunk;
            }
        }

        char* chunk_alloc(size_type block_size,size_type &num_blocks){

            size_type request_bytes=block_size*num_blocks;
            size_type bytes_left=end_free-start_free;
            char *result=nullptr;

            if(bytes_left>request_bytes){
                result=start_free;
                start_free+=request_bytes;
                return result;
            }else if (bytes_left==request_bytes){
                result=start_free;
                start_free= (char*)(operator new(2*request_bytes));
                end_free=start_free+2*request_bytes;
                return result;
            }else if(bytes_left>=block_size){
                num_blocks=bytes_left/block_size;
                result=start_free;
                start_free+=num_blocks*block_size;
                return result;
            }else{
                if(bytes_left>=8){
                    obj **free_list_item=free_list;
                    size_type left_fill_index=get_list_index(bytes_left)-1;
                    free_list_item += left_fill_index;
                    size_type left_fill_block_size=(left_fill_index+1)*8;
                    ((obj*)start_free)->next_block=*free_list_item;
                    *free_list_item=(obj*)start_free;
                    start_free+=left_fill_block_size;
                }


                start_free=(char*)(operator new(2*request_bytes));
                end_free=start_free+2*request_bytes;
                result=chunk_alloc(block_size,num_blocks);
            }
            return result;
        }

    };

    template <typename T> char* allocator<T>::start_free=nullptr;
    template <typename T> char* allocator<T>::end_free=nullptr;
    template <typename T> typename allocator<T>::obj* allocator<T>::free_list[16]={
            nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
            nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
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
