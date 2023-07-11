# GCGP
A modular C++11 closed-loop "GRBL Compatible G-Code Parser" to be used on desktop or embedded

## What is GCGP

GCGP is a C++11 library that does not use the C++ standard library, with the core goal of parsing and interpreting G-Code, while being as compatible to GRBL as possible. This means a GCGP-based system can ideally be controlled using a standard GRBL controller. The parsed G-Code is then passed to user-defined callbacks, or presented in data containers, so that the user can then decide what to do with it.

This library is on one hand a CMake-based C++11 library, allowing it to be used in Desktop applications such as testing environments and simulations, where the focus is to simulate the machine by running the exact same code as in the controller. On the other hand, it is at the same time an Arduino library which will be added to the official Arduino library repository soon. The focus is to provide a way to develop GRBL-compatible CNC controllers that operate in closed-loop mode. Open-loop control is of course also possible like in the original. 

There are stepper-hybrid solutions that work with plain GRBL, but only the servos in itself are closed-loop and the controller knows nothing about them. The goal of this project is to develop real closed-loop GRBL controllers that correctly report set position and actual position. This library is one of many libraries needed.

It does:
 - Take streamed G-Code as character arrays
 - Parse and interpret the G-Code using a GRBL-Syntax
 - Respond like GRBL would by providing a response string
 - Allow new commands that make closed-loop control possible
 - Provide the parsed information to the user for further processing
 - And present the data in a way that allows closed-loop control in contrary to vanilla GRBL

### TL;DR:

GCGP is more or less a modular partly re-implementation of GRBL, allowing for real closed-loop CNC controllers, that can also be perfectly simulated because the code can be run on any platform.

## What GCGP is NOT

GCGP is only a G-Code interpreter. That means it only parses and interprets G-Code and presents the parsed information to the user, where it is presumably passed to another library.

It is NOT a complete re-implementation of GRBL, as it is only one small module of many others you would need, and it is also not a 1-to-1 reimplementation because closed-loop control is the main focus and there are additional features that make this possible.

Also, features are added as needed and it is unlikely that all features of GRBL will be implemented soon.

It does NOT do any of the following:
 - No interface to read or write serial data
 - No Motion planning
 - No Motion control
 - No Kinematics
 - No knowledge of the machine, only forwarding the parsed information
 - Not 100% compatible to GRBL, but as close as possible