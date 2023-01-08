# MU0
MU0 CPU emulator/interpreter

I am learning C so I decided to create this interpreter for the MU0 cpu. From information I read somewhere on the internet - the MU0 CPU is a virtual cpu designed in the Manchester University. It has only 1 register acc, the accumulator, and 8 instructions. Ive also implemented a special instruction `PUT`, it just prints out the value of the accumulator.

I am learning C so I decided to create this interpreter for the MU0 cpu. From information I read somewhere on the internet - the MU0 CPU is a virtual cpu designed in the Manchester University. It has 1 register acc (accumulator, eax on x86) and 8 instructions. Ive also implemented a special instruction `PUT` it just prints out the value of the accumulator.
I've also made it so that the memory and acc stores 32bit signed integers. Maybe a weird choice but I don't care.

### Usage:
MU0 filename options

### Options:
-clock=some-number
-memsize=some-number


There is a problem in the code, but it is only gonna be triggered if you do `-memsize=2147483648` or a number greater than that
