/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWindows under MS Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "glcanvas.h"
#endif

#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#if wxUSE_GLCANVAS

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/settings.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"

#include "wx/glcanvas.h"

static const wxChar *wxGLCanvasClassName = wxT("wxGLCanvasClass");
static const wxChar *wxGLCanvasClassNameNoRedraw = wxT("wxGLCanvasClassNR");

LRESULT WXDLLEXPORT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message,
                                   WPARAM wParam, LPARAM lParam);

/*
 * GLContext implementation
 */

wxGLContext::wxGLContext(bool isRGB, wxGLCanvas *win, const wxPalette& palette)
{
  m_window = win;

  m_hDC = win->GetHDC();

  m_glContext = wglCreateContext((HDC) m_hDC);
  wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGl context") );

  wglMakeCurrent((HDC) m_hDC, m_glContext);
}

wxGLContext::wxGLContext(
               bool isRGB, wxGLCanvas *win,
               const wxPalette& palette,
               const wxGLContext *other  /* for sharing display lists */
             )
{
  m_window = win;

  m_hDC = win->GetHDC();

  m_glContext = wglCreateContext((HDC) m_hDC);
  wxCHECK_RET( m_glContext, wxT("Couldn't create OpenGl context") );

  if( other != 0 )
    wglShareLists( other->m_glContext, m_glContext );

  wglMakeCurrent((HDC) m_hDC, m_glContext);
}

wxGLContext::~wxGLContext()
{
  if (m_glContext)
  {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(m_glContext);
  }
}

void wxGLContext::SwapBuffers()
{
  if (m_glContext)
  {
    wglMakeCurrent((HDC) m_hDC, m_glContext);
    ::SwapBuffers((HDC) m_hDC);    //blits the backbuffer into DC
  }
}

void wxGLContext::SetCurrent()
{
  if (m_glContext)
  {
    wglMakeCurrent((HDC) m_hDC, m_glContext);
  }

  /*
  setupPixelFormat(hDC);
  setupPalette(hDC);
  */
}

void wxGLContext::SetColour(const wxChar *colour)
{
  float r = 0.0;
  float g = 0.0;
  float b = 0.0;
  wxColour *col = wxTheColourDatabase->FindColour(colour);
  if (col)
  {
    r = (float)(col->Red()/256.0);
    g = (float)(col->Green()/256.0);
    b = (float)(col->Blue()/256.0);
    glColor3f( r, g, b);
  }
}


/*
 * wxGLCanvas implementation
 */

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
    EVT_PALETTE_CHANGED(wxGLCanvas::OnPaletteChanged)
    EVT_QUERY_NEW_PALETTE(wxGLCanvas::OnQueryNewPalette)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name,
    int *attribList, const wxPalette& palette) : wxWindow()
{
  m_glContext = (wxGLContext*) NULL;

  bool ret = Create(parent, id, pos, size, style, name);

  if ( ret )
  {
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  }

  m_hDC = (WXHDC) ::GetDC((HWND) GetHWND());

  SetupPixelFormat(attribList);
  SetupPalette(palette);

  m_glContext = new wxGLContext(TRUE, this, palette);
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
              const wxGLContext *shared, wxWindowID id,
              const wxPoint& pos, const wxSize& size, long style, const wxString& name,
              int *attribList, const wxPalette& palette )
  : wxWindow()
{
  m_glContext = (wxGLContext*) NULL;

  bool ret = Create(parent, id, pos, size, style, name);

  if ( ret )
  {
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  }

  m_hDC = (WXHDC) ::GetDC((HWND) GetHWND());

  SetupPixelFormat(attribList);
  SetupPalette(palette);

  m_glContext = new wxGLContext(TRUE, this, palette, shared );
}

// Not very useful for wxMSW, but this is to be wxGTK compliant

wxGLCanvas::wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared, wxWindowID id,
                        const wxPoint& pos, const wxSize& size, long style, const wxString& name,
                        int *attribList, const wxPalette& palette ):
  wxWindow()
{
  m_glContext = (wxGLContext*) NULL;

  bool ret = Create(parent, id, pos, size, style, name);

  if ( ret )
  {
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  }

  m_hDC = (WXHDC) ::GetDC((HWND) GetHWND());

  SetupPixelFormat(attribList);
  SetupPalette(palette);

  wxGLContext *sharedContext=0;
  if (shared) sharedContext=shared->GetContext();
  m_glContext = new wxGLContext(TRUE, this, palette, sharedContext );
}

wxGLCanvas::~wxGLCanvas()
{
  if (m_glContext)
    delete m_glContext;

  ::ReleaseDC((HWND) GetHWND(), (HDC) m_hDC);
}

// Replaces wxWindow::Create functionality, since we need to use a different
// window class
bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
  static bool s_registeredGLCanvasClass = FALSE;

  // We have to register a special window class because we need
  // the CS_OWNDC style for GLCanvas.

  /*
  From Angel Popov <jumpo@bitex.com>

  Here are two snips from a dicussion in the OpenGL Gamedev list that explains
  how this problem can be fixed:

  "There are 5 common DCs available in Win95. These are aquired when you call
  GetDC or GetDCEx from a window that does _not_ have the OWNDC flag.
  OWNDC flagged windows do not get their DC from the common DC pool, the issue
  is they require 800 bytes each from the limited 64Kb local heap for GDI."

  "The deal is, if you hold onto one of the 5 shared DC's too long (as GL apps
  do), Win95 will actually "steal" it from you.  MakeCurrent fails,
  apparently, because Windows re-assigns the HDC to a different window.  The
  only way to prevent this, the only reliable means, is to set CS_OWNDC."
  */

  if (!s_registeredGLCanvasClass)
  {
    WNDCLASS wndclass;

    // the fields which are common to all classes
    wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = sizeof( DWORD ); // VZ: what is this DWORD used for?
    wndclass.hInstance     = wxhInstance;
    wndclass.hIcon         = (HICON) NULL;
    wndclass.hCursor       = ::LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    wndclass.lpszMenuName  = NULL;

    // Register the GLCanvas class name
    wndclass.hbrBackground = (HBRUSH)NULL;
    wndclass.lpszClassName = wxGLCanvasClassName;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;

    if ( !::RegisterClass(&wndclass) )
    {
      wxLogLastError(wxT("RegisterClass(wxGLCanvasClass)"));
      return FALSE;
    }

    // Register the GLCanvas class name for windows which don't do full repaint
    // on resize
    wndclass.lpszClassName = wxGLCanvasClassNameNoRedraw;
    wndclass.style        &= ~(CS_HREDRAW | CS_VREDRAW);

    if ( !::RegisterClass(&wndclass) )
    {
        wxLogLastError(wxT("RegisterClass(wxGLCanvasClassNameNoRedraw)"));

        ::UnregisterClass(wxGLCanvasClassName, wxhInstance);

        return FALSE;
    }

    s_registeredGLCanvasClass = TRUE;
  }

  wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindow without parent") );

  if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
    return FALSE;

  parent->AddChild(this);

  DWORD msflags = 0;

  /*
  A general rule with OpenGL and Win32 is that any window that will have a
  HGLRC built for it must have two flags:  WS_CLIPCHILDREN & WS_CLIPSIBLINGS.
  You can find references about this within the knowledge base and most OpenGL
  books that contain the wgl function descriptions.
  */

  WXDWORD exStyle = 0;
  msflags |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  msflags |= MSWGetStyle(style, & exStyle) ;

  return MSWCreate(wxGLCanvasClassName, NULL, pos, size, msflags, exStyle);
}

static void AdjustPFDForAttributes(PIXELFORMATDESCRIPTOR& pfd, int *attribList)
{
  if (attribList) {
    pfd.dwFlags &= ~PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_COLORINDEX;
    pfd.cColorBits = 0;
    int arg=0;

    while( (attribList[arg]!=0) )
    {
      switch( attribList[arg++] )
      {
        case WX_GL_RGBA:
          pfd.iPixelType = PFD_TYPE_RGBA;
          break;
        case WX_GL_BUFFER_SIZE:
          pfd.cColorBits = attribList[arg++];
          break;
        case WX_GL_LEVEL:
          // this member looks like it may be obsolete
          if (attribList[arg] > 0) {
            pfd.iLayerType = (BYTE)PFD_OVERLAY_PLANE;
          } else if (attribList[arg] < 0) {
            pfd.iLayerType = (BYTE)PFD_UNDERLAY_PLANE;
          } else {
            pfd.iLayerType = (BYTE)PFD_MAIN_PLANE;
          }
          arg++;
          break;
        case WX_GL_DOUBLEBUFFER:
          pfd.dwFlags |= PFD_DOUBLEBUFFER;
          break;
        case WX_GL_STEREO:
          pfd.dwFlags |= PFD_STEREO;
          break;
        case WX_GL_AUX_BUFFERS:
          pfd.cAuxBuffers = attribList[arg++];
          break;
        case WX_GL_MIN_RED:
          pfd.cColorBits += (pfd.cRedBits = attribList[arg++]);
          break;
        case WX_GL_MIN_GREEN:
          pfd.cColorBits += (pfd.cGreenBits = attribList[arg++]);
          break;
        case WX_GL_MIN_BLUE:
          pfd.cColorBits += (pfd.cBlueBits = attribList[arg++]);
          break;
        case WX_GL_MIN_ALPHA:
          // doesn't count in cColorBits
          pfd.cAlphaBits = attribList[arg++];
          break;
        case WX_GL_DEPTH_SIZE:
          pfd.cDepthBits = attribList[arg++];
          break;
        case WX_GL_STENCIL_SIZE:
          pfd.cStencilBits = attribList[arg++];
          break;
        case WX_GL_MIN_ACCUM_RED:
          pfd.cAccumBits += (pfd.cAccumRedBits = attribList[arg++]);
          break;
        case WX_GL_MIN_ACCUM_GREEN:
          pfd.cAccumBits += (pfd.cAccumGreenBits = attribList[arg++]);
          break;
        case WX_GL_MIN_ACCUM_BLUE:
          pfd.cAccumBits += (pfd.cAccumBlueBits = attribList[arg++]);
          break;
        case WX_GL_MIN_ACCUM_ALPHA:
          pfd.cAccumBits += (pfd.cAccumAlphaBits = attribList[arg++]);
          break;
        default:
          break;
      }
    }
  }
}

void wxGLCanvas::SetupPixelFormat(int *attribList) // (HDC hDC)
{
  PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    /* size */
        1,                /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,        /* support double-buffering */
        PFD_TYPE_RGBA,            /* color type */
        16,                /* prefered color depth */
        0, 0, 0, 0, 0, 0,        /* color bits (ignored) */
        0,                /* no alpha buffer */
        0,                /* alpha bits (ignored) */
        0,                /* no accumulation buffer */
        0, 0, 0, 0,            /* accum bits (ignored) */
        16,                /* depth buffer */
        0,                /* no stencil buffer */
        0,                /* no auxiliary buffers */
        PFD_MAIN_PLANE,            /* main layer */
        0,                /* reserved */
        0, 0, 0,            /* no layer, visible, damage masks */
    };

  AdjustPFDForAttributes(pfd, attribList);

  int pixelFormat = ChoosePixelFormat((HDC) m_hDC, &pfd);
  if (pixelFormat == 0) {
    wxLogLastError(_T("ChoosePixelFormat"));
  }
  else {
    if ( !::SetPixelFormat((HDC) m_hDC, pixelFormat, &pfd) ) {
      wxLogLastError(_T("SetPixelFormat"));
    }
  }
}

void wxGLCanvas::SetupPalette(const wxPalette& palette)
{
    int pixelFormat = GetPixelFormat((HDC) m_hDC);
    PIXELFORMATDESCRIPTOR pfd;

    DescribePixelFormat((HDC) m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
    {
    }
    else
    {
      return;
    }

    m_palette = palette;

    if ( !m_palette.Ok() )
    {
        m_palette = CreateDefaultPalette();
    }

    if (m_palette.Ok())
    {
        SelectPalette((HDC) m_hDC, (HPALETTE) m_palette.GetHPALETTE(), FALSE);
        RealizePalette((HDC) m_hDC);
    }
}

wxPalette wxGLCanvas::CreateDefaultPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    int paletteSize;
    int pixelFormat = GetPixelFormat((HDC) m_hDC);

    DescribePixelFormat((HDC) m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    paletteSize = 1 << pfd.cColorBits;

    LOGPALETTE* pPal =
     (LOGPALETTE*) malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
    pPal->palVersion = 0x300;
    pPal->palNumEntries = paletteSize;

    /* build a simple RGB color palette */
    {
    int redMask = (1 << pfd.cRedBits) - 1;
    int greenMask = (1 << pfd.cGreenBits) - 1;
    int blueMask = (1 << pfd.cBlueBits) - 1;
    int i;

    for (i=0; i<paletteSize; ++i) {
        pPal->palPalEntry[i].peRed =
            (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
        pPal->palPalEntry[i].peGreen =
            (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
        pPal->palPalEntry[i].peBlue =
            (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
        pPal->palPalEntry[i].peFlags = 0;
    }
    }

    HPALETTE hPalette = CreatePalette(pPal);
    free(pPal);

    wxPalette palette;
    palette.SetHPALETTE((WXHPALETTE) hPalette);

    return palette;
}

void wxGLCanvas::SwapBuffers()
{
  if (m_glContext)
    m_glContext->SwapBuffers();
}

void wxGLCanvas::OnSize(wxSizeEvent& event)
{
}

void wxGLCanvas::SetCurrent()
{
  if (m_glContext)
  {
    m_glContext->SetCurrent();
  }
}

void wxGLCanvas::SetColour(const wxChar *colour)
{
  if (m_glContext)
    m_glContext->SetColour(colour);
}

// TODO: Have to have this called by parent frame (?)
// So we need wxFrame to call OnQueryNewPalette for all children...
void wxGLCanvas::OnQueryNewPalette(wxQueryNewPaletteEvent& event)
{
  /* realize palette if this is the current window */
  if ( GetPalette()->Ok() ) {
    ::UnrealizeObject((HPALETTE) GetPalette()->GetHPALETTE());
    ::SelectPalette((HDC) GetHDC(), (HPALETTE) GetPalette()->GetHPALETTE(), FALSE);
    ::RealizePalette((HDC) GetHDC());
    Refresh();
    event.SetPaletteRealized(TRUE);
  }
  else
    event.SetPaletteRealized(FALSE);
}

// I think this doesn't have to be propagated to child windows.
void wxGLCanvas::OnPaletteChanged(wxPaletteChangedEvent& event)
{
  /* realize palette if this is *not* the current window */
  if ( GetPalette() &&
       GetPalette()->Ok() && (this != event.GetChangedWindow()) )
  {
    ::UnrealizeObject((HPALETTE) GetPalette()->GetHPALETTE());
    ::SelectPalette((HDC) GetHDC(), (HPALETTE) GetPalette()->GetHPALETTE(), FALSE);
    ::RealizePalette((HDC) GetHDC());
    Refresh();
  }
}

/* Give extensions proper function names. */

/* EXT_vertex_array */
void glArrayElementEXT(GLint i)
{
}

void glColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
}

void glDrawArraysEXT(GLenum mode, GLint first, GLsizei count)
{
#ifdef GL_EXT_vertex_array
    static PFNGLDRAWARRAYSEXTPROC proc = 0;

    if ( !proc )
    {
        proc = (PFNGLDRAWARRAYSEXTPROC) wglGetProcAddress("glDrawArraysEXT");
    }

    if ( proc )
        (* proc) (mode, first, count);
#endif
}

void glEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean *pointer)
{
}

void glGetPointervEXT(GLenum pname, GLvoid* *params)
{
}

void glIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
}

void glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
#ifdef GL_EXT_vertex_array
  static PFNGLNORMALPOINTEREXTPROC proc = 0;

  if ( !proc )
  {
    proc = (PFNGLNORMALPOINTEREXTPROC) wglGetProcAddress("glNormalPointerEXT");
  }

  if ( proc )
    (* proc) (type, stride, count, pointer);
#endif
}

void glTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
}

void glVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer)
{
#ifdef GL_EXT_vertex_array
  static PFNGLVERTEXPOINTEREXTPROC proc = 0;

  if ( !proc )
  {
    proc = (PFNGLVERTEXPOINTEREXTPROC) wglGetProcAddress("glVertexPointerEXT");
  }
  if ( proc )
    (* proc) (size, type, stride, count, pointer);
#endif
}

/* EXT_color_subtable */
void glColorSubtableEXT(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *table)
{
}

/* EXT_color_table */
void glColorTableEXT(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table)
{
}

void glCopyColorTableEXT(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
}

void glGetColorTableEXT(GLenum target, GLenum format, GLenum type, GLvoid *table)
{
}

void glGetColorTableParamaterfvEXT(GLenum target, GLenum pname, GLfloat *params)
{
}

void glGetColorTavleParameterivEXT(GLenum target, GLenum pname, GLint *params)
{
}

/* SGI_compiled_vertex_array */
void glLockArraysSGI(GLint first, GLsizei count)
{
}

void glUnlockArraysSGI()
{
}


/* SGI_cull_vertex */
void glCullParameterdvSGI(GLenum pname, GLdouble* params)
{
}

void glCullParameterfvSGI(GLenum pname, GLfloat* params)
{
}

/* SGI_index_func */
void glIndexFuncSGI(GLenum func, GLclampf ref)
{
}

/* SGI_index_material */
void glIndexMaterialSGI(GLenum face, GLenum mode)
{
}

/* WIN_swap_hint */
void glAddSwapHintRectWin(GLint x, GLint y, GLsizei width, GLsizei height)
{
}


//---------------------------------------------------------------------------
// wxGLApp
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLApp, wxApp)

bool wxGLApp::InitGLVisual(int *attribList)
{
  int pixelFormat;
  PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    /* size */
        1,                /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,        /* support double-buffering */
        PFD_TYPE_RGBA,            /* color type */
        16,                /* prefered color depth */
        0, 0, 0, 0, 0, 0,        /* color bits (ignored) */
        0,                /* no alpha buffer */
        0,                /* alpha bits (ignored) */
        0,                /* no accumulation buffer */
        0, 0, 0, 0,            /* accum bits (ignored) */
        16,                /* depth buffer */
        0,                /* no stencil buffer */
        0,                /* no auxiliary buffers */
        PFD_MAIN_PLANE,            /* main layer */
        0,                /* reserved */
        0, 0, 0,            /* no layer, visible, damage masks */
    };

  AdjustPFDForAttributes(pfd, attribList);

  // use DC for whole (root) screen, since no windows have yet been created
  pixelFormat = ChoosePixelFormat(ScreenHDC(), &pfd);

  if (pixelFormat == 0) {
    wxLogError(_("Failed to initialize OpenGL"));
    return FALSE;
  }

  return TRUE;
}

wxGLApp::~wxGLApp()
{
}

#endif
    // wxUSE_GLCANVAS
