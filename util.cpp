
#include <string.h>
#include <stdio.h>

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