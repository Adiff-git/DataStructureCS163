 Project_CS163_APCS24: Data Structure Visulization

## Introduction

Data Structure Visualization is a program that allows users to visualize the behavior of common data structures such as linked lists, hash table, tree and graph. This program is written in C++ and uses the Raylib library (with Cmake) for graphics rendering.

## Features
This program currently supports the following data structures:
- Singly Linked List
- Chaining Hash Table
- AVL Tree
- Minimum Spanning Tree (Graph)


Main features included:
- Smooth animations of the algorithms responsible for inserting, deleting, searching, and updating values in the data structure.
- Control the speed of the animations and play, pause or reverse the animations.
- Step-by-step code highlight.
- Create random, user defined or load-from-file list.

## Installation

### Prerequisites

- C++ compiler: g++
- RayLib library installation instructions available on the [RayLib Website](https://www.raylib.com/) or [GitHub](https://github.com/raysan5/raylib)
- Tinyfiledialogs library installation instructions available on the [Tinyfiledialogs website](https://sourceforge.net/projects/tinyfiledialogs)
- Cmake installation instructions available on the [Cmake Website](https://cmake.org/download/)

### Building RayLib With Cmake

1. Download or clone the repository to your local machine.
2. Install Raylib library by following the instructions in Raylib's official website or GitHub
3. Navigate to the project directory and open a terminal.
4. Compile the code using the following command:
```console
cmake -S . -B ./build
cmake --build ./build
```

### Integrating tinyfiledialogs

1. Download or clone the tinyfiledialogs source code to your machine (e.g., from SourceForge). Extract if necessary.
2. Copy the two files tinyfiledialogs.c and tinyfiledialogs.h into your project's directory structure. For example, place them alongside your own source code files, or in dedicated subdirectories like src/, include/.
3. Navigate to your project's root directory and open a terminal (or command prompt).
4. Compile your application's source code together with the tinyfiledialogs.c file. You will need to link against the system libraries required by tinyfiledialogs on your platform. Below is a general illustrative example using g++:
```console
 Compile your application source code (.cpp) together with tinyfiledialogs.c. Also, link against the necessary system libraries for your platform (e.g., -lcomdlg32 -lole32 -luuid on Windows).
```
### Running Raylib with Cmake

1. Navigate to the build directory of the project.
2. Run the executable file through the terminal:
```console
./src/build.exe
```
or move the DataVisual.exe file in the "src/" folder to its parent folder and double-click it.

## Demo Video

Here is the [demo video](https://www.youtube.com/watch?v=_w4Hls86Sy8&t=330s)

## Credits

This program was created by Group 1 - 24A02 as a project for CS163 - Introduction to Computer Science 2 at VNU - University of Science.

## License

This project is licensed under the [MIT License](https://opensource.org/license/MIT)