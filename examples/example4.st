func f(var int m, var int n) {
    return 4.0;
}

func factorial(var int n) int {
  if(n == 0 || n == 1) {
    return 1;
  } else {
    return n * factorial(f(5,5));
  }
}

func main() {
  var int i = 1;
  var int res = factorial(i);
}