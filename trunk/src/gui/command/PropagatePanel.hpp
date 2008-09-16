//$Id$
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
#include "GmatStaticBoxSizer.hpp"

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
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
   
private:
   
   static const Integer MAX_PROP_ROW     = 5;
   static const Integer MAX_STOPCOND_ROW = 10;
   static const Integer PROP_NAME_COL = 0;
   static const Integer PROP_SOS_COL  = 1;
   static const Integer STOPCOND_PARAM_COL   = 0;
   static const Integer STOPCOND_RELOPER_COL = 1;
   static const Integer STOPCOND_COND_COL    = 2;
   
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
      //wxString       typeName;
      wxString       relOpStr;
      wxString       goalStr;
      StopCondition *stopCondPtr;
   };
   
   wxGrid *propGrid;
   wxGrid *stopCondGrid;

   wxCheckBox *backPropCheckBox;
   wxTextCtrl *mStopTolTextCtrl;
   wxComboBox *mPropModeComboBox;
   wxComboBox *equalityComboBox;
   
   bool mPropModeChanged;
   bool mPropDirChanged;
   bool mPropSatChanged;
   bool mStopCondChanged;
   bool mStopTolChanged;
   bool mPropGridSelected;
   bool mStopCondGridSelected;
   
   Integer mPropModeCount;
   Integer mPropCount;
   Integer mSpaceObjectCount;
   Integer mStopCondCount;
   PropType mTempProp[MAX_PROP_ROW];
   StopCondType mTempStopCond[MAX_STOPCOND_ROW];
   
   Propagate *thePropCmd;
   Parameter *theParameter;
   
   wxArrayString mObjectTypeList;
   std::vector<StopCondition*> mRemovedStopCondList;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   void InitializeData();
   void DisplayPropagator();
   void DisplayStopCondition();
   void UpdateStopCondition(Integer stopRow);
   
   wxString FormatStopCondDesc(const wxString &varName,
                               const wxString &relOpStr,
                               const wxString &goalStr);
   
   // event handling method
   void OnTextChange(wxCommandEvent& event);
   void OnCellRightClick(wxGridEvent &event);
   void OnCellValueChange(wxGridEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent &event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 44000,
      ID_TEXTCTRL,
      ID_COMBOBOX,
      ID_CHECKBOX,
      ID_GRID,
   };
};

#endif // PropagatePanel_hpp

