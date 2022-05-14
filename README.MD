# Overview

The main goal of this project was to understand how to read a [reference manual](https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf)
and implement it's instructions.
It was tested with the [binary](http://www.emulator101.com/files/cpudiag.asm) containing all instructions.

## Building and Running

There are 2 `.bat` files in the main directory, the `build.bat` and `run.bat`.
To run `build.bat` is needed be in Developer Tools Command Prompt or Powershell having access to cl.exe (msvc compiler).

```
build.bat & run.bat
```

That command will build and run the project, printing all the instructions and ending after all commands had being executed.

## case_gen.py

Is a python script to generate all instructions hex numbers. 
The following example shows how would be the code without the generated hex.

```C
/*
Instruction // Don't mean that instruction exists
[10[ddd]110]
*/
op = emu-pc;
if ((op & 0x11000111) == 0x10000110) {
    // do stuff
}
```

To run the `case_gen.py` is straightforward.

```
python case_gen.py > out.txt
```

The case generation code is really simple, following the instructions as they appear
in the reference manual and creating the needed hex number for the instruction.
