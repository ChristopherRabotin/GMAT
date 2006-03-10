//$Header$
//------------------------------------------------------------------------------
//                              PropagatePanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2003/09/08
// Modified:
//    2004/05/06 Allison Greene: inherit from GmatPanel
//    2004/10/20 Linda Jun: rename from PropgateCommandPanel
/**
 * This class contains the Propagate command setup window.
 */
//------------------------------------------------------------------------------

#ifndef PropagatePanel_hpp
#define PropagatePanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "Propagate.hpp"
#include "StopCondition.hpp"
#include "PropSetup.hpp"

class PropagatePanel : public GmatPanel
{
public:
   // constructors
   PropagatePanel(wxWindow *parent, GmatCommand *cmd);
   ~PropagatePanel();  
   
private:
   
   static const Integer MAX_PROP_ROW = 5;
   static const Integer MAX_STOPCOND_ROW = 10;
   static const Integer PROP_NAME_COL = 0;
   static const Integer PROP_SOS_COL = 1;
   static const Integer PROP_BK_COL = 2;
   static const Integer STOPCOND_NAME_COL = 0;
   static const Integer STOPCOND_DESC_COL = 1;
   
   struct PropType
   {
      bool isChanged;
      Integer  soCount;
      wxString propName;
      wxString soNames;
      wxArrayString soNameList;
   };
   
   struct StopCondType
   {
      bool           isChanged;
      wxString       name;
      wxString       desc;
      wxString       varName;
      wxString       typeName;
      wxString       relOpStr;
      wxString       goalStr;
      Real           tol;
      Integer        repeat;
      StopCondition *stopCondPtr;
   };
   
   wxGrid *propGrid;
   wxGrid *stopCondGrid;
   
   wxTextCtrl *stopNameTextCtrl;
   wxTextCtrl *varNameTextCtrl;
   wxTextCtrl *goalTextCtrl;
   //wxTextCtrl *toleranceTextCtrl;
   
   wxButton *updateButton;
   wxButton *deleteButton;
   wxButton *mStopViewButton;
   wxButton *mGoalViewButton;
   
   wxComboBox *mPropModeComboBox;
   wxComboBox *equalityComboBox;
   
   wxBoxSizer *mMiddleBoxSizer;
   wxStaticBoxSizer *mStopSizer;
   
   bool    mPropModeChanged;
   bool    mPropChanged;
   bool    mStopCondChanged;
   
   Integer mPropModeCount;
   Integer numOfEqualities;
   Integer mPropCount;
   Integer mSpaceObjectCount;
   Integer mStopCondCount;
   Integer mTempPropCount;
   Integer mCurrStopRow;
   PropType mTempProp[MAX_PROP_ROW];
   StopCondType mTempStopCond[MAX_STOPCOND_ROW];
   
   Propagate *thePropCmd;
   std::vector<StopCondition*> mRemovedStopCondList;
   
   // methods inherited from GmatPanel
   virtual void OnCancel(wxCommandEvent &event);
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   void DisplayPropagator();
   void DisplayStopCondition(Integer selRow);
   void UpdateStopCondition();
   void RemoveStopCondition();
   void ClearStopCondition(Integer selRow);
   void ShowDetailedStopCond(Integer stopRow);
   void ActivateUpdateButton();
   
   // event handling method
   void OnTextChange(wxCommandEvent &event);
   void OnComboBoxChange(wxCommandEvent &event);
   void OnButtonClick(wxCommandEvent &event);
   void OnCellLeftClick(wxGridEvent &event);
   void OnCellRightClick(wxGridEvent &event);

   wxString FormatStopCondDesc(const wxString &varName,
                               const wxString &relOpStr,
                               const wxString &goalStr);
   void MoveUpPropData();
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 44000,
      ID_COMBOBOX,
      ID_BUTTON,
      ID_CHECKBOX,
      ID_GRID,
     ID_TEXT,
   };
};

#endif // PropagatePanel_hpp

