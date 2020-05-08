func factorial(int n) {
  if(n == 0 || n == 1) {
    return 1;
  } else {
    return n * factorial(n-1);
  }
}

func main() {
  var int i = 1;
  var int res = factorial(i);
}