Spotless
========

Spotless is a simple JSON validator written in C.

Building
--------

Only *gcc* and *make* are required to compile Spotless.

```
$ make
```

Running
-------

The spotless binary reads from stdin; its exit code is 0 if the
string was well-formed JSON, and 1 otherwise.

```
$ echo '{ "c": 1972, "java": 1995}' | ./spotless; echo $?
0

$ echo 'invalid' | ./spotless; echo $?
spotless: unknown character
1

```

Testing
-------

Testing requires the [bats](https://github.com/sstephenson/bats) testing framework.

```
$ chmod +x test.bats
$ ./test.bats
```

Caveats
-------

- Spotless does not currently ensure that strings are well-formed UTF-8 sequences.
