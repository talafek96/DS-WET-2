#ifndef _BOOM_H
#define _BOOM_H
// #include "avl_tree/AVL.h"
// #include "Array/Array.h" //TODO: Update includes

namespace DS
{
    struct LectureContainer
    {
        int views;
        int course;
        int lecture;

        bool operator<(const LectureContainer& other) const
        {
            if(views < other.views)
            {
                return true;
            }
            else if(views > other.views)
            {
                return false;
            }
            // If reached this point, views == other.views
            if(course > other.course)
            {
                return true;
            }
            else if(course < other.course)
            {
                return false;
            }
            // If reached this point, views == other.views && course == other.course
            if(lecture > other.lecture)
            {
                return true;
            }
            return false;
        }

        bool operator==(const LectureContainer& other) const
        {
            return (views == other.views) && (course == other.course) && (lecture == other.lecture);
        }

        bool operator!=(const LectureContainer& other) const
        {
            return !(*this == other);
        }

        bool operator<=(const LectureContainer& other) const
        {
            return (*this < other) || (*this == other);
        }

        bool operator>(const LectureContainer& other) const
        {
            return !(*this <= other);
        }

        bool operator>=(const LectureContainer& other) const
        {
            return (*this > other) || (*this == other);
        }
    };

    class Boom
    {
    private:
        class SubtreeSize //TODO: Make the rank update functor
        {

        };

        ChainTable<int, DynamicArray<LectureContainer>> course_tree;
        RankAVL<SubtreeSize, LectureContainer,int> lecture_tree;
        int lecture_counter = 0;

    public:
        Boom() = default;
        ~Boom() = default;

        bool addCourse(int course_id, int numOfClasses);
        bool getMostViewedClasses(int numOfClasses, int *courses, int *classes);
        bool removeCourse(int course_id);
        bool watchClass(int course_id, int class_id, int time);
        bool timeViewed(int course_id, int class_id, int *time_viewed);

        class InvalidInput { };
    };
}
#endif