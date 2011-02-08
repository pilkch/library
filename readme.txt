### Chris Pilkington  
Copyright (C) 2005-2011  
<http://chris.iluo.net/> 

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


### Getting a copy of the project on Linux (Fedora 8 used)

Pull this project:  
yum install git  
OR  
sudo apt-get install git-core  
mkdir library  
cd library  
git clone git://breathe.git.sourceforge.net/gitroot/breathe/breathe  
OR  
git clone ssh://pilkch@breathe.git.sourceforge.net/gitroot/breathe/breathe  


### Building on Linux (Fedora 8 used)

Use a cmake file as used in Sudoku and Drive  
Use yum to get all required libraries such as  
*   yum install SDL-devel
*   yum install SDL-image
*   yum install SDL-ttf
*   yum install SDL-net

### Usage

Put the whole library folder somewhere on your hard drive.  
Add to your build paths:  
library/include  
library/src  
library/lib  
Include required files from the library, add any required lib files to the project  
Setup any required build flags  
Compile 
### Build Flags

UNICODE  
BUILD\_PHYSICS\_3D  
BUILD\_PHYSICS\_2D  
BUILD_LEVEL  


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
### Credit

Breathe and Spitfire are created by me, Christopher Pilkington.   
For the parts that are not mine, I try to keep an up to date list of any third party libraries that I use.   
I only use libraries that are license under either the GPL, LGPL or similar and am eternally grateful for the high quality ease of use and generosity of the open source community. 

Box2D  
Copyright (c) 2006-2007 Erin Catto  
All rights reserved  
<http://www.gphysics.com/> 

Open Dynamics Engine  
Copyright (C) 2001-2003 Russell L. Smith  
All rights reserved  
<http://www.q12.org/>  SDL - Simple DirectMedia Layer

  
Copyright (C) 1997-2006 Sam Lantinga  
Sam Lantinga  
<http://www.libsdl.org/>  FreeType Copyright 1996-2001, 2006 by

  
David Turner, Robert Wilhelm, and Werner Lemberg  
<http://www.freetype.org/> 

3DS File Loader  
genjix@gmail.com  
<http://sourceforge.net/projects/scene3ds>  
<http://www.gamedev.net/community/forums/topic.asp?topic_id=313126> 

MD5 RFC 1321 compliant MD5 implementation  
Copyright (C) 2001-2003 Christophe Devine 

Memory manager & tracking software  
Paul Nettle  
Copyright 2000, Fluid Studios  
All rights reserved  
<http://www.FluidStudios.com>
