//$Header$
//------------------------------------------------------------------------------
//                              MatlabFunctionSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/02/11
//
/**
 * Declares MatlabFunctionSetupPanel class.
 */
//------------------------------------------------------------------------------

#ifndef MatlabFunctionSetupPanel_hpp
#define MatlabFunctionSetupPanel_hpp

#include "GmatPanel.hpp"
#include "MatlabFunction.hpp"

class MatlabFunctionSetupPanel: public GmatPanel
{
public:
   // constructors
   MatlabFunctionSetupPanel(wxWindow *parent, const wxString &name);
    
private:

   MatlabFunction *theMatlabFunction;

   wxStaticBoxSizer *mTopSizer;
   wxBoxSizer *mPageSizer;
   wxTextCtrl *mPathTextCtrl;
   wxButton *mBrowseButton;
    
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnButton(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_LISTBOX,
      ID_BUTTON,
      ID_COLOR_BUTTON,
      ID_COMBO,
      ID_TEXTCTRL
   };
};

#endif
