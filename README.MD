# Overview

This project was created with the objective to understand how to read a [reference manual](https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf)
and implement it's instructions.
It was tested with the [binary](http://www.emulator101.com/files/cpudiag.asm) containing all instructions.

## Building and Running

There are 2 `.bat` files in the main directory, the `build.bat` and `run.bat`.
To run `build.bat` is needed be in Developer Tools Command Prompt or Powershell, with that you'll have access to cl.exe (msvc compiler).

```
build.bat & run.bat
```

That command will build and run the project, printing all the instructions and ending after all commands had being executed.

## case_gen.py

Is a python script I made to generate all instructions hex numbers, at first I was extracting bits by hand, wich grew quickly in complexty.
You can see that in following example.

```
/*
Instruction // Don't mean that instruction exists
[10[ddd]110]
*/
op = emu-pc;
if ((op & 0x11000111) == 0x10000110) {
    // do stuff
}
```

That aproach generated tons of ifs and elses, but I finished the project in that way and I refactored to the current aproach, wich is separate the instructions into groups and extract the needed information after.

The big advantage in doing that way is not so much of repeated code.

To run the `case_gen.py` is straightforward.

```
python case_gen.py > out.txt
```

The case generation code is really simple, following the instructions as they appear
in the reference manual and creating the needed hex number for the instruction.

With the cases generated the last thing is extract the info needed from the hex and
execute the instruction.

