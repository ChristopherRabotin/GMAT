//$Header$
//------------------------------------------------------------------------------
//                              ForPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2004/05/17
/**
 * This class contains the Conditional Statement Setup window.
 */
//------------------------------------------------------------------------------
#ifndef ForPanel_hpp
#define ForPanel_hpp

// gui includes
#include "gmatwxdefs.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "Parameter.hpp"
#include "MessageInterface.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "For.hpp"

class ForPanel : public GmatPanel
{
public:
   // constructors
   ForPanel(wxWindow *parent, GmatCommand *cmd);
   ~ForPanel();  

private:
   static const int INDEX_COL = 0;
   static const int START_COL = 1;
   static const int INCR_COL = 2;
   static const int END_COL = 3;
   
   wxString mIndexString;
   wxString mStartString;
   wxString mEndString;
   wxString mIncrString;
   wxGrid *conditionGrid;
   
   bool mIndexIsSet;
   bool mIndexIsParam;
   bool mStartIsParam;
   bool mEndIsParam;
   bool mIncrIsParam;
   
   Real mStartValue;
   Real mIncrValue;
   Real mEndValue;  
   
   wxArrayString mObjectTypeList;
   For *theForCommand;
   Parameter* mIndexParam;  
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // Layout & data handling methods
   void Setup(wxWindow *parent);
   void OnCellValueChange(wxGridEvent& event);
   void OnCellRightClick(wxGridEvent& event);
   
   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_GRID = 99000
   };
};

#endif // ForPanel_hpp
