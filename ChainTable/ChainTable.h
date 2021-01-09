#ifndef _CHAIN_TABLE_H
#define _CHAIN_TABLE_H
#include <math.h>
#include "../DynamicArray/DynamicArray.h"
#include "../RankAVL/AVL.h"
#include "../Exceptions/Exceptions.h"

namespace DS
{
    template<typename VAL_TYPE>
    class ChainTable
    {
    private:
        /*********************************/
        /*        Private Section        */
        /*********************************/
        DynamicArray<AVL<int, VAL_TYPE>> table;
        int elem_counter;
        bool already_expanded;
        double ratio; // Hash function parameter (golden ratio)

        /*   Private Static Variables   */
        static const int STRESS_CONTROL = 2; // elem_counter/size = alpha < STRESS_CONTROL
        static const int INIT_SIZE = 10; // Initial table size
        /*   Private Methods/Static Functions   */
        // Gets a key as input and returns the matching index
        int hash(double key) const
        {
            return floor(table.size()*modf(key*ratio, &key));
        }

        /* Makes a new hash table with new_size allocated cells.
         * Returns false if was unable to create a new table. (due to allocation errors, etc.)
         * The data structure will assume normal behavior until finally
         * able to extend or shorten the table and fix the stress factor accordingly. 
         */
        bool remakeTable(int new_size)
        {
            int table_size = table.size();
            
            class CopyToTable
            {
            public:
                ChainTable<VAL_TYPE> new_table;
                explicit CopyToTable(int new_size) : new_table(ChainTable<VAL_TYPE>(new_size)) { }
                void updateTableSize(int new_size)
                {
                    new_table = ChainTable<VAL_TYPE>(new_size);
                }
                void operator()(const std::shared_ptr<graph_node<int, VAL_TYPE>>& node, int* k)
                {
                    new_table.insert(node->key, node->val);
                    (*k)--;
                }
            };

            CopyToTable functor(1);
            try
            {
                functor.updateTableSize(new_size);
            }
            catch(const std::bad_alloc& e)
            {
                return false;
            }

            for(int i = 0; i < table_size; i++)
            {
                AVL<int, VAL_TYPE>& container = table.get(i);
                if(container.size())
                {
                    container.inOrder(functor);
                }
            }
            *this = functor.new_table;
            already_expanded = true;
            return true;
        }

        bool fixStress()
        {
            int table_size = table.size();
            double stress_factor = static_cast<double>(elem_counter)/table_size;
            bool res = true;
            if((stress_factor >= STRESS_CONTROL) ||
            ((table_size > INIT_SIZE) && already_expanded && (stress_factor < (static_cast<double>(STRESS_CONTROL)/4))))
            {
                // The constant to multiply the current table size with to normalize the stress factor to (1/2)*STRESS_CONTROL:
                double k = (2*static_cast<double>(elem_counter))/(STRESS_CONTROL*table_size);
                res = remakeTable(ceil(static_cast<double>(table_size) * k));
            }
            return res;
        }

    public:
        /**********************************/
        /*         Public Section         */
        /**********************************/
        /*
         * Constructor: ChainTable
         * Usage: ChainTable<VAL_TYPE> table;
         *        ChainTable<VAL_TYPE> table(init_size);
         * ---------------------------------------
         * Creates an empty hash table with init_size pre-allocated cells.
         * If no init_size is inserted, assumes init_size = INIT_SIZE. (static var)
         * Worst time complexity: O(1)
         * 
         * Possible Exceptions:
         * std::bad_alloc
         */
        ChainTable() : 
        table(DynamicArray<AVL<int, VAL_TYPE>>(INIT_SIZE, AVL<int, VAL_TYPE>(), 1)), elem_counter(0), already_expanded(false), ratio((sqrt(5) - 1)/2) { }
        ChainTable(int init_size) : 
        table(DynamicArray<AVL<int, VAL_TYPE>>(init_size, AVL<int, VAL_TYPE>(), 1)), elem_counter(0), already_expanded(false), ratio((sqrt(5) - 1)/2) { }

        ChainTable(const ChainTable<VAL_TYPE>& other) = default;

        virtual ~ChainTable() = default;

        ChainTable<VAL_TYPE>& operator=(const ChainTable<VAL_TYPE>& other)
        {
            int new_table_size = other.table.size();
            DynamicArray<AVL<int, VAL_TYPE>> new_table(new_table_size);
            for(int i = 0; i < new_table_size; i++)
            {
                if(other.table.isInitialized(i))
                {
                    new_table.store(i, other.table.get(i));
                }
            }
            table = new_table;
            elem_counter = other.elem_counter;
            already_expanded = other.already_expanded;
            ratio = other.ratio;
            return *this;
        }

        /*
         * Method: insert
         * Usage: table.insert(key, value);
         * ---------------------------------------
         * Inserts the pair (key, value) into the table.
         * Average time complexity: O(1)
         * 
         * Possible Exceptions:
         * std::bad_alloc
         */
        void insert(int key, const VAL_TYPE& value)
        {
            int hashed = hash(key);
            if(!table.isInitialized(hashed))
            {
                table.store(hashed, AVL<int, VAL_TYPE>());
            }
            AVL<int, VAL_TYPE>& container = table.get(hashed);
            if(container.find(key)) // The key is already in the table => don't increment the element counter
            {
                container.erase(key);
                container.insert(key, value);
            }
            else // The key isn't in the table => add it, increment the element counter and fixStress
            {
                container.insert(key, value);
                elem_counter++;
                fixStress();
            }
        }

        /*
         * Method: erase
         * Usage: table.erase(key);
         * ---------------------------------------
         * Removes key and its contents from the table.
         * Average time complexity: O(1)
         */
        void erase(int key)
        {
            if(!table.isInitialized(hash(key)))
            {
                return;
            }
            AVL<int, VAL_TYPE>& container = table.get(hash(key));
            if(container.find(key))
            {
                container.erase(key);
                elem_counter--;
                fixStress();
            }
        }

        /*
         * Method: get
         * Usage: table.get(key);
         * ---------------------------------------
         * Returns the value matching key in the table.
         * Average time complexity: O(1)
         * 
         * Possible Exceptions:
         * KeyNotFound
         */
        const VAL_TYPE& get(int key) const
        {
            int hashed = hash(key);
            return (table.get(hashed)).at(key);
        }

        VAL_TYPE& get(int key)
        {
            int hashed = hash(key);
            return (table.get(hashed)).at(key);
        }
        
        /*
         * Method: find
         * Usage: table.find(key);
         * ---------------------------------------
         * Returns a bool value indicating if key is in the table.
         * Average time complexity: O(1)
         */
        bool find(int key) const
        {
            return (table.get(hash(key))).find(key);
        }

        /*
         * Method: size
         * Usage: table.size();
         * -----------------------------------
         * Returns the current number of elements in the table.
         * 
         * Possible exceptions:
         * No exception.
         */
        int size() const
        {
            return elem_counter;
        }

        int tableSize() const
        {
            return table.size();
        }
    };
}

#endif