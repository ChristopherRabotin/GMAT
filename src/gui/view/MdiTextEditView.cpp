//$Header$
//------------------------------------------------------------------------------
//                                  MdiTextEditView
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Original File: /samples/docvwmdi/view.cpp
// Author: Julian Smart
// Created: 1998/04/01
// Modified:
//   2003/11/04 Linda Jun - Followed GMAT coding style.
//
/**
 * Declares operations on text edit view.
 */
//------------------------------------------------------------------------------
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"      // for GuiInterpreter pointer
#include "MessageInterface.hpp" // for ClearMessage()

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "MdiTextEditView.hpp"

BEGIN_EVENT_TABLE(MdiTextEditView, wxView)
   EVT_MENU(GmatScript::MENU_SCRIPT_BUILD_OBJECT, MdiTextEditView::OnScriptBuildObject)
   EVT_MENU(GmatScript::MENU_SCRIPT_BUILD_AND_RUN, MdiTextEditView::OnScriptBuildAndRun)
   EVT_MENU(GmatScript::MENU_SCRIPT_RUN, MdiTextEditView::OnScriptRun)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiTextEditView()
//------------------------------------------------------------------------------
MdiTextEditView::MdiTextEditView()
   : wxView()
{
   frame = (wxMDIChildFrame *) NULL;
   textsw = (MdiTextSubFrame *) NULL; 
}

//------------------------------------------------------------------------------
// ~MdiTextEditView()
//------------------------------------------------------------------------------
MdiTextEditView::~MdiTextEditView()
{
}
    
// Handled by wxTextWindow
//------------------------------------------------------------------------------
// void OnDraw(wxDC *WXUNUSED(dc) )
//------------------------------------------------------------------------------
void MdiTextEditView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

//------------------------------------------------------------------------------
// void OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
//------------------------------------------------------------------------------
void MdiTextEditView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint) )
{
}

//------------------------------------------------------------------------------
// bool OnClose(bool deleteWindow)
//------------------------------------------------------------------------------
bool MdiTextEditView::OnClose(bool deleteWindow)
{
   if (!GetDocument()->Close())
      return FALSE;
    
   Activate(FALSE);
    
   if (deleteWindow)
   {
      delete frame;
      return TRUE;
   }
   return TRUE;
}

//------------------------------------------------------------------------------
// bool OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
bool MdiTextEditView::OnScriptBuildObject(wxCommandEvent& WXUNUSED(event))
{
   wxString filename = GetDocument()->GetFilename();
    
   bool status = GmatAppData::GetGuiInterpreter()->
      InterpretScript(std::string(filename.c_str()));

   // Update ResourceTree and MissionTree
   GmatAppData::GetResourceTree()->UpdateResource(true); //loj: 6/29/04 added true
   GmatAppData::GetMissionTree()->UpdateMission(true); //loj: 6/29/04 added true

   return status;
}

//------------------------------------------------------------------------------
// bool OnScriptBuildAndRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
bool MdiTextEditView::OnScriptBuildAndRun(wxCommandEvent& WXUNUSED(event))
{
   bool status = false;
    
   wxString filename = GetDocument()->GetFilename();
    
   status = GmatAppData::GetGuiInterpreter()->
      InterpretScript(std::string(filename.c_str()));

   if (status)
   {
      // Update ResourceTree
      GmatAppData::GetResourceTree()->UpdateResource(true); //loj: 6/29/04 added true
      GmatAppData::GetMissionTree()->UpdateMission(true); //loj: 6/29/04 added true

      //loj: 3/17/04 Should I close all plot window?
      status = GmatAppData::GetGuiInterpreter()->RunScript();
   }
    
   return status;
}

//------------------------------------------------------------------------------
// bool OnScriptRun(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * @note We should have an option to clear message each run. If it runs for
 *       a long time (days, months, etc), we will not see all the output written
 *       to the message window.
 */
//------------------------------------------------------------------------------
bool MdiTextEditView::OnScriptRun(wxCommandEvent& WXUNUSED(event))
{
   //MessageInterface::ClearMessage();
    
   //loj: 3/17/04 Should I close all plot window?
    
   bool status = GmatAppData::GetGuiInterpreter()->RunScript();

   return status;
}
