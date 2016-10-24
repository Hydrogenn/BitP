##BitP v0.4.0

This is an interpreter for a new language, BitP (/bɪd-ɪp/ I think), written in C++.
It is designed to resemble (and be practical as) an assembly language, but compacted into 16 commands so that a human can learn it more easily.
It ended up being more of an esolang...

 - The program reads `script.bp`.
 - This has only been tested on a windows machine.
 - This is not the final version, and some major changes might be in future releases.
 - At release 1.0, everything in the language is finalized.

The C++ code should be legible enough to understand what's going on, but I'll try to give one in English anyway. It's below.

BitP is structured to contain only two values: a stack of 8 values (disc), and the script itself.
Other values exist, but are indicitive of state more than values, such as `reading` which turns off when an IF-THROW fails, causing code to be skipped.

The disc will loop on itself, hence the name 'disc'. Going to the 'next' value in the disc when already at the last will send you back to the first.
'Disc value' is the value in said disc that is being looked at and edited.

The script itself can be edited while running. It is recommended to store long-term values in the script, and use the disc for operations only.
There is nothing *requiring* this, but I personally find it:
 - Easier to read
 - More stable, as all values can be implied as zero with a clean disc


The script you enter is not the script that is interpreted. Future versions will have the compiler and bit interpreter seperate, but right now they are combined.
Each command is compiled into a binary value. That's why there's a long list of bits that comes up when you run a program.
If the value isn't a valid command (or a hexadecimal character) it is ignored by the compiler.


Input and Output deals with ASCII characters.
The debug displays hex values.


##Commands

BIN | HEX | Char | Name | Function
------ | --- | --- | ------------- | ----------------------------------------------------
`0000` | `0` | `$` | VALUE | Instead of reading the next command as a command, take its hexadecimal code and tack<sup>1</sup> that value to the disc value.
`0001` | `1` | `+` | SET | The last bit of the disc value becomes a 1.
`0010` | `2` | `<` | LEFT-SHIFT | Does a single left shift on the disc value.
`0011` | `3` | `>` | RIGHT-SHIFT | Does a single right shift on the disc value.
`0100` | `4` | `[` | IF-THROW | If the disc value is zero, skip all commands until a `]` IF-CATCH is reached. It has special case handling to exclude values placed after a `$` VALUE.
`0101` | `5` | `]` | IF-CATCH | Continues reading commands normally after being skipped by an `[` IF-THROW command.
`0110` | `6` | `@` | GOTO | Jumps the pointer in the script to the location at the disc value. It also clears all values on the disc.
`0111` | `7` | `~` | COMPLEMENT | Set the disc value to the complement of itself.
`1000` | `8` | `,` | PUSH | Look at the next disc value.
`1001` | `9` | `.` | COMMIT | Used in conjunction with `-` SELECT. It overwrites the range remembered from SELECT with the disc value.
`1010` | `A` | `-` | SELECT | This will look at the script and take a range of bits; the range's first value is the first operand, and the range's length<sup>2</sup> is the second. It remembers the value for later use.
`1011` | `B` | `_` | READ | Functions identically to SELECT, but does not remember the value for use with `.` COMMIT.
`1100` | `C` | `&` | AND | Performs a bitwise AND with the first and second operands.
`1101` | `D` | `/`<sup>3</sup> | OR | Performs a bitwise OR with the first and second operands.
`1110` | `E` | `^` | XOR | Performs a bitwise EXCLUSIVE OR with the first and second operands.
`1111` | `F` | `%` | EXTERN | Handles input and output magically<sup>4</sup>.
<sup>1</sup>'Tack' is the best way I can describe what it does. Examples:
 - `$8` on disc value `0000` becomes `1000` (`8` in hex and dec)
 - `$8` on disc value `0001` becomes `0001 1000` (`18` in hex, 24 in dec)
 - `$8$F` will be tack `8` then `F`, becoming `1000 1111`, or `8F` in hex. Notice something?

`A` to `E` are OPERATIONS, and all follow some common behavior.
 - The previous disc value is used as the first operand.
 - The current disc value is used as the second operand.
 - The operation's result is then stored on the first operand.
 - The second operand is discarded and set as zero.
 - Last, the pointer readjusts to look at the result, or shifted back by 1.

 TL;DR operations are in [postfix notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) fill in the extra space with zero.

<sup>2</sup>This really means how many *more* bits are being looked at, so if the second operand is `11` then 4 bits will be taken, and if it is `0` then 1 bit will be taken.
Values above `111111`, which handles 64 bits, will have all but the last 6 bits ignored.

<sup>3</sup>Despite `|` being a conventional OR operation, `/` makes more sense and does not have to compete with division.

<sup>4</sup>No really, if you intend to make an alternative compiler I don't care what you do with this thing.

In *this* compiler, a non-null value will display that ASCII character and leave the disc value unchanged. A null value, however, takes an input string. It sets the disc value to the first character's ASCII value.
If input is called later, then it will look at the *next* character in the input string. A null value is appended to all input strings. If all values are exhausted, another input string is requested.

Which is kind of cheating, but character input is kind of underwhelming.

End