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

## Objects (maps)

`object()` creates an empty map. Bracket access evaluates its key expression,
so keys are values rather than fixed field names. A missing key evaluates to
`nil`; `nil` itself cannot be used as a key.

```
let a = object();

a["x"] = 10;
a[42] = "number key";
a[true] = "boolean key";

print a["x"]; // 10
print a[42];  // number key
print a["missing"]; // nil
```

Keys may be computed at runtime, and maps are reference values:

```
let a = object();
let key = "answer";
a[key] = 42;

let b = a;
b["shared"] = true;

assert a["answer"] == 42;
assert a["shared"] == true;
```
