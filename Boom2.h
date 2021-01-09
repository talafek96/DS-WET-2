#ifndef _BOOM_H
#define _BOOM_H
#include "RankAVL/AVL.h"
#include "RankAVL/RankAVL.h"
#include "DynamicArray/DynamicArray.h"
#include "ChainTable/ChainTable.h"


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

    class Boom2
    {
    private:

        class lectures
        {
        public:
            DynamicArray<LectureContainer> array;
            int top = 0;

            lectures() : array(DynamicArray<LectureContainer>(10, {0, 0, 0}, 2)), top(0) { }
        };

        class SubtreeSize
        {
        public:
            void operator()(std::shared_ptr<graph_node<LectureContainer, int>>& node)
            {
                if(!node)
                {
                    return;
                }
                int left_size = node->left? node->left->val : 0;
                int right_size = node->right? node->right->val : 0;
                node->val = left_size + right_size + 1;
            }
        };

        ChainTable<lectures> course_table;
        RankAVL<SubtreeSize, LectureContainer, int> lecture_tree;
        int lecture_counter = 0;

    public:
        // TODO: Make a constructor
        Boom2();
        ~Boom2() = default;

        bool addCourse(int course_id);
        bool removeCourse(int course_id);
        bool addClass(int course_id, int* class_id);
        bool watchClass(int course_id, int class_id, int time);
        bool timeViewed(int course_id, int class_id, int* time_viewed);
        bool getIthWatchedClass(int i, int* course_id, int* class_id);

        class InvalidInput { };
    };
}
#endif