//
// Created by Sabre Sever on 2017/7/17.
//

#ifndef MINISTL_RB_TREE_H
#define MINISTL_RB_TREE_H


#include <iostream>
#include "allocator.h"

namespace mini{

    typedef bool rb_tree_color_type;
    const rb_tree_color_type  rb_tree_red = false;
    const rb_tree_color_type rb_tree_black=true;

    template <typename ValueType>
    class rb_tree_node{
    public:
        rb_tree_color_type color;
        rb_tree_node *parent;
        rb_tree_node *left;
        rb_tree_node *right;
        ValueType value;
    };


    template <typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc=allocator>
    class rb_tree{

    public:
        typedef rb_tree_node* node_ptr;
        typedef const rb_tree_node* const_node_ptr;
        typedef std::size_t size_type;
        typedef Value& reference;
        typedef const Value& const_reference;

    private:

        allocator<rb_tree_node> rb_tree_node_allocator;

        node_ptr get_node(){
            return rb_tree_node_allocator.allocate();
        }
        void put_node(node_ptr p){
            rb_tree_node_allocator.deallocate(p,1);
        }
        //TODO: all construction may give rise to exception
        node_ptr create_node(const_reference val){
            node_ptr ptr=get_node();
            rb_tree_node_allocator.construct(&(ptr->value),val);
            return ptr;
        }
        node_ptr clone_node(node_ptr node){
            node_ptr clone = create_node(node->value);
            clone->color=node->color;
            clone->left=nullptr;
            clone->right=nullptr;
            clone->parent=nullptr;
        }
        void destroy_node(node_ptr node){
            rb_tree_node_allocator.destroy(node);
            put_node(node);
        }

    private:

        size_type node_count;
        node_ptr nil;
        //node_ptr root;

        Compare key_compare;

        node_ptr& root() const {return nil->parent;}
        node_ptr& left_most() const {return nil->left;}
        node_ptr& right_most() const {return nil->right;}

        static node_ptr& left(node_ptr node){return node->left;}
        static node_ptr& right(node_ptr node){return node->right;}
        static node_ptr& parent(node_ptr node){return node->parent;}
        static reference value(node_ptr node){return node->value;}
        static rb_tree_color_type& color(node_ptr node){return node->color;}

        void init(){
            nil=get_node();
            color(nil)=rb_tree_black;
            parent(nil)=nil;
            left(nil)=nil;
            right(nil)=nil;
        }
        void clear(){

        }

    public:
        rb_tree(const Compare& comp=Compare()):node_count(0),key_compare(comp){ init(); }

        ~rb_tree(){
            clear();
            put_node(nil);
        }

        void rb_left_rotate(node_ptr x);

        void rb_right_rotate(node_ptr x);

        void rb_insert_fixup(node_ptr node);

        void insert_unique(const_reference v);

        void DebugMidorderTraverseCore(node_ptr node){
            if(node==nil)
                return;
            DebugMidorderTraverseCore(node->left);
            std::cout<<node->value<<" ";
            DebugMidorderTraverseCore(node->right);
        }
        void DebugMidorderTraverse(){
            DebugMidorderTraverseCore(root());
            std::cout<<std::endl;
        }

        void DebugPreorderTraverseCore(node_ptr node){
            if(node==nil)
                return;
            std::cout<<node->value<<"  ";
            DebugPreorderTraverseCore(node->left);
            DebugPreorderTraverseCore(node->right);
        }
        void DebugPreorderTraverse(){
            DebugPreorderTraverseCore(root());
            std::cout<<std::endl;
        }



    };

    template <typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc=allocator>
    void rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::
    rb_left_rotate(node_ptr x){
        node_ptr y = x->right;
        x->right=y->left;
        if(x->right!=nil)
            x->right->parent=x;
        y->parent=x->parent;
        if(y->parent==nil)
            nil->parent=y;
        else if(x==x->parent->left)
            x->parent->left=y;
        else
            x->parent->right=y;
        y->left=x;
        x->parent=y;
    }

    template <typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc=allocator>
    void rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::
    rb_right_rotate(node_ptr x){
        node_ptr y=x->left;
        x->left=y->right;
        if(x->left!=nil)
            x->left->parent=x;
        y->parent=x->parent;
        if(x->parent==nil)
            nil->parent=y;
        else if(x->parent->left==x)
            x->parent->left=y;
        else
            x->parent->right=y;
        y->right=x;
        x->parent=y;
    }

    template <typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc=allocator>
    void rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::
    rb_insert_fixup(node_ptr node){
        while(node->parent->color==rb_tree_red){
            if(node->parent==node->parent->parent->left){
                node_ptr uncle = node->parent->parent->right;
                if(uncle->color==rb_tree_red){
                    node->parent->color=rb_tree_black;
                    uncle->color=rb_tree_black;
                    node->parent->parent->color=rb_tree_red;
                    node=node->parent->parent;
                }else{
                    if(node==node->parent->right){
                        node=node->parent;
                        rb_left_rotate(node);
                    }
                    node->parent->color=rb_tree_black;
                    node->parent->parent->color=rb_tree_red;
                    rb_right_rotate(node->parent->parent);
                }
            }else{
                node_ptr uncle = node->parent->parent->left;
                if(uncle->color==rb_tree_red){
                    node->parent->color=rb_tree_black;
                    uncle->color=rb_tree_black;
                    node->parent->parent->color=rb_tree_red;
                    node=node->parent->parent;
                }else{
                    if(node==node->parent->left){
                        node=node->parent;
                        rb_right_rotate(node);
                    }
                    node->parent->color=rb_tree_black;
                    node->parent->parent->color=rb_tree_red;
                    rb_left_rotate(node->parent->parent);
                }
            }
            root()->color=rb_tree_black;
        }
    }

    template <typename Key,typename Value,typename KeyOfValue,typename Compare,typename Alloc=allocator>
    void rb_tree<Key,Value,KeyOfValue,Compare,Alloc>::
    insert_unique(const_reference v){
        node_ptr new_node = create_node(v);
        color(new_node)=rb_tree_red;

        node_ptr y=nil;
        node_ptr x=root();
        while(x!=nil){
            y=x;
            if( v < x->value)
                x=x->left;
            else
                x=x->right;
        }
        new_node->parent=y;
        if(x==nil)
            nil->parent=new_node;
        else if(v < y->value)
            y->left=new_node;
        else
            y->right=new_node;
        new_node->left=nil;
        new_node->right=nil;

        rb_insert_fixup(new_node);
    }



}


#endif //MINISTL_RB_TREE_H
