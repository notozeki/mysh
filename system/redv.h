#ifndef REDV_H
#define REDV_H

typedef struct {
	int** entries;
	int ep;
} Redv;

Redv* new_redv();
Redv* redv_copy(const Redv* src);
void redv_append_entry(Redv* redv, int src, int dst);
void delete_redv(Redv* redv);

#endif // REDV_H
