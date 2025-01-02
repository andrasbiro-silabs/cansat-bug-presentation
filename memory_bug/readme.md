# Memory corruption

Note that everything "works". If a variable is put _before_ 
`array[20]`, it will have the data.

This however depends on CPU architecture and the linker.
