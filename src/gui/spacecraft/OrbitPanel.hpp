//$Header$
//------------------------------------------------------------------------------
//                            OrbitPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
//
// Author: Allison Greene
// Created: 2004/04/08
/**
 * This class contains information needed to setup users spacecraft ballistics
 * and mass through GUI
 * 
 */
//------------------------------------------------------------------------------
#ifndef OrbitPanel_hpp
#define OrbitPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "Spacecraft.hpp"
#include "TimeConverter.hpp"
#include "StateConverter.hpp"
#include "CoordinateConverter.hpp"
#include "CoordinateSystem.hpp"
#include "Rvector6.hpp"
#include "Anomaly.hpp"

class OrbitPanel: public wxPanel
{
public:
    OrbitPanel(wxWindow *parent, 
               Spacecraft *spacecraft,
               SolarSystem *solarsystem, 
               wxButton *theApplyButton);
    ~OrbitPanel();
    void SaveData();
    void LoadData();
   
protected:
   Anomaly           anomaly;
   TimeConverter     timeConverter;
   StateConverter    stateConverter;

private:
   Spacecraft     *theSpacecraft;
   SolarSystem    *theSolarSystem;
   GuiItemManager *theGuiManager;
   GuiInterpreter *theGuiInterpreter;

   bool mIsCoordSysChanged;
   bool mIsTextChanged;
   bool mIsStateChanged;

   Rvector6 mCartState;
   CoordinateConverter mCoordConverter;

   void Create();
   void AddElements(wxWindow *parent);
    
   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);

   void InitializeCoordinateSystem(CoordinateSystem *cs);
   void SetLabelsUnits(const std::string &stateType);

//   wxString bodiesArray[100];

   wxButton *theApplyButton;

   wxStaticText *description1;
   wxStaticText *description2;
   wxStaticText *description3;
   wxStaticText *description4;
   wxStaticText *description5;
   wxStaticText *description6;
    
   wxStaticText *label1;
   wxStaticText *label2;
   wxStaticText *label3;
   wxStaticText *label4;
   wxStaticText *label5;
   wxStaticText *label6;

   wxStaticText *anomalyStaticText;
    
   wxTextCtrl *textCtrl1;
   wxTextCtrl *textCtrl2;
   wxTextCtrl *textCtrl3;
   wxTextCtrl *textCtrl4;
   wxTextCtrl *textCtrl5;
   wxTextCtrl *textCtrl6;
    
   wxTextCtrl *epochValue;

   wxPanel *elementsPanel;

   wxComboBox *anomalyComboBox;
   wxComboBox *mCoordSysComboBox;
   wxComboBox *epochFormatComboBox;
   wxComboBox *stateTypeComboBox;

   std::string fromStateType;
   std::string fromEpochFormat;
   std::string fromCoordSys;
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 30200,
      ID_TEXTCTRL,
      ID_COMBOBOX,
        
      ID_STATIC_ELEMENT     
   };
};
#endif

