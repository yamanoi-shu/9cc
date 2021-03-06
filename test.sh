#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but go $actual"
    exit 1
  fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 '5+20-4;'
assert 41 ' 12 + 34 - 5;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 1 '1==1;'
assert 0 '1==2;'
assert 1 '1!=2;'
assert 0 '1!=1;'
assert 1 '1<=2;'
assert 0 '2<=1;'
assert 1 '1<=1;'
assert 1 '1<2;'
assert 0 '2<1;'
assert 0 '1<1;'
assert 1 '2>=1;'
assert 0 '1>=2;'
assert 1 '1>=1;'
assert 1 '2>1;'
assert 0 '1>2;'
assert 0 '1>1;'
assert 1 'a=1; a;'
assert 4 'a=1; a+3;'
assert 1 'abc=1; abc;'
assert 4 'abc=1; abc+3;'
assert 1 'return 1;'
assert 2 'return 2; return 1;'
assert 3 'a=2; return a+1;'
assert 2 'x = 1; if (x == 1) x = 2; return x;'
assert 1 'x = 1; if (x != 1) x = 2; return x;'
assert 3 'x = 1; if (x != 1) x = 2; else x = 3; return x;'
assert 5 'x = 1; y = 1; if (x == 1) {x = 2; y = 3;} return x+y;'
assert 2 'x = 1; y = 1; if (x != 1) {x = 2; y = 2;} return x+y;'
assert 5 'x = 1; while (x != 5) x = x + 1; return x;'
assert 6 'x = 1; for (i = 1; i < 6; i = i+1) x = x + 1; return x;'

echo OK
