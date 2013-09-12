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
   Byte alpha;
};

void SetColor(GlColorType color, Byte red, Byte green, Byte blue);
void DrawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum style,
                GLenum orientation = GLU_OUTSIDE, GLenum normals = GL_SMOOTH,
                GLenum textureCoords = GL_TRUE);
void DrawLine(GlColorType *color, const Rvector3 &start, const Rvector3 &end);
void DrawLine(float red, float green, float blue, const Rvector3 &start, const Rvector3 &end);
void DrawLine(double x1, double y1, double x2, double y2);
void DrawCube(float x, float y, float z);
void DrawSpacecraft(float radius, GlColorType *color1, GlColorType *color2, bool drawSphere = true);
void DrawEquatorialPlanes();
void DrawCircle(GLUquadricObj *qobj, Real radius);
void DrawCircle(double x1, double y1, double radius, bool fill = true);
void DrawSquare(double x1, double y1, double radius, bool fill = true);
void DrawStringAt(const wxString &str, const Rvector3 &point);
void DrawStringAt(const wxString &str, GLfloat x, GLfloat y, GLfloat z,
                  GLfloat k);

#endif
