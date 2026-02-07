#include <stdio.h>
#include <unistd.h>

int main() {
  // Since there are 3 forks in total, there will be 2^3 = 8 processes created. 
  // 8 processes will print "Hello" 8 times. 
  fork();
  fork();
  fork();
  
  printf("Hello\n"); // All 8 processes execute this
    return 0;
}
