#ifndef _ARRAY_INC
#define _ARRAY_INC
#include <iostream>

namespace DS
{
    template<typename T>
    class Array
    {
    private:
        /* Instance variables */
        T* data;
        int max_size;
    public:
        /*********************************/
        /*        Public Section        */
        /*********************************/
        /*
         * Constructor: Array<T>
         * Usage: Array<T> new_array(size);
         * ---------------------------------
         * Initializes a new Array that stores objects of type <T>.
         * The default constructor creates an empty Array.
         * The second form creates and allocates an array with size elements.
         * The max size of the array is constant and cannot be realloced.
         * 
         * Possible exceptions:
         * std::bad_alloc
         */
        explicit Array(int size) : data(new T[size]), max_size(size) { }
        Array() : data(nullptr), max_size(0) { };

        /*
         * Copy Constructor: Array<T>
         * Usage: Array<t> new_array = arr;
         * --------------------------------
         * Initializes a new Array.  
         * Creates a new array that is a copy of arr.
         * 
         * Possible exceptions:
         * No assignment operator to class T, std::bad_aloc
         */
        Array(const Array& arr) : max_size(arr.size())
        {
            T* new_data = new T[arr.size()];
            try
            {
                for (int i = 0 ; i < arr.size(); i++)
                {
                    new_data[i] = arr[i];
                }
            } catch (...) {
                delete[] new_data;
                throw;
            }
            data = new_data;
        }
        
        /*
         * Destructor: ~Array<T>
         * ---------------------
         * Frees any heap storage allocated by this array.
         */
        ~Array()
        {
            delete[] data;
        }

        /*
         * Operator: =
         * Usage: this_array = target_arr;
         * ----------------------
         * Replaces every single element in the left hand array to be equal
         * to target_arr's elements.
         * 
         * Possible exceptions:
         * No assignment operator to class T, std::bad_aloc
         */
        Array& operator=(const Array& target_arr)
        {
            if (this == &target_arr)
            {
                return *this;
            }
            T* temp_data = new T[target_arr.size()];
            try 
            {
                for(int i = 0 ; i < target_arr.size(); i++)
                {
                    temp_data[i] = target_arr[i];
                }
            } catch (...) {
                delete[] temp_data;
                throw;
            }

            delete[] data;
            data = temp_data;
            max_size = target_arr.max_size;
            return *this;
        }

        /*
         * Method: size
         * Usage: int size = this_arr.size();
         * -----------------------------------
         * Returns the number of elements in the array.
         */
        int size() const noexcept
        {
            return max_size;
        }

        /*
         * Operator: []
         * Usage: T element = this_arr[index];
         * ----------------------
         * Returns the <index> element stored in the array.
         */
        T& operator[](int index)
        {
            return data[index];
        }
        
        const T& operator[](int index) const
        {
            return data[index];
        }
    };
}
#endif