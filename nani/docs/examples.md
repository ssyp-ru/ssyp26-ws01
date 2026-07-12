# Nani — Examples

## Comments

```
// This is a comment.
print 1 + 1; // This is a trailing comment.
```

## Values and arithmetic

```
print nil;
print true;
print false;
print "hello, Nani";

print 1 + 2;
print 10 - 4;
print 2 * 3.5;
print 7 / 2;
print -5;
print -(3 + 4);
```

## Operator precedence and grouping

```
print 1 + 2 * 3;        // 7
print (1 + 2) * 3;      // 9
print 2 * 3 + 4 * 5;    // 26
print !(1 + 1 == 3);    // true

print true or false and false; // true
print (true or false) and false; // false
```

## Assertions

```
assert true;
assert 1 < 2;
assert (1 + 1) == 2;
assert "lox" == "lox";
```

These statements cause runtime errors:

```
assert false;
assert 1;       // assert requires a boolean true value
assert nil;     // assert requires a boolean true value
```

## Variables and blocks

```
let greeting = "hello";
let answer = 40 + 2;

print greeting;
print answer;

{
  let answer = 100;
  print answer; // 100
}

print answer; // 42
```

## Conditions and loops

```
let temperature = 23;

if (temperature < 0) {
  print "freezing";
} else if (temperature < 25) {
  print "cool";
} else {
  print "warm";
}

let n = 3;
while (n > 0) {
  print n;
  n = n - 1;
}

for (let i = 0; i < 3; i = i + 1) {
  print i;
}
```

## Functions

```
fn add(left, right) {
  return left + right;
}

fn greet(name) {
  print "Hello, " + name;
}

print add(20, 22);
greet("Ada");
```

```
let prefix = "item: ";

fn showItem(number) {
  print prefix + number;
}

showItem("7");
```

## Structs, fields, and methods

```
struct Point {
  init(x, y) {
    this.x = x;
    this.y = y;
  }

  move(dx, dy) {
    this.x = this.x + dx;
    this.y = this.y + dy;
  }

  lengthSquared() {
    return this.x * this.x + this.y * this.y;
  }
}

let point = Point(2, 3);
point.move(1, -1);
print point.x;               // 3
print point.lengthSquared(); // 13
```

Fields may also be added after an instance is created:

```
struct Box {}

let box = Box();
box.value = "stored value";
print box.value;
```
