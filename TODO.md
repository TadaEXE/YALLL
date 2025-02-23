# TODO

_This list is not complete_

- Buildsystem:
The current "Buildsystem" (if you can even call it one) is kinda cringe.
I want it to be a bit more modular and not just one fucking file glob_recurseing through everything

- The to_operation debarcle:
I need to find a better way (that is not writing my own lexer/parser) for dynamically generating a `to_operation` function

- Tuples:
I want tuples as a native datatype and automatic tuple unpacking for function returns

- Tests:
I don't have any tests at the moment which sucks HARD. It's just easier to run the whole goddamn compiler and look at the logs to "test" instead of writing actual tests. I need some easy to use test suite that I can use to write unittest and compile tests for small test programs.
