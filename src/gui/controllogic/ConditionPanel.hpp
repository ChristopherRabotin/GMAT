//$Id$
//------------------------------------------------------------------------------
//                              ConditionPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2011/01/06 based on IfPanel which was developed by Waka Waktola
//
/**
 * This class contains the If and While command setup window.
 */
//------------------------------------------------------------------------------
#ifndef ConditionPanel_hpp
#define ConditionPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"

class ConditionPanel : public GmatPanel
{
public:
   // constructors
   ConditionPanel(wxWindow *parent, GmatCommand *cmd);
   ~ConditionPanel();  
   
private:
   wxGrid *conditionGrid;
   
   static const int MAX_ROW     = 10;
   static const int MAX_COL     = 6;
   static const int COMMAND_COL = 0;
   static const int LHS_SEL_COL = 1;
   static const int LHS_COL     = 2;
   static const int COND_COL    = 3;
   static const int RHS_SEL_COL = 4;
   static const int RHS_COL     = 5;
   
   ConditionalBranch *theCommand;
   Parameter *theParameter;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   void UpdateSpecialColumns();
   
   // data handling methods
   void GetNewValue(Integer row, Integer col);
   
   // Grid table event methods
   void OnCellDoubleClick(wxGridEvent& event);
   void OnCellLeftClick(wxGridEvent& event);
   void OnCellRightClick(wxGridEvent& event);
   void OnCellValueChange(wxGridEvent& event);
   void OnKeyDown(wxKeyEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_GRID = 50000,
   };
   
   Integer mNumberOfConditions;
   Integer mNumberOfLogicalOps;
   
   std::vector<bool> mLhsIsParam;
   std::vector<bool> mRhsIsParam;
   
   wxArrayString mObjectTypeList;
   StringArray mLogicalOpStrings;
   StringArray mLhsList;
   StringArray mEqualityOpStrings;
   StringArray mRhsList;   
};

#endif // ConditionPanel_hpp
