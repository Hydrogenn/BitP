BitP v0.1.0

This is an interpreter for a new language, BitP, written in C++.
It is designed to resemble (and be practical as) an assembly language, but compacted into 16 commands so that a human can understand it more easily.

The C++ code should be legible enough to understand what's going on, but I'll try to give one in English anyway. It's below.


BitP is structured to contain only two values: an array of 8 values (disc), and the script itself.
Other values exist, but are indicitive of state more than values, such as whether to ignore the code being read because of, say, an IF loop.

The disc will loop on itself, hence the name 'disc'. Going to the 'next' value in the disc when already at the last will send you back to the first.
'Disc value' is the value in said disc that is being looked at and edited. 'last disc value' and 'next disc value' means the ones 'before' and 'after'.

The script itself can be edited while running. It is recommended to store long-term values in the script, and use the disc for operations only. There is nothing *requiring* this, if you want to really want a, uh, challenge.


The script you enter is not the script that is interpreted. Future versions will have the compiler and bit interpreter seperate, but right now they are combined.
Each command is compiled into a binary value. If the value isn't a valid command (or a hexadecimal character) it is ignored.


The debug, inputs and outputs all involve decimal numbers. That is an accidental (albeit useful) abstraction.
Future versions will involve outputting debug data in hex, and actual values as characters (without the new line).


Commands are below.

BIN | HEX | CHAR | NAME | FUNCTION
------ | --- | --- | ------------- | ----------------------------------------------------
0000 | 0 | $ | VALUE | Instead of reading the next command as a command, take its hexadecimal code and tack that value to the disc value. 1
0001 | 1 | + | SET | The last bit of the disc value becomes a 1.
0010 | 2 | < | LEFT-SHIFT | Does a single left shift on the disc value.
0011 | 3 | > | RIGHT-SHIFT | Does a single right shift on the disc value.
0100 | 4 | ; | LINE-BREAK | Clears all values on the disc.
0101 | 5 | : | IF | If the value is zero, skip all commands until a ';' is reached. Otherwise, clear all values on the disc.
0110 | 6 | @ | GOTO | Jumps the pointer in the script to the location at the disc value. It also clears all values on the disc.
0111 | 7 | # | DESYNC | Nothing, right now. It's intended to do some multithreading stuff, but it doesn't right now.
1000 | 8 | , | PUSH | Look at the next disc value.
1001 | 9 | . | COMMIT | Used in conjunction with '-' SELECT.
1010 | A | - | SELECT | This will look at the script and take a range of bits; the range's first value is the first operand, and the range's length is the second (plus one). 2,3
1011 | B | _ | READ | Functions identically to SELECT, but does not remember the value for use with '.' COMMIT.
1100 | C | & | AND | Performs a bitwise AND with the first and second operands.
1101 | D | / | OR | Performs a bitwise OR with the first and second operands. 4
1110 | E | ^ | XOR | Performs a bitwise EXCLUSIVE OR with the first and second operands.
1111 | F | % | EXTERN | Handles input and output magically 5
1: 'Tack' is the best way I can describe what it does. An example would be:

$8 on disc value 0 becomes 0100 (8 in hex or dec)
but $8 on disc value 0001 becomes 0001 1000 (18 in hex, 24 in dec)

2: A to E are OPERATIONS, and all follow some common behavior.
 - The last disc value is used as the first operand.
 - The current disc value is used as the second operand.
 - The operation's result is then stored on the first operand.
 - The second operand is discarded and set as zero.
 - Last, the pointer readjusts to look at the result, or shifted back by 1.
 
3: Ranges above 111111, which handles 64 bits, will have all but the last 6 ignored.

4: Despite '|' being a conventional OR operation, '/' makes more sense and does not have to compete with division.

5: No really, if you intend to make an alternative compiler I don't care what you do with this thing.

The last bits indicate where the value is being output to, or what input to get a value from. That's the rule. No more, no less.
In THIS compiler, values ending in a '1' will take an input number, whereas values ending in a '0' will output the number.

Actually, please don't make alternative compilers (yet). The language isn't complete, and even minor changes will break each and every script that came before it.