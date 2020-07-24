var int global1 = 5;
var int global2 = 10 * global1;

func foo() int {
    var int local1 = global1;
    var int local2 = local1 + 2;
    var int local3 = local2 * 5 + 1;
    return local3;
}

func main() int {
    return 0;
}