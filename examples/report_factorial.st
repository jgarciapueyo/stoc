func factorial(var int n) int {
    var int res = 1;
    while n > 0 {
        res = res * n;
        n = n - 1;
    }
    return res;
}

func main() {
    const int c = 10;
    print("Factorial of number ");
    print(c);
    print(" is ");
    println(factorial(c));
}