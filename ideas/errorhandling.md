# YALLL Errorhandling

The YALLL programming language features no exceptions, but errors as values instead. Every error should be handled as soon as possible. If an error accures that can't be handled in any way or form, an application can panic as a last resort.

## The Errable Value

If a function can return both an error and an expected value, there needs to be a type that can hold both possilbities but only one at a time. YALLL solves this using the errable (`?`) type prefix. If a variable is declared using an errable type it is called an errable value and can be assigned to either the declared type or any error. 

## Error Look Up Table (ELUT)

The **ELUT** is a globally stack allocated look up table for all errors that are defined in the source code. Every error that is used during program execution is a pointer to the **ELUT**. 

## Functions

By default every function is treated as errable.

### The `noerr` Keyword

Returning an error is an opt-out functionality for every function. A function can opt-out of returning errors by using the `noerr` keyword. In this case it garentuees to the caller that it's impossilbe to reach an error during the function. It's considered best practic to have every that won't return errors marked as such (at some point the compiler will enforce this), because the compiler can reduce overhead, if errorhandling can be omitted.
If a function is declared as `noerr` it can be called like one would expect in any other language.

```
func noerr foo() : i32 {
    return 1;
}

i32 bar = foo();
```

***Note*** Eventhough it will be possible to return an errable type with a noerr function at first, this will definitly change in the future.

### Implementation of Functions

### Errable

If a function is errable, it's compiled return type will always be a pointer to the **ELUT** with a nullptr meaning that there was no error. The actual return value is a pointer that is implicitly added as the first parameter to the function signature. The caller is expected to allocate the needed space for the return value on the stack and implicitly pass the pointer to said space as the first argument when calling the callee.

### Non-Errable

If a function is declared using the `noerr` keyword, no changes to the function signature or the way a function is called will be made.


