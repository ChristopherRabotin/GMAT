//$Id$
//------------------------------------------------------------------------------
//                              Stars.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Phillip Silvia, Jr., Derived from Gene Stillman's code for Odyssey
// Created: 2009/09/21
/**
 * This class handles loading the star and constellation information and drawing
 * them in to the scene. 
 */
//------------------------------------------------------------------------------

#include "GLStars.hpp"
#include "wx/textfile.h"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "Rendering.hpp"

#ifndef M_PI
// wx/math.h defines this.  So does math.h in VS 2010, but not earlier incarnations
#define M_PI 3.1415926535897932384626433832795
#endif

const GLfloat SOURCELIGHT99[] = {0.99f, 0.99f, 0.99f, 1.0f};

GLStars* GLStars::theInstance = NULL;

//------------------------------------------------------------------------------
// GLStars()
//------------------------------------------------------------------------------
/**
 * Initialize certain values
 */
//------------------------------------------------------------------------------
GLStars::GLStars()
{
   DesiredStarCount = 0;
   NumLines = 0;
}

//------------------------------------------------------------------------------
// ~GLStars()
//------------------------------------------------------------------------------
/**
 * Destroy the instance of the class on the way out
 */
//------------------------------------------------------------------------------
GLStars::~GLStars()
{
   if (theInstance)
      delete theInstance;
   theInstance = NULL;
}

//------------------------------------------------------------------------------
// GLStars* Instance()
//------------------------------------------------------------------------------
/**
 * Return the created instance of the Stars
 */
//------------------------------------------------------------------------------
GLStars* GLStars::Instance()
{
   if (theInstance == NULL)
      theInstance = new GLStars;
   return theInstance;
}

//------------------------------------------------------------------------------
// void ReadTextStars()
//------------------------------------------------------------------------------
/**
 * Read the star information from the file
 */
//------------------------------------------------------------------------------
void GLStars::ReadTextStars()
{
   wxString buffer, TxtFilename;
   Real RightAscension;
   Real Declination;
   Real VisualMag;
   Real sinRA, cosRA, cosDec;
   int i;
   int index = 0;
   Real DegToRad = M_PI / 180.0;
   GLfloat VisMagStep = 0.0f;
   const GLfloat VSScale = 0.30f;
   const GLfloat StarDimmest = 0.02f;
   const GLfloat BaseScale = 9.0f;
   const GLfloat StarRange = 2.0;
   const GLfloat RangeFactor = 0.0;
   
   FileManager *fm = FileManager::Instance();
   TxtFilename = (fm->GetFullPathname("STAR_FILE")).c_str();
   
   // Try opening the file
   wxTextFile StarsFile;
   if (!StarsFile.Open(TxtFilename))
   {
      wxString msg = "Could not open Star Catalog at ";
      msg.Append(TxtFilename);
      MessageInterface::LogMessage(msg + "\n");
      return;
   }
   
   // The first line is the north star, so it gets special treatment
   buffer = StarsFile.GetFirstLine();
   // Gets its right ascension, declination, and magnitude from the star catalog
   GetStarValues(buffer, &RightAscension, &Declination, &VisualMag);
   
   // Calculate the position of the star...
   sinRA = sin(RightAscension * DegToRad);
   cosRA = cos(RightAscension * DegToRad);
   cosDec = cos(Declination * DegToRad);
   
   // ...and store it in our array. 
   StarsVA[0][CosRAcosDec] = StarRange * cosRA * cosDec;
   StarsVA[0][SinRAcosDec] = StarRange * sinRA * cosDec;
   StarsVA[0][SinDec] = StarRange * sin(Declination * DegToRad);
   StarsVA[0][Range] = RangeFactor;
   // Store the point size and the count of its group
   PointSize[index] = 8.0f;
   GroupCount[index] = 1;
   GroupIndex[index] = 0;
   index++;
   GroupIndex[index] = 1;
   
   // Parse through the whole file and repeat this process, storing the star in the appropriate group
   for (i = 1, buffer = StarsFile.GetNextLine(); i < MAXSTARS && !StarsFile.Eof(); i++,
           buffer = StarsFile.GetNextLine())
   {
      GetStarValues(buffer, &RightAscension, &Declination, &VisualMag);
      sinRA = sin(RightAscension * DegToRad);
      cosRA = cos(RightAscension * DegToRad);
      cosDec = cos(Declination * DegToRad);
      
      StarsVA[i][CosRAcosDec] = StarRange * cosRA * cosDec;
      StarsVA[i][SinRAcosDec] = StarRange * sinRA * cosDec;
      StarsVA[i][SinDec] = StarRange * sin(Declination * DegToRad);
      StarsVA[i][Range] = RangeFactor;
      
      if (VisualMag > VisMagStep)
      {
         GroupCount[index] = i - GroupIndex[index];
         PointSize[index] = StarDimmest + ((BaseScale - VisMagStep) * VSScale);
         index++;
         if (index >= GROUPCOUNT)
            break;
         GroupIndex[index] = i;
         VisMagStep += STARSTEP;
      }
   }
   
   // Finalize the values and close the file
   GroupCount[index] = i - GroupIndex[index] - 1;
   PointSize[index] = StarDimmest + ((BaseScale - VisMagStep) * VSScale);
   LastGroupUsed = index;
   StarsFile.Close();
   
   // Store how many star positions we loaded from the file
   MaxDrawStars = i - 1;
}

//------------------------------------------------------------------------------
// void GetStarValues(wxString buffer, Real *RightAscension, Real *Declination, Real *VisualMag)
//------------------------------------------------------------------------------
/**
 * Retrieves the Right Ascension, Declination, and Visual Magnitude of the star from the
 * line in the file
 */
//------------------------------------------------------------------------------
void GLStars::GetStarValues(wxString buffer, Real *RightAscension, Real *Declination, Real *VisualMag)
{
   wxString str;
   unsigned int i = 0;
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] == '+')
         continue;
      else if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(RightAscension);
   str.Empty();
   buffer = buffer.AfterFirst(' ');
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] == '+')
         continue;
      else if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(Declination);
   str.Empty();
   buffer = buffer.AfterFirst(' ');
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] == '+')
         continue;
      else if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(VisualMag);
   str.Empty();
}

//------------------------------------------------------------------------------
// void ReadTextConstellations()
//------------------------------------------------------------------------------
/**
 * Read in the star information for the constellations.
 */
//------------------------------------------------------------------------------
void GLStars::ReadTextConstellations()
{
   wxString buffer, TxtFilename;
   Real RightAscension1, RightAscension2, Declination1, Declination2;
   Real sinRA, cosRA, cosDec;
   Real DegToRad = M_PI / 180.0;
   const GLfloat StarRange = 2.0;
   const GLfloat RangeFactor = 0.0;
   int i;
   
   FileManager *fm = FileManager::Instance();
   TxtFilename = (fm->GetFullPathname("CONSTELLATION_FILE")).c_str();
   
   // Attempt to open the constellation file
   wxTextFile ConstellationFile;
   if (!ConstellationFile.Open(TxtFilename))
   {
      wxString msg = "Could not open Constellation File at ";
      msg.Append(TxtFilename);
      MessageInterface::LogMessage(msg + "\n");
      return;
   }
   
   NumLines = 0;
   NumConstellations = 0;
   
   // Go through the file and store all of the star pairs that make up the constellation lines
   for (i = 0, buffer = ConstellationFile.GetNextLine(); i < MAXLINES && !ConstellationFile.Eof();
        buffer = ConstellationFile.GetNextLine())
   {
      // Skip the comment lines
      if (buffer[0] == '#')
         continue;
      if (buffer[0] == 'N')
      {
         ConstellationIndex[NumConstellations][1] = i-1;
         NumConstellations++;
         ConstellationNames[NumConstellations] = buffer.AfterFirst(' ');
         ConstellationIndex[NumConstellations][0] = i;
         continue;
      }
      
      // Retrieve the pair of Right Ascensions and Declinations from the line
      GetConstellationValues(buffer, &RightAscension1, &Declination1, &RightAscension2, &Declination2);
      // Convert the Right Ascensions from days to degrees
      RightAscension1 *= 15;
      RightAscension2 *= 15;

      // Calculate the star positions and store them in the array
      sinRA = sin(RightAscension1 * DegToRad);
      cosRA = cos(RightAscension1 * DegToRad);
      cosDec = cos(Declination1 * DegToRad);
      
      CLines[i][0] = StarRange * cosRA * cosDec;
      CLines[i][1] = StarRange * sinRA * cosDec;
      CLines[i][2] = StarRange * sin(Declination1 * DegToRad);
      CLines[i][3] = RangeFactor;
      
      sinRA = sin(RightAscension2 * DegToRad);
      cosRA = cos(RightAscension2 * DegToRad);
      cosDec = cos(Declination2 * DegToRad);
      
      CLines[i+1][0] = StarRange * cosRA * cosDec;
      CLines[i+1][1] = StarRange * sinRA * cosDec;
      CLines[i+1][2] = StarRange * sin(Declination2 * DegToRad);
      CLines[i+1][3] = RangeFactor;
      i+=2;
   }
   
   // Close the constellation file
   ConstellationFile.Close();
   
   // Store how many constellation line vertices we loaded in
   NumLines = i - 1;

   ConstellationIndex[NumConstellations][1] = i - 1;
}

//------------------------------------------------------------------------------
// void GetConstellationValues(wxString buffer, Real *RightAscension1, 
//         Real *Declination1, Real *RightAscension2, Real *Declination2)
//------------------------------------------------------------------------------
/**
 * Parse the line from the file and extract the pair of Right Ascensions and Declinations
 */
//------------------------------------------------------------------------------
void GLStars::GetConstellationValues(wxString buffer, Real *RightAscension1, 
                                     Real *Declination1, Real *RightAscension2,
                                     Real *Declination2)
{
   wxString str;
   unsigned int i = 0;
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(Declination1);
   str.Empty();
   buffer = buffer.AfterFirst(' ');
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(RightAscension1);
   str.Empty();
   buffer = buffer.AfterFirst(' ');
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(Declination2);
   str.Empty();
   buffer = buffer.AfterFirst(' ');
   
   while (buffer[0] == ' ')
      buffer = buffer.AfterFirst(' ');
   
   for (i = 0; i < buffer.Length(); i++)
   {
      if (buffer[i] != '\n' && buffer[i] != ' ')
         str.Append(buffer[i]);
      else 
         break;
   }
   
   str.ToDouble(RightAscension2);
   str.Empty();
   buffer = buffer.AfterFirst(' ');
}

//------------------------------------------------------------------------------
// void InitStars()
//------------------------------------------------------------------------------
/**
 * Initialize the stars and constellations, reading them in from the files.
 */
//------------------------------------------------------------------------------
void GLStars::InitStars()
{
   SetDesiredStarCount(42000);
   
   ReadTextStars();
   ReadTextConstellations();
   if (MaxDrawStars > DesiredStarCount)
      MaxDrawStars = DesiredStarCount;
}

//------------------------------------------------------------------------------
// void DrawStarsVA(GLfloat ColorAlpha, int starCount, bool drawConstellations)
//------------------------------------------------------------------------------
/**
 * Draw the stars and the constellation lines, if you want them.
 */
//------------------------------------------------------------------------------
void GLStars::DrawStarsVA(GLfloat ColorAlpha, int starCount, bool drawConstellations)
{
   // The colors for the stars, north star, and constellation lines
   SetDesiredStarCount(starCount);
   GLfloat StarWhite[4] = {1.0f, 1.0f, 1.0f, 1.0f};
   GLfloat StarBlue[4] = {0.2f, 0.2f, 1.0f, 1.0f};
   GLfloat LineWhite[4] = {1.0f, 1.0f, 1.0f, 0.6f};
   
   // Set the alpha value
   StarWhite[3] = ColorAlpha;
   StarBlue[3] = ColorAlpha;
   
   // Store the lighting bit. We need a special lighting so the stars and lines will show.
   glPushAttrib(GL_LIGHTING_BIT);
   glLightfv(GL_LIGHT0, GL_AMBIENT, SOURCELIGHT99);
   
   // Anti-aliasing. 
   glEnable(GL_POINT_SMOOTH);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
   
   // We want to use vertex arrays for efficiency
   glEnableClientState(GL_VERTEX_ARRAY);
   glDisable(GL_DEPTH_TEST);
   // Point to our array of star vertices
   glVertexPointer(4, GL_DOUBLE, 0, StarsVA);

   // First the north star
   glColor4fv(StarBlue);
   
   // Make it big so it stands out
   glPointSize(8.0f);
   // Draw the north star only
   glDrawArrays(GL_POINTS, 0, 1);
   int TotalDrawnStars = 1;
   
   // The rest of the stars are white
   glColor4fv(StarWhite);
   // Point again to the stars array
   glVertexPointer(4, GL_DOUBLE, 0, StarsVA);
   
   // Iterate through the groups and draw them all
   for (int i = 1; i <= LastGroupUsed; i++)
   {
      // Groups are organized by visual magnitude
      glPointSize(PointSize[i]);
      // Draw them all
      glDrawArrays(GL_POINTS, GroupIndex[i], GroupCount[i]);
      // Count the stars we've drawn so far
      TotalDrawnStars += GroupCount[i];
      // If we start getting too many, stop
      if (TotalDrawnStars >= DesiredStarCount || TotalDrawnStars >= MaxDrawStars)
         break;
   }
   
   // Draw the constellation lines if the user wants them
   if (drawConstellations)
   {
      // Set the color
      glColor4fv(LineWhite);
      // Point to the vertex array
      glVertexPointer(4, GL_FLOAT, 0, CLines);
      // Enable smoothing and make these lines thin
      glEnable(GL_LINE_SMOOTH);
      glLineWidth(0.3f);
      // Draw the lines
      glDrawArrays(GL_LINES, 0, NumLines);
      
      for (int i = 1; i < NumConstellations; i++)
      {
         GLfloat w = CLines[ConstellationIndex[i][0]][3];
         if (w == 0.0) w = 0.00001f;
         DrawStringAt(ConstellationNames[i],
                      CLines[ConstellationIndex[i][0]][0],
                      CLines[ConstellationIndex[i][0]][1],
                      CLines[ConstellationIndex[i][0]][2],
                      w);
      }
   }
   
   glEnable(GL_DEPTH_TEST);
   // Disable vertex arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   
   // Disable blending
   glDisable(GL_BLEND);
   // Pop the lighting bit back
   glPopAttrib();
}
