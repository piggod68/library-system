#pragma once
#include <string>

// 关联类：连接学生和课程，并记录成绩
class GradeOfCourse {
private:
    int studentID;       // 关联学生
    int grade;           // 成绩
    std::string courseNo; // 关联课程（这里用课程号关联Course类）
public:
    GradeOfCourse();
    GradeOfCourse(int _studentID, int _grade, std::string _courseNo);
    ~GradeOfCourse();
    void showInfo();
};