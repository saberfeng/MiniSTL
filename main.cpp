#include <iostream>
#include <memory>
#include <string>
#include "allocator.h"

using std::cout;
using std::endl;

void TestAllocator(){

    mini::allocator<int> int_alloc;
    int *int_ptr=int_alloc.allocate(1);
    int_alloc.construct(int_ptr,100);
    cout<<*int_ptr<<endl;
    int_alloc.destroy(int_ptr);
    int_alloc.destroy(int_ptr);
    int_alloc.deallocate(int_ptr,1);

    mini::allocator<std::string> string_alloc;
    std::string *string_ptr=string_alloc.allocate(2);
    string_alloc.construct(string_ptr,"first");
    string_alloc.construct(string_ptr+1,"second");
    cout<<*string_ptr<<endl<<*(string_ptr+1)<<endl;
    string_alloc.destroy(string_ptr);
    string_alloc.destroy(string_ptr+1);
    string_alloc.deallocate(string_ptr,2);
}


int main() {

    TestAllocator();


    return 0;
}