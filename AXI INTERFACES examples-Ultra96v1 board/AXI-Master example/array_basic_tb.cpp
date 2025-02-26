#include <stdio.h>

void array_basic(volatile int *a, int length, int value);

int main()
{
  int i;
  int A[50];
  int B[50];
  int value = 7;
  int length = 50;

  printf("HLS AXI-Stream no side-channel data example\n");
  //Put data into A
  for(i=0; i < 50; i++){
    A[i] = i;
  }

  //Call the hardware function
  array_basic(A, length, value);

  //Run a software version of the hardware function to validate results
  for(i=0; i < length; i++){
    B[i] = i + value;
  }

  //Compare results
  for(i=0; i < 50; i++){
    if(B[i] != A[i]){
      printf("i = %d A = %d B= %d\n",i,A[i],B[i]);
      printf("ERROR HW and SW results mismatch\n");
      return 1;
    }
  }
  printf("Success HW and SW results match\n");
  return 0;
}