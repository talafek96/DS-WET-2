#include <iostream>
#include "ChainTable/ChainTable.h"
int main()
{
    using namespace DS;
    ChainTable<int> table;

    for(int i = 0; i < 10000; i++)
    {
        table.insert(i, 10*i);
    }
    for(int i = 0; i < 9500; i++)
    {
        table.erase(i);
    }
    int a = 50;
    std::cout << "The key " << a << " is " << (table.find(a)? "" : "not ") << "in the table." << std::endl;
    std::cout << "The size of the table is: " << table.tableSize() << ".\n";
    return 0;
}