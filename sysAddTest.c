#include <stdio>

int add(int a, int b, int* val_p);

int main(){
  int a = 1;
  int b = 2;
  int sum=0;
  int * store_result=&sum;
  printf( "You have entered %d and %d.\n",a,b);
  printf("%d\n",*store_result );
  add(a,b,store_result);
  printf("%d\n",*store_result );
  sum = *store_result;
  printf("%d\n",sum );
  return 0;
}

int add(int a, int b, int* val_p){
  int val;
  val = a+b;
  *val_p = val;
  return 0;
}
