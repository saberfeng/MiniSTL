//
// Created by Sabre Sever on 2017/7/29.
//

#ifndef MINISTL_VECTOR_H
#define MINISTL_VECTOR_H

#include <initializer_list>
#include <algorithm>
#include <memory>
#include "allocator.h"
#include "construct.h"

namespace mini {

    template<class Value, class Allocator=allocator <Value>>
    class vector {

    public:
        typedef Value value_type;
        typedef Allocator allocator_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type *iterator;
        typedef const value_type *const_iterator;

        vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}

        explicit vector(size_type count, const_reference value = value_type());

        template<class InputIterator>
        vector(InputIterator first, InputIterator last);

        vector(const vector &other);

        vector(std::initializer_list<value_type> ilist);

        iterator begin() { return start; }

        const_iterator cbegin() const { return start; }

        iterator end() { return finish; }

        const_iterator cend() const { return finish; }

        void push_back(const_reference value);

        iterator insert(iterator pos, const_reference value);

        iterator insert(const_iterator pos, const_reference value);

        void insert(iterator pos, size_type count, const_reference value);

        iterator insert(const_iterator pos, size_type count, const_reference value);

        template<class InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last);

        template<class InputIterator>
        void insert(const_iterator pos, InputIterator first, InputIterator last);

        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);


    private:

        void insert_aux(iterator pos, const_reference value);

        allocator_type value_allocator;

        iterator start;
        iterator finish;
        iterator end_of_storage;
    };


    template<class Value, class Allocator>
    void vector<Value, Allocator>::push_back(const_reference value) {
        if (finish != end_of_storage) {
            construct(finish, value);
            ++finish;
        } else {
            insert_aux(finish, value);
        }
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::insert(iterator pos, size_type count, const_reference value) {
        size_type current_capacity = end_of_storage - start;
        size_type current_size = finish - start;
        if (current_size + count <= current_capacity){

        }
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::insert_aux(iterator pos, const_reference value) {
        if (finish != end_of_storage) {
            construct(finish, *(finish - 1));
            ++finish;
            std::copy_backward(pos, finish - 2, finish - 1);
            construct(pos, value);
        } else {
            size_type old_size = end_of_storage - start;
            size_type new_size = old_size ? 2 * old_size : 1;
            iterator old_start = start;
            start = value_allocator.allocate(new_size);
            std::uninitialized_copy(old_start, pos, start);
            iterator new_pos = start + (pos - old_start);
            construct(new_pos, value);
            std::uninitialized_copy(pos, finish, new_pos + 1);
            finish = start + old_size + 1;
            end_of_storage = start + new_size;
            if (old_size) {
                value_allocator.deallocate(old_start, old_size);
            }
        }
    }


}


#endif //MINISTL_VECTOR_H
