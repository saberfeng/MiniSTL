//
// Created by Sabre Sever on 2017/7/27.
//

#ifndef MINISTL_DEQUE_H
#define MINISTL_DEQUE_H

#include "allocator.h"

namespace mini {

    std::size_t deque_buffer_size(std::size_t n, std::size_t size) {
        return n != 0 ? n : (size < 512 ? std::size_t(512 / size) : std::size_t(1));
    }

    template<class Value, class Reference, class Pointer>
    class deque_iterator : public iterator_base<random_access_iterator_tag, Value, ptrdiff_t, Pointer, Reference> {
    public:

        typedef Value value_type;
        typedef std::size_t size_type;
        typedef deque_iterator<Value,Value& ,Value*> iterator;
        typedef deque_iterator<Value, Reference, Pointer> self;
        typedef pointer *map_pointer;

        template<class Value> friend
        class deque;

        deque_iterator() : start(nullptr), end(nullptr), cur(nullptr), node(nullptr) {}

        deque_iterator(pointer start_ptr,
                       pointer end_ptr,
                       pointer cur_ptr,
                       map_pointer node_ptr) :
                start(start_ptr), end(end_ptr), cur(cur_ptr), node(node_ptr) {}

        //allows conversion from iterator to const_iterator
        deque_iterator(const iterator& other):
                start(other.start),end(other.end),cur(other.end),node(other.node){}

        reference operator*() { return *cur; }

        pointer operator->() { return cur; }

        void set_node(pointer *new_node) {
            node = new_node;
            start = *node;
            end = start + deque_buffer_size(0, sizeof(value_type));
        }

        self &operator++() {
            ++cur;
            if (cur == end) {
                set_node(node + 1);
                cur = start;
            }
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self &operator--() {
            if (cur == start) {
                set_node(node - 1);
                cur = end;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        self &operator+=(difference_type n) {
            size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
            difference_type node_steps = 0;
            difference_type node_offset = 0;
            if (n > 0) {
                node_steps = (n + (cur - start)) / buffer_size;
                node_offset = (n + (cur - start)) % buffer_size;

            } else if (n < 0) {
                node_steps = (n - (end - cur) + 1) / buffer_size;
                node_offset = buffer_size + (n - (end - cur)) % buffer_size;
            }
            set_node(node + node_steps);
            cur = start + node_offset;
            return *this;
        }

        self &operator-=(difference_type n) {
            return *this += -n;
        }

        self operator+(difference_type n) {
            self tmp = *this;
            return tmp += n;
        }

        self operator-(difference_type n) {
            self tmp = *this;
            return tmp -= n;
        }

    private:
        pointer start;
        pointer end;
        pointer cur;
        map_pointer node;

    };

    template<class Value, class Allocator = allocator <Value>>
    class deque {

    public:
        typedef Value value_type;
        typedef Allocator allocator_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef deque_iterator<Value, Value &, Value *> iterator;
        typedef deque_iterator<Value, const Value &, const Value *> const_iterator;

        explicit deque() {init();}

        explicit deque(size_type count, const_reference value = value_type());

        template<class InputIterator>
        deque(InputIterator first, InputIterator last);

        deque(const deque &other);

        deque(std::initializer_list<value_type> init);

        reference       front() {return *start;}
        const_reference front() const {return *start;}
        reference       back() {return *end;}
        const_reference back() const {return *end;}

        

    private:

        typedef pointer *map_pointer;

        map_pointer map;
        size_type map_size;

        iterator start;
        iterator end;

        Allocator value_allocator;
        allocator <pointer> map_allocator;


        void init() {
            map = map_allocator.allocate(8);
            map_size = 8;
            map_pointer node = map + map_size / 2;
            size_type buffer_size = deque_buffer_size(0, sizeof(value_type));
            *node = value_allocator.allocate(buffer_size);

            start = iterator(*node, *node + buffer_size, *node + buffer_size / 2, node);
            end = iterator(*node, *node + buffer_size, *node + buffer_size / 2, node);
        }


    };


}

#endif //MINISTL_DEQUE_H
