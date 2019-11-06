#ifndef NAUTYWRAPPER_H
#define NAUTYWRAPPER_H

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* return nonzero if the graphs represented by the sequences adj1 and adj2 of
 * adjacency lists are isomorphic, else return zero (use sigma=NULL to skip) */
int nautywrapper_is_isomorphic(int isdir,int n,int *adj1,int *adj2,int *sigma);

/* write the generators of Aut(G), where G is represented by the sequence
 * adj of adjacency lists, to the temporary file f and returns length of f
 * (leaves the file open, it should be closed afterwards) */
void nautywrapper_aut_generators(int isdir,int n,int *adj,FILE *f);

/* compute the canonical labeling/graph for the input graph represented by the sequence
 * adj of adjacency lists, output as clab and/or cgrph and cols (use NULL to skip) */
void nautywrapper_canonical(int isdir, int n, int *adj,int *clab,unsigned long *cgrph,int *cols);

/* return SETWORDSNEEDED(n) */
int nautywrapper_words_needed(int n);

#if defined(__cplusplus)
}
#endif

#endif // NAUTYWRAPPER_H
