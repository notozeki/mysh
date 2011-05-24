#ifndef EMBED_H
#define EMBED_H

int embed_command_match(const char* name);
int embed_command_call(const char* name, char* const argv[]);

#endif // EMBED_H
