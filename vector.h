//
// Created by Sabre Sever on 2017/7/29.
//

#ifndef MINISTL_VECTOR_H
#define MINISTL_VECTOR_H

#include <initializer_list>
#include "allocator.h"
#include "construct.h"

namespace mini{

    template<class Value,class Allocator=allocator<Value>>
    class vector{

    public:
        typedef Value value_type;
        typedef Allocator allocator_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;

        vector():start(nullptr),finish(nullptr),end_of_storage(nullptr){}
        explicit vector(size_type count,const_reference value=value_type());
        template<class InputIterator>
        vector(InputIterator first,InputIterator last);
        vector(const vector& other);
        vector(std::initializer_list<value_type> ilist);

        iterator begin(){return start;}
        const_iterator cbegin() const {return start;}
        iterator end(){return finish;}
        const_iterator cend() const {return finish;}

        void push_back(const_reference value);



    private:

        void insert_aux(iterator pos,const_reference value);


        iterator start;
        iterator finish;
        iterator end_of_storage;
    };


    template<class Value,class Allocator>
    void vector<Value,Allocator>::push_back(const_reference value){
        if(finish!=end_of_storage){
            construct(finish,value);
            ++finish;
        }else{
            insert_aux(finish,value);
        }
    }


}



#endif //MINISTL_VECTOR_H
