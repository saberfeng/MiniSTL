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

        ~vector() {
            clear();
            value_allocator.deallocate(start, end_of_storage - start);
        }

        vector &operator=(const vector &other) {
            clear();
            for (const_iterator iter = other.cbegin(); iter != other.cend(); ++iter)
                push_back(*iter);
            return *this;
        }

        vector &operator=(std::initializer_list<value_type> ilist){
            clear();
            for(auto &item:ilist)
                push_back(item);
            return *this;
        }

        void assign(size_type count,const_reference value){
            clear();
            for(int i=0;i<count;++i)
                push_back(value);
        }

        template<class InputIterator>
        void assign(InputIterator first,InputIterator last){
            clear();
            while(first!=last)
                push_back(*first++);
        }

        void assign(std::initializer_list<value_type> ilist){
            clear();
            for(auto &item:ilist)
                push_back(item);
        }

        iterator begin() { return start; }

        const_iterator cbegin() const { return start; }

        iterator end() { return finish; }

        const_iterator cend() const { return finish; }

        void push_back(const_reference value);

        iterator insert(const_iterator pos, const_reference value);

        iterator insert(const_iterator pos, size_type count, const_reference value);

        template<class InputIterator>
        void insert(const_iterator pos, InputIterator first, InputIterator last);

        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist);

        void resize(size_type count);

        void resize(size_type count, const_reference value);

        void pop_back();

        void clear();

        iterator erase(const_iterator pos);

        iterator erase(const_iterator first, const_iterator last);

        size_type capacity() { return end_of_storage - start; }

        size_type size() { return finish - start; }

        bool empty() { return size() == 0; }

        reference front() { return *start; }

        const_reference front() const { return *start; }

        reference back() { return *finish; }

        const_reference back() const { return *finish; }

        reference operator[](size_type pos) { return *(start + pos); }

        const_reference operator[](size_type pos) const { return *(start + pos); }

        reference at(size_type pos) { return *(start + pos); }

        const_reference at(size_type pos) const { return *(start + pos); }

        void swap(vector& other);

    private:

        void insert_aux(iterator pos, const_reference value);

        allocator_type value_allocator;

        iterator start;
        iterator finish;
        iterator end_of_storage;
    };

    template<class Value, class Allocator>
    vector<Value, Allocator>::vector(size_type count, const_reference value):
            start(nullptr), finish(nullptr), end_of_storage(nullptr) {
        for (int i = 0; i < count; ++i)
            push_back(value);
    }

    template<class Value, class Allocator>
    template<class InputIterator>
    vector<Value, Allocator>::vector(InputIterator first, InputIterator last):
            start(nullptr), finish(nullptr), end_of_storage(nullptr) {
        while (first != last)
            push_back(*first++);
    }

    template<class Value, class Allocator>
    vector<Value, Allocator>::vector(const vector &other):
            start(nullptr), finish(nullptr), end_of_storage(nullptr) {
        for (iterator iter = other.cbegin(); iter != other.cend(); ++iter)
            push_back(*iter);
    }

    template<class Value, class Allocator>
    vector<Value, Allocator>::vector(std::initializer_list<value_type> ilist):
            start(nullptr), finish(nullptr), end_of_storage(nullptr) {
        for (auto &item:ilist)
            push_back(item);
    }


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
    typename vector<Value, Allocator>::iterator
    vector<Value, Allocator>::insert(const_iterator position, const_reference value) {
        iterator pos=start+(position-start);
        if (finish != end_of_storage) {
            construct(finish, *(finish - 1));
            ++finish;
            std::copy_backward(pos, finish - 2, finish - 1);
            construct(pos, value);
        } else {
            insert_aux(pos, value);
        }
        return pos;
    }

    template<class Value, class Allocator>
    typename vector<Value,Allocator>::iterator
    vector<Value, Allocator>::insert(const_iterator position, size_type count, const_reference value) {
        iterator pos=start+(position-start);
        size_type old_capacity = end_of_storage - start;
        size_type old_size = finish - start;
        size_type size_after_pos = finish - pos;
        size_type size_before_pos = pos - start;
        if (old_size + count <= old_capacity) {
            if (count > finish - pos) {
                std::uninitialized_fill_n(finish, count - size_after_pos, value);
                iterator old_finish = finish;
                finish += count - size_after_pos;
                std::uninitialized_copy(pos, old_finish, finish);
                finish += size_after_pos;
                std::fill(pos, old_finish, value);
            } else {
                std::uninitialized_copy(finish - count, finish, finish);
                std::copy_backward(pos, finish - count, finish);
                std::fill(pos, pos + count, value);
                finish += count;
            }
        } else {
            size_type new_len = std::max(2 * old_capacity, old_size + count);
            iterator old_start = start;
            start = value_allocator.allocate(new_len);
            std::uninitialized_copy(old_start, pos, start);
            std::uninitialized_fill_n(start + size_before_pos, count, value);
            std::uninitialized_copy(pos, finish, start + size_before_pos + count);
            destroy(old_start, finish);
            value_allocator.deallocate(old_start, old_size);
            finish = start + old_size + count;
            end_of_storage = start + new_len;
        }
    }

    template<class Value, class Allocator>
    template<class InputIterator>
    void vector<Value, Allocator>::insert(const_iterator position, InputIterator first, InputIterator last) {
        iterator pos=start+(position-start);
        while (first != last){
            pos = insert(pos, *first++);
            ++pos;
        }
    }

    template<class Value, class Allocator>
    typename vector<Value, Allocator>::iterator
    vector<Value, Allocator>::insert(const_iterator position, std::initializer_list<value_type> ilist) {
        iterator pos = start+(position-start);
        for (auto &item:ilist){
            pos = insert(pos, item);
            ++pos;
        }
        return pos;
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::insert_aux(iterator pos, const_reference value) {
        if (finish != end_of_storage) {
            construct(finish, *(finish - 1));
            ++finish;
            std::copy_backward(pos, finish - 2, finish - 1);
            construct(pos, value);
        } else {
            size_type old_capacity = end_of_storage - start;
            size_type new_size = old_capacity ? 2 * old_capacity : 1;
            iterator old_start = start;
            start = value_allocator.allocate(new_size);
            std::uninitialized_copy(old_start, pos, start);
            iterator new_pos = start + (pos - old_start);
            construct(new_pos, value);
            std::uninitialized_copy(pos, finish, new_pos + 1);
            finish = start + old_capacity + 1;
            end_of_storage = start + new_size;
            if (old_capacity) {
                value_allocator.deallocate(old_start, old_capacity);
            }
        }
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::resize(size_type count) {
        resize(count, value_type());
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::resize(size_type count, const_reference value) {
        size_type old_size = finish - start;
        if (count < old_size) {
            destroy(start + count, finish);
            finish = start + count;
        } else if (count > old_size) {
            size_type old_capacity = end_of_storage - start;
            if (count > old_capacity) {
                size_type new_capacity = std::max(2 * old_capacity, count);
                iterator old_start = start;
                start = value_allocator.allocate(new_capacity);
                std::uninitialized_copy(old_start, finish, start);
                destroy(old_start, finish);
                value_allocator.deallocate(old_start, old_capacity);
                end_of_storage = start + new_capacity;
            }
            std::uninitialized_fill_n(start + old_size, count - old_size, value_type());
            finish = start + count;
        }
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::pop_back() {
        --finish;
        destroy(finish);
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::clear() {
        destroy(start, finish);
        finish = start;
    }

    template<class Value, class Allocator>
    typename vector<Value, Allocator>::iterator
    vector<Value, Allocator>::erase(const_iterator position) {
        iterator pos=start+(position-start);
        if (start == finish)
            return finish;
        std::copy(pos + 1, finish, pos);
        pop_back();
        return pos;
    }

    template<class Value, class Allocator>
    typename vector<Value, Allocator>::iterator
    vector<Value, Allocator>::erase(const_iterator _first, const_iterator _last) {
        iterator first=start+(_first-start);
        iterator last=start+(_last-start);
        std::copy(last, finish, first);
        iterator new_finish = first + (finish - last);
        destroy(new_finish, finish);
        finish = new_finish;
        return first;
    }

    template<class Value, class Allocator>
    void vector<Value, Allocator>::swap(vector& other) {
        std::swap(start,other.start);
        std::swap(finish,other.finish);
        std::swap(end_of_storage,other.end_of_storage);
    }

}


#endif //MINISTL_VECTOR_H
