//$Header$
//------------------------------------------------------------------------------
//                              TrajPlotCanvas
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Implements TrajPlotCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "TrajPlotCanvas.hpp"
#include "TextTrajectoryFile.hpp"

#include "wx/glcanvas.h"

#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/gl.h>
#    include <OpenGL/glu.h>
#  else
#    include <gl.h>
#    include <glu.h>
#  endif
#else
#  include <gl/gl.h>
#  include <gl/glu.h>
#  include <il/il.h>
#  include <il/ilu.h>
#  include <il/ilut.h>
#endif

BEGIN_EVENT_TABLE(TrajPlotCanvas, wxGLCanvas)
    EVT_SIZE(TrajPlotCanvas::OnSize)
    EVT_PAINT(TrajPlotCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TrajPlotCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(TrajPlotCanvas::OnMouse)
END_EVENT_TABLE()

enum TFrameMode
{
   GCI_FRAME,
   SOL_ROT_FRAME,
   SELENOCENTRIC_FRAME,
   EARTH_MOON_ROT_FRAME,
   HELIOCENTRIC_FRAME,
   ANY_BODY_FRAME,
   ANY_BODY_ROT_FRAME
};

enum BodyType
{
    SUN = 0,
    MERCURY,
    VENUS,
    EARTH,
    MARS,
    JUPITER,
    SATURN,
    NEPTUNE,
    PLUTO,
    MOON
};
    
struct GlColorType
{
   Byte red;
   Byte green;
   Byte blue;
   Byte not_used;
};

const int BLACK32 = 0x00000000;
const int WHITE32 = 0x00ffffff;
const int RED32   = 0x000000ff;
const int GREEN32 = 0x00008000;
const int GRAY32  = 0x00808080;

//----------------------------
//  static data
//----------------------------
static int *sIntColor = new int;
static GlColorType *sGlColor = (GlColorType*)sIntColor;

//------------------------------------------------------------------------------
// TrajPlotCanvas(wxWindow *parent, wxWindowID id,
//                const wxPoint& pos, const wxSize& size,
//                long style, const wxString& name)
//------------------------------------------------------------------------------
TrajPlotCanvas::TrajPlotCanvas(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style, name)
{    
    // initalize data members
    mTextTrajFile = NULL;
    mInitialized = false;
    mNumData = 0;

    // view
    mCanvasSize = size;
    mDefaultViewX = 90.0;
    mDefaultViewY = 0.0;
    mDefaultViewZ = 0.0;
    mDefaultViewDist = 30000;
    
    mCurrViewX = mDefaultViewX;
    mCurrViewY = mDefaultViewY;
    mCurrViewZ = mDefaultViewZ;
    mCurrViewDist = mDefaultViewDist;
    
    m_fCamTransX = 0;
    m_fCamTransY = 0;
    m_fCamTransZ = 0;

    mAxisLength = mCurrViewDist;
    mCurrViewFrame = GCI_FRAME;
    mPivotBody = EARTH;
    mCurrBody = EARTH;
    mMaxZoomIn = MAX_EARTH_ZOOM_IN;
    mRotateAboutXaxis = true;
    mRotateAboutYaxis = false;
    mRotateAboutZaxis = false;
    mRotateXy = true;

    mZoomAmount = 300.0;
    
    // projection
    ChangeProjection(size.x, size.y, mAxisLength);
        
    mEarthRadius = 6378.14; //km
    mShowWireFrame = false;
    mShowEquatorialPlane = true;
    mUseTexture = true;
    mEquatorialPlaneColor = GRAY32;
    
    for (int i=0; i<MAX_BODIES; i++)
    {
        mTextureIndex[i] = 999;
    }

}

//------------------------------------------------------------------------------
// ~TrajPlotCanvas()
//------------------------------------------------------------------------------
TrajPlotCanvas::~TrajPlotCanvas()
{
    if (mTextTrajFile)
        delete mTextTrajFile;
}

//------------------------------------------------------------------------------
// void ShowDefaultView()
//------------------------------------------------------------------------------
void TrajPlotCanvas::ShowDefaultView()
{
    int clientWidth, clientHeight;
    GetClientSize(&clientWidth, &clientHeight);

    mCurrViewX = mDefaultViewX;
    mCurrViewY = mDefaultViewY;
    mCurrViewZ = mDefaultViewZ;
    mCurrViewDist = mDefaultViewDist;
    mAxisLength = mCurrViewDist;
    m_fCamTransX = 0;
    m_fCamTransY = 0;
    m_fCamTransZ = 0;

    ChangeView(mCurrViewX, mCurrViewY, mCurrViewZ);
    ChangeProjection(clientWidth, clientHeight, mAxisLength);
    Refresh(false);
}

//------------------------------------------------------------------------------
// void ZoomIn()
//------------------------------------------------------------------------------
void TrajPlotCanvas::ZoomIn()
{
    double realDist = (mAxisLength - mZoomAmount) / log(mAxisLength);
    mAxisLength = mAxisLength - realDist;

    //mAxisLength = mAxisLength - mZoomAmount;
    
    if (mAxisLength < mMaxZoomIn)
        mAxisLength = mMaxZoomIn;

    ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);

    Refresh(false);
}

//------------------------------------------------------------------------------
// void ZoomOut()
//------------------------------------------------------------------------------
void TrajPlotCanvas::ZoomOut()
{
    // the further object is the faster zoom out
    double realDist = (mAxisLength + mZoomAmount) / log(mAxisLength);
    mAxisLength = mAxisLength + realDist;

    //mAxisLength = mAxisLength + mZoomAmount;
    
    ChangeProjection(mCanvasSize.x, mCanvasSize.y, mAxisLength);
    
    Refresh(false);
}

//------------------------------------------------------------------------------
// void ShowWireFrame(bool flag)
//------------------------------------------------------------------------------
void TrajPlotCanvas::ShowWireFrame(bool flag)
{
    mShowWireFrame = flag;
    Refresh(false);
}

//------------------------------------------------------------------------------
// void ShowEquatorialPlane(bool flag)
//------------------------------------------------------------------------------
void TrajPlotCanvas::ShowEquatorialPlane(bool flag)
{
    mShowEquatorialPlane = flag;
    Refresh(false);
}

//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnPaint(wxPaintEvent& event)
{
    // must always be here
    wxPaintDC dc(this);
    
#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();    
    
    if (mShowWireFrame)
    {
        glPolygonMode(GL_FRONT, GL_LINE); // for wire frame
        glPolygonMode(GL_BACK, GL_LINE);  // for wire frame
    }
    else
    {
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }

    SetProjection();
    DrawPicture();
    glFlush();
    SwapBuffers();
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);
    
    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int nWidth, nHeight;
    GetClientSize(&nWidth, &nHeight);
    mCanvasSize.x = nWidth;
    mCanvasSize.y = nHeight;
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        //loj: need this to make picture not to stretch to canvas
        ChangeProjection(nWidth, nHeight, mAxisLength);
        SetCurrent();
        glViewport(0, 0, (GLint) nWidth, (GLint) nHeight);
    }
}

//------------------------------------------------------------------------------
// void OnEraseBackground(wxEraseEvent& event)
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnEraseBackground(wxEraseEvent& event)
{
    // Do nothing, to avoid flashing on MSW
}

//------------------------------------------------------------------------------
// void OnMouse(wxMouseEvent& event)
//------------------------------------------------------------------------------
void TrajPlotCanvas::OnMouse(wxMouseEvent& event)
{
    int flippedY;
    int clientWidth, clientHeight;
    int mouseX, mouseY;
    
    GetClientSize(&clientWidth, &clientHeight);
    ChangeProjection(clientWidth, clientHeight, mAxisLength);
    
    mouseX = event.GetX();
    mouseY = event.GetY();
        
    //for rotating
    if (event.Dragging())
    {
        // First, flip the mouseY value so it is oriented right (bottom left is 0,0)
        flippedY = clientHeight - mouseY;
    
        GLfloat fEndX = m_fLeftPos + ((GLfloat)mouseX /(GLfloat)clientWidth) *
            (m_fRightPos - m_fLeftPos);
        GLfloat fEndY = m_fBottomPos + ((GLfloat)flippedY /(GLfloat)clientHeight)*
            (m_fTopPos - m_fBottomPos);

        
        //------------------------------
        // translating
        //------------------------------
        if (event.ShiftDown() && event.LeftIsDown())
        {
            // Do a X/Y Translate of the camera
            m_fCamTransX += (fEndX - m_fStartX);
            m_fCamTransY += (fEndY - m_fStartY);

            m_fStartX = fEndX;
            m_fStartY = fEndY;
            
            mouseY = clientHeight - mouseY;

            // repaint
            Refresh(false);
        }
        //------------------------------
        // rotating
        //------------------------------
        else if (event.LeftIsDown())
        {        
            ComputeView(fEndX, fEndY);
            ChangeView(mCurrViewX, mCurrViewY, mCurrViewZ);
            
            mouseY = clientHeight - mouseY;
            
            // repaint
            Refresh(false);
        }
        //------------------------------
        // zooming
        //------------------------------
        else if (event.RightIsDown())
        {            
            // find the length
            double x2 = pow(mouseX - mLastMouseX, 2);
            double y2 = pow(mouseY - mLastMouseY, 2);
            double length = sqrt(x2 + y2);
            mZoomAmount = length * 100;

            if (mouseY > mLastMouseY || mouseX > mLastMouseX)
                ZoomIn();
            else
                ZoomOut();
            
            mLastMouseX = mouseX;
            mLastMouseY = mouseY;
        }

        // save last position
        m_fStartX = m_fLeftPos + ((GLfloat)mouseX / (GLfloat)clientWidth) *
            (m_fRightPos - m_fLeftPos);
        m_fStartY = m_fBottomPos + ((GLfloat)mouseY / (GLfloat)clientHeight) *
            (m_fTopPos - m_fBottomPos);
        
    } // end if (event.Dragging())
    
    mLastMouseX = mouseX;
    mLastMouseY = mouseY;
    
    wxLogStatus(MdiPlot::mdiParentGlFrame,
                wxT("X = %d Y = %d lastX = %f lastY = %f Zoom amount = %f Distance = %f"),
                event.GetX(), event.GetY(), m_fStartX, m_fStartY, mZoomAmount, mAxisLength);
    event.Skip();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TrajPlotCanvas::InitGL(void)
{
    // remove back faces
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glDepthFunc(GL_LESS);
    
    // speedups
    glEnable(GL_DITHER);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    
    // initalize devIL library
    ilInit();
    ilutRenderer(ILUT_OPENGL);

    // try to load textures
    if (!LoadGLTextures())
        return false;

    mInitialized = true;
    
    return true;
}

//---------------------------------------------------------------------------
//  bool LoadGLTextures()
//---------------------------------------------------------------------------
bool TrajPlotCanvas::LoadGLTextures()
{     
    // always load Earth texture
    ILboolean status = ilLoadImage("earth-0512.jpg");
   
    if (status != 1)
        return false;
    
    mTextureIndex[EARTH] = ilutGLBindTexImage();
    
    return true;
}

//---------------------------------------------------------------------------
//  void SetProjection()
//---------------------------------------------------------------------------
void TrajPlotCanvas::SetProjection()
{
   // Setup the world view
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION); // first go to projection mode
   glPushMatrix();
   glLoadIdentity();            // init the matrix (need this to see SC traj)
   SetupWorld();                // set it up
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
}

//---------------------------------------------------------------------------
//  void SetupWorld()
//---------------------------------------------------------------------------
void TrajPlotCanvas::SetupWorld()
{
   // Setup how we view the world

   glOrtho(m_fLeftPos, m_fRightPos, m_fBottomPos, m_fTopPos,
           m_fViewNear, m_fViewFar);
   
   //Translate Camera
   glTranslatef(m_fCamTransX, m_fCamTransY, m_fCamTransZ);

   //put camera transformations here.
   if (mRotateAboutXaxis)
   {
      glRotatef(m_fCamRotationY, 0.0, 1.0, 0.0);
      glRotatef(m_fCamRotationZ, 0.0, 0.0, 1.0);
      glRotatef(m_fCamRotationX, 1.0, 0.0, 0.0);
   }
   else if (mRotateAboutYaxis)
   {
      glRotatef(m_fCamRotationZ, 0.0, 0.0, 1.0);
      glRotatef(m_fCamRotationX, 1.0, 0.0, 0.0);
      glRotatef(m_fCamRotationY, 0.0, 1.0, 0.0);
   }
   else
   {
      glRotatef(m_fCamRotationX, 1.0, 0.0, 0.0);
      glRotatef(m_fCamRotationY, 0.0, 1.0, 0.0);
      glRotatef(m_fCamRotationZ, 0.0, 0.0, 1.0);
   }
   
   //camera moves opposite direction to center on object
   //this is the point of rotation
   switch (mPivotBody)
   {
   case EARTH:
      glTranslatef(-mTempEarthPos[mNumData-1][0],
                   -mTempEarthPos[mNumData-1][1],
                   -mTempEarthPos[mNumData-1][2]);
      break;
   default:
//        glTranslatef(-tempBodiesPosD[mPivotBody][mNumData-1][0],
//                     -tempBodiesPosD[mPivotBody][mNumData-1][1],
//                     -tempBodiesPosD[mPivotBody][mNumData-1][2]);
          
      break;
   }
} // end SetupWorld()

//---------------------------------------------------------------------------
//  void ComputeView(GLfloat fEndX, GLfloat fEndY)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ComputeView(GLfloat fEndX, GLfloat fEndY)
{
   // Calculate a percentage of how much the mouse has moved
   // NOTE: When moving the mouse left-right, we want to rotate about the
   // Y axis, and vice versa
   float fYAmnt = 360*(fEndX - m_fStartX)/(m_fRightPos - m_fLeftPos);
   float fXAmnt = 360*(fEndY - m_fStartY)/(m_fBottomPos - m_fTopPos);

   
   // always rotate the y axis
   mCurrViewY = m_fCamRotationY + fYAmnt;

   // Are we rotating the x or the z in this case?
   if (mRotateXy)
   {
      // x axis
      mCurrViewX = m_fCamRotationX + fXAmnt - 270;
   }
   else
   {
      // z axis
      mCurrViewZ = m_fCamRotationZ + fXAmnt;
   }

   m_fStartX = fEndX;
   m_fStartY = fEndY;
}

//---------------------------------------------------------------------------
//  void DrawPicture()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawPicture()
{
    glClearColor(0.0, 0.0, 0.0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    glLoadIdentity();

    DrawEarthTrajectory();
    DrawSpacecraftTrajectory();
    
    // draw equatorial plane
    if (mShowEquatorialPlane)
        DrawEquatorialPlane();
    
    glPopMatrix();
    
} // end DrawPicture()

//---------------------------------------------------------------------------
//  void DrawEquatorialPlane()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawEquatorialPlane()
{
   int i;
   float endPos[3];
   float distance;
   double angle;
   
   static const Real RAD_PER_DEG =
       3.14159265358979323846264338327950288419716939937511 / 180.0;
   
   distance = (double)mAxisLength;

   glPushMatrix();
   glLoadIdentity();
   glBegin(GL_LINES);
   
   // set color
   *sIntColor = mEquatorialPlaneColor;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   //-----------------------------------
   // draw lines
   //-----------------------------------
   for (i=0; i<360; i+=15)
   {
      angle = RAD_PER_DEG * ((double)i);
      
      endPos[0] = distance * cos(angle);
      endPos[1] = distance * sin(angle);
      endPos[2] = 0.0;
      
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(endPos[0], endPos[1], endPos[2]);
   }
   
   glEnd();
   glPopMatrix();

   //-----------------------------------
   // draw circles
   //-----------------------------------
   glPushMatrix();
   glLoadIdentity();
   
   GLUquadricObj *qobj = gluNewQuadric();   
   for(i=1; i<10; i++)
   {
      gluQuadricDrawStyle(qobj, GLU_LINE  );
      gluQuadricNormals  (qobj, GLU_SMOOTH);
      gluQuadricTexture  (qobj, GL_FALSE  );
      gluDisk(qobj, i*distance/10, i*distance/10, 50, 1);
   }
   gluDeleteQuadric(qobj);
   
   glPopMatrix();
   
} // end DrawEquatorialPlane()

//---------------------------------------------------------------------------
//  void DrawEarth()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawEarth()
{
   if (mPivotBody == EARTH)
   {
       glColor3f(1.0, 1.0, 1.0);

       if (mUseTexture)
       {
           if (mTextureIndex[EARTH] != 999)
           {
               glBindTexture(GL_TEXTURE_2D, mTextureIndex[EARTH]);
               glEnable(GL_TEXTURE_2D);
           }
       }
       
       GLUquadricObj* qobj = gluNewQuadric();
       gluQuadricDrawStyle(qobj, GLU_FILL );
       gluQuadricNormals  (qobj, GLU_SMOOTH);
       gluQuadricTexture  (qobj, GL_TRUE  );
       gluSphere(qobj, mEarthRadius, 50, 50);
       gluDeleteQuadric(qobj);
       
       glDisable(GL_TEXTURE_2D);
       glMatrixMode(GL_MODELVIEW);
   }
} // end DrawEarth()


//---------------------------------------------------------------------------
//  void DrawEarthTrajectory()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawEarthTrajectory()
{
   // set color
   *sIntColor = GREEN32;
   glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);

   // Draw Earth trajectory line based on points
   glPushMatrix();
   glLoadIdentity();
   glBegin(GL_LINES);

   // if current frame is not GCI
   if (mCurrViewFrame != GCI_FRAME)
   {
      for (int i=1; i<mNumData; i++)
      {
         if (mTime[i] >= mTime[i-1])
         {
            glVertex3fv(mTempEarthPos[i-1]);
            glVertex3fv(mTempEarthPos[i]);
         }
      }
   }
   glEnd();
   glPopMatrix();
   
   //-------------------------------------------------------
   //draw earth with texture
   //-------------------------------------------------------
   if (mNumData > 1)
   {
       glPushMatrix();
       glLoadIdentity();
       
       // put earth at final position
       glTranslatef(mTempEarthPos[mNumData-1][0],
                    mTempEarthPos[mNumData-1][1],
                    mTempEarthPos[mNumData-1][2]);
       
       DrawEarth();
       glPopMatrix();
   }

   //glPopMatrix(); //loj: added
   
} // end DrawEarthTrajectory()

//---------------------------------------------------------------------------
//  void DrawSpacecraftTrajectory()
//---------------------------------------------------------------------------
void TrajPlotCanvas::DrawSpacecraftTrajectory()
{
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_LINES);
   
    // Draw spacecraft trajectory line based on points
    for (int i=1; i<mNumData; i++)
    {
        if (mTime[i] >= mTime[i-1])
        {
            *sIntColor = mScTrajColor[i];
            glColor3ub(sGlColor->red, sGlColor->green, sGlColor->blue);
            
            glVertex3fv(mTempScPos[i-1]);
            glVertex3fv(mTempScPos[i]);
        }
    }
    
    glEnd();
    glPopMatrix();

} // end DrawSpacecraftTrajectory()

//---------------------------------------------------------------------------
//  void ChangeView(float viewX, float viewY, float viewZ)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ChangeView(float viewX, float viewY, float viewZ)
{

   m_fCamRotationX = (int)(viewX) % 360 + 270;
   m_fCamRotationY = (int)(viewY) % 360;
   m_fCamRotationZ = (int)(viewZ) % 360;

   // don't let the rotation angles build up to some insane size
   if (m_fCamRotationY > 360)
      m_fCamRotationY -= 360;
   else if (m_fCamRotationY < 0)
      m_fCamRotationY += 360;

   // don't let the rotation angles build up to some insane size
   if (m_fCamRotationX > 450)
      m_fCamRotationX -= 360;
   else if (m_fCamRotationX < 90)
      m_fCamRotationX += 360;
   
   // don't let the rotation angles build up to some insane size
   if (m_fCamRotationZ > 360)
      m_fCamRotationZ -= 360;
   else if (m_fCamRotationZ < 0)
      m_fCamRotationZ += 360;
  
} // end ChangeView()

//---------------------------------------------------------------------------
//  void SetProjection(int width, int height, float distance)
//---------------------------------------------------------------------------
void TrajPlotCanvas::ChangeProjection(int width, int height, float distance)
{    
    GLfloat fAspect = (GLfloat) height / (GLfloat) width;
        
    m_fViewLeft   = -mAxisLength/2;
    m_fViewRight  =  mAxisLength/2;
    m_fViewTop    =  mAxisLength/2;
    m_fViewBottom = -mAxisLength/2;
    m_fViewNear   = -mAxisLength/2;
    m_fViewFar    =  mAxisLength/2;
        
    // save the size we are setting the projection for later use
    if (width <= height)
    {
        m_fLeftPos = m_fViewLeft;
        m_fRightPos = m_fViewRight;
        m_fBottomPos = m_fViewBottom*fAspect;
        m_fTopPos = m_fViewTop*fAspect;
    }
    else
    {
        m_fLeftPos = m_fViewLeft / fAspect;
        m_fRightPos = m_fViewRight / fAspect;
        m_fBottomPos = m_fViewBottom;
        m_fTopPos = m_fViewTop;
    }
}


//------------------------------------------------------------------------------
// int ReadTextTrajectory(const wxString &filename)
//------------------------------------------------------------------------------
int TrajPlotCanvas::ReadTextTrajectory(const wxString &filename)
{
    int numDataPoints = 0;
    mTextTrajFile =  new TextTrajectoryFile(std::string(filename.c_str()));
    
    if (mTextTrajFile->Open())
    {
        mTrajectoryData = mTextTrajFile->GetData();
        
        numDataPoints = mTrajectoryData.size();

        for(int i=0; i<numDataPoints && i < MAX_DATA; i++)
        {
            mScTrajColor[mNumData] = RED32;
            mTime[mNumData] = mTrajectoryData[i].time;
            mTempScPos[mNumData][0] = mTrajectoryData[i].x;
            mTempScPos[mNumData][1] = mTrajectoryData[i].y;
            mTempScPos[mNumData][2] = mTrajectoryData[i].z;
            mTempEarthPos[mNumData][0] = 0.0;
            mTempEarthPos[mNumData][1] = 0.0;
            mTempEarthPos[mNumData][2] = 0.0;
            mNumData++;
        }

        mTextTrajFile->Close();
        wxLogStatus(MdiPlot::mdiParentGlFrame, wxT("Number of data points: %d"), numDataPoints);
    }
    else
    {
        wxString info;
        info.Printf(_T("Cannot open trajectory file name: %s\n"),
                    filename.c_str());
        
        wxMessageDialog msgDialog(this, info, _T("ReadTextTrajectory File"));
        msgDialog.ShowModal();
        return numDataPoints;
    }
        
    // initialize GL
    if (!InitGL())
    {
        wxMessageDialog msgDialog(this, _T("InitGL() failed"), _T("ReadTextTrajectory File"));
        msgDialog.ShowModal();
        return false;
    }
//      else
//      {
//          wxMessageDialog msgDialog(this, _T("InitGL() successful"), _T("ReadTextTrajectory File"));
//          msgDialog.ShowModal();
//      }

    mInitialized = true;
    
    return numDataPoints;
    
} //end ReadTextTrajectory()


//------------------------------------------------------------------------------
// void UpdateSpacecraft(const Real &time, const Real &posX,
//                       const Real &posY, const Real &posZ);
//------------------------------------------------------------------------------
void TrajPlotCanvas::UpdateSpacecraft(const Real &time, const Real &posX,
                                      const Real &posY, const Real &posZ)
{
    if (mNumData < MAX_DATA)
    {
        mScTrajColor[mNumData] = RED32;
        mTime[mNumData] = time;
        mTempScPos[mNumData][0] = posX;
        mTempScPos[mNumData][1] = posY;
        mTempScPos[mNumData][2] = posZ;
        mTempEarthPos[mNumData][0] = 0.0;
        mTempEarthPos[mNumData][1] = 0.0;
        mTempEarthPos[mNumData][2] = 0.0;
        mNumData++;
    }
    Refresh(false);
}
