# Typesystem

The YALLL programming language features all base types a modern programmer needs as well as custom types i.e. classes and interfaces.

## Type Prefixes

When using a type it can be prefixed with the mutable (`!`) and errable (`?`) prefix. These prefixes are commutative, i.e. it doesn't matter in which order they are added to the type. They also can only be added once per type. This is to avoid unnecessary complexity that would be added, if !?i32 != ?!i32. Type prefixes can be thought of as flags that will be enabled, if used.

### Why the Simple Route?

There is definitely something to gain by having a difference between different orders of prefixes. !?i32 could mean the erralbe i32 can be mutated into another errable i32 and ?!32 could mean the mutable i32 can also be an error. But then it would only make sense to also allow longer chains of those prefixes like !?!i32. The !?!i32 would express that this value can be mutated into another errable i32 that is in turn can also be mutated into another i32. Having a system so unnecessary complex that won't be beneficial to most use cases is pure insanity and will lead to a bunch of unreadable code when it could be of use. YALLL is supposed to be easy to get and having !?!?!i32 or similar misses the point.
