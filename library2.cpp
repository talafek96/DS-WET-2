#include "library2.h"
#include "Boom2.h"

using namespace DS;

void* Init()
{
    Boom2* DS;
    try
    {
        DS = new Boom2();
    }
    catch(const std::bad_alloc& e)
    {
        DS = NULL;
    }
    return (void*)DS;
}

StatusType AddCourse(void* DS, int courseID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    bool res;
    try
    {
        res = static_cast<Boom2*>(DS)->addCourse(courseID);
    }
    catch(const Boom2::InvalidInput& e)
    {
        return INVALID_INPUT;
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    if(!res)
    {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType RemoveCourse(void *DS, int courseID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    bool res;
    try
    {
        res = static_cast<Boom2*>(DS)->removeCourse(courseID);
    }
    catch(const Boom2::InvalidInput& e)
    {
        return INVALID_INPUT;
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    if(!res)
    {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType AddClass(void* DS, int courseID, int* classID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    bool res;
    try
    {
        res = static_cast<Boom2*>(DS)->addClass(courseID, classID);
    }
    catch(const Boom2::InvalidInput& e)
    {
        return INVALID_INPUT;
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    if(!res)
    {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType WatchClass(void *DS, int courseID, int classID, int time)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    bool res;
    try
    {
        res = static_cast<Boom2*>(DS)->watchClass(courseID, classID, time);
    }
    catch(const Boom2::InvalidInput& e)
    {
        return INVALID_INPUT;
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    if(!res)
    {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType TimeViewed(void *DS, int courseID, int classID, int* timeViewed)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    bool res;
    try
    {
        res = static_cast<Boom2*>(DS)->timeViewed(courseID, classID, timeViewed);
    }
    catch(const Boom2::InvalidInput& e)
    {
        return INVALID_INPUT;
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    if(!res)
    {
        return FAILURE;
    }
    return SUCCESS;
}

StatusType GetIthWatchedClass(void* DS, int i, int* courseID, int* classID)
{
    if(!DS)
    {
        return INVALID_INPUT;
    }
    bool res;
    try
    {
        res = static_cast<Boom2*>(DS)->getIthWatchedClass(i, courseID, classID);
    }
    catch(const Boom2::InvalidInput& e)
    {
        return INVALID_INPUT;
    }
    catch(const std::bad_alloc& e)
    {
        return ALLOCATION_ERROR;
    }
    if(!res)
    {
        return FAILURE;
    }
    return SUCCESS;
}

void Quit(void **DS)
{
    if(!DS)
    {
        return;
    }
    delete static_cast<Boom2*>(*DS);
    *DS = NULL;
}