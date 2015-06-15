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
 * 同一个文件中，包含同一个星的多条记录
 * 对该文件中的相同的星进行匹配，统计每个星记录的个数，按照由大到小排序
 * 对匹配个数为1，2，大于等于3的分别输出到文件
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
