func main() {
    const bool a = true && true;
    println(a);
    var bool b = false || a;
    println(b);
    var int integer = 5 + 5 * (7 - 4) / 3;
    println(integer);
    var float f = 6.0 * 7.0 / 3.5;
    println(f);
    var float g = 9.0 - 4.6;
    println(g);
    // var int casting = integer + f;   // error!
    const string characters = "string";
    println(characters);
    const string characters2 = "string2";
    println(characters2);
    var bool isEqual = f == g;
    println(isEqual);
}