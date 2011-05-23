#ifndef ERROR_H
#define ERROR_H

#define cannot_continue(reason) _cannot_continue(__func__, reason)
#define die_of_bug(reason) _die_of_bug(__FILE__, __LINE__, reason)

void print_last_error(const char* place);
void _cannot_continue(const char* place, const char* reason);
void _die_of_bug(const char* file, int line, const char* reason);

#endif // ERROR_H
