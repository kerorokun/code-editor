#ifndef ACTION_H
#define ACTION_H

#include <string>

#define INSERT_ACTION 1
#define DELETE_ACTION 2
#define NULL_ACTION -1

typedef struct {
    int action = NULL_ACTION;
    std::string editString;
    int cursorIndex;
} Action;

#endif