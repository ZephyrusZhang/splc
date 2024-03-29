# splc

## Features

### Data Types

- [x] `int`: decimal or hexadecimal, signed

- [x] `float`: only dot form
- [x] `char`: normal character or hex-form character
- [x] `struct`
- [x] pointer: `int *`  ~~`float *``char *`~~. `&` for addressing, `*` for address resolution

### Control Flow

- [x] `if`, ~~`else if`~~, `else`

- [x] `while`

- [x] `for`
- [x] `break`, `continue`

### Comment

- [x] single line comment: `//`
- [x] multiple line comment: `/* */`

### Pre-process

- [x] `#include`

### Error recognizing

- [x] Lexical error (error type A) when there are undefined characters or tokens in the SPL program, or identifiers starting with digits.

- [x] Syntax error (error type B) when the program has an illegal structure, such as missing closing symbol. Please find as many syntax errors as possible.



## Semantic Analysis

### Error Type

- [x] **Type 1** a variable is used without a definition
- [x] **Type 2** a function is invoked without a definition
- [x] **Type 3** a variable is redefined in the same scope
- [x] **Type 4** a function is redefined (in the global scope, since we don’t have nested functions)
- [x] **Type 5** unmatching types appear at both sides of the assignment operator (=)
- [x] **Type 6** rvalue appears on the left-hand side of the assignment operator
- [x] **Type 7** unmatching operands, such as adding an integer to a structure variable
- [x] **Type 8** a function’s return value type mismatches the declared type
- [x] **Type 9** a function’s arguments mismatch the declared parameters (either types or numbers, or both)
- [x] **Type 10** applying indexing operator ([...]) on non-array type variables
- [x] **Type 11** applying function invocation operator (foo(...)) on non-function names
- [x] **Type 12** array indexing with a non-integer type expression
- [x] **Type 13** accessing members of a non-structure variable (i.e., misuse the dot operator)
- [x] **Type 14** accessing an undefined structure member
- [x] **Type 15** redefine the same structure type



- [x] break or continue statement not within loop
- [x] dereference of a non-pointer
- [x] 32-bit signed integer overflow
- [x] hex char literal overflow
- [x] non-boolean expression at the conditional statement

## Other Feature

- [x] forced type conversion
- [x] return type guess of an undefined function
