# CGES

**This README is under construction.**

## Overview

![phongshading](https://imgur.com/VeMHXqI.png) ![pathtracing](https://imgur.com/M6Pphya.png)

CGES is switchable interactive renderer made with [Intel's Embree](https://www.embree.org/index.html), to compare rendering methods.
"CGES" comes from "CG effects switcher".

## Demo movie
https://www.youtube.com/watch?v=hRr8_boZRdo


## Build

- Required environment
  - Windows OS
  - Visual Studio
- Visual Studio 2022 with v143 platform toolset on Windows 11 is confirmed to build successfully.

- Build steps:
  1. Clone this repository.
  2. Open powershell and execute `install_dependencies.ps1`. Libraries required to compile are automatically downloaded from the internet and placed to appropriate directories.
  3. Open `CGES.sln` in Visual Studio and build.
  4. `bin/` containing an executable file will appear in the project root directory.
- The executable file will be aborted with exception without no data(3D models, textures). We are now dealing with distributing such data in progress.

## How to operate

- Arrow key (Up, Down, Right, Left)

  Camera will move.

- I / O key

  Zoom in & out.

- Esc key

  Terminate renderer.

- Number key(1, 2, ..., 0)

  Switch rendering methods.

  1. Phong shading
  2. Pathtracing
  3. Bidirectional Pathtracing



## Required dependencies

- [Embree 3.6.1](https://www.embree.org/index.html)
- [glm 0.9.9.800](https://glm.g-truc.net/0.9.9/index.html)
- [GLFW 3.3](https://www.glfw.org/)
- [stb library](https://github.com/nothings/stb)
