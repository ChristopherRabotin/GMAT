//$Id$
//------------------------------------------------------------------------------
//                              GmatPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/02
//
/**
 * Implements GmatPanel class.
 */
//------------------------------------------------------------------------------

#include "GmatPanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"

#include "ShowScriptDialog.hpp"
#include "ShowSummaryDialog.hpp"

//#define DEBUG_GMAT_PANEL_SAVE 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GmatPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_SUMMARY, GmatPanel::OnSummary)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// GmatPanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs GmatPanel object.
 *
 * @param <parent> parent window.
 *
 */
//------------------------------------------------------------------------------
GmatPanel::GmatPanel(wxWindow *parent, bool showScriptButton)
    : wxPanel(parent)
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   canClose = true;
   mDataChanged = false;
   
   mShowScriptButton = showScriptButton;
   
   theParent = parent;
   
   #ifdef DEBUG_GMATPANEL
   MessageInterface::ShowMessage
      ("GmatPanel::GmatPanel() entered. theGuiInterpreter=<%p>\n", theGuiInterpreter);
   #endif
   
   int borderSize = 3;
   //wxStaticBox *topStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *middleStaticBox = new wxStaticBox( this, -1, wxT("") );
   wxStaticBox *bottomStaticBox = new wxStaticBox( this, -1, wxT("") );

   // create sizers
   thePanelSizer = new wxBoxSizer(wxVERTICAL);
   //theTopSizer = new wxStaticBoxSizer( topStaticBox, wxVERTICAL );
   theMiddleSizer = new wxStaticBoxSizer( middleStaticBox, wxVERTICAL );
   theBottomSizer = new wxStaticBoxSizer( bottomStaticBox, wxVERTICAL );
   wxBoxSizer *theButtonSizer = new wxBoxSizer(wxHORIZONTAL);

   //wxBoxSizer *theTopButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   
   // create bottom buttons
   theOkButton = new wxButton
      (this, ID_BUTTON_OK, "OK", wxDefaultPosition, wxDefaultSize, 0);
   theApplyButton = new wxButton
      (this, ID_BUTTON_APPLY, "Apply", wxDefaultPosition, wxDefaultSize, 0);
   theCancelButton = new wxButton
      (this, ID_BUTTON_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, 0);
   //theHelpButton = new wxButton
   //   (this, ID_BUTTON_HELP, "Help", wxDefaultPosition, wxDefaultSize, 0);
   theScriptButton = new wxButton
      (this, ID_BUTTON_SCRIPT, "Show Script", wxDefaultPosition, wxDefaultSize, 0);
   theSummaryButton = new wxButton
      (this, ID_BUTTON_SUMMARY, "Command Summary", wxDefaultPosition, wxDefaultSize, 0);
   
   // use different color for Show Script, and Command Summary for now
   theScriptButton->SetForegroundColour(*wxBLUE);
   theSummaryButton->SetForegroundColour(*wxBLUE);
   
   // add items to top sizer
   //theTopButtonSizer->Add(theScriptButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   //theTopButtonSizer->Add(theSummaryButton, 0, wxALIGN_RIGHT | wxALL, borderSize);
   //theTopSizer->Add(theTopButtonSizer, 0, wxALIGN_RIGHT | wxALL, borderSize);
   
   // adds the buttons to button sizer
   theButtonSizer->Add(theOkButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theApplyButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theCancelButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   //theButtonSizer->Add(theHelpButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(100, 20, 0, wxALIGN_CENTER | wxALL, borderSize);   
   theButtonSizer->Add(theScriptButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   theButtonSizer->Add(theSummaryButton, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   theBottomSizer->Add(theButtonSizer, 0, wxALIGN_CENTER | wxALL, borderSize);
   
   //topStaticBox->Show(mShowScriptButton);

   mObject = NULL;
   
}


//------------------------------------------------------------------------------
// virtual void EnableUpdate(bool enable = true)
//------------------------------------------------------------------------------
void GmatPanel::EnableUpdate(bool enable)
{
   #if DEBUG_GMAT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("GmatPanel::EnableUpdate() enable=%d\n", enable);
   #endif
   
   GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();
   
   if (enable)
   {
      mDataChanged = true;
      mdichild->SetDirty(true);
   }
   else
   {
      mDataChanged = false;
      mdichild->SetDirty(false);
   }
   
   //theApplyButton->Enable(enable);
   //theOkButton->Enable(enable);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool GmatPanel::PrepareObjectNameChange()
{
   return canClose;
}


//------------------------------------------------------------------------------
// virtual void ObjectNameChanged(Gmat::ObjectType type, const wxString &oldName,
//                                const wxString &newName)
//------------------------------------------------------------------------------
void GmatPanel::ObjectNameChanged(Gmat::ObjectType type, const wxString &oldName,
                                  const wxString &newName)
{
   // Do we need anything here?
}


//------------------------------------------------------------------------------
// virtual void OnApply()
//------------------------------------------------------------------------------
/**
 * Saves the data and remain unclosed.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnApply(wxCommandEvent &event)
{
   #if DEBUG_GMAT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("GmatPanel::OnApply() mDataChanged=%d\n", mDataChanged);
   #endif
   
   if (mDataChanged)
   {
      SaveData();
      GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();
      if (canClose)
      {
         mdichild->SetDirty(false);
         theGuiInterpreter->ConfigurationChanged(mObject, true);
         EnableUpdate(false);
      }
   }
}


//------------------------------------------------------------------------------
// virtual void OnOk()
//------------------------------------------------------------------------------
/**
 * Saves the data and closes the page
 */
//------------------------------------------------------------------------------
void GmatPanel::OnOK(wxCommandEvent &event)
{
   #if DEBUG_GMAT_PANEL_SAVE
   MessageInterface::ShowMessage
      ("GmatPanel::OnOK() mDataChanged=%d\n", mDataChanged);
   #endif
   
   if (mDataChanged)
   {
      SaveData();
      GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();

      if (canClose)
      {
         mdichild->SetDirty(false);
         theGuiInterpreter->ConfigurationChanged(mObject, true);
      }
   }
   
   if (canClose)
      GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// virtual void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void GmatPanel::OnCancel(wxCommandEvent &event)
{
   GmatMdiChildFrame* mdichild = (GmatMdiChildFrame*)theParent->GetParent();
   mdichild->SetDirty(false);
   GmatAppData::Instance()->GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// virtual void OnHelp()
//------------------------------------------------------------------------------
/**
 * Shows Helps
 */
//------------------------------------------------------------------------------
void GmatPanel::OnHelp(wxCommandEvent &event)
{
   // open separate window to show help?
}


//------------------------------------------------------------------------------
// virtual void OnScript()
//------------------------------------------------------------------------------
/**
 * Shows Scripts
 */
//------------------------------------------------------------------------------
void GmatPanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL) {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject);
   ssd.ShowModal();
}


//------------------------------------------------------------------------------
// virtual void OnSummary()
//------------------------------------------------------------------------------
/**
 * Shows command summary
 */
//------------------------------------------------------------------------------
void GmatPanel::OnSummary(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL) {
      title = "Command Summary for ";
      title += mObject->GetName().c_str();
   }
   ShowSummaryDialog ssd(this, -1, title, (GmatCommand*)mObject);
   ssd.ShowModal();
}


//------------------------------------------------------------------------------
// bool CheckReal(Real &rvalue, const std::string &str,
//                const std::string &field, const std::string &expRange,
//                bool onlyMsg = false, bool positive = false, bool zeroOk = false)
//------------------------------------------------------------------------------
/*
 * This method checks if input string is valid real number. It uses
 * GmatStringUtil::ToReal() to convert string to Real value. This method
 * pops up the error message and sets canClose to false if input string is
 * not a real number.
 *
 * @param  rvalue   Real value to be set if input string is valid
 * @param  str      Input string value
 * @param  field    Field name should be used in the error message
 * @param  expRange Expected value range to be used in the error message
 * @param  onlyMsg  if true, it only shows error message (default is false)
 * @param  positive if true, the value must be positive (default is false)
 * @param  zeroOk   if true, zero is allowed (default is false)
 */
//------------------------------------------------------------------------------
bool GmatPanel::CheckReal(Real &rvalue, const std::string &str,
                          const std::string &field, const std::string &expRange,
                          bool onlyMsg, bool positive, bool zeroOk)
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
      
      if (!positive ||
          (positive && rval > 0) ||
          (zeroOk && rval >= 0))
         return true;
   }
   
   MessageInterface::PopupMessage
      (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(),
       expRange.c_str());
      
   canClose = false;
   return false;
}


//------------------------------------------------------------------------------
// bool CheckInteger(Integer &ivalue, const std::string &str,
//                   const std::string &field, const std::string &expRange,
//                   bool onlyMsg = false, bool positive, bool zeroOk)
//------------------------------------------------------------------------------
/*
 * This method checks if input string is valid integer number. It uses
 * GmatStringUtil::ToInteger() to convert string to Integer value. This method
 * pops up the error message and sets canClose to false if input string is
 * not an integer number.
 *
 * @param  ivalue   Integer value to be set if input string is valid
 * @param  str      Input string value
 * @param  field    Field name should be used in the error message
 * @param  expRange Expected value range to be used in the error message
 * @param  onlyMsg  if true, it only shows error message (default is false)
 * @param  positive if true, the value must be positive (default is false)
 * @param  zeroOk   if true, zero is allowed (default is false)
 */
//------------------------------------------------------------------------------
bool GmatPanel::CheckInteger(Integer &ivalue, const std::string &str,
                             const std::string &field, const std::string &expRange,
                             bool onlyMsg, bool positive, bool zeroOk)
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
      
      if (!positive ||
          (positive && ival > 0) ||
          (zeroOk && ival >= 0))
         return true;
   }
   
   MessageInterface::PopupMessage
      (Gmat::ERROR_, mMsgFormat.c_str(), str.c_str(), field.c_str(),
       expRange.c_str());
   
   canClose = false;
   return false;
}


//------------------------------------------------------------------------------
// bool CheckVariable(const std::string &varName, Gmat::ObjectType ownerType,
//                    const std::string &field, const std::string &expRange,
//                    bool allowNumber  = true, bool allowNonPlottable = false)
//------------------------------------------------------------------------------
/*
 * Checks if input variable is a Number, Variable, Array element, or parameter of
 * input owner type.
 *
 * @param  varName    Input variable name to be checked
 * @param  ownerType  Input owner type (such as Gmat::SPACECRAFT)
 * @param  field      Field name should be used in the error message
 * @param  expRange   Expected value range to be used in the error message
 * @param  allowNumber  true if varName can be a Real number 
 * @param  allowNonPlottable  true if varName can be a non-plottable
 *
 * @return true if varName is valid
 */
//------------------------------------------------------------------------------
bool GmatPanel::CheckVariable(const std::string &varName, Gmat::ObjectType ownerType,
                              const std::string &field, const std::string &expRange,
                              bool allowNumber, bool allowNonPlottable)
{
   int retval = theGuiManager->
      IsValidVariable(varName.c_str(), Gmat::SPACECRAFT, allowNumber,
                      allowNonPlottable);
   
   if (retval == -1)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The variable \"%s\" for field \"%s\" "
          "does not exist.\nPlease create it first from the ParameterSelectDialog or "
          "from the Resource Tree.\n", varName.c_str(), field.c_str());
      
      canClose = false;
      return false;
   }
   else if (retval == 3)
   {
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The Parameter \"%s\" for field \"%s\" "
          "has undefined object \"%s\".\nPlease create proper object first "
          "from the Resource Tree.\n", varName.c_str(), field.c_str(), ownerName.c_str());
      
      canClose = false;
      return false;
   }
   else if (retval == 4)
   {
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);
      
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "The Parameter \"%s\" for field \"%s\" "
          "has unknown Parameter type \"%s\".\n", varName.c_str(), field.c_str(),
          type.c_str());
      
      canClose = false;
      return false;
   }
   else if (retval == 0)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, mMsgFormat.c_str(), varName.c_str(), field.c_str(),
          expRange.c_str());
      
      canClose = false;
      return false;
   }
   
   return true;
}

//-------------------------------
// protected methods
//-------------------------------

//------------------------------------------------------------------------------
// void Show()
//------------------------------------------------------------------------------
/**
 * Shows the panel.
 */
//------------------------------------------------------------------------------
void GmatPanel::Show()
{
   // add items to panel sizer
   
   //thePanelSizer->Add(theTopSizer, 0, wxGROW | wxALL, 1);
   thePanelSizer->Add(theMiddleSizer, 1, wxGROW | wxALL, 1);
   thePanelSizer->Add(theBottomSizer, 0, wxGROW | wxALL, 1);
   
   // displays the script button
   //thePanelSizer->Show(theTopSizer, mShowScriptButton);
   theScriptButton->Show(mShowScriptButton);
   thePanelSizer->Layout();
   
   // tells the enclosing window to adjust to the size of the sizer
   SetAutoLayout( TRUE );
   SetSizer(thePanelSizer); //use the sizer for layout
   thePanelSizer->Fit(this); //loj: if theParent is used it doesn't show the scroll bar
   thePanelSizer->SetSizeHints(this); //set size hints to honour minimum size
   
   LoadData();
   
   if (mObject == NULL)
   {
      MessageInterface::ShowMessage("*** WARNING *** GmatPanel object not set\n");
      mMsgFormat =
         "The value of \"%s\" for field \"%s\" is not an allowed value. \n"
         "The allowed values are: [%s].";
   }
   else
   {
      if (mObject->IsOfType(Gmat::COMMAND))
      {
         mMsgFormat =
            "The value of \"%s\" for field \"%s\" on command \""
            + mObject->GetTypeName() +  "\" is not an allowed value. \n"
            "The allowed values are: [%s].";
      }
      else
      {
         mMsgFormat =
            "The value of \"%s\" for field \"%s\" on object \""
            + mObject->GetName() +  "\" is not an allowed value. \n"
            "The allowed values are: [%s].";
      }
   }
   
   EnableUpdate(false);
   
   // We want to enable all the time (12/22/06)
   //theApplyButton->Disable();
   //theOkButton->Disable();
   //theHelpButton->Disable();  // Not implemented yet
   
   if ((mObject == NULL) || (!mObject->IsOfType(Gmat::COMMAND)))
      theSummaryButton->Hide();
}


