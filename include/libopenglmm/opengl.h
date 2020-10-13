/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington      *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

// This is a simple wrapper around OpenGL to make it a little bit more modern and easier to work with

#ifndef LIBOPENGLMM_OPENGL_H
#define LIBOPENGLMM_OPENGL_H

#include <libopenglmm/libopenglmm.h>

// OpenGL headers
#if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
#ifdef BUILD_LIBOPENGLMM_OPENGL_STRICT
#define GL3_PROTOTYPES 1 // Use OpenGL 3 core profile only
#else
#ifdef __WIN__
#include <GL/GLee.h>
#include <GL/glu.h>
#endif
#endif

#ifdef __WIN__
#include <GL3/gl3w.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#else
#include <GL/GLee.h>
#include <GL/glu.h>
#endif

#endif // LIBOPENGLMM_OPENGL_H
