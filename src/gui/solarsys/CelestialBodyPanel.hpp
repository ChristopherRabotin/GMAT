//$Header$
//------------------------------------------------------------------------------
//                              CelestialBodyPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/07/06
//
/**
 * Declares CelestialBodyPanel class.
 */
//------------------------------------------------------------------------------

#ifndef CelestialBodyPanel_hpp
#define CelestialBodyPanel_hpp

#include "GmatPanel.hpp"
#include "CelestialBody.hpp"
#include "Planet.hpp"

class CelestialBodyPanel: public GmatPanel
{
public:
   
   // constructors
   CelestialBodyPanel(wxWindow *parent, const wxString &name);
    
private:
   
   CelestialBody *theCelestialBody;
   Planet *thePlanet;
   wxString bodyName;
   
   bool isTextModified;
   bool isStateTextModified;
   bool isRotDataSourceChanged;
   
   wxTextCtrl *mMuTextCtrl;
   wxTextCtrl *mEquatorialRadiusTextCtrl;
   wxTextCtrl *mFlatteningTextCtrl;
   wxTextCtrl *mEpochTextCtrl;
   wxTextCtrl *mIntervalTextCtrl;
   wxTextCtrl *mElement1TextCtrl;
   wxTextCtrl *mElement2TextCtrl;
   wxTextCtrl *mElement3TextCtrl;
   wxTextCtrl *mElement4TextCtrl;
   wxTextCtrl *mElement5TextCtrl;
   wxTextCtrl *mElement6TextCtrl;
   wxTextCtrl *mElement7TextCtrl;

   wxStaticText *muStaticText;
   wxStaticText *unitStaticTextMu;
   wxStaticText *equatorialRadiusStaticText;
   wxStaticText *unitStaticTextER;
   wxStaticText *flatteningStaticText;
   wxStaticText *unitStaticTextFlat;
   wxStaticText *epochStaticText;
   wxStaticText *unitStaticTextepoch;
   wxStaticText *intervalStaticText;
   wxStaticText *unitStaticTextinterval;
   wxStaticText *initialStaticText;
   wxStaticText *centralBodyText;
   wxStaticText *noCentralBodyText;
   wxStaticText *nameStaticText1;
   wxStaticText *unitStaticText1;
   wxStaticText *nameStaticText2;
   wxStaticText *unitStaticText2;
   wxStaticText *nameStaticText3;
   wxStaticText *unitStaticText3;
   wxStaticText *nameStaticText4;
   wxStaticText *unitStaticText4;
   wxStaticText *nameStaticText5;
   wxStaticText *unitStaticText5;
   wxStaticText *nameStaticText6;
   wxStaticText *unitStaticText6;
   wxStaticText *rotDataSourceText;
   
   wxComboBox *rotDataSourceCB;
   
   // methods inherited from GmatPanel
   virtual void Create();
   virtual void LoadData();
   virtual void SaveData();
    
   void EnableAll(bool enable);

   // event handling
   void OnTextUpdate(wxCommandEvent& event);
   void OnComboBoxChange(wxCommandEvent& event);
   
   DECLARE_EVENT_TABLE();

   // IDs for the controls and the menu commands
   enum
   {     
      ID_TEXT = 9000,
      ID_TEXTCTRL,
      ID_COMBO
   };
   
   static const std::string KEP_ELEMENT_NAMES[6];
   static const std::string KEP_ELEMENT_UNITS[6];
};

#endif
