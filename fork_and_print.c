int main() {
  // Since there are 3 forks in total, there will be 2^3 = 8 processes created. 
  // 8 processes will print "Hello" 8 times. 
  fork();
  fork();
  fork();
  putc("Hello\n");
}
