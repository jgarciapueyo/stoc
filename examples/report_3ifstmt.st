func main() {
    var int temp = 12;
    if temp > 30 {
        println("It's hot");
        if temp > 40 {
            println("It's very very hot");
        }
    } else if temp >= 15 {
        println("It's warm");
    } else {
        println("It's cold");
    }
}