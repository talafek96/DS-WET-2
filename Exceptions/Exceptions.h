#ifndef _DS_EXCEPTIONS_H
#define _DS_EXCEPTIONS_H

namespace DS
{
    /***********************************/
    /*        Exception Section        */
    /***********************************/
    class Exceptions 
    {  
    public:
        virtual ~Exceptions() = default;
    };
    class OutOfBounds : public Exceptions { };
    class KeyNotFound : public Exceptions { };
}

#endif