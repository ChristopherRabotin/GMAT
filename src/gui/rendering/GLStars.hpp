//$Id$
//------------------------------------------------------------------------------
//                              Stars.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Phillip Silvia, Jr., Derived from Gene Stillman's code for Odyssey
// Created: 2009/09/21
/**
 * This class handles loading the star and constellation information and drawing
 * them in to the scene. 
 *
 * GLStars is a singletons class, as there is no real need to have multiple
 * instances of it active. To use it, simple call InitStars on the instance
 * in the canvas that is drawing the stars, then call DrawStarsVA during your
 * draw loop. The file location is determined by the FileManager
 */
//------------------------------------------------------------------------------

#ifndef _GLStars_h_
#define _GLStars_h_

#include "gmatdefs.hpp"
#include "gmatwxdefs.hpp"
//#include <GL/glu.h>      // GL Utilities

const int   MAXSTARS   = 42101;     // Max Number Stars in arrays
const int   MAXLINES   = 1600;      // Max Number of Constellation line vertices
const int   MAXCON     = 120;
const float STARSTEP   = 0.5f;      // Scale into this size groups
const int   GROUPCOUNT = 18;        // Number of groups

enum StarOrder {
    CosRAcosDec = 0,                // cos(RA)*cosDec
    SinRAcosDec,                    // sin(RA)*cosDec
    SinDec,                         // sin(Dec)
    Range                           // Set to zero, draw at infinity
};


class GLStars
{
public:
   static GLStars* Instance();
   void InitStars();                        // Initialize the Stars, including loading from file
   void DrawStarsVA(GLfloat ColorAlpha, int starCount, bool drawConstellations);    // Vertex Array Method, with alpha
   void SetDesiredStarCount(int count) {
      if (DesiredStarCount <= MAXSTARS)
         DesiredStarCount = count;
      else
         DesiredStarCount = MAXSTARS;
      };
   int  GetDesiredStarCount() { return DesiredStarCount; };
  
protected:
   GLStars();
   ~GLStars();
   GLStars(const GLStars&);
   GLStars& operator= (const GLStars&);

private:
   void ReadBinaryStars();
   void ReadTextStars();
   void ReadTextConstellations();
   
   Real  StarsVA[MAXSTARS][4];              // cos(RA)*cosDec
                                            // sin(RA)*cosDec
                                            // sin(Dec)
                                                                                                                  // 0 (to draw at infinity)
   GLfloat     CLines[MAXLINES][4];                       // Constellation points
   wxString    ConstellationNames[MAXCON];   // The names of the constellations
   int         ConstellationIndex[MAXCON][2]; // The starting and ending indices for each constellation
   int         GroupIndex[GROUPCOUNT];      // Indexes into the m_StarsVA array
   int         GroupCount[GROUPCOUNT];      // # of stars in this group
   Real        PointSize[GROUPCOUNT];       // Group GLpointsize
   int         LastGroupUsed;               // Not all groups may be used
   int         MaxDrawStars;                // Not Array Size
   int         DesiredStarCount;            // DesiredStars, if we want it to be adjustable
   int         NumLines;                    // Number of constellation lines loaded
   int         NumConstellations;           // Number of constellations loaded
   static      GLStars* theInstance;         // The singleton instance of the Stars

   // Used for parsing the file
   void GetStarValues(wxString buffer, Real *RightAscension, Real *Declination, Real *VisualMag);
   void GetConstellationValues(wxString buffer, Real *RightAscension1, 
      Real *Declination1, Real *RightAscension2, Real *Declination2);
};
#endif
