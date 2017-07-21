#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include "allocator.h"
#include "rb_tree.h"

using std::cout;
using std::endl;

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

    mini::rb_tree<int, int, std::less<int>> tree;
    cout << "before insert size: " << tree.size() << endl;

    std::vector<int> input={10,7,8,15,5,6,11,13,12,20,14,16};

    for(auto &i:input)
        tree.insert_unique(i);

    cout << endl << "after insert size: " << tree.size() << endl;
    cout << "Traverse:" << endl;
    tree.DebugMidorderTraverse();
    tree.DebugPreorderTraverse();

    typename mini::rb_tree<int, int, std::less<int>>::node_ptr node;

    std::vector<int> vec={10,6,16,14,12,11,13,5,7,8,15,20};

    for(auto &i : vec){
        node = tree.find(i);
        tree.erase(node);
        cout<<"erase "<<i<<": "<<endl;
        tree.DebugMidorderTraverse();
    }

}


int main() {

    //TestAllocator();
    TestRBTree();

    return 0;
}