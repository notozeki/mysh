#ifndef STRING_H
#define STRING_H

typedef struct tagString String;

String* new_string(const char* init_string);
void string_append(String* str, const char* hind);
void string_append_char(String* str, const char c);
void string_renew(String* str, const char* renew_string);
const char* string_c_str(String* str);
int string_cmp1(String* str, const String* str2);
int string_cmp2(String* str, const char* str2);
int string_size(String* str);
void delete_string(String* str);

#endif // STRING_H
