# Function Calls

## LLVM Function signiture

For llvm every function will implicitly get a pointer as it's first argument. This pointer points to a pointer that will be allocated and set to a nullptr by the caller before executing the call. The callee then either puts a pointer to the corresponding error in the OET, if there was an error that needs to be returned. This way we can just check if the pointer still points to a nullptr or if errorhandling needs to be done, after the callee has returned. In case of an error, the real return value of the callee is ub and shall not be used for any calculations.

