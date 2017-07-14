#BitP v0.7.0

This is an interpreter for a programming language, BitP (/bɪd-ɪp/), written in C++.
It was designed to resemble (and be practical as) an assembly language, but compacted into 16 commands so that a human can learn it more easily.
It ended up being more of an esolang...

 - The program (bitp\_7.exe) reads any file you give it. You must include the file extension.
 - A debug version has been included (bitp\_7\_debug.exe) which will print out information at every code point.
 - The .dll files are required to run both versions.
 - This has only been tested on a windows machine.
 - bitp.cpp contains the source code. If the .exe files are not quite working, compile this source for your machine.
 - At release 1.0, everything in the language is finalized. Everything after that point will not affect how .bp files are read.
   - Very few programs will break between now and v1.0.

The C++ code should be legible enough to understand what's going on, but I'll try to give one in English by explaining each command.

__`#` VALUE:__ All commands in this language have a hexadecimal value, from 0-F.
This language would completely break down if there were more or less commands than 16. There was an attempt to cheat back in v0.6.0, but that has been reverted. (Because it wasn't as fun!)

So, what VALUE does is say, "Ignore that next command. We want it's value, not its function."

The value goes on something called a "disc". This contains up to 8 64-bit values. At any given time, exactly one of these values is being pointed at.

If there is already a value there, then VALUE doesn't get rid of it entirely. It instead shifts it left 4 times, and then puts the specified value there.
This seems wierd at first, but consider the following code:

`#{#@##`

If it were to replace the value entirely, you would have `0` as the disc value (the value for #). That would mean only the first 4 bits can be changed. That'd waste 60 bits!
However, with the new process, it sets the value at the disc to `3A0`. That's more like it!

Also, you do not have to type the command every time. The compiler recognizes `0` and `#` as the same thing. So that above gibberish can be translated into:

`#3#A#0`

And it would still set the disc value to `3A0`. Nice! But we've only covered one command.

__`,` NEXT:__ Looks at the next value on the disc. It can only go one way (normally), and that is forward, by using this operator. So, the following:

`#3#A#0,#A#C`

Puts the entire disc at something like `3A0, [AC], 0, 0...`. The disc value is `AC`, marked with the brackets.

Worth noting is that after the 8th value, the disc will loop back around at 1, and _not_ throw an error.

__`~` NOT__ (We'll get to `{` and `}`, don't worry!): Flip every bit in the disc value. So, binary 1 becomes a 0, and every 0 a 1. ez pz.

`#3#A#0,#A#C~` ends with the disc at `3A0, [FFFFFFFFFFFFFF53], 0...`.

__`&` AND:__ If you do not know what a __[bitwise AND](https://en.wikipedia.org/wiki/Bitwise_operation)__ is, you're going to have a bad time. If you do, this command does just that.

Like a lot of other commands, this is an _OPERATION_. That means it takes the _previous_ disc value as the operator, the _current_ disc value as the operand,
puts the result in the _previous_ disc value, clears the current disc value, and points back one step. Which is a lot. Here's an example:

`#3#A#0,#A#C&` ends with the disc at `[A0]...`. It's basically just a [postfix](https://en.wikipedia.org/wiki/Reverse_Polish_notation) operation.

That covers XOR, OR, LSHIFT, and RSHIFT as well; they are all described [here.](https://en.wikipedia.org/wiki/Bitwise_operation)

<details>
	</summary>Knowing this: What does `#3#A,#A#C,#F&^` return?</summary>
	`3A ^ (AC & F)` = `3A ^ F` = `36`. Read up on the above if you got the wrong answer but already know what those operators do.
</details>

The disc defaults all values to 0, so `,#3#A#2&`, `,#F#F#F&` and `,~&` all would have a disc value of 0.
The initial comma isn't neccessary either, since operations too will look around the disc, to the 8th value -- which also defaults to 0.

__`[` INPUT:__ Takes a single character of input.
The user actually inputs a string, but this function will get that input character by character, only asking for more input when the string is exhausted.
Every string is appended with a `/0 NUL` character, so the program can tell when user input has ended.

__`]` OUTPUT:__ Spits out the current disc value as output.
Note: Even though input and output have matching brackets, they do not _have_ to match.

__`%` PORT:__ As of now, it's a NOP. Just don't use it. It'll get replaced or added sometime.

__`@` IF-GOTO:__ Jump to a different part in the program if a condition is met.
Note that this is still technically an operation; the previous value is whether or not to jump, and the current value is _where_ to jump.
(Any value other than 0 makes the program jump.)
Both of these are set to 0 regardless of whether it jumps or not.

`#1,#0@` is an endless loop. `#1` isn't 0 so it will jump, and #0 refers to the very first character in the program. (It's 0 indexed.)

`#1,#6@` does nothing. The sixth character is right after the @. (It's 0 indexed.) __It jumps according to that _character_ in the program.__ Have fun working with that!
It isn't actually that bad. If you format your code like the examples (`.bp` files) it shouldn't be too hard to keep track of where you're going.

<details>
	</summary>Knowing this: What does `#4#0,#1,#7@/]` print?</summary>
	`@`. It returns `@`.
	
	First off, we put the value `40` in hex in the disc, then 1, then 7. Then we do the IF-GOTO function. If we look at the disc, we're at `40, 1, [7]...`.
	IF-GOTO sees the 1 there (which isn't 0) and so jumps to 7th character (0 indexed!). That's the comma. Now we're at `40, [0], 0...`, and after the comma at `40, 0, [0]...`.
	Then we put `7` there and perform IF-GOTO again. This time, however, the previous value is __zero__, and no jump is performed! Everything is still cleared, so we're back at `40, [0], 0...` but _in front of_ the IF-GOTO.
	`0 | 40` gets you 40, and then you print that character out. It's `@`.
	
	Now I hope you never see code like that again, but the longer the code, the longer those GOTO indexes get. Hopefully that didn't give you more of a headache than counting out 20 characters at a time would have.
</details>

__`=` IF-THREAD:__ Very similar to IF-GOTO, but this one doesn't just go to the specified location; it _creates a new thread_ at that location.
Now, C++ thread support isn't quite working on my compiler, so if you are using it, this has no performance benefit in the provided `.exe`. However, if it can be multithreaded, do so!
It'll speed things up for at least _some_ people, and with a mere 16 commands working overtime, you'll need all the speed you can get.

Now, before we cover the last 3 commands, I have to tell you something.

This language is self-modifying. Any code you have written can be changed by the code itself.
These last three commands let you do that. But they're also great for mundane things, like storing variables.
Just keep that in mind when you use them that they are actually writing _in the code._ If things go haywire fast, that's why.
Also, the compiler removes comments and replaces semi-readable characters with the same hex code used by `#` VALUE.

__`:` READ:__ Read a portion of the program's code.

This is an operation. Actually, everything from hex `5` to `C` is an operation.
The operator is the index (0-indexed!) in the code, and the operand is the number of bits to be red from that index _minus one_; if the operand is 0, 1 bit is red.
Note that READ's index is not the same as IF-GOTO's and IF-THREAD's, because it looks at _bits_, not _hex digits_.

As an example:
`#4,#3:` terminates with a disc of `[4]...`, because it reads the first `4` with 3 more bits than 1.

`:` terminates with the very first bit of the program (`1`), because it uses the default value `0` as both the first and second operators.

__`{` REMEMBER:__ This takes inputs very similar to read, but does not modify the disc at all. Instead, it _records_ the two values for use in:

__`}` COMMIT:__ Overwrite the bits selected by REMEMBER with the ones currently on the disc value.

`#1#8,#3{,#1}` Modifies the program so that the bits between `0x18` and `0x1A` are set to `0x1`. (Remember, REMEMBER doesn't remove any values, so that comma is necessary this time.)
The program reads `#1#8,#1{,#1}` when finished.
If it were to be run again (which there is no way to do currently, as the program resets on termination) it change again to `#1#8,#5{,#1}`.
Which would change again to `#1#8,#0&,#1}`. Which changes to `<1#8,#0&,#1}`. Which changes to... itself.

Well, that's pretty much everything. If you have questions, ask! A cheat sheet is below. Hope this helped.

###Command Cheat Sheet

BIN | HEX | Char | Name | Function
------ | --- | --- | ------------- | ----------------------------------------------------
`0000` | `0` | `#` | VALUE | Instead of reading the next command as a command, take its hexadecimal code and tack<sup>1</sup> that value to the disc value.
`0001` | `1` | `,` | NEXT | Look at the next disc value.
`0010` | `2` | `{` | REMEMBER | This will record the current and last disc values. The last disc value is an index in the code, and the current disc value is a length.<sup>2</sup> Used in conjunction with `}` COMMIT.
`0011` | `3` | `}` | COMMIT | Used in conjunction with `{` REMEMBER. It overwrites the range remembered by REMEMBER with the disc value.
`0100` | `4` | `~` | NOT | Performs a __bitwise not__ with the disc value.
`0101` | `5` | `^` | XOR | Operation. Performs a __bitwise exclusive or__.
`0110` | `6` | `&` | AND | Operation. Performs a __bitwise and__.
`0111` | `7` | `/`<sup>3</sup> | OR | Operation. Performs a __bitwise or__.
`1000` | `8` | `<` | LSHIFT | Operation. Performs a __left bit shift__, treating the previous disc value as the operand and the second disc value as the number of repetitions. (The value is unsigned.)
`1001` | `9` | `>` | RSHIFT | Operation. Performs a __right bit shift__, treating the previous disc value as the operand and the second disc value as the number of repetitions. (The value is unsigned.)
`1010` | `A` | `@` | IF-GOTO | Jumps the pointer in the script to the location at the disc value if the LAST disc value exists<sup>4</sup>. This acts as an operation, returning 0.
`1011` | `B` | `=` | IF-THREAD | Creates a new thread at the location at the disc value if the LAST disc value exists.<sup>4</sup>. The new thread has an empty disc. This acts as an operation (for the main thread), returning the last disc value, so the program can tell whether a new thread was created.
`1100` | `C` | `:` | READ | Operation. Acts similarly to REMEMBER, but instead of recording the index and length for COMMIT, it reads the value at that location for that length.
`1101` | `D` | `%` | PORT | Changes the hardware device with which to perform I/O functions with.<sup>5</sup>
`1110` | `E` | `[` | INPUT | Take a single character of input.<sup>6</sup>
`1111` | `F` | `]` | OUTPUT | Display the current disc value as a character in any format.
`????` | `?` | `_` | UNDEFINED | Replaced with 4 arbitrary<sup>7</sup> bits at compile time.
`----` | `-` | `*` | TODO | Stops compilation, or allows the user to continue by replacing them with UNDEFINED.

<sup>1</sup>It performs a left-shift on the current disc value by 4, and then performs an OR operation with the new and old values.

<sup>2</sup>This really means how many *more* bits are being looked at, so if the second operand is `3` then 4 bits will be taken, and if it is `0` then 1 bit will be taken.
Values above `3F`, which handles 64 bits, will always return 0.

<sup>3</sup>`|` is conventional, but this language is anything but.

<sup>4</sup>Is not 0.

<sup>5</sup>Does nothing in this version.

<sup>6</sup>Though it requests an entire string from the user, and appends `NUL` to that character.

<sup>7</sup>Compiler defined.


End