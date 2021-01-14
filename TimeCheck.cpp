#include <map>
#include <iostream>
#include <sstream>
#include <functional>
#include <cmath>
#include <exception>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <random>
#include <set>
#include <climits>

// Edit the path if necessary
#include "library2.h"

using std::cout;
using std::endl;
using std::string;
using namespace std::chrono;

// Asserts that x is true
#define ASSERT_TEST(x) do{if(!(x)){ \
    std::cout<<"Failed assertion at line "<<__LINE__<<" in "<<__func__<<std::endl;\
    return false; }} while(false);

// Asserts that x causes no errors
#define ASSERT_NO_ERROR(x) do{try{x;}catch(std::exception& e){std::cout<<"Failed assertion at line "<<__LINE__<<" in "<<__func__<<\
    ": received error "<<"\""<<e.what()<<"\""<<" while expecting no errors"<<std::endl; return false;}} while(false);

// Asserts that x causes this error (and no other error)
#define ASSERT_ERROR(x, error) do{ \
    try{x; std::cout<<"Failed assertion at line "<<__LINE__<<" in "<<__func__<<\
    ": received no error while expecting "<<#error<<std::endl; return false;}\
    catch(error& e){}\
    catch(const std::exception& e){std::cout<<"Failed assertion at line "<<__LINE__<<" in "<<__func__<<\
    ": received error: "<<"\""<<e.what()<<"\" while expecting "<<"\""<<#error<<"\""<<std::endl; return false;}\
    } while(false);

// Asserts that x causes this error (and no other error) with message
#define ASSERT_ERROR_WITH_MESSAGE(x, error, message) do{ \
    try{x; std::cout<<"Failed assertion at line "<<__LINE__<<" in "<<__func__<<\
    ": received no error while expecting "<<#error<<std::endl; return false;}\
    catch(error& e){ASSERT_TEST(string(e.what()) == (message));}\
    catch(const std::exception& e){std::cout<<"Failed assertion at line "<<__LINE__<<" in "<<__func__<<\
    ": received error: "<<"\""<<e.what()<<"\" while expecting "<<"\""<<#error<<"\""<<std::endl; return false;}\
    } while(false);

#define ADD_TEST(x) tests[#x]=x;

typedef enum {
    DIFF = -4,
} getReturnCode;

// Helper function to check the result of AddClass
int AddClassAUX(void* DS, int courseID, int expected_classID){
    int classID;
    int return_code = AddClass(DS,courseID,&classID);
    if(return_code == SUCCESS){
        if(classID != expected_classID){
            return DIFF;
        }        
    }
    return return_code;
}

// Helper function to calculate the average duration of a vector of durations
long long calculateAverageDuration(std::vector<long long> durations, int repetitions){
    long long sum = 0;
    for(long long& element : durations){
        sum += element;
    }
    return sum/repetitions;
}

// Test functions:

// Function to check the complexity of a certain input size
bool measureRuntime(int num_courses, int classes_per_course, int time_per_class, int repetitions, std::vector<long long>& result){
    
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
    
    std::uniform_int_distribution<> int_distribution(1,INT_MAX);
    std::uniform_int_distribution<> num_classes_distribution(1,num_courses*classes_per_course);
    
    std::set<int> course_IDs_set;
    while((int)course_IDs_set.size() < num_courses + repetitions){
        course_IDs_set.insert(int_distribution(generator));
    }
    std::vector<int> course_IDs;
    course_IDs.assign(course_IDs_set.begin(),course_IDs_set.end());

    // Create vectors for storing repetition results. Will be used to calculate average runtime.
    std::vector<long long> init_durations;
    std::vector<long long> add_course_durations;
    std::vector<long long> add_class_durations;
    std::vector<long long> watch_durations;
    std::vector<long long> time_durations;
    std::vector<long long> remove_durations;
    std::vector<long long> get_durations;
    std::vector<long long> quit_durations;
    // Perform the measurements.
    for(int i = 0; i < repetitions; i++){
        // Measure the time it takes to initialize.
        auto start = high_resolution_clock::now();
        void* DS = Init();
        auto stop = high_resolution_clock::now(); 
        auto init_duration = duration_cast<nanoseconds>(stop - start); 
        init_durations.push_back(init_duration.count());
        ASSERT_TEST(DS);
        Quit(&DS);
        ASSERT_TEST(DS == nullptr);            
    }

    //Initialize the system and fill it with courses and classes.
    void* DS = Init();
    ASSERT_TEST(DS);

    // Fill the system with courses.
    for(int i = 0; i < num_courses; i++){
        ASSERT_TEST(AddCourse(DS,course_IDs[i]) == SUCCESS);
        for(int j = 0; j < classes_per_course; j++){
            ASSERT_TEST(AddClassAUX(DS,course_IDs[i],j) == SUCCESS);
        }
    }
    // Fill the system so that all of the classes have time viewed.
    for(int i = 0; i < num_courses; i++){
        for(int j = 0; j < classes_per_course; j++){
            ASSERT_TEST(WatchClass(DS,course_IDs[i],j,time_per_class) == SUCCESS);
        }
    }

    for(int i = 0; i < repetitions; i++){
        
        int course_to_add = course_IDs[num_courses + i];

        int courseID;
        int classID;

        // Measure the time it takes to add a course.
        auto start = high_resolution_clock::now();
        ASSERT_TEST(AddCourse(DS,course_to_add) == SUCCESS);
        auto stop = high_resolution_clock::now(); 
        auto add_course_duration = duration_cast<nanoseconds>(stop - start); 
        add_course_durations.push_back(add_course_duration.count());
        
        for(int j = 0; j < classes_per_course; j++){
            ASSERT_TEST(AddClassAUX(DS,course_to_add,j) == SUCCESS);
        }

        // Measure the time it takes to add a class.
        start = high_resolution_clock::now();
        ASSERT_TEST(AddClass(DS,course_to_add,&classID) == SUCCESS);
        stop = high_resolution_clock::now(); 
        ASSERT_TEST(classID == classes_per_course);
        auto add_class_duration = duration_cast<nanoseconds>(stop - start); 
        add_class_durations.push_back(add_class_duration.count());

        // Measure the time it takes to add time to a class.
        start = high_resolution_clock::now();
        ASSERT_TEST(WatchClass(DS,course_to_add,classes_per_course,time_per_class) == SUCCESS);
        stop = high_resolution_clock::now();
        auto watch_duration = duration_cast<nanoseconds>(stop - start); 
        watch_durations.push_back(watch_duration.count());

        // Measure the time it takes to add time to a class.
        int time;
        start = high_resolution_clock::now();
        ASSERT_TEST(TimeViewed(DS,course_to_add,classes_per_course,&time) == SUCCESS);
        stop = high_resolution_clock::now();
        ASSERT_TEST(time == time_per_class)
        auto time_duration = duration_cast<nanoseconds>(stop - start); 
        time_durations.push_back(time_duration.count());

        // Measure the time it takes to remove a course.
        start = high_resolution_clock::now(); 
        ASSERT_TEST(RemoveCourse(DS,course_to_add) == SUCCESS);
        stop = high_resolution_clock::now();
        auto remove_duration = duration_cast<nanoseconds>(stop - start); 
        remove_durations.push_back(remove_duration.count());

        // Measure the time it takes to get a random s.
        start = high_resolution_clock::now(); 
        int position = num_classes_distribution(generator);
        ASSERT_TEST(GetIthWatchedClass(DS,position,&courseID,&classID) == SUCCESS);
        stop = high_resolution_clock::now();
        auto get_duration = duration_cast<nanoseconds>(stop - start);
        get_durations.push_back(get_duration.count());
    }
    // Measure the time it takes to quit.
    auto start = high_resolution_clock::now();
    Quit(&DS);
    auto stop = high_resolution_clock::now(); 
    auto quit_duration = duration_cast<nanoseconds>(stop - start); 
    quit_durations.push_back(quit_duration.count());
    ASSERT_TEST(DS == nullptr); 

    // Calculate the average duration of each operations.
    long long init_average_duration = calculateAverageDuration(init_durations,repetitions);
    long long add_course_average_duration = calculateAverageDuration(add_course_durations,repetitions);
    long long add_class_average_duration = calculateAverageDuration(add_class_durations,repetitions);
    long long watch_average_duration = calculateAverageDuration(watch_durations,repetitions);
    long long time_average_duration = calculateAverageDuration(time_durations,repetitions);
    long long remove_average_duration = calculateAverageDuration(remove_durations,repetitions);
    long long get_average_duration = calculateAverageDuration(get_durations,repetitions);
    // Not using average for quit because that would take a lot of time to run.
    long long quit_average_duration = quit_durations[0];

    // Return the results as a vector.
    std::vector<long long> output = {init_average_duration,add_course_average_duration,add_class_average_duration,watch_average_duration,time_average_duration,remove_average_duration,get_average_duration,quit_average_duration};
    result = output;
    return true;
}

// Measure the time it takes for inputs in logarithmic scale to perform the various functions.
bool testTimeComplexity(){
    // Change these parameters to set the size of the test:
    int base = 10;
    int max_power = 5;
    int number_of_classes_per_course = 10;
    int time_per_class = 10;
    int repetitions = 100;
    // Run the test on different output sizes in logarithmic scale.
    std::vector<long long> result;
    for(int i = 1; i <= max_power; i++){
        std::vector<long long> result_i;
        ASSERT_TEST(measureRuntime(pow(base,i),number_of_classes_per_course,time_per_class,repetitions,result_i));
        result.insert(result.end(),result_i.begin(),result_i.end());
    }
    // Print the results of the tests
    std::cout<<"****Showing average runtime results from "<<repetitions<<" repetitions (except Quit):****"<<std::endl;
    std::cout<<"****Ignore the first result in each function, it's probably larger than the others****"<<std::endl;
    std::cout<<"Init ****O(1)****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"AddCourse ****O(1) amortized****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+1]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"AddClass ****O(1) amortized****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+2]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"WatchClass ****O(log(M+2)) amortized****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+3]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"TimeViewed ****O(1) amortized****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+4]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"RemoveCourse ****O(mlog(M)) amortized****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+5]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"GetIthWatchedClass ****O(log(M+2))****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+6]<<" nanoseconds."<<std::endl;
    }
    std::cout<<"Quit ****O(n+M)****:"<<std::endl;
    for(int i = 1; i <= max_power; i++){
        std::cout<<pow(base,i)<<": "<<result[(i-1)*8+7]<<" nanoseconds."<<std::endl;
    }
    return true;
}

// Functions to run the program:

bool run_test(std::function<bool()> test, std::string test_name){
    auto start = high_resolution_clock::now(); 
    if(!test()){
        std::cout<<test_name<<" - FAILED."<<std::endl;
        return false;
    }
    auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<milliseconds>(stop - start); 
    std::cout<<test_name<<" - PASSED in "<<duration.count()<<" milliseconds."<<std::endl;
    return true;
}

int main(){

    std::map<std::string, std::function<bool()>> tests;

    ADD_TEST(testTimeComplexity);

    int passed = 0;
    for (std::pair<std::string, std::function<bool()>> element : tests)
    {
        passed += run_test(element.second, "Running " + element.first);
    }

    std::cout<<"Passed "<<passed<<" out of "<<tests.size()<<" tests."<<std::endl;

    return 0;
}