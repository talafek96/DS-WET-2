#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H
#include <cassert>
#include "Array.h"
#include "../Exceptions/Exceptions.h"

namespace DS
{
    template<typename VAL_TYPE>
    class DynamicArray
    {
    protected:
        Array<VAL_TYPE> values;
        Array<int> B;
        Array<int> index_stack;
        int top = 0; // The next index_stack index to write into
        int num_initialized; // Number of initialized elements
        int max_size; // Size of the current array
        VAL_TYPE default_val;

        int realloc_factor;

        /***********************************/
        /*        Protected Section        */
        /***********************************/
        //**** Auxiliary Functions ****//
        static int max(int a, int b)
        {
            return a >= b? a : b;
        }
        

    public:
        /*********************************/
        /*        Public Section        */
        /*********************************/
        /*
         * Constructor: DynamicArray<T>
         * Usage: DynamicArray<T> new_array(size, default_val);
         *        DynamicArray<T> new_array(size);
         * -----------------------------------
         * Initializes a new empty Array that stores objects of type <T>.
         * Creates and allocates an array with size elements.
         * Creating a dynamic array without stating the default val will default
         * it to the default constructor value of VAL_TYPE.
         * The max size of the elements in the array is dynamic and will be
         * reallocated automatically when the array fills up.
         * DO NOT INITIALIZE TO SIZE 0.
         * REALLOC_FACTOR HAS TO BE >= 1!
         * 
         * Possible exceptions:
         * std::bad_alloc
         */
        explicit DynamicArray(int max_size, VAL_TYPE default_val = VAL_TYPE(), int re_fact = 1) : 
        values(Array<VAL_TYPE>(max_size)), B(Array<int>(max_size)), index_stack(Array<int>(max_size)),
        top(0), num_initialized(0), max_size(max_size), default_val(default_val), realloc_factor(re_fact) { }
        /*
         * Copy Constructor: DynamicArray<T>
         * Usage: DynamicArray<T> new_array = arr;
         * -----------------------------------
         * Creates a new array that is a copy of other.
         * 
         * Possible exceptions:
         * No assignment operator to class T, std::bad_aloc
         */
        DynamicArray(const DynamicArray& other) :
        values(other.values), B(other.B), index_stack(other.index_stack), top(other.top), num_initialized(other.num_initialized),
        max_size(other.max_size), default_val(other.default_val), realloc_factor(other.realloc_factor) { }

        virtual ~DynamicArray() = default;
        /*
         * Operator: =
         * Usage: this_array = target_arr;
         * -----------------------------------
         * Replaces every single element in the left hand array to be equal
         * to target_arr's elements.
         * 
         * Possible exceptions:
         * No assignment operator to class T, std::bad_aloc
         */
        DynamicArray& operator=(const DynamicArray& target_arr) = default;

        // Return a bool value to determine whether the dynamic array at index i is initialized.
        bool isInitialized(int i) const noexcept
        {
            assert(i >= 0);
            return ((i < max_size) && (B[i] < top) && (B[i] >= 0) && (index_stack[B[i]] == i));
        }

        /*
         * Method: get
         * Usage: array.get(i);
         * -----------------------------------
         * Returns the element in the cell i of the array.
         * Can only access elements in range of size(), otherwise
         * throws an exception.
         * 
         * Possible exceptions:
         * OutOfBounds
         */
        const VAL_TYPE& get(int i) const
        {
            if(i >= max_size)
            {
                throw OutOfBounds();
            }
            if(isInitialized(i))
            {
                return values[i];
            }
            return default_val;
        }

        VAL_TYPE& get(int i)
        {
            if(i >= max_size)
            {
                throw OutOfBounds();
            }
            if(isInitialized(i))
            {
                return values[i];
            }
            return default_val;
        }

        /*
         * Method: store
         * Usage: array.store(i, val);
         * -----------------------------------
         * Stores the value 'val' in the cell i of the array.
         * Can only access elements in range of size(), otherwise
         * throws and exception.
         * Will only be able to exceed the bounds of the array once all
         * of the cells have been initialized.
         * 
         * Possible exceptions:
         * OutOfBounds, std::bad_alloc
         */
        void store(int i, VAL_TYPE val)
        {
            if(i >= max_size)
            {
                throw OutOfBounds();
            }
            if(!isInitialized(i))
            {
                index_stack[top] = i;
                B[i] = top;
                top++;
                num_initialized++;
            }
            values[i] = val;
            
            // Check to see if the array is now full:
            if(realloc_factor > 1 && num_initialized >= max_size)
            {
                expandArray();
            }
        }

        /*
         * Method: size
         * Usage: array.size();
         * -----------------------------------
         * Returns the current max size of the array.
         * 
         * Possible exceptions:
         * No exception.
         */
        int size() const noexcept
        {
            return max_size;
        }

        /*
         * Method: initialized
         * Usage: array.initialized();
         * -----------------------------------
         * Returns the current number of initialized cells
         * in the array.
         * 
         * Possible exceptions:
         * No exception.
         */
        int initialized() noexcept
        {
            return num_initialized;
        }

        void expandArray(int factor = -1)
        {
            if (factor == -1)
            {
                factor = realloc_factor;
            }
            int new_size = max_size*factor;
            DynamicArray<VAL_TYPE> new_array(new_size, VAL_TYPE(), realloc_factor);
            for(int i = 0; i < max_size; i++)
            {
                new_array.store(i, get(i));
            }
            *this = new_array;
        }
    };
}

#endif