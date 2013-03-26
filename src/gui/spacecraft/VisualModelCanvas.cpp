//$Id$
//------------------------------------------------------------------------------
//                            VisualModelCanvas
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC, Thinking Systems, Inc., and Schafer Corp.,
// under AFRL NOVA Contract #FA945104D03990003
//
// Author: Phillip Silvia
// Created: 2009/08/13
// Modified: Dunn Idle
// Date:     2010/07/21
// Changes:  Made Earth Axes and Labels Positive, New RGB Color Scheme
/**
* These classes and methods implement OpenGL display of the earth and
* orbit trajectories.  The upgrade by Schafer allows 3D spacecraft
* models to be drawn in the correct position and attitude.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "ColorTypes.hpp"          // for GmatColor::
#include "Camera.hpp"
#include "Light.hpp"
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
#include "VisualModelCanvas.hpp"
#include "MessageInterface.hpp"
#include "Rendering.hpp"
#include "ModelManager.hpp"
#include "ViewCanvas.hpp"          // for static int GmatGLCanvasAttribs[2]
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()


BEGIN_EVENT_TABLE(VisualModelCanvas, wxGLCanvas)
   EVT_PAINT(VisualModelCanvas::OnPaint)
   EVT_MOUSE_EVENTS(VisualModelCanvas::OnMouse)
   EVT_KEY_DOWN(VisualModelCanvas::OnKeyDown)
END_EVENT_TABLE()


//#define DEBUG_LOAD_MODEL
//#define DEBUG_ON_PAINT
//#define DEBUG_GL_CONTEXT


//------------------------------------------------------------------------------
// VisualModelCanvas(wxWindow *parent, Spacecraft *spacecraft, ...)
//------------------------------------------------------------------------------
VisualModelCanvas::VisualModelCanvas(wxWindow *parent, Spacecraft *spacecraft,
   const wxWindowID id, const wxPoint &pos, const wxSize &size, const wxString &name, long style)
   #ifdef __WXMSW__
   // Constructor with explicit wxGLContext with default GL attributes
   // It is getting pixel format error with GmatGLCanvasAttribs on Windows, so set to default.
   : wxGLCanvas(parent, id, 0, pos, size, style, name)
   #elif __WXMAC__
   // Constructor with implicit wxGLContext with default GL attributes
   : wxGLCanvas(parent, id, pos, size, style, name)
   #elif __linux
   // Constructor with explicit wxGLContext
   // Double buffer activation needed in Linux (Patch from Tristan Moody)
   : wxGLCanvas(parent, id, ViewCanvas::GmatGLCanvasAttribs, pos, size, style, name)
   #else
   // Constructor with explicit wxGLContext with default GL attributes
   : wxGLCanvas(parent, id, 0, pos, size, style, name)
   #endif
{
   #ifdef DEBUG_MODEL_CANVAS
   MessageInterface::ShowMessage
      ("\nVisualModelCanvas() constructor entered, spacecraft=<%p>\n", spacecraft);
   #endif
   
   vParent = parent;   
   currentSpacecraft = spacecraft;
   lastMouseX = 0;
   lastMouseY = 0;
   
   mCamera.Relocate(15000.0f, 15000.0f, 15000.0f, 0.0f, 0.0f, 0.0f);
   mLight.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
   mLight.SetPosition(0.01f, 1.0f, 0.3f);
   mLight.SetDirectional(true);
   
   glLightfv(GL_LIGHT0, GL_SPECULAR, mLight.GetColor());
   
   // enable the light
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   showEarth = false;
   needToLoadModel = false;
   glInitialized = false;
   recentered = false;
   
   glClearColor(0.0, 0.0, 0.0, 1);
   // Clear the color and depth bits
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//------------------------------------------------------------------------------
// ~VisualModelCanvas()
//------------------------------------------------------------------------------
VisualModelCanvas::~VisualModelCanvas()
{
   // @note - LOJ: 2012.07.24
   // Do not delete theContext here since it is shared with other GL plots.
}

//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent &event)
//------------------------------------------------------------------------------
/**
 * Paints the canvas. Has A LOT of work.
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::OnPaint(wxPaintEvent &event)
{
   #ifdef DEBUG_ON_PAINT
   MessageInterface::ShowMessage
      ("VisualModelCanvas::OnPaint() entered, glInitialized=%d, needToLoadModel=%d\n",
       glInitialized, needToLoadModel);
   #endif
   
   float offset[3] = {0.0f, 0.0f, 0.0f}, rotation[3] = {0.0f, 0.0f, 0.0f}, scale;
   
   // Set the drawing context
   wxPaintDC dc(this);
   
   if (!SetGLContext())
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** Cannot set GL context in VisualModelCanvas::OnPaint()\n");
      return;
   }
   
   // Initialize OpenGL if it hasn't been initialized yet
   if (!glInitialized)
   {
      InitGL();
      glInitialized = true;
   }
   
   // Loading the model needs to be done within the OpenGL Context, so we do it here
   if (needToLoadModel)
      LoadModel();
   
   // Set OpenGL to recognize the counter clockwise defined side of a polygon
   // as its 'front' for lighting and culling purposes
   glFrontFace(GL_CCW);
   
   // Enable face culling, so that polygons facing away (defined by front face)
   // from the viewer aren't drawn (for efficiency).
   // Tell OpenGL to use glColor() to get material properties for
   glEnable(GL_COLOR_MATERIAL);
   // Set both front and back material parameters and ambient and diffuse material
   // parameters to track the current color 
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
   // Set the clear color to black
   glClearColor(0.0, 0.0, 0.0, 1);
   // Clear the color and depth bits
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   // Set up the viewport
   GLint h = GetSize().GetHeight(), w = GetSize().GetWidth();
   glViewport(0, 0, w, h);
   // Set up the projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float aspect = 300.0f / 350.0f;
   gluPerspective(mCamera.fovDeg, aspect, 50.0f, 50000000.0f);
   // Set up the camera's matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(mCamera.position[0], mCamera.position[1], mCamera.position[2],
             0, 0, 0,
             mCamera.up[0], mCamera.up[1], mCamera.up[2]);
   
   // Set up the light and enable lighting
   float lpos[4];
   mLight.GetPositionf(lpos);
   glLightfv(GL_LIGHT0, GL_POSITION, lpos);
   glLightfv(GL_LIGHT0, GL_SPECULAR, mLight.GetColor());
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   
   // Draw the model
   if (currentSpacecraft->modelID == -1)
   {
      GlColorType *red = (GlColorType*)&GmatColor::RED32,
         *yellow = (GlColorType*)&GmatColor::YELLOW32;
      DrawSpacecraft(198, red, yellow);
   }
   else
   {
      offset[0] = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetX"));
      offset[1] = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetY"));
      offset[2] = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelOffsetZ"));
      rotation[0] = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelRotationX"));
      rotation[1] = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelRotationY"));
      rotation[2] = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelRotationZ"));
      scale = currentSpacecraft->GetRealParameter(currentSpacecraft->GetParameterID("ModelScale"));
      loadedModel->SetBaseOffset(offset[0], offset[1], offset[2]);
      // Dunn changed the subscript below from [3] to [2]
      loadedModel->SetBaseRotation(true, rotation[0], rotation[1], rotation[2]);
      loadedModel->SetBaseScale(scale, scale, scale);
      loadedModel->Draw(true);
   }
   
   glDisable(GL_LIGHTING);
   // Draw the axes
   DrawAxes();
   if (showEarth)
   {
      // Draw a wireframe earth for size and location reference
      glColor3f(0.20f, 0.20f, 0.50f);
      // @todo - do we need a pointer to any actual CelestialBody object here, to get current EqRadius?
      //         (and for other two places where radius is set, below)
      DrawSphere(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH], 15, 15, GLU_LINE);
      glDisable(GL_TEXTURE_2D);
   }
   // Complete the call
   glFlush();
   SwapBuffers();

   #ifdef DEBUG_ON_PAINT
   MessageInterface::ShowMessage
      ("VisualModelCanvas::OnPaint() leaving, needToLoadModel=%d\n", needToLoadModel);
   #endif
   
}

//------------------------------------------------------------------------------
// void OnMouse(wxMouseEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes all mouse controls. The mouse controls can move the camera around.
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::OnMouse(wxMouseEvent &event)
{
   float mouseX = event.m_x, mouseY = event.m_y;
   if (event.Dragging())
   {
      // Rotates the camera around the center when the left mouse button is used
      if (event.LeftIsDown())
      {
         float angleX = (lastMouseX - mouseX) / 300.0;
         float angleY = (lastMouseY - mouseY) / 300.0;
         mCamera.Rotate(angleX, angleY, 0.0, false, true);
         Refresh(false);
      }
      // Zooms the camera when the right mouse button is used
      if (event.RightIsDown())
      {
         Real x2 = (lastMouseX - mouseX) * (lastMouseX - mouseX);
         Real y2 = (mouseY - lastMouseY) * (mouseY - lastMouseY);
         Real length = sqrt(x2 + y2);

         Real distance = (mCamera.view_center - mCamera.position).GetMagnitude();
         
         Real zoom = length * distance / 500;
         if (mouseY < lastMouseY)
            mCamera.Translate(0, 0, -zoom, false);
         else
            mCamera.Translate(0, 0, zoom, false);
         Refresh(false);
      }
      if (event.MiddleIsDown())
      {
         float roll = (mouseY - lastMouseY) / 400.0;
         mCamera.Rotate(0.0, 0.0, roll, false, true);
         Refresh(false);
      }
   }
   // Record the mouse position
   lastMouseX = mouseX;
   lastMouseY = mouseY;
}

//------------------------------------------------------------------------------
// void OnKeyDown(wxKeyEvent &event)
//------------------------------------------------------------------------------
/**
 * Processes keyboard events.
 * @note No keyboard commands are implemented.
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::OnKeyDown(wxKeyEvent &event)
{
}

//---------------------------------------------------------------------------
// void DrawAxes()
//---------------------------------------------------------------------------
/**
 * Draws the axes.
 */
//---------------------------------------------------------------------------
void VisualModelCanvas::DrawAxes()
{
   wxString axisLabel;
   GLfloat viewDist;

   glLineWidth(2.0);

   //------------------------------------------------------------------------
   // Draw Desired Model Body Axes - Note from Dunn.  The rendered model will
   // rotate with respect to these axes.  These are the axes that are used to
   // define an attitude offset in the Spacecraft Attitude tab using CSFixed
   // mode.
   //------------------------------------------------------------------------

   viewDist = (float) GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]; // stays the same
   
   Rvector3 axis;
   Rvector3 origin;
   origin.Set(0, 0, 0);
   
   // PS - See Rendering.cpp
   // Note from Dunn.  The signs for these axes were changed after the big fix
   // in Enhanced3DViewCanvas where we removed all the minus signs from X and Y
   // trajectory, sun-line, and orbit normal location variables.  Also the
   // color convention for the three ECI axes was changed to RGB after the
   // development team reached consensus.
   //
   // Set X-Axis
   axis.Set(viewDist, 0, 0);
   DrawLine(1, 0, 0, origin, axis);

   // Set Y-Axis
   axis.Set(0, viewDist, 0);
   DrawLine(0, 1, 0, origin, axis);

   // Set Z-Axis
   axis.Set(0, 0, viewDist);
   DrawLine(0, 0, 1, origin, axis);
   
   //-----------------------------------
   // throw some text out...
   //-----------------------------------

   // Label X-Axis
   glColor3f(1, 0, 0);     // red
   axisLabel = "+X ";
   DrawStringAt(axisLabel, +viewDist, 0.0, 0.0, 1.0);

   // Label Y-Axis
   glColor3f(0, 1, 0);     // green
   axisLabel = "+Y ";
   DrawStringAt(axisLabel, 0.0, +viewDist, 0.0, 1.0);
   
   // Label Z-Axis
   glColor3f(0, 0, 1);     // blue
   axisLabel = "+Z ";
   DrawStringAt(axisLabel, 0.0, 0.0, +viewDist, 1.0);
   
   glLineWidth(1.0);
}

//------------------------------------------------------------------------------
// void Rotate(bool useDegrees, float xAngle, float yAngle, float zAngle)
//------------------------------------------------------------------------------
/**
 * Rotates the model around the given axis.
 *
 * @param useDegrees True if the values are degrees, false if they are radians
 * @param xAngle The x rotation
 * @param yAngle The y rotation
 * @param zAngle The z rotation
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::Rotate(bool useDegrees, float xAngle, float yAngle, float zAngle)
{
   if (needToLoadModel)
      LoadModel();
   if (currentSpacecraft->modelID != -1 && loadedModel->IsLoaded())
      loadedModel->SetBaseRotation(useDegrees, xAngle, yAngle, zAngle);
   Refresh(false);
}

//------------------------------------------------------------------------------
// void Translate(float x, float y, float z)
//------------------------------------------------------------------------------
/**
 * Translate the model along the given axes.
 *
 * @param x Translation along the x-axis
 * @param y Translation along the y-axis
 * @param z Translation along the z-axis
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::Translate(float x, float y, float z)
{
   if (needToLoadModel)
      LoadModel();
   if (currentSpacecraft->modelID != -1 && loadedModel->IsLoaded())
      loadedModel->SetBaseOffset(x,y,z);
   Refresh(false);
}

//------------------------------------------------------------------------------
// void Scale(float xScale, float yScale, float zScale)
//------------------------------------------------------------------------------
/**
 * Scales the model along the given axes. Tends to be given a universal value,
 * but you never know what people might want.
 *
 * @param xScale The scale factor along the x-axis
 * @param yScale The scale factor along the y-axis
 * @param zScale The scale factor along the z-axis
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::Scale(float xScale, float yScale, float zScale)
{
   if (needToLoadModel)
      LoadModel();
   if (currentSpacecraft->modelID != -1 && loadedModel->IsLoaded())
      loadedModel->SetBaseScale(xScale, yScale, zScale);
   Refresh(false);
}

//------------------------------------------------------------------------------
// void RecenterModel()
//------------------------------------------------------------------------------
/**
 * Recenters the model based on its axis-aligned bounding box.
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::RecenterModel(float *offset)
{
   #ifdef DEBUG_RECENTER
   MessageInterface::ShowMessage
      ("RecenterModel() entered, offset=[%f, %f, %f]\n", offset[0], offset[1], offset[2]);
   #endif
   
   if (needToLoadModel)
      LoadModel();

   #ifdef DEBUG_RECENTER
   MessageInterface::ShowMessage
      ("   modelID=%d, isLoaded=%d\n", currentSpacecraft->modelID, loadedModel->isLoaded);
   #endif
   
   if (currentSpacecraft->modelID != -1 && loadedModel->IsLoaded())
   {
      vector_type bsphere_center = loadedModel->GetBSphereCenter();
      float x = -bsphere_center.x;
      float y = -bsphere_center.y;
      float z = -bsphere_center.z;
      offset[0] = x;
      offset[1] = y;
      offset[2] = z;
      loadedModel->SetBaseOffset(x, y, z);
      recentered = true;
   }
   
   Refresh(false);
   
   #ifdef DEBUG_RECENTER
   MessageInterface::ShowMessage
      ("RecenterModel() leaving, offset=[%f, %f, %f]\n", offset[0], offset[1], offset[2]);
   #endif
}

//------------------------------------------------------------------------------
// void AutoscaleModel()
//------------------------------------------------------------------------------
/**
 * Rescales the model based on the Earth and model radius.
 */
//------------------------------------------------------------------------------
float VisualModelCanvas::AutoscaleModel()
{
   float earthRadius = (float) GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH];
   float modelRadius = loadedModel->GetBSphereRadius();

   #ifdef DEBUG_AUTO_SCALE
   MessageInterface::ShowMessage
      ("VisualModelCanvas::AutoscaleModel() loadedModel=<%p>, modelRadius=%f\n",
       loadedModel, modelRadius);
   #endif
   
   return earthRadius / (5.0f * modelRadius);
}

//------------------------------------------------------------------------------
// void LoadModel(const wxString &filePath)
//------------------------------------------------------------------------------
/**
 * Sets up the flags to loaded the given model.
 */
//------------------------------------------------------------------------------
bool VisualModelCanvas::LoadModel(const wxString &filePath)
{
   #ifdef DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("VisualModelCanvas::LoadModel(filePath) entered, filePath='%s'\n", filePath.c_str());
   #endif
   
   if (GmatFileUtil::DoesFileExist(filePath.c_str()))
   {
      modelPath = filePath;
      needToLoadModel = true;
      Refresh(false);
      return true;
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** The model file '%s' does not exist. Please check the path.\n",
          filePath.c_str());
      return false;
   }
}

//------------------------------------------------------------------------------
// void LoadModel()
//------------------------------------------------------------------------------
/**
 * Actually loads model with saved member data modelPath.
 */
//------------------------------------------------------------------------------
void VisualModelCanvas::LoadModel()
{
   #ifdef DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("VisualModelCanvas::LoadModel() entered, modelPath='%s'\n", modelPath.c_str());
   #endif
   ModelManager *mm = ModelManager::Instance();
   std::string mP = modelPath.c_str();
   currentSpacecraft->modelID = mm->LoadModel(mP);
   loadedModel = mm->GetModel(currentSpacecraft->modelID);
   needToLoadModel = false;
   #ifdef DEBUG_LOAD_MODEL
   MessageInterface::ShowMessage
      ("VisualModelCanvas::LoadModel() leaving, loadedModel=<%p>\n", loadedModel);
   #endif
}

//------------------------------------------------------------------------------
// bool SetGLContext()
//------------------------------------------------------------------------------
/**
 * Creates GL context if not already created and sets to ModelManager to share
 * with other GL canvas. 
 *
 * @return true if GL context is not NULL, false otherwise
 */
//------------------------------------------------------------------------------
bool VisualModelCanvas::SetGLContext()
{
   #ifdef DEBUG_GL_CONTEXT
   MessageInterface::ShowMessage
      ("VisualModelCanvas::SetGLContext() entered, theContext=<%p>\n", theContext);
   #endif
   
   bool retval = false;
   
   #ifndef __WXMAC__
      ModelManager *mm = ModelManager::Instance();
      if (!mm->GetSharedGLContext())
      {
         wxGLContext *glContext = new wxGLContext(this);
         #ifdef DEBUG_GL_CONTEXT
         MessageInterface::ShowMessage
            ("   Setting new wxGLContext(this)<%p> to ModelManager::theContext\n", glContext);
         #endif
         mm->SetSharedGLContext(glContext);
      }
      
      // Use the shared context from the ModelManager
      theContext = mm->GetSharedGLContext();
      if (theContext)
      {
         theContext->SetCurrent(*this);
         retval = true;
      }
   #else
      // Use implicit GL context on Mac
      theContext = GetContext();
      SetCurrent();
      retval = true;
   #endif
      
      
   #ifdef DEBUG_GL_CONTEXT
   MessageInterface::ShowMessage
      ("VisualModelCanvas::SetGLContext() returning %d, theContext=<%p>\n",
       retval, theContext);
   #endif
   
   return retval;
}

