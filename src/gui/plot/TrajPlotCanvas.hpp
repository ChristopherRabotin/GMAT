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
 * Declares TrajPlotCanvas for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef TrajPlotCanvas_hpp
#define TrajPlotCanvas_hpp

#include "gmatwxdefs.hpp"
#include "MdiGlPlotData.hpp"
#include "TextTrajectoryFile.hpp"
#include "wx/glcanvas.h"

const int MAX_DATA = 20000;
const int MAX_EARTH_ZOOM_IN = 12756;
const int MAX_BODIES = 20;

class TrajPlotCanvas: public wxGLCanvas
{
public:
    TrajPlotCanvas(wxWindow *parent, const wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, long style = 0,
                   const wxString& name = wxT("TrajPlotCanvas"));
    ~TrajPlotCanvas();

    // initialization
    bool InitGL(void);
    
    // events
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnMouse(wxMouseEvent &event);
    void OnEraseBackground(wxEraseEvent &event);

    // view
    bool IsInitialized() { return mInitialized; };
    void ShowDefaultView();
    void ZoomIn();
    void ZoomOut();
    void ShowWireFrame(bool flag);
    void ShowEquatorialPlane(bool flag);

    // data
    int  ReadTextTrajectory(const wxString &filename);
    void UpdateSpacecraft(const Real &time, const Real &posX,
                          const Real &posY, const Real &posZ);
    
private:
    TextTrajectoryFile *mTextTrajFile;
    TrajectoryArray mTrajectoryData;

    // Data members used by the mouse
    GLfloat m_fStartX, m_fStartY;

    // Actual params of the window
    GLfloat m_fLeftPos, m_fRightPos, m_fBottomPos, m_fTopPos;

    // Camera rotations
    GLfloat m_fCamRotationX, m_fCamRotationY, m_fCamRotationZ;

    // Camera translations
    GLfloat m_fCamTransX, m_fCamTransY, m_fCamTransZ;
    
    // draw option
    float mAxisLength;
    bool mShowWireFrame;
    bool mShowEquatorialPlane;

    // color
    int mEquatorialPlaneColor;
    
    // texture
    GLuint mTextureIndex[MAX_BODIES];
    bool mUseTexture;
    
    // rotating
    bool mRotateXy;
    bool mRotateAboutXaxis;
    bool mRotateAboutYaxis;
    bool mRotateAboutZaxis;
    bool mRotateEarthToEnd;
    double lastLongitudeD;
    
    // zooming
    float mZoomAmount;
    int   mLastMouseX;
    int   mLastMouseY;
    
    // initialization and limit
    bool mInitialized;
    bool mDdataCountOverLimit;
    int  mNumData;

    // time
    double mTime[MAX_DATA];

    // spacecraft
    float mTempScPos[MAX_DATA][3];
    int   mScTrajColor[MAX_DATA];
    
    // bodies
    float mEarthRadius;
    float mEarthGciPos[MAX_DATA][3];
    float mTempEarthPos[MAX_DATA][3];
    
    // coordinate frame
    //loj: for build2 only GCI frame will show
    short mCurrViewFrame;
    short mCurrBody;
    short mPivotBody;
    int   mMaxZoomIn;

    // view
    wxSize mCanvasSize;
    GLfloat m_fViewLeft;
    GLfloat m_fViewRight;
    GLfloat m_fViewTop;
    GLfloat m_fViewBottom;
    GLfloat m_fViewNear;
    GLfloat m_fViewFar;
    float mDefaultViewX;
    float mDefaultViewY;
    float mDefaultViewZ;
    float mDefaultViewDist;
    float mCurrViewX;
    float mCurrViewY;
    float mCurrViewZ;
    float mCurrViewDist;
    
    // initialization
    bool LoadGLTextures();

    // view objects
    void SetupWorld();
    void SetProjection();
    void ComputeView(GLfloat fEndX, GLfloat fEndY);
    void ChangeView(float viewX, float viewY, float viewZ);
    //float viewDistance);
    void ChangeProjection(int width, int height, float distance);
    
    // drawing objects
    void DrawPicture();
    void DrawEarth();
    void DrawEarthTrajectory();
    void DrawSpacecraftTrajectory();
    void DrawEquatorialPlane();

    DECLARE_EVENT_TABLE()
};

#endif
