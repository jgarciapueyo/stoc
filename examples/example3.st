func sub1(var int m) int {
    return m-1;
}

func foo(var int n) int {
  if(n == 0 || n == 1) {
    return 1;
  } else {
    return sub1(n+1) * foo(sub1(n));
  }
}

func main() {
  var int i = 5;
  var int res = foo(i);
  println(res);
}