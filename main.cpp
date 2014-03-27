/* 
 * File:   main.cpp
 * Author: xy
 *
 * Created on March 11, 2014, 9:49 AM
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "StarClassify.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
  
  if(argc<4){
    printf("Usage: StarClassify infine outfine errorbox!\n");
    return 1;
  }
  
  char *infile = argv[1];
  char *outfile = argv[2];
  float errorBox = atof(argv[3]);

  CrossMatchClassify *cmc = new CrossMatchClassify();
  cmc->match(infile, outfile, errorBox);
  
  return 0;
}
