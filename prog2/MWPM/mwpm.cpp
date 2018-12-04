/**
    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    It is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this file.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include <PerfectMatching.h>

/** Check whether rval is 0. If not, jump to CLEANUP.*/
#define COMBOcheck_val(rval,msg) {                                        \
    if ((rval)) {                                                          \
       fflush(stdout);                                                     \
       fprintf (stderr, "%s at %s, line %d\n", (msg),__FILE__,__LINE__);   \
       goto CLEANUP;                                                       \
    }                                                                      \
}

#define COMBOcheck_NULL(item,msg) {                                        \
    if ((!item)) {                                                         \
       fflush(stdout);                                                     \
       fprintf (stderr, "%s at %s, line %d\n", (msg),__FILE__,__LINE__);   \
       rval = 1;                                                           \
       goto CLEANUP;                                                       \
    }                                                                      \
}


/** Read a graph instance from a file.
    f       : filename containing the problem graph
    pncount : number of vertices in the problem graph (will be filled)
              the vertices are numbered by 0,...,pncount-1 (different from the DIMACS-format!)
    pecount : number of edges in the problem graph (will be filled)
              the edges are numbered by 0,...,pecount-1
    pelist  : edge-list, consisting of an array of length 2*pecount containing
              edges as consecutive node-pairs, 
              i.e. edge i is connecting pelist[2*i] and pelist[2*i + 1]
    peweights: weights of the vertices, consisting of an array of length pecount.
*/
int read_dimacs (char *f, int *pncount, int *pecount, int **pelist, int **peweights)
{
  int rval      = 0;
  int haveprob  = 0;
  int icount    = 0;
  
  int  ncount   = 0;
  int  ecount   = 0;
   int* elist   = (int*) NULL;
  int* eweights = (int*) NULL;
  
  int i, end0, end1, weight;

  int n;
  char buf[256];
  char* p;

  FILE* in = (FILE*) NULL;

  *pncount = *pecount = 0;
  if (*pelist)    free(*pelist);
  if (*peweights) free(*peweights);
  
  in = fopen (f, "r");
  if (!in) {
    fprintf (stderr, "Unable to open %s for input\n", f);
    rval = 1;  goto CLEANUP;
  }

  while (fgets (buf, 254, in) != (char *) NULL) {
    p = buf;
    if (p[0] == 'p') {
      const char* delim = " \t\n";
      char* data = (char *) NULL;
      strtok(p,delim); /* get 'p' */
      
      if (haveprob) {
	fprintf (stderr, "ERROR in Dimacs file -- two p-lines.\n");
	rval =1; goto CLEANUP;
      }	  
      haveprob = 1;
      data = strtok(NULL,delim); /* get type */
      if ( strcmp(data,"edge") && strcmp(data,"edges") &&
	   strcmp(data,"col")  && strcmp(data,"graph")) {
	fprintf (stderr, "ERROR in Dimacs file -- not an edge file\n");
	rval = 1;  goto CLEANUP;
      }
      data = strtok(NULL,delim);
      sscanf (data, "%d", &ncount);
      data = strtok(NULL,delim);
      sscanf (data, "%d", &ecount);
      
      printf ("Number of Nodes: %d\n", ncount);
      printf ("Number of Edges: %d\n", ecount);
      
      elist = (int*) malloc (2 * ecount * sizeof(int));
      COMBOcheck_NULL (elist, "out of memory for elist");
      eweights = (int*)  malloc (ecount * sizeof(int));
      COMBOcheck_NULL (eweights, "out of memory for eweights");
      for (i = 0; i < ecount; i++) eweights[i] = 1;
    } else if (p[0] == 'e') {
      if (!haveprob) {
	fprintf (stderr, "ERROR in Dimacs file -- no problem defined \n");
	rval = 1;  goto CLEANUP;
      }
      if (icount >= ecount) {
	fprintf (stderr, "ERROR in Dimacs file -- to many edges\n");
	rval = 1;  goto CLEANUP;
      }
      p++;
      sscanf (p, "%d %d %d", &end0, &end1, &weight);
      elist[2*icount] = end0-1;    /* Number nodes from 0, not 1 */
      elist[2*icount+1] = end1-1;
      eweights[icount]  = weight;

      icount++;

      
      if (!haveprob) {
	fprintf (stderr, "ERROR in Dimacs file -- n before p\n");
	rval = 1;  goto CLEANUP;
      }
    }
  }
   
  /* Some dimacs col-instances are buggy => reduce # edges to icount*/
  *pncount = ncount;
  *pecount = icount; 
  *pelist  = elist;
  *peweights = eweights;
  
CLEANUP:
  if (rval) {
    if (elist)    free (elist);
    if (eweights) free (eweights);
  }
   
  if (in) fclose (in);

  return rval;
}


int build_minimum_weight_perfect_matching (PerfectMatching **pm,
					   int ncount, int ecount, int *elist, int *eweights)
{
  int rval = 0;
  int i;
  *pm = new PerfectMatching(ncount, ecount);
  COMBOcheck_NULL(pm, "failed to allocate pm");
  
  for (i = 0; i < ecount; ++i) {
     PerfectMatching::EdgeId new_edge = (*pm)->AddEdge(elist[2*i], elist[2*i+1], eweights[i]);
  }

 CLEANUP:
  return rval;
}


int main (int ac, char **av)
{
  char* dimacs_fname = (char*) NULL;
  int   rval = 0;
  int   ncount, ecount,i;
  int*  elist    = (int*) NULL;
  int*  eweights = (int*) NULL;
  int   total_weight = 0;
  PerfectMatching *pm = (PerfectMatching*) NULL;
  double*  x  = (double*) NULL;
    
  if (ac <2) {
    printf("Usage mvc <filename>\n");
    rval = 1; goto CLEANUP;
  }
  dimacs_fname = av[1];

  rval = read_dimacs (dimacs_fname, &ncount, &ecount, &elist, &eweights);
  COMBOcheck_val (rval, "read_dimacs failed");
  
  rval = build_minimum_weight_perfect_matching (&pm,  ncount,  ecount, elist, eweights);
  COMBOcheck_val (rval, "build_lp failed");

  pm->Solve();

  printf ("c printing solution:\n");
  for (i = 0; i < ecount; ++i) {
     int is_matched = pm->GetSolution(i);
     if (is_matched) {
	int dimacs_edge_id = i+1;
	printf ("e %d\n", dimacs_edge_id,is_matched);
	total_weight += eweights[i];
     }
  }
  printf ("c total weight %d\n", total_weight);
  
 CLEANUP: 
  if (elist)    free (elist);
  if (eweights) free (eweights);
  if (x)        free (x);
  delete pm;
  
  return rval;
}
