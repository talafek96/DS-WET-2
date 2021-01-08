#include <iostream>
#include "ChainTable/ChainTable.h"
int main()
{
    using namespace DS;
    ChainTable<int> table;
    // table.insert(0, 0);
    // table.insert(1, 1);
    // table.insert(2, 2);
    // table.insert(3, 3);
    // table.insert(4, 4);
    // table.insert(5, 5);
    // table.insert(6, 6);
    // table.insert(7, 7);
    // table.insert(8, 8);
    // table.insert(9, 9);
    // table.insert(10, 10);

    // std::cout << table.get(0) << std::endl;
    // std::cout << table.get(1) << std::endl;
    // std::cout << table.get(2) << std::endl;
    // std::cout << table.get(3) << std::endl;
    // std::cout << table.get(4) << std::endl;
    // std::cout << table.get(5) << std::endl;
    // std::cout << table.get(6) << std::endl;
    // std::cout << table.get(7) << std::endl;
    // std::cout << table.get(8) << std::endl;
    // std::cout << table.get(9) << std::endl;
    // std::cout << table.get(10) << std::endl;
    
    for(int i = 0; i < 100000; i++)
    {
        table.insert(i, 10*i);
    }
    std::cout << "The inserted elements are: " << std::endl;
    for(int i = 0; i < 100000; i++)
    {
        std::cout << "(" << i << ", " << table.get(i) << ")\n";
    }
    table.erase(1);
    std::cout << table.find(1) << std::endl;
    return 0;
}