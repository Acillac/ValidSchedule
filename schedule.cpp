/**
 * @file schedule.cpp
 * Exam scheduling using graph coloring
 */

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <fstream>

#include "schedule.h"
#include "utils.h"
#include <algorithm>

/**
 * Given a filename to a CSV-formatted text file, create a 2D vector of strings where each row
 * in the text file is a row in the V2D and each comma-separated value is stripped of whitespace
 * and stored as its own string. 
 * 
 * Your V2D should match the exact structure of the input file -- so the first row, first column
 * in the original file should be the first row, first column of the V2D.
 *  
 * @param filename The filename of a CSV-formatted text file. 
 */
V2D file_to_V2D(const std::string & filename){
    // Your code here!
    V2D ans;
    std::ifstream in(filename);
    std::string str;
    std::vector<std::string> lines;
    while (std::getline(in, str)) {
        lines.push_back(str);
    }
    for (std::string line : lines) {
        std::vector<std::string> each_line;
        std::string word = "";
        for (char letter : line) {
            if (letter != ' ' && letter != ',') {
                word += letter;
            }
            if (letter == ',') {
                each_line.push_back(word);
                word = "";
            }
        }
        each_line.push_back(word);
        ans.push_back(each_line);
    }
    return ans;
}

/**
 * Given a course roster and a list of students and their courses, 
 * perform data correction and return a course roster of valid students (and only non-empty courses).
 * 
 * A 'valid student' is a student who is both in the course roster and the student's own listing contains the course
 * A course which has no students (or all students have been removed for not being valid) should be removed
 * 
 * @param cv A 2D vector of strings where each row is a course ID followed by the students in the course
 * @param student A 2D vector of strings where each row is a student ID followed by the courses they are taking
 */
V2D clean(const V2D & cv, const V2D & student) {
    // YOUR CODE HERE
    V2D ans;
    std::map<std::string, std::vector<std::string>> classes;
    for (std::vector<std::string> row : student) {
        for (unsigned i = 1; i < row.size(); i++) {
            classes[row[0]].push_back(row[i]);
        }
    }
    for (std::vector<std::string> row : cv) {
        bool found = false;
        std::vector<std::string> output_row;
        // std::cout << row[0] << std::endl;
        output_row.push_back(row[0]);
        for (unsigned i = 1; i < row.size(); i++) {
            if (classes.find(row[i]) != classes.end()) {
                if (std::find(classes[row[i]].begin(), classes[row[i]].end(), row[0]) != classes[row[i]].end()) {
                    found = true;
                    output_row.push_back(row[i]);
                }
            }
        }
        if (found) {
            ans.push_back(output_row);
        }
    }
    return ans;
}

/**
 * Given a collection of courses and a list of available times, create a valid scheduling (if possible).
 * 
 * A 'valid schedule' should assign each course to a timeslot in such a way that there are no conflicts for exams
 * In other words, two courses who share a student should not share an exam time.
 * Your solution should try to minimize the total number of timeslots but should not exceed the timeslots given.
 * 
 * The output V2D should have one row for each timeslot, even if that timeslot is not used.
 * 
 * As the problem is NP-complete, your first scheduling might not result in a valid match. Your solution should 
 * continue to attempt different schedulings until 1) a valid scheduling is found or 2) you have exhausted all possible
 * starting positions. If no match is possible, return a V2D with one row with the string '-1' as the only value. 
 * 
 * @param courses A 2D vector of strings where each row is a course ID followed by the students in the course
 * @param timeslots A vector of strings giving the total number of unique timeslots
 */
V2D schedule(const V2D &courses, const std::vector<std::string> &timeslots){
    // Your code here!
    // create empty V2D with the start of each row as the timeslot
    V2D ans;
    for (unsigned i = 0; i < timeslots.size(); i++) {
        std::vector<std::string> row;
        row.push_back(timeslots[i]);
        ans.push_back(row);
    }
    // initialize a matrix that is # courses length and width
    std::vector<std::vector<int>> matrix;
    matrix.resize(courses.size());
    for (unsigned i = 0; i < courses.size(); i++) {
        matrix[i].resize(courses.size(), 0);
    }
    // loop through courses and set all connections between classes to 1
    for (unsigned i = 0; i < courses.size() - 1; i++) {
        for (unsigned j = i + 1; j < courses.size(); j++) {
            for (unsigned k = 1; k < courses[i].size(); k++) {
                for (unsigned l = 1; l < courses[j].size(); l++) {
                    if (courses[i][k] == courses[j][l]) {
                        matrix[i][j] = 1;
                        matrix[j][i] = 1;
                    }
                }
            }
        }
    }
    // create a map that stores the graph color value for each course (all start at -1)
    std::map<std::string, int> graph_color;
    for (std::vector<std::string> course : courses) {
        graph_color[course[0]] = -1;
    }

    
    std::map<std::string, int> temp;
    int max = -1;
    // loop through every possible course starting position
    for (unsigned k = 0; k < courses.size(); k++) {
        // loop through every x position of each class
        temp = graph_color;
        max = -1;
        for (unsigned i = 0 + k; i < matrix.size() + k; i++) {
            std::map<std::string, std::vector<std::string>> row_map;
            // loop through every y position of each class
            for (unsigned j = 0; j < matrix.size(); j++) {
                // if connection between 2 classes, map key: (first class) value: all classes it is connected to in that row
                if (matrix[i % courses.size()][j] == 1) {
                    row_map[courses[i % courses.size()][0]].push_back(courses[j][0]);
                }
            }
            // if no connections, set to 0
            if (row_map[courses[i % courses.size()][0]].size() == 0) {
                temp[courses[i % courses.size()][0]] = 0;
                if (0 > max) {
                    max = 0;
                }
            } else {
                // adds all connected class values to a set
                std::set<int> total;
                for (unsigned a = 0; a < row_map[courses[i % courses.size()][0]].size(); a++) {
                    total.insert(temp[row_map[courses[i % courses.size()][0]][a]]);
                }
                // finds lowest unfound color value
                int counter = 0;
                while (total.find(counter) != total.end()) {
                    counter++;
                }
                // adds color value to the graph
                temp[courses[i % courses.size()][0]] = counter;
                if (counter > max) {
                    max = counter;
                }
                if (max + 1 > (int)timeslots.size()) {
                break;
                }
            }
        }
        if (max + 1 <= (int)timeslots.size()) {
            for (std::vector<std::string> course : courses) {
                ans[temp[course[0]]].push_back(course[0]);
            }
            return ans;
        }
    }
    if (max + 1 <= (int)timeslots.size()) {
        for (std::vector<std::string> course : courses) {
            ans[temp[course[0]]].push_back(course[0]);
        }
        return ans;    
    } else {
        V2D fake_ans;
        std::vector<std::string> row;
        row.push_back("-1");
        fake_ans.push_back(row);
        return fake_ans;
    }
}
