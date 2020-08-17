func multiplybyfour(var int n) int {
    print("integer: ");
    return n * 4;
}

func multiplybyfour(var float n) float {
    print("float: ");
    return n * 4.0;
}

func main() {
    var int a = multiplybyfour(5);
    println(a);
    var float b = multiplybyfour(5.0);
    println(b);
}