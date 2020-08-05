func f() int {
    println("func f()");
    return 2;
}

func f(var int i) int {
    println("func f(var int i)");
}

func main() {
    for var int i = 0; i < 10; i = i + 1 {
        println(i);
    }

    var int a = f(6);
    // it does not show error
    print(a);
}