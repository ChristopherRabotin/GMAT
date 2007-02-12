//$Header$
//------------------------------------------------------------------------------
//                              GmatDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Declares GmatDialog class. This class provides base class of GMAT Dialogs.
 */
//------------------------------------------------------------------------------

#ifndef GmatDialog_hpp
#define GmatDialog_hpp

#include "gmatwxdefs.hpp"
#include <wx/sizer.h>
#include <wx/control.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include "wx/radiobut.h"

#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"

class GmatDialog : public wxDialog
{
public:
   
   // constructors
   GmatDialog(wxWindow *parent, wxWindowID id, const wxString& title,
              GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
   
   virtual void EnableUpdate(bool enable = true);
   
   virtual void OnOK(wxCommandEvent &event);
   virtual void OnCancel(wxCommandEvent &event);
   virtual void OnHelp(wxCommandEvent &event);
   virtual void OnClose(wxCloseEvent &event);
   
   bool CheckReal(Real &rvalue, const std::string &str,
                  const std::string &field, const std::string &expRange,
                  bool onlyMsg = false);
   
   bool CheckInteger(Integer &ivalue, const std::string &str,
                     const std::string &field, const std::string &expRange,
                     bool onlyMsg = false);
   
protected:
   
   // member functions
   // Changed Show() to ShowDate(), because it conflicts with wxDialog::Show()
   virtual void ShowData();   
   virtual void Create() = 0;
   virtual void LoadData() = 0;
   virtual void SaveData() = 0;
   virtual void ResetData() = 0;
   
   // member data
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   GmatBase *mObject;
   bool canClose;
   bool mDataChanged;
   std::string mMsgFormat;
   
   wxWindow *theParent;
    
   wxBoxSizer *theDialogSizer;
   wxBoxSizer *theButtonSizer;
   
   #if __WXMAC__
   wxBoxSizer *theMiddleSizer;
   wxBoxSizer *theBottomSizer;
   #else
   wxStaticBoxSizer *theMiddleSizer;
   wxStaticBoxSizer *theBottomSizer;
   #endif
   
   wxButton *theOkButton;
   wxButton *theCancelButton;
   //wxButton *theHelpButton;

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      ID_BUTTON_OK = 8100,
      ID_BUTTON_CANCEL,
      ID_BUTTON_HELP,
      ID_GMAT_DIALOG_LAST
   };

};

#endif // GmatDialog_hpp
