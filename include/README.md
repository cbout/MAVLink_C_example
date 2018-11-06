# The C MAVLink library

## Introduction

The c_library_v1 is an auto-generated library, for the general developer guide click [here][devguide]. You can find it in this [repository][c_lib]. At the start of our project it would have been useful to find a simple explanation of the C library. In this file we will describe the library as we understand it and we will trying to make your research into this one easiest.

## Architecture

The c_library_v1 is composed by various folders, the main folder is  **`common/`** which contains global MAVLink standard messages and structure, there is also **`minimal/`** and **`standard/`** which do not implement all the common messages. Other folders are sort of "plug in" library that implement specific messages standard, like the folder **`ardupilotmega/`**, this one implement all the common's function and add standard **ardupilot** function, each specific autopilot have is own folder (folder's name is equivalent to autopilot's name). We will take **`common/`** and **`ardupilotmega/`** as example to explain the library.

## How to include?

The include is the same for all autopilot, in your `.c` file you have just to insert in header this include : `#include <mavlink.h>`. The only change is in the compilation, in fact if you want to use just common messages, just add `-I [path]/c_library_v1/common` in your gcc command line. In the same way, if you want to use ardupilot messages just use `-I [path]/c_library_v1/ardupilotmega` instead of the last one.

Note: common's function are include in the other folder, that's mean you don't need to add `-I [path]/c_library_v1/common` if you have include an other folder. 

## common/

In the common folder you will find many functions and files, the most important one is `common.h`, it contains all the defines (except messages' id) that allow you to make a readable code. 

[devguide]:https://mavlink.io/en/	"MAVLink's libs general dev guide"
[c_lib]: https://github.com/mavlink/c_library_v1	"C library v1 repository"