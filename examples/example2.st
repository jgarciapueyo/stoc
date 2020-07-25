func modulo(var int a, var int b) int {
    while a >= b {
        a = a - b;
    }
    return a;
}

func gcd(var int n, var int m) int {
  if m == 0 {
    return n;
  }
  else {
    // Use own modulo function
    var int mod = modulo(n, m);
    return gcd(m, mod);
  }
}

func main() int {
  var int GCD = gcd(12, 10);
  return 0;
}