#define SIZE 1024
int _start()
{
  int __attribute__((aligned(4096))) A[SIZE] = {0};
  int sum = 0;
  for (int i = 0; i < SIZE; i++)
    A[i] = 2;
  for (int i = 0; i < SIZE; i++)
    sum += A[i];
  return sum;
}