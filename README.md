# QuantumGL

Repository for migrating the software to compile and execute on a current Linux version.

A description of QuantumGL can be found at http://www.uni-graz.at/imawww/vqm/

## Overview
QuantumGL is an OpenGL-based tool for the visualization of volume data. It allows users to define data visualization methods and parameters in text files, which can then be modified interactively through the graphical user interface. The project represents an effort to modernize and maintain the codebase for contemporary computing environments.

Originally developed by Wolfgang Thaller in 2004, QuantumGL provides powerful features for scientific visualization including:
- Interactive 3D visualization of volume data
- Support for real and complex vector fields
- Customizable visualization methods (isosurfaces, slices, vectors, flow lines)
- Interactive parameter adjustment through GUI
- Support for both ASCII and binary data files

## Status
Last updated: January 6, 2025

## Requirements

### Mac OS X
- Xcode
- OpenGL (included with macOS)

### Windows
- MinGW32
- libz
- libpng
- OpenGL support

### Linux/Unix
- X11 with OpenGL support
- GLUT or freeglut
- C++ compiler with C++11 support or higher

## Building and Running

### Mac OS X
1. Open `QuantumGL.pbproj` in Xcode
2. Build the project using Xcode's build system
3. If modifying the parser (quantum.y) or lexer (quantum.l):
   ```bash
   make parser
   ```

### Windows
1. Install MinGW32 from www.mingw.org
2. Build using:
   ```bash
   make -f Makefile.mingw
   ```

### Linux/Unix
Build using:
```bash
make -f Makefile.x11
```

## Alternative Build Methods
The project includes several other build configurations:
- `Makefile.clang` - For building with Clang
- `MakefileEmscripten.clang` - For WebAssembly builds
- `MakefileOSX.clang` - For macOS builds using Clang
- `MakefileOSX_GLFW.clang` - For macOS builds using GLFW

## Usage

### Visualization Task Files
QuantumGL uses text files to define visualization tasks. These files support:
- Variables with real values that can be modified through the GUI
- Field expressions and vector expressions
- Comments (using #, //, /* */, or (* *))
- Floating-point constants and mathematical expressions
- Predefined variables for camera and lighting parameters

### Data Files
The program supports two types of data files:
1. ASCII Data Files
2. Binary Data Files (compatible between Intel and PowerPC using IEEE-754 format)

### Visualization Features
- Isosurfaces with cyclic and cutout options
- 2D Slices through 3D data
- Vector field visualization
- Flow line visualization
- Customizable bounding box and grid display
- Adjustable resolution and world size
- Interactive camera and lighting controls

## Contributing
Contributions to help with the migration effort are welcome. Please ensure your code follows the project's coding standards and includes appropriate documentation.

## License
Please refer to the LICENSE file in the repository for licensing information.

## Documentation
Detailed documentation can be found in `QuantumGL Documentation.pdf` and `docs.md`

## Contact
For more information about the original project, visit http://www.uni-graz.at/imawww/vqm/
