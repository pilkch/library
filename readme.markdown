### Breathe Game Engine and Spitfire Utility Library  
 Chris Pilkington  
 Copyright (C) 2005-present  
 [http://chris.iluo.net/](http://chris.iluo.net/)

### License

This library is free software; you can redistribute it and/or  
 modify it under the terms of the GNU Lesser General Public  
 License as published by the Free Software Foundation; either  
 version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,  
 but WITHOUT ANY WARRANTY; without even the implied warranty of  
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU  
 Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public  
 License along with this library; if not, write to the Free Software  
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA  

### What is this?

This is a collection of libraries including:

*   Spitfire a collection of utility classes for dealing with maths, strings, files and threading
*   Breathe game engine using SDL, OpenGL, OpenAL and Bullet
*   libopenglmm a wrapper for OpenGL with support for OpenGL 3.3 core
*   libtrashmm a library for deleting files on Linux
*   libvoodoomm a library for dealing with images
*   libgtkmm a wrapper for using GTK on Linux
*   libwin32mm a wrapper for hiding all the Win32 rubbish
*   libxdgmm a wrapper for xdg functionality on Linux

### Spitfire Utility Library

algorithm  
math  
platform  
storage  
util  

Spitfire provides all the basic functionality required for an application. It doesn't have any dependencies on third party libraries (Apart from native OS libraries) and doesn't force you to say, derive your application from a base class or structure your code in a particular way. Because it is so loosely coupled, you can easily just use the parts of Spitfire that you need and nothing more, and it can be used in any type of application; service, gui, fullscreen game or wxWidgets/Qt/GTK.

### Breathe Game Engine

audio  
communication  
game  
gui  
loader_3ds  
physics  
render  
vehicle  

Breathe makes heavy use of spitfire for basic functionality such as string utilities, filesystem functions and math library. It then extends these to add a scenegraph, OpenGL rendering and OpenAL audio to create a game engine. Breathe is only meant for game programming (Or at least applications that use SDL and create an OpenGL context for example editors) and is much more heavy handed in how you can use its classes. They often derive from the SDL thread wrapper cThread and assume that you want to use the various third party libraries, locking you in. Breathe started out as (And still is) a monolithic game engine library containing all of the Spitfire classes as well. There is still some overlap, but in the future the split will be more pronounced and we will have two separate libraries. Eventualy all Spitfire classes will be in their own namespace ("spitfire") and the transformation will be complete.

**TODO: Draw relationship diagram from sheet of paper**

## Building

### Get a copy of the project

`git clone git@github.com:pilkch/library.git`  
OR  
`git clone https://github.com/pilkch/git@github.com:pilkch/library.git`  

### Building on Windows

Install required packages:  
git
mingw
cmake
VS Code
boost

### Building on Linux

Use yum to get all required libraries such as:
```bash
sudo yum install gcc gcc-c++ make cmake gtest-devel gmock-devel
sudo yum install gnutls-devel
sudo yum install libxdg-basedir-devel
sudo yum install SDL2-devel SDL2_image-devel SDL2_mixer-devel SDL2_net-devel SDL2_ttf-devel
sudo yum install mesa-libGL1-devel mesa-libGLU1-devel
sudo yum install GLee-devel
sudo yum install freetype-devel
```

Gcc 13.2 C++ experimental library has a small issue, two constructors are not inlined, so you may have to modify the headers:
```bash
sudo sed -i "/const error_category& socket_category() noexcept/c\  inline const error_category& socket_category() noexcept" /usr/include/c++/13/experimental/socket
sudo sed -i "/const error_category& stream_category() noexcept/c\  inline const error_category& stream_category() noexcept" /usr/include/c++/13/experimental/buffer
```

To build and run the tests:
```bash
cd tests
cmake .
make
```

### Usage

Put the whole library folder somewhere on your hard drive.  
Add to your build paths:  
library/include  
library/src  
library/lib  
Include required files from the library, add any required lib files to the project  
Setup any required build flags  
Compile

### Credit

Breathe and Spitfire are created by me, Christopher Pilkington.   
For the parts that are not mine, I try to keep an up to date list of any third party libraries that I use.   
I only use libraries that are license under either the GPL, LGPL or similar and am eternally grateful for the high quality, ease of use and generosity of the open source community.

Box2D  
 Copyright (c) 2006-2007 Erin Catto  
 All rights reserved  
 [http://www.gphysics.com/](http://www.gphysics.com/)

Open Dynamics Engine  
 Copyright (C) 2001-2003 Russell L. Smith  
 All rights reserved  
 [http://www.q12.org/](http://www.q12.org/)

SDL - Simple DirectMedia Layer  
 Copyright (C) 1997-2006 Sam Lantinga  
 Sam Lantinga  
 [http://www.libsdl.org/](http://www.libsdl.org/)

FreeType Copyright 1996-2001, 2006 by  
 David Turner, Robert Wilhelm, and Werner Lemberg  
 [http://www.freetype.org/](http://www.freetype.org/)

3DS File Loader  
 genjix@gmail.com  
 [http://sourceforge.net/projects/scene3ds](http://sourceforge.net/projects/scene3ds)  
 [http://www.gamedev.net/community/forums/topic.asp?topic_id=313126](http://www.gamedev.net/community/forums/topic.asp?topic_id=313126)

MD5 RFC 1321 compliant MD5 implementation  
 Copyright (C) 2001-2003 Christophe Devine

Memory manager & tracking software  
 Paul Nettle  
 Copyright 2000, Fluid Studios  
 All rights reserved  
 [http://www.FluidStudios.com](http://www.FluidStudios.com)
 