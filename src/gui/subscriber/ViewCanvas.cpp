//$Id$
//------------------------------------------------------------------------------
//                              ViewCanvas
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
// Created: 2010/04/19
/**
 * Implements ViewCanvas for 3D visualization.
 */
//------------------------------------------------------------------------------
#include "ViewCanvas.hpp"


//------------------------------------------------------------------------------
// ViewCanvas(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <parent>   parent window pointer
 * @param <id>       window id
 * @param <pos>      position (top, left) where the window to be placed 
 * @param <size>     size of the window
 * @param <name>     title of window
 * @param <style>    style of window
 *
 */
//------------------------------------------------------------------------------
ViewCanvas::ViewCanvas(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       const wxString& name, long style)
   #ifdef __USE_WX280_GL__
   : wxGLCanvas(parent, id, 0, pos, size, style, name)
   #else
   : wxGLCanvas(parent, id, pos, size, style, name)
   #endif
{
}


//------------------------------------------------------------------------------
// ~ViewCanvas()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ViewCanvas::~ViewCanvas()
{
}


//------------------------------------------------------------------------------
// Rvector3 ComputeEulerAngles()
//------------------------------------------------------------------------------
Rvector3 ViewCanvas::ComputeEulerAngles()
{
   Rvector3 modAngle;

   #ifndef COMPUTE_EULER_ANGLE
   return modAngle;

   
   #else
   
   bool error = false;
   Rvector3 eulerAngle;
   Rmatrix33 finalMat;
   
   #ifdef USE_MODELVIEW_MAT
   {   
      // model view matrix
      static GLfloat sViewMat[16];
      
      glGetFloatv(GL_MODELVIEW_MATRIX, sViewMat);
      
      // OpenGL stores matrix in column major: ith column, jth row.
      
      Rmatrix33 mvmat(sViewMat[0], sViewMat[1], sViewMat[2],
                      sViewMat[4], sViewMat[5], sViewMat[6],
                      sViewMat[8], sViewMat[9], sViewMat[10]);
      
      finalMat = mvmat;
      
      #ifdef DEBUG_TRAJCANVAS_EULER
      MessageInterface::ShowMessage
         ("ViewCanvas::ComputeEulerAngles() sViewMat=\n"
          "   %f, %f, %f, %f\n   %f, %f, %f, %f\n"
          "   %f, %f, %f, %f\n   %f, %f, %f, %f\n",
          sViewMat[0], sViewMat[1], sViewMat[2], sViewMat[3],
          sViewMat[4], sViewMat[5], sViewMat[6], sViewMat[7],
          sViewMat[8], sViewMat[9], sViewMat[10], sViewMat[11],
          sViewMat[12], sViewMat[13], sViewMat[14], sViewMat[15]);
      MessageInterface::ShowMessage
         ("ViewCanvas::ComputeEulerAngles() mvmat=%s\n",
          mvmat.ToString().c_str());
      MessageInterface::ShowMessage
         ("ViewCanvas::ComputeEulerAngles() finalMat=%s\n",
          finalMat.ToString().c_str());
      #endif
   }
   #else
   {   
      Rvector3 upAxis((Real)mfUpXAxis, (Real)mfUpYAxis, (Real)mfUpZAxis);
      Rvector3 rotAxis = Rvector3((Real)mfCamRotXAxis, (Real)mfCamRotYAxis,
                                  (Real)mfCamRotZAxis);
      
      #ifdef DEBUG_TRAJCANVAS_EULER
      MessageInterface::ShowMessage
         ("ViewCanvas::ComputeEulerAngles() mfUpAngle=%f, upAxis=%s\n",
          mfUpAngle, upAxis.ToString().c_str());
      
      MessageInterface::ShowMessage
         ("ViewCanvas::ComputeEulerAngles() mfCamSingleRotAngle=%f, rotAxis=%s\n",
          mfCamSingleRotAngle, rotAxis.ToString().c_str());
      #endif
      
      Rmatrix33 upMat, rotMat;
      
      try
      {
         // compute cosine matrix
         if (upAxis.GetMagnitude() > 0.0)
            upMat = GmatAttUtil::ToCosineMatrix(mfUpAngle * RAD_PER_DEG, upAxis);
         
         if (rotAxis.GetMagnitude() > 0.0)
            rotMat = GmatAttUtil::ToCosineMatrix(mfCamSingleRotAngle * RAD_PER_DEG, rotAxis);
         
         //finalMat = rotMat * upMat;
         finalMat = upMat * rotMat;
         
         #ifdef DEBUG_TRAJCANVAS_EULER
         MessageInterface::ShowMessage
            ("ViewCanvas::ComputeEulerAngles() \n  rotMat=%s  upMat=%s  "
             "finalMat=%s\n", rotMat.ToString().c_str(),
             upMat.ToString().c_str(), finalMat.ToString().c_str());
         #endif
         
      }
      catch (BaseException &e)
      {
         error = true;
         MessageInterface::ShowMessage
            ("*** ERROR *** ViewCanvas::ComputeEulerAngles() %s\n",
             e.GetFullMessage().c_str());
      }
   }
   #endif
   
   if (error)
      return modAngle;
   
   
   try
   {
      // convert finalMat to Euler angle
      if (mRotateAboutXaxis)
         eulerAngle = GmatAttUtil::ToEulerAngles(finalMat, 2, 3, 1);
      else if (mRotateAboutYaxis)
         eulerAngle = GmatAttUtil::ToEulerAngles(finalMat, 3, 1, 2);
      else
         eulerAngle = GmatAttUtil::ToEulerAngles(finalMat, 1, 2, 3);
            
      eulerAngle = eulerAngle * DEG_PER_RAD;
      
      #ifdef DEBUG_TRAJCANVAS_EULER
      MessageInterface::ShowMessage
         ("ViewCanvas::ComputeEulerAngles() eulerAngle=%s\n",
          eulerAngle.ToString().c_str());
      #endif
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** ViewCanvas::ComputeEulerAngles() %s\n",
          e.GetFullMessage().c_str());
   }
   
   modAngle = eulerAngle;
   
   //loj: 9/29/05
   // How can I compute modified rotation angle in general way?
   
   if (eulerAngle.GetMagnitude() == 0.0)
   {
      if (mRotateAboutXaxis) // 2-3-1 (Default)
      {
         modAngle[0] = -90 - 270;
         modAngle[1] = 90;
         modAngle[2] = 0;
      }
   }
   else
   {
      if (mRotateAboutXaxis) // 2-3-1 (Default)
      {
         if (eulerAngle(2) == 0.0)
         {
            if (mUseGluLookAt)
            {
               modAngle[0] = eulerAngle(0) + 90;
               modAngle[1] = eulerAngle(2);
               modAngle[2] = eulerAngle(1) + 180;
            }
            else
            {
               modAngle[0] = eulerAngle(0) - 270;
               modAngle[1] = eulerAngle(2);
               modAngle[2] = eulerAngle(1);
            }
         }
         else
         {
            if (mUseGluLookAt)
            {
               modAngle[0] = eulerAngle(0);
               modAngle[1] = eulerAngle(2) - 180;;
               modAngle[2] = eulerAngle(1);
            }
            else
            {
               modAngle[0] = eulerAngle(0);
               modAngle[1] = eulerAngle(2) - 90;
               modAngle[2] = eulerAngle(1);
            }
         }
      }
      else if (mRotateAboutZaxis) // 1-2-3
      {
         if (eulerAngle(2) == 0.0)
         {
            modAngle[0] = eulerAngle(0);
            modAngle[1] = eulerAngle(1) - 90;
            modAngle[2] = eulerAngle(2) + 90;
         }
         else
         {
            modAngle[0] = eulerAngle(0) - 180;
            modAngle[1] = eulerAngle(1);
            modAngle[2] = eulerAngle(2) - 90;
         }
      }
   }
   
   #ifdef DEBUG_TRAJCANVAS_EULER
   MessageInterface::ShowMessage
      ("ViewCanvas::ComputeEulerAngles() modAngle=%s\n",
       modAngle.ToString().c_str());
   #endif

   return modAngle;

   #endif
}


//---------------------------------------------------------------------------
//  void CopyVector3(Real to[3], Real from[3])
//---------------------------------------------------------------------------
void ViewCanvas::CopyVector3(Real to[3], Real from[3])
{
   to[0] = from[0];
   to[1] = from[1];
   to[2] = from[2];
}


//---------------------------------------------------------------------------
// bool LoadImage(const std::string &fileName)
//---------------------------------------------------------------------------
bool ViewCanvas::LoadImage(const std::string &fileName)
{
#ifndef SKIP_DEVIL
   return false;
   
#else
   #if DEBUG_TRAJCANVAS_INIT
   MessageInterface::ShowMessage
      ("ViewCanvas::LoadImage() Not using DevIL. file='%s'\n", fileName.c_str());
   #endif
   
   if (fileName == "")
      return false;

   // Moved to GmatApp (loj: 2009.01.08)
   //::wxInitAllImageHandlers();
   
   wxImage image = wxImage(fileName.c_str());
   int width = image.GetWidth();
   int height = image.GetHeight();
   
   GLubyte *data = image.GetData();
   
   if (data == NULL)
      return false;
   
   #if DEBUG_TRAJCANVAS_INIT
   int size = width * height * 3;
   MessageInterface::ShowMessage
      ("   width=%d, height=%d, size=%d\n", width, height, size);
   #endif
   
   // Why is image upside down?
   // Get vertial mirror
   wxImage mirror = image.Mirror(false);
   GLubyte *data1 = mirror.GetData();
   
   //used for min and magnifying texture
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   
   //pass image to opengl
   #ifndef __WXGTK__
      // This call crashes GMAT on Linux, so it is excluded here. 
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                     GL_UNSIGNED_BYTE, data1);
   #endif
   
   return true;
#endif
}

