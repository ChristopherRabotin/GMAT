//$Id$
//------------------------------------------------------------------------------
//                        File: Rendering.hpp      
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/17
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef _RENDERING_H
#define _RENDERING_H

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"         // for GetGuiInterpreter()
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
#include "Rvector3.hpp"

struct GlColorType
{
   Byte red;
   Byte green;
   Byte blue;
   Byte not_used;
};

void SetColor(GlColorType color, Byte red, Byte green, Byte blue);
void DrawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum style,
                GLenum orientation = GLU_OUTSIDE, GLenum normals = GL_SMOOTH,
                GLenum textureCoords = GL_TRUE);
void DrawLine(GlColorType *color, Rvector3 start, Rvector3 end);
void DrawLine(float red, float green, float blue, Rvector3 start, Rvector3 end);
void DrawCube(float x, float y, float z);
void DrawSpacecraft(float radius, GlColorType *color1, GlColorType *color2);
void DrawEquatorialPlanes();
void DrawCircle(GLUquadricObj *qobj, Real radius);
void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z,
                  GLfloat k);

#endif
