//$Id$
//------------------------------------------------------------------------------
//                              gmatOpenGLSupport
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
// Refactored here for visualization purposes
/**
* Declares gmatOpenGLSupport for opengl plot.
*/
//------------------------------------------------------------------------------
#ifndef GmatOpenGLSupport_hpp
#define GmatOpenGLSupport_hpp

// windows specific functions
void InitGL();
bool SetPixelFormatDescriptor();
void SetDefaultGLFont();
void ScreenShotSave(char* ImagePath);
#endif
