#include "GradeOfCourse.h"
#include <iostream>

// 修复重点：初始化列表必须放在参数括号的后面，用冒号开始！
GradeOfCourse::GradeOfCourse(int _studentID, int _grade, std::string _courseNo)
    : studentID(_studentID), grade(_grade), courseNo(_courseNo) {
}

GradeOfCourse::GradeOfCourse() : studentID(0), grade(0), courseNo("") {
}

GradeOfCourse::~GradeOfCourse() {
}

void GradeOfCourse::showInfo() {
    // 修复重点：补全了分号，并且补上了成绩和课程的输出
    std::cout << "Student ID: " << studentID 
              << ", Course No: " << courseNo 
              << ", Grade: " << grade << std::endl;
}