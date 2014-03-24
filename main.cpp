/* 
 * File:   main.cpp
 * Author: xy
 *
 * Created on March 11, 2014, 9:49 AM
 */

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <stdio.h>

#include "StarClassify.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

  char *fname = "abcd 123 234 34.7";
  int fline = 0;
  CrossMatchClassify *cmc = new CrossMatchClassify();
  //cmc->readStarFile(fname, fline);
  char a[10];
  int b;
  float c;
  sscanf(fname, "%s%*d%d%f", &a, &b, &c);
  cout<<a<<"**"<<endl;
  cout<<b<<endl;
  cout<<c<<endl;
  
  return 0;
}

bool isEmpty(char *str, int len) {

  if (NULL != str && '\0' != str[0]) {
    for (int i = 0; i < len && '\0' != str[i]; i++) {
      if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' &&
              str[i] != '\n' && str[i] != '\x0b') {
        return false;
      }
    }
  }
  return true;
}

bool hasNumber(char *str, int len) {

  if (NULL != str && '\0' != str[0]) {
    for (int i = 0; i < len && '\0' != str[i]; i++) {
      if (str[i] >= '0' && str[i] <= '9') {
        return true;
      }
    }
  }
  return false;
}

struct Star1 {
  int x;
  int y;
  float mag;
};

class Star2 {
public:
  int x;
  int y;
  float mag;
public:
  void distance(Star2 *star);
};

void isEmptyHasNumberTest() {

  char *a = "15 12.5";
  char *b = "a13.5 2.9";
  char *c = "  ";
  char *d = "  \t";
  char *e = "  \n";
  printf("%d", hasNumber(a, strlen(a)));
  printf("%d", hasNumber(b, strlen(b)));
  printf("%d", hasNumber(c, strlen(c)));
  printf("%d", hasNumber(d, strlen(d)));
  printf("%d", hasNumber(e, strlen(e)));
  printf("\n");
  printf("%d", isEmpty(a, strlen(a)));
  printf("%d", isEmpty(b, strlen(b)));
  printf("%d", isEmpty(c, strlen(c)));
  printf("%d", isEmpty(d, strlen(d)));
  printf("%d", isEmpty(e, strlen(e)));
  printf("\n");
  float af = 0, bf = 0;
  printf("%d\n", sscanf(a, "%f%f", &af, &bf));
  printf("%f\t%f\n", af, bf);
  printf("%d\n", sscanf(b, "%f %f", &af, &bf));
  printf("%f\t%f\n", af, bf);
  printf("%d\n", sscanf(c, "%f %f", &af, &bf));
  printf("%f\t%f\n", af, bf);
  printf("%d\n", sscanf(d, "%f %f", &af, &bf));
  printf("%f\t%f\n", af, bf);
  printf("%d\n", sscanf(e, "%f %f", &af, &bf));
  printf("%f\t%f\n", af, bf);
  printf("%d\n", EOF);
}