# Object Error Table

## Idea

The object error tabel (OET from now on) is a compiled lookup table for all errors that are defined in a source. During compilation the compiler will create one OET per source. The OET is a direct mapping of a 32bit hash generated from the error signiture to the error signiture. Every interaction from the source with an error will result in a lookup of the OET. To ensure that multipel OETs can be linked together, the file name of the OET will be prefixed by a 64bit hash of the source directories structure parsed into a string > "hash_oet.ll".
