#include "config.h"
#if defined HAVE_LIBNAUTY && defined HAVE_NAUTY_NAUTUTIL_H
#include "nauty/nausparse.h"
#include "nauty/nautinv.h"
#include "nautywrapper.h"

static DEFAULTOPTIONS_GRAPH(opts_undir);
static DEFAULTOPTIONS_DIGRAPH(opts_dir);

typedef struct { int a,b; } pair;

int pair_less(const void *p,const void *q) {
    return ((pair*)p)->a-((pair*)q)->a;
}

void color_graph(int n,int *lab,int *ptn,int *col) {
    pair *lst=(pair*)malloc(n*sizeof(pair));
    for (int i=0;i<n;++i) {
        pair *p=&lst[i];
        p->a=col[i];
        p->b=i;
    }
    qsort((void*)lst,(size_t)n,sizeof(pair),pair_less);
    for (int i=0;i<n;++i) {
        pair *p=&lst[i];
        lab[i]=p->b;
        ptn[i]=(i>=n-1 || p->a!=lst[i+1].a)?0:1;
    }
    free(lst);
}

int int_less(const void *p,const void *q) {
    return *(int*)p-*(int*)q;
}

int nautywrapper_words_needed(int n) {
    return SETWORDSNEEDED(n);
}

int nautywrapper_is_isomorphic(int isdir,int n,int *adj1,int *adj2,int *sigma) {
    DYNALLSTAT(int,lab1,lab1_sz);
    DYNALLSTAT(int,lab2,lab2_sz);
    DYNALLSTAT(int,ptn1,ptn1_sz);
    DYNALLSTAT(int,ptn2,ptn2_sz);
    DYNALLSTAT(int,col1,col1_sz);
    DYNALLSTAT(int,col2,col2_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    DYNALLSTAT(graph,g1,g1_sz);
    DYNALLSTAT(graph,g2,g2_sz);
    DYNALLSTAT(graph,cg1,cg1_sz);
    DYNALLSTAT(graph,cg2,cg2_sz);
    optionblk *options=isdir!=0?&opts_dir:&opts_undir;
    statsblk stats;
    int m=SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    DYNALLOC1(int,lab1,lab1_sz,n,"malloc");
    DYNALLOC1(int,lab2,lab2_sz,n,"malloc");
    DYNALLOC1(int,ptn1,ptn1_sz,n,"malloc");
    DYNALLOC1(int,ptn2,ptn2_sz,n,"malloc");
    DYNALLOC1(int,col1,col1_sz,n,"malloc");
    DYNALLOC1(int,col2,col2_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
    DYNALLOC2(graph,g1,g1_sz,n,m,"malloc");
    EMPTYGRAPH(g1,m,n);
    DYNALLOC2(graph,g2,g2_sz,n,m,"malloc");
    EMPTYGRAPH(g2,m,n);
    DYNALLOC2(graph,cg1,cg1_sz,n,m,"malloc");
    DYNALLOC2(graph,cg2,cg2_sz,n,m,"malloc");
    options->getcanon=TRUE;
    options->writeautoms=FALSE;
    options->outfile=NULL;
    options->defaultptn=FALSE;
    int i=0,j=0,k,read_col=1;
    /* create the first graph */
    while (1) {
        if ((k=adj1[i++])==-1) { if ((++j)==n) break; read_col=1; }
        else if (read_col==1) { col1[j]=k; read_col=0; }
        else if (j<k && isdir==0) { ADDONEEDGE(g1,j,k,m); }
        else if (isdir!=0) { ADDONEARC(g1,j,k,m); }
    }
    /* create the second graph */
    i=j=0;
    read_col=1;
    while (1) {
        if ((k=adj2[i++])==-1) { if ((++j)==n) break; read_col=1; }
        else if (read_col==1) { col2[j]=k; read_col=0; }
        else if (j<k && isdir==0) { ADDONEEDGE(g2,j,k,m); }
        else if (isdir!=0) { ADDONEARC(g2,j,k,m); }
    }
    color_graph(n,lab1,ptn1,col1);
    color_graph(n,lab2,ptn2,col2);
    boolean isomorphic;
    qsort((void*)col1,(size_t)n,sizeof(int),int_less);
    qsort((void*)col2,(size_t)n,sizeof(int),int_less);
    for (i=0;i<n;++i) {
        if (col1[i]!=col2[i])
            break;
    }
    if (i==n) { // colors match
        /* canonically label both graphs */
        densenauty(g1,lab1,ptn1,orbits,options,&stats,m,n,cg1);
        densenauty(g2,lab2,ptn2,orbits,options,&stats,m,n,cg2);
        /* return nonzero iff the canonical labelings match */
        size_t cnt=0;
        for (;cnt<m*(size_t)n;++cnt) {
            if (cg1[cnt]!=cg2[cnt]) break;
        }
        isomorphic=cnt==m*(size_t)n?TRUE:FALSE;
        if (isomorphic && sigma!=NULL) {
            for (i=0;i<n;++i) {
                sigma[lab1[i]]=lab2[i];
            }
        }
    } else isomorphic=FALSE; // colors do not match
    DYNFREE(lab1,lab1_sz);
    DYNFREE(lab2,lab2_sz);
    DYNFREE(ptn1,ptn1_sz);
    DYNFREE(ptn2,ptn2_sz);
    DYNFREE(col1,col1_sz);
    DYNFREE(col2,col2_sz);
    DYNFREE(orbits,orbits_sz);
    DYNFREE(g1,g1_sz);
    DYNFREE(g2,g2_sz);
    DYNFREE(cg1,cg1_sz);
    DYNFREE(cg2,cg2_sz);
    return isomorphic;
}

void nautywrapper_aut_generators(int isdir,int n,int *adj,FILE *f) {
    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,col,col_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    DYNALLSTAT(graph,g,g_sz);
    optionblk *options=isdir!=0?&opts_dir:&opts_undir;
    statsblk stats;
    int m=SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    DYNALLOC1(int,lab,lab_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,col,col_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
    DYNALLOC2(graph,g,g_sz,n,m,"malloc");
    EMPTYGRAPH(g,m,n);
    options->getcanon=FALSE;
    options->writeautoms=TRUE;
    options->linelength=RAND_MAX;
    options->outfile=f;
    options->defaultptn=FALSE;
    int i=0,j=0,k,read_col=1;
    /* create the graph */
    while (1) {
        if ((k=adj[i++])==-1) { if ((++j)==n) break; read_col=1; }
        else if (read_col==1) { col[j]=k; read_col=0; }
        else if (j<k && isdir==0) { ADDONEEDGE(g,j,k,m); }
        else if (isdir!=0) { ADDONEARC(g,j,k,m); }
    }
    color_graph(n,lab,ptn,col);
    densenauty(g,lab,ptn,orbits,options,&stats,m,n,NULL);
    DYNFREE(lab,lab_sz);
    DYNFREE(ptn,ptn_sz);
    DYNFREE(col,col_sz);
    DYNFREE(orbits,orbits_sz);
    DYNFREE(g,g_sz);
}

void nautywrapper_canonical(int isdir,int n,int *adj,int *clab,unsigned long *cgrph,int *cols) {
    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,col,col_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    DYNALLSTAT(graph,g,g_sz);
    DYNALLSTAT(graph,cg,cg_sz);
    optionblk *options=isdir!=0?&opts_dir:&opts_undir;
    statsblk stats;
    int m=SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    DYNALLOC1(int,lab,lab_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,col,col_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
    DYNALLOC2(graph,cg,cg_sz,n,m,"malloc");
    DYNALLOC2(graph,g,g_sz,n,m,"malloc");
    EMPTYGRAPH(g,m,n);
    options->getcanon=TRUE;
    options->writeautoms=FALSE;
    options->outfile=NULL;
    options->defaultptn=FALSE;
    int i=0,j=0,k,read_col=1;
    /* create the graph */
    while (1) {
        if ((k=adj[i++])==-1) { if ((++j)==n) break; read_col=1; }
        else if (read_col==1) { col[j]=k; read_col=0; }
        else if (j<k && isdir==0) { ADDONEEDGE(g,j,k,m); }
        else if (isdir!=0) { ADDONEARC(g,j,k,m); }
    }
    color_graph(n,lab,ptn,col);
    densenauty(g,lab,ptn,orbits,options,&stats,m,n,cg);
    if (clab!=NULL) {
        for (i=0;i<n;++i) {
            clab[i]=lab[i];
        }
    }
    if (cgrph!=NULL) {
        size_t cnt=0;
        for (;cnt<m*(size_t)n;++cnt) {
            cgrph[cnt]=cg[cnt];
        }
    }
    if (cols!=NULL) {
        qsort((void*)col,(size_t)n,sizeof(int),int_less);
        for (i=0;i<n;++i) {
            cols[i]=col[i];
        }
    }
    DYNFREE(lab,lab_sz);
    DYNFREE(ptn,ptn_sz);
    DYNFREE(col,col_sz);
    DYNFREE(orbits,orbits_sz);
    DYNFREE(g,g_sz);
    DYNFREE(cg,cg_sz);
}
#else // HAVE_LIBNAUTY
#include <stdio.h>
int nautywrapper_is_isomorphic(int isdir,int n,int *adj1,int *adj2,int *sigma){
    return 0;
}
void nautywrapper_aut_generators(int isdir,int n,int *adj,FILE *f){}
void nautywrapper_canonical(int isdir,int n,int *adj,int *clab,unsigned long *cgrph,int *cols){
    *clab=*cgrph=*cols=16;
}
#endif // HAVE_LIBNAUTY
