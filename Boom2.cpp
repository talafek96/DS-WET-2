#include "Boom2.h"

namespace DS
{
    Boom2::Boom2() : 
    lecture_tree(RankAVL<SubtreeSize, LectureContainer, int>(SubtreeSize())), lecture_counter(0) { }
    
    // Returns false if the course already exist, true if the insertion succeeded.
    bool Boom2::addCourse(int course_id)
    {
        if (course_id <= 0)
        {
            throw InvalidInput();
        }
        if (course_table.find(course_id))
        {
            return false;
        }
        
        course_table.insert(course_id, lectures());
        return true;
    }

    // Returns false if there is no course with the given id, true if the deletion succeeded.
    bool Boom2::removeCourse(int course_id)
    {
        if(course_id <= 0)
        {
            throw InvalidInput();
        }
        if(!course_table.find(course_id))
        {
            return false;
        }

        auto& lecture_arr = course_table.get(course_id).array;
        int num_of_lectures = course_table.get(course_id).top;
        for(int i = 0; i < num_of_lectures; i++)
        {
            lecture_tree.erase(lecture_arr.get(i));
        }
        course_table.erase(course_id);
        return true;
    }

    // Returns false if the course doesn't exist, true if the class was added successfully.
    bool Boom2::addClass(int course_id, int* class_id)
    {
        if(course_id <= 0)
        {
            throw InvalidInput();
        }
        if(!course_table.find(course_id))
        {
            return false;
        }

        lectures& lectures_arr = course_table.get(course_id);
        int top = lectures_arr.top;
        LectureContainer lecture = {0, course_id, top};
        lectures_arr.array.store(top, lecture);
        *class_id = top;
        lectures_arr.top++;
        return true;
    }

    // Returns false if the course doesn't exist, true if time was added successfully.
    bool Boom2::watchClass(int course_id, int class_id, int time)
    {
        if(time <= 0 || class_id < 0 || course_id <= 0)
        {
            throw InvalidInput();
        }
        if(!course_table.find(course_id))
        {
            return false;
        }
        lectures& lecture_arr = course_table.get(course_id);
        if(class_id + 1 > lecture_arr.array.initialized())
        {
            throw InvalidInput();
        }

        auto old_lecture = lecture_arr.array.get(class_id);
        LectureContainer new_lecture = {old_lecture.views + time, old_lecture.course, old_lecture.lecture};
        lecture_arr.array.store(class_id, new_lecture);
        lecture_tree.erase(old_lecture);
        lecture_tree.insert(new_lecture, 0);
        return true;
    }

    bool Boom2::timeViewed(int course_id, int class_id, int* time_viewed)
    {
        if(course_id <= 0 || class_id < 0)
        {
            throw InvalidInput();
        }
        if(!course_table.find(course_id))
        {
            return false;
        }
        lectures& lecture_arr = course_table.get(course_id);
        if(class_id + 1 > lecture_arr.array.initialized())
        {
            throw InvalidInput();
        }

        *time_viewed = lecture_arr.array.get(class_id).views;
        return true;
    }

    bool Boom2::getIthWatchedClass(int i, int* course_id, int* class_id)
    {
        if(i <= 0)
        {
            throw InvalidInput();
        }
        if(lecture_tree.size() < i)
        {
            return false;
        }
        
        class FindIthWatchedClass
        {
            int i;
        public:
            FindIthWatchedClass(int i) : i(i) { }

            SearchPath operator()(std::shared_ptr<graph_node<LectureContainer, int>>& node)
            {
                SearchPath result = SearchPath::end;
                int right_rank = node->right? node->right->val : 0;
                if(right_rank == i - 1) // The current node is the wanted node.
                {
                    return result;
                }
                else if(right_rank > i - 1) // The node we are looking for is in the right sub-tree.
                {
                    node = node->right;
                    result = SearchPath::right;
                }
                else // right_rank < i - 1: The node we are looking for is in the left sub-tree.
                {
                    node = node->left;
                    result = SearchPath::left;
                    i = i - right_rank - 1;
                }
                
                return result;
            }
        };

        FindIthWatchedClass calc_functor(i);
        const std::shared_ptr<graph_node<LectureContainer, int>>& target = lecture_tree.rank(calc_functor);
        *course_id = target->key.course;
        *class_id = target->key.lecture;

        return true;
    }

}