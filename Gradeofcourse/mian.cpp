#include <iostream>
#include "Student.h"
#include "Course.h"
#include "GradeOfCourse.h"

int main() {
    // 1. 创建课程
    Course c1("CS101", "C++ Programming", "2026-Fall");
    
    // 2. 创建学生
    Student s1(1001, "Zhang San");

    // 3. 建立关联（连接）：记录张三在CS101课程的成绩
    GradeOfCourse record1(1001, 95, "CS101");

    std::cout << "--- 作业演示：关联与连接 ---" << std::endl;
    s1.showInfo();
    std::cout << "Course Title: " << c1.getCourseTitle() << std::endl;
    record1.showInfo(); // 输出关联信息

    return 0;
}