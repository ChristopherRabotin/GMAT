//$Header$
//------------------------------------------------------------------------------
//                              OpenGlPlotSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/02/12
/**
 * Declares OpenGlPlotSetupPanel class. This class allows user to setup OpenGL Plot.
 */
//------------------------------------------------------------------------------
#ifndef OpenGlPlotSetupPanel_hpp
#define OpenGlPlotSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "OpenGlPlot.hpp"
#include "RgbColor.hpp"

class OpenGlPlotSetupPanel: public GmatPanel
{
public:
   OpenGlPlotSetupPanel(wxWindow *parent, const wxString &subscriberName);
   
protected:
   //Subscriber *mSubscriber; //loj: 9/28/04 commented out
   OpenGlPlot *mOpenGlPlot;
   
   bool mIsScChanged;
   bool mIsColorChanged;
   int mScCount;

   std::string mSelScName;
   std::map<std::string, RgbColor> mOrbitColorMap;
   std::map<std::string, RgbColor> mTargetColorMap;
   
   wxColour mScOrbitColor;
   wxColour mScTargetColor;
   
   wxListBox *mScAvailableListBox;
   wxListBox *mScSelectedListBox;
    
   wxButton *addScButton;
   wxButton *removeScButton;
   wxButton *clearScButton;
   wxButton *mScOrbitColorButton;
   wxButton *mScTargetColorButton;
   
   wxCheckBox *plotCheckBox;
   wxCheckBox *wireFrameCheckBox;
   wxCheckBox *targetStatusCheckBox;

   wxFlexGridSizer *mFlexGridSizer;
   wxBoxSizer *mScOptionBoxSizer;

   void OnAddSpacecraft(wxCommandEvent& event);
   void OnRemoveSpacecraft(wxCommandEvent& event);
   void OnClearSpacecraft(wxCommandEvent& event);
   void OnSelectSpacecraft(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnOrbitColorClick(wxCommandEvent& event);
   void OnTargetColorClick(wxCommandEvent& event);
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   DECLARE_EVENT_TABLE();
    
   // IDs for the controls and the menu commands
   enum
   {     
      TEXTCTRL = 93000,
      LISTBOX,
      SC_SEL_LISTBOX,
      CHECKBOX,
      ADD_SC_BUTTON,
      REMOVE_SC_BUTTON,
      CLEAR_SC_BUTTON,
      SC_ORBIT_COLOR_BUTTON,
      SC_TARGET_COLOR_BUTTON
   };

private:
   void ShowSpacecraftOption(const wxString &name, bool show = true);
  
};
#endif
