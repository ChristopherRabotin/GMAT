//$Id$
//------------------------------------------------------------------------------
//                              MdiChild3DViewFrame
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
/**
 * Declares MdiChild3DViewFrame for 3D visualization
 */
//------------------------------------------------------------------------------
#ifndef MdiChild3DViewFrame_hpp
#define MdiChild3DViewFrame_hpp

#include "MdiChildViewFrame.hpp"
#include "gmatdefs.hpp"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class MdiChild3DViewFrame: public MdiChildViewFrame
{
public:
   MdiChild3DViewFrame(wxMDIParentFrame *parent, const wxString& plotName,
                       const wxString& title, const wxPoint& pos,
                       const wxSize& size, const long style);
   virtual ~MdiChild3DViewFrame();
      
protected:
   
   DECLARE_EVENT_TABLE();
};
#endif
