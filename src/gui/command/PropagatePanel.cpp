//$Header$
//------------------------------------------------------------------------------
//                              PropagatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/08/29
// Modified:
//    2004/05/06 Allison Greene: inherit from GmatPanel
//    2004/10/20 Linda Jun: rename from PropgateCommandPanel
/**
 * This class contains the Propagate Command Setup window.
 */
//------------------------------------------------------------------------------

#include <wx/variant.h>
#include "PropagatePanel.hpp"
#include "ParameterSelectDialog.hpp"
#include "SpaceObjectSelectDialog.hpp"
#include "PropagatorSelectDialog.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PROPAGATE_PANEL 1

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(PropagatePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)
   EVT_BUTTON(ID_BUTTON_HELP, GmatPanel::OnHelp)
   
   EVT_BUTTON(ID_BUTTON_CANCEL, PropagatePanel::OnCancel)
   
   EVT_TEXT(ID_TEXTCTRL, PropagatePanel::OnTextChange)
   EVT_BUTTON(ID_BUTTON, PropagatePanel::OnButtonClick)
   EVT_COMBOBOX(ID_COMBOBOX, PropagatePanel::OnComboBoxChange)
   EVT_GRID_CELL_LEFT_CLICK(PropagatePanel::OnCellLeftClick)
   EVT_GRID_CELL_RIGHT_CLICK(PropagatePanel::OnCellRightClick)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// PropagatePanel(wxWindow *parent, GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * A constructor.
 */
//------------------------------------------------------------------------------
PropagatePanel::PropagatePanel(wxWindow *parent, GmatCommand *cmd)
   : GmatPanel(parent)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::PropagatePanel() entered\n");
   #endif
   
   thePropCmd = (Propagate *)cmd;

   // Default values
   mPropModeChanged = false;
   mPropChanged = false;
   mStopCondChanged = false;
   
   mStopCondCount = 0;
   mPropCount = 0;
   mPropModeCount = 1;
   numOfEqualities = 6;
   mTempPropCount = 0;
   mCurrStopRow = 0;
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      mTempProp[i].isChanged = false;
      mTempProp[i].propName = "";
      mTempProp[i].soNames = "";
      mTempProp[i].soNameList.Clear();
   }

   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   {
      mTempStopCond[i].isChanged = false;
      mTempStopCond[i].name = "";
      mTempStopCond[i].desc = "";
      mTempStopCond[i].varName = "";
      mTempStopCond[i].typeName = "";
      mTempStopCond[i].relOpStr = "=";
      mTempStopCond[i].goalStr = "0.0";
      mTempStopCond[i].tol = BaseStopCondition::STOP_COND_TOL;
      mTempStopCond[i].repeat = 1;
      mTempStopCond[i].stopCondPtr = NULL;
   }
   
   mObjectTypeList.Add("Spacecraft");
   
   Create();
   Show();
   updateButton->Disable();
   theApplyButton->Disable();
}


//------------------------------------------------------------------------------
// void OnCancel()
//------------------------------------------------------------------------------
/**
 * Close page.
 */
//------------------------------------------------------------------------------
void PropagatePanel::OnCancel(wxCommandEvent &event)
{
   mRemovedStopCondList.clear();
   GmatAppData::GetMainFrame()->CloseActiveChild();
}


//------------------------------------------------------------------------------
// ~PropagatePanel()
//------------------------------------------------------------------------------
PropagatePanel::~PropagatePanel()
{
   mObjectTypeList.Clear();
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void PropagatePanel::Create()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage("PropagatePanel::Create() entered\n");
   #endif
   
   Integer bsize = 1; // bordersize
   
   // wxGrid
   propGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,100), wxWANTS_CHARS);
   
   propGrid->CreateGrid(MAX_PROP_ROW, 3, wxGrid::wxGridSelectCells);
   propGrid->SetColSize(0, 200);
   propGrid->SetColSize(1, 400);
   propGrid->SetColSize(2, 80);
   propGrid->SetMargins(0, 0);
   propGrid->SetColLabelValue(0, _T("Propagator"));
   propGrid->SetColLabelValue(1, _T("Spacecraft List"));
   propGrid->SetColLabelValue(2, _T("Backward?"));
   propGrid->SetRowLabelSize(0);
   propGrid->SetScrollbars(5, 8, 15, 15);
   propGrid->EnableEditing(false);
   
   // create and insert boolean choice editor
   //wxGridCellAttr *attrBool = new wxGridCellAttr;
   //attrBool->SetEditor(new wxGridCellBoolEditor);
   //attrBool->SetRenderer(new wxGridCellBoolRenderer);
   //propGrid->SetColAttr(2, attrBool);
   //propGrid->SetColFormatBool(2);
   
   stopCondGrid =
      new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(700,120), wxWANTS_CHARS);
   
   stopCondGrid->CreateGrid(MAX_STOPCOND_ROW, 2, wxGrid::wxGridSelectCells);
   stopCondGrid->SetColSize(0, 200);
   stopCondGrid->SetColSize(1, 480);
   stopCondGrid->SetMargins(0, 0);
   stopCondGrid->SetColLabelValue(0, _T("Name"));
   stopCondGrid->SetColLabelValue(1, _T("Condition"));
   stopCondGrid->SetRowLabelSize(0);
   stopCondGrid->SetScrollbars(5, 14, 15, 15);
   stopCondGrid->EnableEditing(false);
   
   //wxTextCtrl
   stopNameTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(180,-1), 0);
   varNameTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(180,-1), 0);
   goalTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(180,-1), 0);
   //toleranceTextCtrl =
   //   new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
   //                  wxDefaultPosition, wxSize(100,-1), 0);
   
   //wxButton
   updateButton =
      new wxButton(this, ID_BUTTON, wxT("Update"),
                    wxDefaultPosition, wxDefaultSize, 0);
   deleteButton =
      new wxButton(this, ID_BUTTON, wxT("Delete"),
                    wxDefaultPosition, wxDefaultSize, 0);
   mStopViewButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                   wxDefaultPosition, wxSize(75, -1), 0);
   mGoalViewButton =
      new wxButton(this, ID_BUTTON, wxT("View"),
                   wxDefaultPosition, wxSize(75, -1), 0);
               
   //StringArray
   StringArray propModes = thePropCmd->GetStringArrayParameter
      (thePropCmd->GetParameterID("AvailablePropModes"));
   
   mPropModeCount = propModes.size();

   //wxString
   wxString *propModeList = new wxString[mPropModeCount];
   for (Integer i=0; i<mPropModeCount; i++)
   {
      propModeList[i] = propModes[i].c_str();
   }
   
   if (strcmp(propModeList[0].c_str(), "") == 0)
      propModeList[0] = "None";
      
   wxString logicalOpArray[] =
   {
      wxT("="),
      wxT(">"),
      wxT("<"),
      wxT(">="),
      wxT("<="),
      wxT("!=")
   };
                  
   //wxComboBox
   mPropModeComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(propModeList[0].c_str()), wxDefaultPosition,
                      wxSize(150,-1), mPropModeCount, propModeList,
                      wxCB_DROPDOWN|wxCB_READONLY);
   equalityComboBox =
      new wxComboBox(this, ID_COMBOBOX, wxT(logicalOpArray[0]), wxDefaultPosition,
                     wxSize(40,-1), numOfEqualities, logicalOpArray,
                     wxCB_DROPDOWN|wxCB_READONLY);

   //wxStaticText
   wxStaticText *synchStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Propagate Mode:  "),
                        wxDefaultPosition, wxDefaultSize, 0);
   wxStaticText *nameStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Name"), 
                        wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT);
   wxStaticText *varStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Variable"), 
                       wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT);

   //wxStaticText *tolStaticText =
   //   new wxStaticText(this, ID_TEXT, wxT("Tolerance"), 
   //                       wxDefaultPosition, wxDefaultSize, 0);
      
   #if __WXMAC__
   // wxStaticText
   wxStaticText *title1StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Propagators and Spacecraft"),
                        wxDefaultPosition, wxSize(120,20),
                        wxBOLD);
   title1StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title2StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Stopping Conditions"),
                        wxDefaultPosition, wxSize(120,20),
                        wxBOLD);
   title2StaticText->SetFont(wxFont(14, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTWEIGHT_BOLD,
                                             false, _T(""), wxFONTENCODING_SYSTEM));
   wxStaticText *title3StaticText =
      new wxStaticText( this, ID_TEXT, wxT("Stopping Conditions Details"),
                        wxDefaultPosition, wxSize(120,20),
                        wxST_NO_AUTORESIZE);
   title3StaticText->SetFont(wxFont(12, wxSWISS, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                                             true, _T(""), wxFONTENCODING_SYSTEM));
                                  
   //wxSizers
   wxBoxSizer *boxSizer1 = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *boxSizer2 = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *boxSizer3 = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *boxSizer4 = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *boxSizer5 = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *boxSizer6 = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *boxSizer7 = new wxBoxSizer(wxHORIZONTAL);
   
   //Adding objects to sizers
   boxSizer1->Add(title1StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer1->Add(boxSizer2, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer1->Add(title2StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer1->Add(boxSizer3, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   //Adding the spacecraft objects
   boxSizer4->Add(synchStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer4->Add(mPropModeComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer2->Add(boxSizer4, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer2->Add(propGrid, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   //Adding the stopping condition objects
   boxSizer6->Add(nameStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer6->Add(stopNameTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer6->Add(updateButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer6->Add(deleteButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer7->Add(varStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add(varNameTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add(mStopViewButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add(equalityComboBox, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add(goalTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer7->Add(mGoalViewButton, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   //boxSizer7->Add(tolStaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   //boxSizer7->Add(toleranceTextCtrl, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer5->Add(boxSizer6, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer5->Add(boxSizer7, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   boxSizer3->Add(stopCondGrid, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer3->Add(title3StaticText, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   boxSizer3->Add(boxSizer5, 0, wxGROW|wxALIGN_LEFT|wxALL, bsize);
   
   theMiddleSizer->Add(boxSizer1, 0, wxGROW, bsize);
   #else
   //wxSizers
   wxBoxSizer *propModeSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *stopNameSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *varSizer = new wxBoxSizer(wxHORIZONTAL);
   wxBoxSizer *stopDetailSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
   
   mMiddleBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   wxStaticBox *propStaticBox =
      new wxStaticBox(this, -1, wxT("Propagators and Spacecraft"));
   wxStaticBox *stopDetailStaticBox =
      new wxStaticBox(this, -1, wxT("Stopping Condition Details"));
   wxStaticBox *stopCondStaticBox =
      new wxStaticBox(this, -1, wxT("Stopping Conditions"));
   
   wxStaticBoxSizer *propSizer =
      new wxStaticBoxSizer(propStaticBox, wxVERTICAL);
   wxStaticBoxSizer *detailSizer =
      new wxStaticBoxSizer(stopDetailStaticBox, wxVERTICAL); 
   mStopSizer = new wxStaticBoxSizer(stopCondStaticBox, wxVERTICAL);
   
   //Adding objects to sizers
   propModeSizer->Add(synchStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   propModeSizer->Add(mPropModeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   propSizer->Add(propModeSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   propSizer->Add(propGrid, 0, wxALIGN_CENTER|wxALL, bsize);
   
   stopNameSizer->Add(nameStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   stopNameSizer->Add(stopNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   stopNameSizer->Add(updateButton, 0, wxALIGN_LEFT|wxALL, bsize);
   stopNameSizer->Add(deleteButton, 0, wxALIGN_LEFT|wxALL, bsize);
   
   varSizer->Add(varStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   varSizer->Add(varNameTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   varSizer->Add(mStopViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   varSizer->Add(equalityComboBox, 0, wxALIGN_CENTRE|wxALL, bsize);
   varSizer->Add(goalTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   varSizer->Add(mGoalViewButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   //varSizer->Add(tolStaticText, 0, wxALIGN_CENTER|wxALL, bsize);
   //varSizer->Add(toleranceTextCtrl, 0, wxALIGN_CENTER|wxALL, bsize);
   
   stopDetailSizer->Add(stopNameSizer, 0, wxALIGN_LEFT|wxALL, bsize);    
   stopDetailSizer->Add(varSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   detailSizer->Add(stopDetailSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   mStopSizer->Add(stopCondGrid, 0, wxALIGN_CENTER|wxALL, bsize);
   mStopSizer->Add(detailSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   mMiddleBoxSizer->Add(propSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   mMiddleBoxSizer->Add(mStopSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   panelSizer->Add(mMiddleBoxSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   theMiddleSizer->Add(panelSizer, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   #endif
   
   mPropModeComboBox->Enable(true);
   deleteButton->Enable(true);
   equalityComboBox->Enable(false);
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void PropagatePanel::LoadData()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage("PropagatePanel::LoadData() entered\n");
   #endif

   // Set the pointer for the "Show Script" button
   mObject = thePropCmd;
   
   //----------------------------------
   // propagation mode
   //----------------------------------
   std::string mode = thePropCmd->GetStringParameter
      (thePropCmd->GetParameterID("PropagateMode"));
   mPropModeComboBox->SetStringSelection(mode.c_str());

   //----------------------------------
   // propagator
   //----------------------------------
   Integer propId = thePropCmd->GetParameterID("Propagator");

   // Get the list of propagators (aka the PropSetups)
   StringArray propNames = thePropCmd->GetStringArrayParameter(propId);
   mPropCount = propNames.size();
   
   StringArray soList;
   
   mTempPropCount = mPropCount;
   
   Integer scId = thePropCmd->GetParameterID("Spacecraft");

   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mPropCount=%d\n", mPropCount);
   #endif

   Integer soCount = 0;
   
   for (Integer i=0; i<mPropCount; i++)
   {
      mTempProp[i].propName = wxT(propNames[i].c_str());

      // Get the list of spacecraft and formations
      soList = thePropCmd->GetStringArrayParameter(scId, i);
      soCount = soList.size();
      
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() propName=%s, soCount=%d\n",
       propNames[i].c_str(), soCount);
   #endif
   
      Integer actualSoCount = 0;
      for (Integer j=0; j<soCount; j++)
      {
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::LoadData() soNameList[%d]=<%s>\n",
             j, soList[j].c_str());
         #endif
         
         // verify space object actually exist
         if (theGuiInterpreter->GetObject(soList[j]) ||
             theGuiInterpreter->GetObject(soList[j]))
         {
            actualSoCount++;
            mTempProp[i].soNameList.Add(soList[j].c_str());
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "The SpaceObject:<%s> was not created, "
                "so removed from the display list\n", soList[j].c_str());
         }
      }

      soCount = actualSoCount;
      mTempProp[i].soCount = actualSoCount;
      
      if (soCount > 0)
      {
         for (Integer j=0; j<soCount-1; j++)
         {
            mTempProp[i].soNames += soList[j].c_str();
            mTempProp[i].soNames += ", ";
         }

         mTempProp[i].soNames += soList[soCount-1].c_str();
      }
   }

   //----------------------------------
   // stopping conditions
   //----------------------------------
   ObjectArray &stopArray =
      thePropCmd->GetRefObjectArray(Gmat::STOP_CONDITION);
   mStopCondCount = stopArray.size();
   
   stopCondGrid->SelectRow(0);
   //Parameter *stopParam;
   
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::LoadData() mStopCondCount=%d\n",
       mStopCondCount);
   #endif
   
   StopCondition  *stopCond;
   for (Integer i=0; i<mStopCondCount; i++)
   {
      stopCond = (StopCondition *)stopArray[i];
      
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::LoadData() stopArray[%d]=%s\n",
          i, stopCond->GetName().c_str());
      #endif
      
      // StopCondition created from the script might not have been configured (unnamed)
      if (stopCond != NULL)
      {
         mTempStopCond[i].stopCondPtr = stopCond;
         mTempStopCond[i].name = wxT(stopCond->GetName().c_str());
         mTempStopCond[i].varName = wxT(stopCond->GetStringParameter("StopVar").c_str());
         mTempStopCond[i].typeName = wxT(stopCond->GetTypeName().c_str());
         mTempStopCond[i].goalStr = stopCond->GetStringParameter("Goal").c_str();
         mTempStopCond[i].tol = stopCond->GetRealParameter("Tol");
         mTempStopCond[i].repeat = stopCond->GetIntegerParameter("Repeat");
         wxString str = FormatStopCondDesc(mTempStopCond[i].varName,
                                           mTempStopCond[i].relOpStr,
                                           mTempStopCond[i].goalStr);
         mTempStopCond[i].desc = str;
      }
   }
   
   mCurrStopRow = 0;
   DisplayPropagator();
   DisplayStopCondition(mCurrStopRow);
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void PropagatePanel::SaveData()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::SaveData() mTempPropCount=%d, mRemovedStopCondList.size()=%d\n",
       mTempPropCount, mRemovedStopCondList.size());
   #endif
   
   //---------------------------------------------
   // Delete any removed stopCondition pointer
   //---------------------------------------------
   for (unsigned int i=0; i<mRemovedStopCondList.size(); i++)
   {
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::SaveData() deleting StopCondition addr=%d\n",
          mRemovedStopCondList[i]);
      #endif
      
      delete mRemovedStopCondList[i];
   }
   
   //-------------------------------------------------------
   // Saving propagation mode
   //-------------------------------------------------------
   if (mPropModeChanged)
   {
      mPropModeChanged = false;
      wxString str = mPropModeComboBox->GetStringSelection();
      if (str.CmpNoCase("None") == 0)
         str = "";
      thePropCmd->SetStringParameter
         (thePropCmd->GetParameterID("PropagateMode"), str.c_str());
   }
   
   Integer soCount = 0;
   
   //-------------------------------------------------------
   // Saving propagator
   //-------------------------------------------------------

   try
   {
      if (mPropChanged)
      {
         Integer propId = thePropCmd->GetParameterID("Propagator");
         Integer scId = thePropCmd->GetParameterID("Spacecraft");
         
         // Clear propagator and spacecraft list
         thePropCmd->TakeAction("Clear", "Propagator");
         
         mPropCount = mTempPropCount;
         
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() mPropCount=%d\n", mPropCount);
         #endif
         
         for (Integer i=0; i<mPropCount; i++)
         {
            thePropCmd->SetStringParameter
               (propId, std::string(mTempProp[i].propName.c_str()));
            
            //---------------------------------------
            // Saving spacecraft
            //---------------------------------------
            soCount = mTempProp[i].soNameList.Count();
            
            #if DEBUG_PROPAGATE_PANEL
            MessageInterface::ShowMessage
               ("PropagatePanel::SaveData() soCount=%d\n", soCount);
            #endif
            
            for (Integer j=0; j<soCount; j++)
            {
               #if DEBUG_PROPAGATE_PANEL > 1
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() propIndex=%d, soNameList[%d]=%s\n",
                   i, j, mTempProp[i].soNameList[j].c_str());
               #endif
               
               thePropCmd->SetStringParameter
                  (scId, std::string(mTempProp[i].soNameList[j].c_str()), i);
               
            }
         }
      } //if (mPropChanged)
      
      if (updateButton->IsEnabled())
         UpdateStopCondition();
      
      //-------------------------------------------------------
      // Saving stopping condition
      //-------------------------------------------------------
      if (mStopCondChanged)
      {
         mStopCondChanged = false;
         
         Integer stopCount = 0;
         for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
         {
            if (mTempStopCond[i].desc != "")
               stopCount++;
         }
         
         #if DEBUG_PROPAGATE_PANEL
         MessageInterface::ShowMessage
            ("PropagatePanel::SaveData() stopCount=%d\n", stopCount);
         #endif
         
         //---------------------------------------------
         // if any stoppping condition removed,
         // clear Propagate StopCondition array and
         // set flag to add as new
         //---------------------------------------------
         if (stopCount < mStopCondCount)
         {
            thePropCmd->TakeAction("Clear", "StopCondition");
            for (Integer i=0; i<stopCount; i++)
               mTempStopCond[i].isChanged = true;
         }
         
         mStopCondCount = stopCount;
         
         //---------------------------------------------
         // Set stopCondition.
         //---------------------------------------------
         for (Integer i=0; i<mStopCondCount; i++)
         {
            if (mTempStopCond[i].isChanged)
            {
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::SaveData() mTempStopCond[%d].isChanged=%d\n", i,
                   mTempStopCond[i].isChanged);
               #endif
               
               if (mTempStopCond[i].stopCondPtr != NULL)
               {
                  mTempStopCond[i].stopCondPtr->
                     SetName(std::string(mTempStopCond[i].name.c_str()));
                  
                  mTempStopCond[i].stopCondPtr->
                     SetStringParameter("StopVar", mTempStopCond[i].varName.c_str());
                  
                  mTempStopCond[i].stopCondPtr->
                     SetStringParameter("Goal", mTempStopCond[i].goalStr.c_str());
                  
                  mTempStopCond[i].stopCondPtr->
                     SetRealParameter("Tol", mTempStopCond[i].tol);
                  
                  mTempStopCond[i].stopCondPtr->
                     SetIntegerParameter("Repeat", mTempStopCond[i].repeat);
                  
                  thePropCmd->
                     SetRefObject(mTempStopCond[i].stopCondPtr, Gmat::STOP_CONDITION,
                                  "", i);
                  
                  #if DEBUG_PROPAGATE_PANEL
                  StopCondition *stopCond = (StopCondition*)thePropCmd->
                     GetRefObject(Gmat::STOP_CONDITION,
                                  mTempStopCond[i].stopCondPtr->GetName(), i);
                  MessageInterface::ShowMessage
                     ("PropagatePanel::SaveData() name=%s, stopVarType=%s, goal = %s\n",
                      stopCond->GetName().c_str(),
                      stopCond->GetStringParameter("StopVar").c_str(),
                      stopCond->GetStringParameter("Goal").c_str());
                 #endif
               }
            }
         }
      }//if (mStopCondChanged)
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetMessage());
   }
}


//------------------------------------------------------------------------------
// void DisplayPropagator()
//------------------------------------------------------------------------------
void PropagatePanel::DisplayPropagator()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayPropagator() entered\n");
   #endif

   wxString name;
   for (Integer i=0; i<mTempPropCount; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, mTempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SOS_COL, mTempProp[i].soNames);
      propGrid->SetReadOnly(i, PROP_NAME_COL, true);
      propGrid->SetReadOnly(i, PROP_SOS_COL, true);
      
      name = mTempProp[i].propName;
      Integer x = name.Find("-");
      
      if (x == -1)
      {
          propGrid->SetCellValue(i, PROP_BK_COL, "No");
      }
      else
      {
          propGrid->SetCellValue(i, PROP_BK_COL, "Yes");
      }            
   }

   //propGrid->SelectRow(0);    
}


//------------------------------------------------------------------------------
// void DisplayStopCondition(Integer selRow)
//------------------------------------------------------------------------------
void PropagatePanel::DisplayStopCondition(Integer selRow)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::DisplayStopCondition() selRow=%d\n", selRow);
   #endif
   
   //----- for stopCondGrid - show all
   for (Integer i=0; i<MAX_STOPCOND_ROW; i++)
   {
      stopCondGrid->SetCellValue(i, STOPCOND_NAME_COL, mTempStopCond[i].name);
      stopCondGrid->SetCellValue(i, STOPCOND_DESC_COL, mTempStopCond[i].desc);        
   }

   //----- for detailed stop condition - show first row
   stopNameTextCtrl->SetValue(mTempStopCond[selRow].name);
   varNameTextCtrl->SetValue(mTempStopCond[selRow].varName);
   equalityComboBox->SetSelection(equalityComboBox->
                                  FindString(mTempStopCond[selRow].relOpStr));

   goalTextCtrl->SetValue(wxVariant(mTempStopCond[selRow].goalStr));

   //wxString strVal;
   //strVal.Printf("%le", mTempStopCond[selRow].tol);
   //toleranceTextCtrl->SetValue(strVal);
   
   stopCondGrid->SelectRow(selRow);
   varNameTextCtrl->Disable();
   
   if (mTempStopCond[selRow].varName.Contains(".Periapsis") ||
       mTempStopCond[selRow].varName.Contains(".Apoapsis"))
   {
      goalTextCtrl->Disable();
   }
}


//------------------------------------------------------------------------------
// void UpdateStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::UpdateStopCondition()
{
   Integer stopRow = mCurrStopRow;
   
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::UpdateStopCondition() stopRow=%d\n", stopRow);
   #endif
   
   wxString oldStopName = mTempStopCond[stopRow].name;
   mTempStopCond[stopRow].name = stopNameTextCtrl->GetValue();
   mTempStopCond[stopRow].varName = varNameTextCtrl->GetValue();
   mTempStopCond[stopRow].goalStr = goalTextCtrl->GetValue();
   //mTempStopCond[stopRow].tol = atof(toleranceTextCtrl->GetValue().c_str());
            
   wxString str = FormatStopCondDesc(mTempStopCond[stopRow].varName,
                                     mTempStopCond[stopRow].relOpStr,
                                     mTempStopCond[stopRow].goalStr);
   mTempStopCond[stopRow].desc = str;
   
   //-----------------------------------------------------------------
   // create StopCondition if new StopCondition
   //-----------------------------------------------------------------
   if (oldStopName.IsSameAs(""))
   {
      StopCondition *stopCond = (StopCondition*)theGuiInterpreter->
         CreateStopCondition("StopCondition", mTempStopCond[stopRow].name.c_str());
      mTempStopCond[stopRow].stopCondPtr = stopCond;

      if (stopCond == NULL)
      {
         MessageInterface::ShowMessage
            ("PropagatePanel::UpdateStopCondition() Unable to create "
             "StopCondition: name=%s\n", mTempStopCond[stopRow].name.c_str());
      }
   }
   else
   {
      mTempStopCond[stopRow].stopCondPtr->
         SetName(std::string(mTempStopCond[stopRow].name.c_str()));
   }
   
   stopCondGrid->SetCellValue(stopRow, STOPCOND_NAME_COL,
                              mTempStopCond[stopRow].name);
   stopCondGrid->SetCellValue(stopRow, STOPCOND_DESC_COL,
                              mTempStopCond[stopRow].desc);
   mTempStopCond[stopRow].isChanged = true;
   
   updateButton->Enable(false);
   mStopCondChanged = true;
   theApplyButton->Enable(true);
}


//------------------------------------------------------------------------------
// void RemoveStopCondition()
//------------------------------------------------------------------------------
void PropagatePanel::RemoveStopCondition()
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::RemoveStopCondition() mCurrStopRow=%d\n",
       mCurrStopRow);
   #endif
  
   // Add deleted StopCondition pointer to list so it can be removed
   // later in SaveData()
   if (mTempStopCond[mCurrStopRow].stopCondPtr != NULL)
   {
      //delete mTempStopCond[mCurrStopRow].stopCondPtr;
      //mTempStopCond[mCurrStopRow].stopCondPtr = NULL;
      
      mRemovedStopCondList.push_back(mTempStopCond[mCurrStopRow].stopCondPtr);
      
      #if DEBUG_PROPAGATE_PANEL
      MessageInterface::ShowMessage
         ("PropagatePanel::RemoveStopCondition() remove StopCondition addr=%d\n",
          mRemovedStopCondList.back());
      #endif
   }
   
   // go through the table and move upward if name is not blank
   for (Integer i=mCurrStopRow; i<MAX_STOPCOND_ROW; i++)
   {
      if (mTempStopCond[i+1].name != "")
      {
         // move upward
         mTempStopCond[i].isChanged = mTempStopCond[i+1].isChanged;
         mTempStopCond[i].name = mTempStopCond[i+1].name;
         mTempStopCond[i].desc = mTempStopCond[i+1].desc;
         mTempStopCond[i].varName = mTempStopCond[i+1].varName;
         mTempStopCond[i].typeName = mTempStopCond[i+1].typeName;
         mTempStopCond[i].relOpStr = mTempStopCond[i+1].relOpStr;
         mTempStopCond[i].goalStr = mTempStopCond[i+1].goalStr;
         mTempStopCond[i].tol = mTempStopCond[i+1].tol;
         mTempStopCond[i].repeat = mTempStopCond[i+1].repeat;
         mTempStopCond[i].stopCondPtr = mTempStopCond[i+1].stopCondPtr;
      }
      else
      {
         ClearStopCondition(i);
         break;
      }
   }

   mStopCondChanged = true;
   
   (mCurrStopRow > 0) ? mCurrStopRow = mCurrStopRow-1 : mCurrStopRow = 0;

   DisplayStopCondition(mCurrStopRow);
}


//------------------------------------------------------------------------------
// void ClearStopCondition(Integer selRow)
//------------------------------------------------------------------------------
void PropagatePanel::ClearStopCondition(Integer selRow)
{
   #if DEBUG_PROPAGATE_PANEL
   MessageInterface::ShowMessage
      ("PropagatePanel::ClearStopCondition() selRow=%d\n", selRow);
   #endif
   
   mTempStopCond[selRow].isChanged = false;
   mTempStopCond[selRow].name = "";
   mTempStopCond[selRow].desc = "";
   mTempStopCond[selRow].varName = "";
   mTempStopCond[selRow].typeName = "";
   mTempStopCond[selRow].relOpStr = "=";
   mTempStopCond[selRow].goalStr = "0.0";
   mTempStopCond[selRow].tol = 0.0;
   mTempStopCond[selRow].repeat = 1;
   mTempStopCond[selRow].stopCondPtr = NULL;
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnTextChange(wxCommandEvent& event)
{
   if (stopNameTextCtrl->IsModified() || varNameTextCtrl->IsModified() ||
       goalTextCtrl->IsModified())//    || toleranceTextCtrl->IsModified())
   {
      ActivateUpdateButton();
   }
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnComboBoxChange(wxCommandEvent& event)
{
   // Assume single selection
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   Integer stopRow = stopRows[0];
    
   if (event.GetEventObject() == mPropModeComboBox)
   {
      mPropModeChanged = true;
      theApplyButton->Enable(true);
   }
   else if (event.GetEventObject() == equalityComboBox)
   {
      wxString str = FormatStopCondDesc(mTempStopCond[stopRow].varName,
                                        mTempStopCond[stopRow].relOpStr,
                                        mTempStopCond[stopRow].goalStr);
      mTempStopCond[stopRow].desc = str;
      mTempStopCond[stopRow].isChanged = true;
      updateButton->Enable(true);
      theApplyButton->Enable(true);
   }
   else
      event.Skip();
}


//------------------------------------------------------------------------------
// void OnButtonClick(wxCommandEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnButtonClick(wxCommandEvent& event)
{
   if (event.GetEventObject() == mStopViewButton)
   {
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         varNameTextCtrl->SetValue(newParamName);
         
         // if stopping condition parameter changed, give it a new name
         stopNameTextCtrl->SetValue("StopOn" + newParamName);
         
         // if Apoapsis or Periapsis, disable goal
         if (newParamName.Contains(".Periapsis") ||
             newParamName.Contains(".Apoapsis"))
         {
            goalTextCtrl->Disable();
         }
         else
         {
            goalTextCtrl->Enable();
         }
         
         updateButton->Enable(true);
         theApplyButton->Enable(true);
      }
   }
   else if (event.GetEventObject() == mGoalViewButton)
   {
      // show dialog to select parameter
      ParameterSelectDialog paramDlg(this, mObjectTypeList);
      paramDlg.ShowModal();
      
      if (paramDlg.IsParamSelected())
      {
         wxString newParamName = paramDlg.GetParamName();
         goalTextCtrl->SetValue(newParamName);
         
         updateButton->Enable(true);
         theApplyButton->Enable(true);
      }
   }
   else if (event.GetEventObject() == updateButton)
   {
      UpdateStopCondition();
      theApplyButton->Enable(true);
   }
   else if (event.GetEventObject() == deleteButton)
   {
      RemoveStopCondition();
      theApplyButton->Enable(true);
   }
   else
      event.Skip();
}


//Note: Do not remove this code. It is used in showing stopping condition
//------------------------------------------------------------------------------
// void OnCellLeftClick(wxGridEvent& event)
//------------------------------------------------------------------------------
/*
 * On this event stopping condition can be added by clicking View button and
 * Update button to make update to the stopping condition grid.
 */
//------------------------------------------------------------------------------
void PropagatePanel::OnCellLeftClick(wxGridEvent& event)
{
   Integer row = event.GetRow();
  
   if (event.GetEventObject() == stopCondGrid)
   {
      stopCondGrid->SelectRow(row);
    
      if (updateButton->IsEnabled())
         UpdateStopCondition();
      
      mCurrStopRow = row;
      ShowDetailedStopCond(row);
      updateButton->Disable();
   }
   else if (event.GetEventObject() == propGrid)
   {
      propGrid->SelectRow(row);
   }
}


//------------------------------------------------------------------------------
// void OnCellRightClick(wxGridEvent& event)
//------------------------------------------------------------------------------
void PropagatePanel::OnCellRightClick(wxGridEvent& event)
{    
   Integer row = event.GetRow();
   Integer col = event.GetCol();

   if (row <= mTempPropCount)
   {
      if (event.GetEventObject() == propGrid)
      {
         propGrid->SelectRow(row);
         
         if (col == 0)
         {
            //----------------------------------
            // select propagator
            //----------------------------------
            PropagatorSelectDialog propDlg(this, mTempProp[row].propName);
            propDlg.ShowModal();
            
            if (propDlg.HasSelectionChanged())
            {
               wxString newPropName = propDlg.GetPropagatorName();
               
               if (newPropName == "") // remove propagator
               {
                  propGrid->SetCellValue(row, col, "");
                  mTempProp[row].isChanged = true;
                  mTempProp[row].propName = "";
                  if (row+1 < mTempPropCount)
                     MoveUpPropData();
                  else
                     mTempPropCount--;
               }
               else // change propagator
               {
                  propGrid->SetCellValue(row, col, newPropName);
                  mTempProp[row].isChanged = true;
                  mTempProp[row].propName = newPropName;
                  if (mTempPropCount <= row)
                     mTempPropCount = row + 1;
               }
               
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::OnCellRightClick() mTempPropCount=%d\n",
                   mTempPropCount);
               #endif
               
               mPropChanged = true;
               theApplyButton->Enable(true);
            }
         }
         else if (col == 1)
         {
            //----------------------------------
            // select spacecraft
            //----------------------------------
            wxArrayString soExcList;
            Integer soCount = 0;
            for (Integer i=0; i<mTempPropCount; i++)
            {
               soCount = mTempProp[i].soNameList.Count();
            
               for (Integer j=0; j<soCount; j++)
                  soExcList.Add(mTempProp[i].soNameList[j]);
            }
            
            SpaceObjectSelectDialog soDlg(this, mTempProp[row].soNameList, soExcList);
            soDlg.ShowModal();
         
            if (soDlg.IsSpaceObjectSelected())
            {
               wxArrayString &names = soDlg.GetSpaceObjectNames();
               mTempProp[row].isChanged = true;
               mTempProp[row].soNames = "";
               soCount = names.GetCount();
            
               #if DEBUG_PROPAGATE_PANEL
               MessageInterface::ShowMessage
                  ("PropagatePanel::OnCellRightClick() soCount=%d\n",
                   soCount);
               #endif
               
               mTempProp[row].soNameList.Clear();
               for (Integer j=0; j<soCount; j++)
               {
                  mTempProp[row].soNameList.Add(names[j]);
                  
                  #if DEBUG_PROPAGATE_PANEL
                  MessageInterface::ShowMessage
                     ("PropagatePanel::OnCellRightClick() soNameList[%d]=%s\n",
                      j, mTempProp[row].soNameList[j].c_str());
                  #endif
               }
            
               for(Integer j=0; j < soCount-1; j++)
               {
                  mTempProp[row].soNames += names[j].c_str();
                  mTempProp[row].soNames += ", ";
               }

               mTempProp[row].soNames += names[soCount-1].c_str();
               mTempProp[row].soCount = soCount;
               
               propGrid->SetCellValue(row, col, mTempProp[row].soNames);
               
               DisplayStopCondition(0);
               mPropChanged = true;
               theApplyButton->Enable(true);
            }
         }
         else if (col == 2)
         {
            wxString s1 = propGrid->GetCellValue(row, PROP_NAME_COL);
            wxString s2 = propGrid->GetCellValue(row, PROP_SOS_COL);
            
            // Exit if empty row
            if ( (s1 == "") || (s2 == "") )
               return;        

            wxString strArray[] = {wxT("Yes"), wxT("No")};        
      
            wxSingleChoiceDialog dialog(this, 
               _T("Propagate Backwards? \n"),
               _T("Backwards"), 2, strArray);
               
            dialog.SetSelection(0);
            
            if (dialog.ShowModal() == wxID_OK)
            {
               wxString str = dialog.GetStringSelection();
               wxString temp = propGrid->GetCellValue(row, col);
               
               wxString name = mTempProp[row].propName;
                  
               if (str == "Yes")
               {
                  Integer x = name.Find("-");
      
                  if (x == -1)
                     mTempProp[row].propName.Prepend("-");  
                  
                  propGrid->SetCellValue(row, PROP_BK_COL, "Yes");

               }
               else if (str == "No")
               {
                  wxString name = mTempProp[row].propName;
                  name.Replace("-", "", false);
                  mTempProp[row].propName = name; 
                  propGrid->SetCellValue(row, PROP_BK_COL, "No");   
               } 
               theApplyButton->Enable(true);
               mPropChanged = true;   
            }
         }
      }
   }
}

//------------------------------------------------------------------------------
// void ShowDetailedStopCond(Integer stopRow)
//------------------------------------------------------------------------------
void PropagatePanel::ShowDetailedStopCond(Integer stopRow)
{
   stopNameTextCtrl->SetValue(mTempStopCond[stopRow].name);
   varNameTextCtrl->SetValue(mTempStopCond[stopRow].varName);
   equalityComboBox->SetSelection(equalityComboBox->
                                  FindString(mTempStopCond[stopRow].relOpStr));
   goalTextCtrl->SetValue(wxVariant(mTempStopCond[stopRow].goalStr));
   
   wxString strVal;
   strVal.Printf("%le", mTempStopCond[stopRow].tol);
   //toleranceTextCtrl->SetValue(strVal);
   
   // if param name containes Apoapsis or Periapsis, disable goal
   if (mTempStopCond[stopRow].varName.Contains(".Periapsis") ||
       mTempStopCond[stopRow].varName.Contains(".Apoapsis"))
   {
      goalTextCtrl->Disable();
   }
   else
   {
      goalTextCtrl->Enable();
   }      
}


//------------------------------------------------------------------------------
// void ActivateUpdateButton()
//------------------------------------------------------------------------------
void PropagatePanel::ActivateUpdateButton()
{
   // assume single selection
   wxArrayInt stopRows = stopCondGrid->GetSelectedRows();
   Integer stopRow = stopRows[0];

   mTempStopCond[stopRow].isChanged = true;
   updateButton->Enable(true);
   theApplyButton->Enable(true);
}


//------------------------------------------------------------------------------
// wxString FormatStopCondDesc(const wxString &varName, const wxString &relOpStr,
//                             const wxString &goalStr)
//------------------------------------------------------------------------------
wxString PropagatePanel::FormatStopCondDesc(const wxString &varName,
                                            const wxString &relOpStr,
                                            const wxString &goalStr)
{
   wxString goalTempStr = goalStr;
   wxString desc;
   wxString opStr = relOpStr;
   
   if (varName.Contains("Apoapsis") || varName.Contains("Periapsis"))
   {
      opStr = "";
      goalTempStr = "";
   }
   
   desc = varName + " " + opStr + " " + goalTempStr;
    
   return desc;
}


//------------------------------------------------------------------------------
// void MoveUpPropData()
//------------------------------------------------------------------------------
void PropagatePanel::MoveUpPropData()
{
   PropType tempProp[MAX_PROP_ROW];
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      tempProp[i].isChanged = mTempProp[i].isChanged;
      tempProp[i].propName = mTempProp[i].propName;
      tempProp[i].soNames = mTempProp[i].soNames;
      tempProp[i].soNameList = mTempProp[i].soNameList;
   }
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      mTempProp[i].isChanged = false;
      mTempProp[i].propName = "";
      mTempProp[i].soNames = "";
      mTempProp[i].soNameList.Clear();
   }
   
   Integer propCount = 0;
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      if (tempProp[i].propName != "")
      {
         mTempProp[propCount].isChanged = tempProp[i].isChanged;
         mTempProp[propCount].propName = tempProp[i].propName;
         mTempProp[propCount].soNames = tempProp[i].soNames;
         mTempProp[propCount].soNameList = tempProp[i].soNameList;
         propCount++;
      }
   }

   mTempPropCount = propCount;
   
   for (Integer i=0; i<MAX_PROP_ROW; i++)
   {
      propGrid->SetCellValue(i, PROP_NAME_COL, mTempProp[i].propName);
      propGrid->SetCellValue(i, PROP_SOS_COL, mTempProp[i].soNames);
   }
}

