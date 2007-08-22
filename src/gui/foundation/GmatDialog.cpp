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
 * Implements GmatDialog class.
 */
//------------------------------------------------------------------------------

#include "GmatDialog.hpp"
#include "GmatAppData.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_GMAT_DIALOG_SAVE 1
//#define DEBUG_GMAT_DIALOG_CLOSE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON_OK, GmatDialog::OnOK)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatDialog::OnCancel)
   EVT_BUTTON(ID_BUTTON_HELP, GmatDialog::OnHelp)
   EVT_CLOSE(GmatDialog::OnClose) 
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//            GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs GmatDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 * @param  obj     object to be used for setting values
 *
 */
//------------------------------------------------------------------------------
GmatDialog::GmatDialog(wxWindow *parent, wxWindowID id, const wxString& title,
                       GmatBase *obj, const wxPoint& pos, const wxSize& size,
                       long style)
   : wxDialog(parent, id, title, pos, size, style, title)
{
   mObject = obj;
   
   canClose = true;
   mDataChanged = false;
   int borderSize = 2;
   
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   theParent = parent;
    
   theDialogSizer = new wxBoxSizer(wxVERTICAL);
   theButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   
   #if __WXMAC__
   theMiddleSizer = new wxBoxSizer(wxVERTICAL);
   theBottomSizer = new wxBoxSizer(wxVERTICAL);
   #else
   wxStaticBox *middleStaticBox = new wxStaticBox(this, -1, wxT(""));
   wxStaticBox *bottomStaticBox = new wxStaticBox(this, -1, wxT(""));
   theMiddleSizer = new wxStaticBoxSizer(middleStaticBox, wxVERTICAL);
   theBottomSizer = new wxStaticBoxSizer(bottomStaticBox, wxVERTICAL);
   #endif
   
   // create bottom buttons
   theOkButton =
      new wxButton(this, ID_BUTTON_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);
   
   theCancelButton =
      new wxButton(this, ID_BUTTON_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
   
   //theHelpButton =
   //   new wxButton(this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);
   
   // adds the buttons to button sizer    
   theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   //theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
}


//------------------------------------------------------------------------------
// virtual void EnableUpdate(bool enable = true)
//------------------------------------------------------------------------------
void GmatDialog::EnableUpdate(bool enable)
{
   #if DEBUG_GMAT_DIALOG_SAVE
   MessageInterface::ShowMessage
      ("GmatDialog::EnableUpdate() enable=%d\n", enable);
   #endif
   
   if (enable)
      mDataChanged = true;
   else
      mDataChanged = false;
}


//------------------------------------------------------------------------------
// void OnOK()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatDialog::OnOK(wxCommandEvent &event)
{
   SaveData();

   #if DEBUG_GMAT_DIALOG_SAVE
   MessageInterface::ShowMessage
      ("GmatDialog::OnOK() canClose=%d\n", canClose);
   #endif
   
   if (canClose)
   {
      mDataChanged = false;
      Close();
   }
}


//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatDialog::OnCancel(wxCommandEvent &event)
{
   ResetData();
   mDataChanged = false;
   Close();
}


//------------------------------------------------------------------------------
// void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatDialog::OnHelp(wxCommandEvent &event)
{
   // open separate window to show help
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void GmatDialog::OnClose(wxCloseEvent &event)
{
   #if DEBUG_GMAT_DIALOG_CLOSE
   MessageInterface::ShowMessage
      ("GmatDialog::OnClose() mDataChanged=%d\n", mDataChanged);
   #endif
   
   if (mDataChanged)
   {
      if ( wxMessageBox(_T("Changes will be lost. \nDo you really want to close?"),
                        _T("Please confirm"), wxICON_QUESTION | wxYES_NO) != wxYES )
      {
         event.Veto();
         return;
      }
   }
   
   event.Skip();
}


//------------------------------------------------------------------------------
// bool CheckReal(Real &rvalue, const std::string &str,
//                const std::string &field, const std::string &expRange,
//                bool onlyMsg = false)
//------------------------------------------------------------------------------
/*
 * This method checks if input string is valid real number. It uses
 * GmatStringUtil::ToReal() to convert string to Real value. This method
 * pops up the error message and sets canClose to false if input string is
 * invaid real number.
 *
 * @param  rvalue   Real value to be set if input string is valid
 * @param  str      Input string value
 * @param  field    Field name should used in the error message
 * @param  expRange Expected value range to be used in the error message
 * @param  onlyMsg  if true, it only shows error message
 */
//------------------------------------------------------------------------------
bool GmatDialog::CheckReal(Real &rvalue, const std::string &str,
                          const std::string &field, const std::string &expRange,
                          bool onlyMsg)
{
   //MessageInterface::ShowMessage
   //   ("===> CheckReal() str=%s, field=%s, expRange=%s\n", str.c_str(),
   //    field.c_str(), expRange.c_str());
   
   if (onlyMsg)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }
   
   // check for real value
   Real rval;
   if (GmatStringUtil::ToReal(str, &rval))
   {
      rvalue = rval;
      return true;
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }
}


//------------------------------------------------------------------------------
// bool CheckInteger(Integer &ivalue, const std::string &str,
//                   const std::string &field, const std::string &expRange,
//                   bool onlyMsg = false)
//------------------------------------------------------------------------------
/*
 * This method checks if input string is valid integer number. It uses
 * GmatStringUtil::ToInteger() to convert string to Integer value. This method
 * pops up the error message and sets canClose to false if input string is
 * invaid integer number.
 *
 * @param  ivalue   Integer value to be set if input string is valid
 * @param  str      Input string value
 * @param  field    Field name should used in the error message
 * @param  expRange Expected value range to be used in the error message
 * @param  onlyMsg  if true, it only shows error message
 */
//------------------------------------------------------------------------------
bool GmatDialog::CheckInteger(Integer &ivalue, const std::string &str,
                             const std::string &field, const std::string &expRange,
                             bool onlyMsg)
{
   if (onlyMsg)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }
   
   // check for integer value
   Integer ival;
   if (GmatStringUtil::ToInteger(str, &ival))
   {
      ivalue = ival;
      return true;
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }
}


//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void ShowData()
//------------------------------------------------------------------------------
/**
 * Shows the dialog.
 */
//------------------------------------------------------------------------------
void GmatDialog::ShowData()
{
   // add items to middle sizer
   
   theDialogSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   theDialogSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout(TRUE);
   SetSizer(theDialogSizer); //use the sizer for layout
   theDialogSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   theDialogSizer->SetSizeHints(this); //set size hints to honour minimum size
   
   // Set icon if icon file is in the start up file
   FileManager *fm = FileManager::Instance();
   try
   {
      wxString iconfile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      #if defined __WXMSW__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_ICO));
      #elif defined __WXGTK__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_XPM));
      #elif defined __WXMAC__
         SetIcon(wxIcon(iconfile, wxBITMAP_TYPE_PICT_RESOURCE));
      #endif
   }
   catch (GmatBaseException &e)
   {
      //MessageInterface::ShowMessage(e.GetMessage());
   }
   
   CenterOnScreen(wxBOTH);
   
   // We want always enable OK button
   //theOkButton->Disable();
   //theHelpButton->Disable(); //loj: for future build
   
   if (mObject == NULL)
   {
      mMsgFormat =
         "The value of \"%s\" for field \"%s\" is not an allowed value. \n"
         "The allowed values are: [%s].";
   }
   else
   {
      mMsgFormat =
         "The value of \"%s\" for field \"%s\" on object \""
         + mObject->GetName() +  "\" is not an allowed value. \n"
         "The allowed values are: [%s].";
   }
   
   LoadData();
   
}


