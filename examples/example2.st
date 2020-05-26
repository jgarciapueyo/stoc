func gcd(var int n, var int m) int {
  if m == 0 {
    return n;
  }
  else {
    // Use n - m because there is no modulo operator
    return gcd(m, n - m);
  }
}

func main() {
  var int GCD = gcd(10, 12);
}