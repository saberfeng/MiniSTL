#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <cstdlib>
#include "allocator.h"
#include "rb_tree.h"
#include "set.h"

using std::cout;
using std::endl;

template<typename T>
struct identity {
    const T &operator()(const T &x) { return x; }
};

void TestAllocator() {

    mini::allocator<int> int_alloc;
    int *int_ptr = int_alloc.allocate(1);
    int_alloc.construct(int_ptr, 100);
    cout << *int_ptr << endl;
    int_alloc.destroy(int_ptr);
    int_alloc.deallocate(int_ptr, 1);

    mini::allocator<std::string> string_alloc;
    std::string *string_ptr = string_alloc.allocate(2);
    string_alloc.construct(string_ptr, "first");
    string_alloc.construct(string_ptr + 1, "second");
    cout << *string_ptr << endl << *(string_ptr + 1) << endl;
    string_alloc.destroy(string_ptr);
    string_alloc.destroy(string_ptr + 1);
    string_alloc.deallocate(string_ptr, 2);
}

void TestRBTree() {

    mini::rb_tree<int, int, identity<int>, std::less<int>> tree1;
    cout << "before insert size: " << tree1.size() << endl;

    std::vector<int> input = {10, 7, 8, 15, 5, 6, 11, 11, 11, 11, 13, 12, 8, 20, 14, 16};

    for (auto &i:input)
        tree1.insert_unique(i);

    mini::rb_tree<int, int, identity<int>, std::less<int>> tree(tree1);

    cout << endl << "after insert size: " << tree.size() << endl;
    cout << "Traverse:" << endl;
    tree.DebugMidorderTraverse();
    tree.DebugPreorderTraverse();

    typename mini::rb_tree<int, int, identity<int>, std::less<int>>::node_ptr node;


    mini::rb_tree_iterator<int, int &, int *> iter = tree.begin();
    cout << "iterator iterates:" << endl;
    for (int i = 0; i < 20; ++i) {
        cout << *iter << " ";
        ++iter;
    }
    cout << endl;

    cout << "reversely iterates:" << endl;
    for (int i = 0; i < 24; ++i) {
        --iter;
        cout << *iter << " ";
    }
    cout << endl;

    std::vector<int> vec = {10, 6, 16, 14, 12, 11, 13, 5, 7, 8, 15, 20};

    for (auto &i : vec) {
        node = tree.find(i);
        tree.erase(node);
        cout << "erase " << i << ": " << endl;
        tree.DebugMidorderTraverse();
    }

}

void TestSet(){
    mini::set<int> s;
    s.insert({10, 7, 8, 15, 5, 6, 11, 11, 11, 11, 13, 12, 8, 20, 14, 16});
    mini::set<int>::iterator iter=s.begin();
    while(iter!=s.end())
        cout<<*iter++<<" ";
    cout<<endl;
}

int main() {

    //TestAllocator();
    TestRBTree();
    TestSet();

    return 0;
}