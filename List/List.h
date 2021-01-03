#ifndef _LIST_DS_H
#define _LIST_DS_H

namespace DS
{
    template<typename TYPE>
    struct ListNode
    {
        TYPE key;
        std::shared_ptr<ListNode<TYPE>> next;
    };

    enum SuccessType
    {
        Failure = 0, Success, AlreadyInList
    };

    template<typename TYPE>
    class List
    {
    protected:
        std::shared_ptr<ListNode<TYPE>> header;
        int size = 0;

    public:
        List();
        List(const List& other)
        virtual ~List();
        
        List& operator=(const List& other);
        SuccessType insert(const TYPE& key);
        const std::shared_ptr<ListNode<TYPE>>& get(const TYPE& key);
        void remove(const TYPE& key);
    };
}
#endif