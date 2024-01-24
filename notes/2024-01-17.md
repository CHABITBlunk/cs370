# Operating Systems

## A modern computer is a complex system
- Multiple processors
- Main memory & disks
- Keyboard, mouse, & displays
- Network interfaces
- I/O devices

## Why do we need operating systems?
- If every programmer had to understand how all these components work, software development would be arduous
- Managing all components & using them optimally is a challenge

## Common misconceptions about the OS
- Is it the program that users interact with? (e.g. shell, GUI)
- No, this doesn't even use the OS

| User interface program |
| Operating system    |
| Bare hardware    |

## Where the OS fits in
- OS runs on bare hardware in kernel mode
    - Complete access to all hardware
    - Can execute any instruction that the machine is capable of executing
- Provides the base for all software
    - Rest of software runs in user mode
    - Only a subset of machine instructions is available
- Controls hardware & coordinates its use among various programs

## Kernel & user modes
- Everything running in kernel is part of OS
- Some programs running outside it are part of it or at least closely associated w it

## Operating systems tend to be huge, complex, & long-lived
- Source code of an OS - ~5M lines of code (for the kernel)
- Application programs such as GUI, libraries, & application software is 10-20 times that
- Operating systems are long-lived because they're hard to write, people hate to throw them out
- Evolve over periods of time
    - Windows 95/98/Me is one OS
    - Windows NT/2000/XP/Vista/7/8 is another
    - System V, Solaris, BSD derived from original Unix
    - Linux is a fresh code base
        - Closely modeled on Unix
    - OS X based on BSD4.4

## Example: floppy disk I/O done using NEC PD765
- 16 commands
    - Read/write, move disk arm, formatting tracks, &c.
    - Load 1-9 bytes into device register
- Most basic commands are read & write
    - 13 parameters packed into 9 bytes
        - Address of disk block, number of sectors/track, inter-sector gap spacing, &c.
- When the operation is completed, controller returns 23 status & error fields packed into 7 bytes
- You must also check motor status
    - If it is off, turn it on before reading/writing
    - Don't leave motor on too long - floppy disk will wear out
    - Tradeoff: long startup delay vs wearing out disk

## Average programmer doesn't want to have any of this
- They just want to deal with a simple, high-level abstraction
- For a disk, this would mean a collection of named files
    - Operations include open, read, write, close, &c.
    - But not whether the recording should use FM or the state of the motor

## Why does hardware have a complex interface?
- Backward compatibility? (not really)
- Desire to save money
- Sometimes hardware designers don't realize (or care) how much trouble they cause

## Importance of abstractions
- Abstraction is the key to managing complexity
- Good abstractions turn a nearly impossible task into two manageable ones
    1. Defining & implementing abstractions
    2. Using abstractions to solve problem
- E.g. file

## Two views of OS
- Top-down view - providing abstractions to application programs
- Bottom-up view - manage all pieces of a complex system

## Defining OSs
- Solves problem of creating usable computing system - makes solving problems easier
- Control, allocate, & mediate access to resources
- One program that is running all the time: kernel

## What the instruction set looks like
- Load a word from memory into register & from register into memory
- Combine two operands from register, memory, or both into a result
    - Add two words & store result in a register or memory

## When a program needs to read a word in memory
- Start at L1: cache hardware checks if the needed line is in cache
- Yes -> cache hit
    - Request satisfied in 1-10 clock cycles
    - No memory access needed
- No -> cache miss
    - Read in L2, repeat
    - No cache holds the data -> read to memory (very long latency, 1000s of cycles possible)
        - Don't believe the sequential bandwidth numbers advertised as true in practical scenarios!
