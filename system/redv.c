#include <stdio.h>
#include <stdlib.h>
#include "redv.h"
#include "../common/error.h"

#define MAX_ENTRY 256

Redv* new_redv()
{
        Redv* redv;
        redv = malloc(sizeof(Redv));
	if ( redv == NULL ) {
		cannot_continue("malloc");
	}
        redv->entries = malloc(MAX_ENTRY);
	if ( redv->entries == NULL ) {
		cannot_continue("malloc");
	}
        redv->ep = 0;
        return redv;
}

Redv* redv_copy(const Redv* src)
{
	Redv* dst;
	int i;

	if ( src == NULL ) return new_redv();

	dst = new_redv();
	for ( i = 0; i < src->ep; i++ ) {
		dst->entries[i] = src->entries[i];
	}
	dst->ep = i;
	return dst;
}

void redv_append_entry(Redv* redv, int src, int dst)
{
        if ( redv->ep == MAX_ENTRY ) {
		fprintf(stderr, "too many redirects\n");
		return;
        }
        redv->entries[redv->ep] = malloc(sizeof(int) * 2);
        redv->entries[redv->ep][0] = src;
        redv->entries[redv->ep][1] = dst;
        (redv->ep)++;
}

void delete_redv(Redv* redv)
{
        int i;

        for ( i = 0; i < redv->ep; i++ ) {
                free(redv->entries[i]);
        }
        free(redv->entries);
        free(redv);
}
