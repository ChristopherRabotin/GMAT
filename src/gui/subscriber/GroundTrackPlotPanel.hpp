//$Id$
//------------------------------------------------------------------------------
//                              GroundTrackPlotPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2013/09/17
/**
 * Declares GroundTrackPlotPanel class. This class allows user to setup ground
 * track plot.
 */
//------------------------------------------------------------------------------
#ifndef GroundTrackPlotPanel_hpp
#define GroundTrackPlotPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GroundTrackPlot.hpp"

#ifdef __USE_COLOR_FROM_SUBSCRIBER__
#include <wx/clrpicker.h>           // for wxColorPickerCtrl
#include "RgbColor.hpp"
#include "ColorTypes.hpp"
#endif

class GroundTrackPlotPanel: public GmatPanel
{
public:
   GroundTrackPlotPanel(wxWindow *parent, const wxString &subscriberName);
   ~GroundTrackPlotPanel();
   
   virtual bool PrepareObjectNameChange();
   virtual void ObjectNameChanged(Gmat::ObjectType type,
                                  const wxString &oldName,
                                  const wxString &newName);
protected:
   GroundTrackPlot *mGroundTrackPlot;
   
   bool mHasIntegerDataChanged;
   bool mHasRealDataChanged;
   bool mHasDataOptionChanged;
   bool mHasObjectListChanged;
   bool mHasCentralBodyChanged;
   bool mHasTextureMapChanged;

   wxString mCentralBody;
   wxString mTextureFile;
   
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   bool mHasOrbitColorChanged;
   bool mHasTargetColorChanged;
   std::map<std::string, RgbColor> mOrbitColorMap;
   std::map<std::string, RgbColor> mTargetColorMap;
   wxColour mOrbitColor;
   wxColour mTargetColor;
   wxColourPickerCtrl *mOrbitColorCtrl;
   wxColourPickerCtrl *mTargetColorCtrl;
   #endif

   
   wxTextCtrl *mDataCollectFreqTextCtrl;
   wxTextCtrl *mUpdatePlotFreqTextCtrl;
   wxTextCtrl *mNumPointsToRedrawTextCtrl;
   wxTextCtrl *mTextureMapTextCtrl;
   
   wxButton *mTextureMapBrowseButton;
   
   wxCheckBox *mShowPlotCheckBox;
   
   wxCheckListBox *mObjectCheckListBox;
   
   wxComboBox *mCentralBodyComboBox;
   wxComboBox *mSolverIterComboBox;
   
   // for initializing data
   void InitializeData();
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
   
   // event handler
   void OnBrowseButton(wxCommandEvent& event);
   void OnCheckBoxChange(wxCommandEvent& event);
   void OnCheckListBoxChange(wxCommandEvent& event);
   void OnCheckListBoxSelect(wxCommandEvent& event);
   
   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   void OnColorPickerChange(wxColourPickerEvent& event);
   #endif
   
   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();
   
   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXTCTRL = 93000,
      ID_COMBOBOX,
      ID_CHECKLISTBOX,
      ID_CHECKBOX,
      ID_BROWSE_BUTTON,
      #ifdef __USE_COLOR_FROM_SUBSCRIBER__
      ID_COLOR_CTRL,
      #endif
   };
   
private:

   #ifdef __USE_COLOR_FROM_SUBSCRIBER__
   void ShowSpacePointColor(const wxString &name,
                            UnsignedInt color = GmatColor::RED);
   void SaveObjectColors(const wxString &which,
                         std::map<std::string, RgbColor> &colorMap);
   #endif
};
#endif


