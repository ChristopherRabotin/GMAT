//$Id$
//------------------------------------------------------------------------------
//                              MdiChildGroundTrackFrame
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2011/06/06
/**
 * Declares MdiChildGroundTrackFrame for ground track plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildGroundTrackFrame_hpp
#define MdiChildGroundTrackFrame_hpp

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

class MdiChildGroundTrackFrame: public MdiChildViewFrame
{
public:
   MdiChildGroundTrackFrame(wxMDIParentFrame *parent, const wxString& plotName,
                            const wxString& title, const wxPoint& pos,
                            const wxSize& size, const long style);
   virtual ~MdiChildGroundTrackFrame();
   
   virtual void SetGl2dDrawingOption(const std::string &centralBodyName,
                        const std::string &textureMap,
                        Integer footPrintOption);
   
protected:
   
   DECLARE_EVENT_TABLE();
};
#endif
