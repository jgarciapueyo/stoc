func f(var bool b) int {
    var int i = 0;

    while i < 10 {
        var int a = 10;
        return 1;
    }
    return 3;
}

func main() int {
    const int c = f(true);
    println(c);
    return 0;
}