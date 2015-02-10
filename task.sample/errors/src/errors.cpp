#include <cstdio>

int x;

int main(int argc, char ** argv)
{
  int a[10] = {0};
  // int b[5] = {0};
  int * c = new int[10];

  printf("a[10] = %d\n", a[10]);
  printf("c[5] = %d\n", c[5]);
  printf("x = %d\n", x);
  // printf("c[10] = %d\n", c[10]);

  return 0;
}
