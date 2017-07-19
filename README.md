# BitP v0.7.0

You're here for the challenge, aren't you?
If you need help on how this works, read the wiki. If that's all gibberish, there are a few examples provided.

This is an interpreter for a programming language, BitP (/bɪd-ɪp/), written in C++.
It was designed to resemble (and be practical as) an assembly language, but compacted into 16 commands so that a human can learn it more easily.
It ended up being more of an esolang...

 - The program (bitp\_7.exe) reads any file you give it. You must include the file extension.
 - A debug version has been included (bitp\_7\_debug.exe) which will print out information at every code point.
 - The .dll files are required to run both versions.
 - This has only been tested on a windows machine.

bitp.cpp contains the source code. If the .exe files are not quite working, compile this source for your machine.

At release 1.0, everything in the language is finalized. Everything after that point will not affect how .bp files are read.
Also, very few programs will break between now and v1.0.