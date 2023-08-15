# Tennis Game Simulation using OpenGL

![Tennis Game](game.png)

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Features](#features)
- [Usage](#usage)
- [Controls](#controls)
- [Credits](#credits)
- [License](#license)

## Introduction

This project aims to create an immersive simulation of a tennis game using the OpenGL graphics library. The simulation
includes a detailed tennis court, a scoreboard, interactive rackets representing players, and a crowd. The simulation
allows players to experience the excitement of a tennis match through realistic graphics and
gameplay elements.

## Installation

### Command Line

1. Download and Install CMake
2. Open a terminal
3. Run cmake:

```
cd <source_folder>
cmake -S . -B <build_folder>
cmake --build <build_folder> --target install
```

To generate an Xcode project from the command line use:

```
cd <source_folder>
cmake -S . -B <build_folder> -G Xcode
cmake --build <build_folder> --target install
```

**NOTE:** In the case of cmake the `<source_folder>` is the topmost folder with
a CMakeLists.txt file.

**NOTE:** The `<build_folder>` can be anything but is usually just named `build`.

**NOTE:** The `-G` flag sets the generator of the project to use. On **Linux**
and **macOS** this defaults to **Unix Makefiles**. On **Windows** the default
is **Visual Studio**.

**NOTE:** It's usually a good idea to delete the `<build_folder>` if you switch
generators.

### CMake GUI

You can use the CMake GUI to configure and generate the Visual Studio Project.

1. Download and Install CMake
2. Open the CMake GUI
3. Set the `Where is the source code:` field to the source folder
4. Set the `Where to build the binaries:` field to different location (e.g.: source_folder/build)
5. Click Configure
6. Select the project generator (this will be the project file that compiles program)
7. Click Generate

#### Windows

Once the project is created you can build the **ALL_BUILD** project followed by
building the **INSTALL** project.

#### Xcode

Once the project is created you can build the **ALL_BUILD** target followed by
the **INSTALL** target.

#### Unix Make File

In the terminal:

```
cd <build_folder>
make install
```

## Running

Once built and installed, all the relevant files will be in the **dist** folder
at the top level of the source folder, along with the program assets, such as
textures, shaders and geometry. The assets generally need to be in the same
path as the program.

### Linux/macOS

In the terminal type:

```
./main
```

### Windows

Click on the **main.exe** from the File Explorer.

## Features

- Realistic tennis court environment with detailed textures and dimensions.
- Interactive tennis rackets that players can control using keyboard inputs.
- Scoreboard to keep track of the game's progress and display the scores.
- Immersive crowd and environment elements to enhance the user experience.
- Borrowed 3D assets (crowd, trees, decorative items, etc.) integrated to create an immersive atmosphere.

## Usage

Once the application is running, you can interact with the virtual world using the following controls:

## Controls



## Credits

Good place to learn OpenGL |
https://www.learnopengl.com

Official OpenGL 4 Reference Documentation |
https://www.khronos.org/registry/OpenGL-Refpages/gl4/

Official OpenGL Wiki |
https://www.khronos.org/opengl/wiki/

Easy to Navigate OpenGL Function Documentation |
http://docs.gl/

GLM Documentation |
https://glm.g-truc.net/0.9.9/index.html

GLFW Documentation |
https://www.glfw.org/

Good place for advanced graphics theory |
https://www.scratchapixel.com/

FloatyMonkey | Computer Graphics - YouTube |
https://www.youtube.com/playlist?list=PL2fqtyC9Wl02ycOM1oXzAcTByTMepnhj_

Free3D | 3D Models for Free |
https://free3d.com/

GeeksforGeeks | Basic Transformations in OPENGL |
https://www.geeksforgeeks.org/basic-transformations-opengl/

Mahadevan, S | Unit Sphere Object |
https://gist.github.com/svmhdvn/efc5779e79780e8b72fb3040423f512f

TurboSquid. 3D Models for Professionals |
https://www.turbosquid.com/

Unsplash | Beautiful Free Images & Pictures |
https://unsplash.com/

## License

