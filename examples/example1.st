func factorial(var int n) int {
  if(n == 0 || n == 1) {
    return 1;
  } else {
    return n * factorial(n-1);
  }
}

func main() {
  var int i = 10;
  var int res = factorial(i);
  println("Result is: ");
  println(res);
}