//$Header$
//------------------------------------------------------------------------------
//                              MSISE90Dialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit MSISE90 drag parameters.
 */
//------------------------------------------------------------------------------

#ifndef MSISE90Dialog_hpp
#define MSISE90Dialog_hpp

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatDialog.hpp"

#include "DragForce.hpp"

class MSISE90Dialog : public GmatDialog
{
public:
    MSISE90Dialog(wxWindow *parent, DragForce *dragForce);
    ~MSISE90Dialog();
    DragForce* GetForce();
private:   
    wxStaticText *solarFluxStaticText;
    wxStaticText *avgSolarFluxStaticText;
    wxStaticText *geomagneticIndexStaticText;
    wxStaticText *fileNameStaticText;
    
    wxTextCtrl *solarFluxTextCtrl;
    wxTextCtrl *avgSolarFluxTextCtrl;
    wxTextCtrl *geomagneticIndexTextCtrl;
    wxTextCtrl *fileNameTextCtrl;
    
    wxButton *browseButton;
    
    wxRadioButton *userInputRadioButton;
    wxRadioButton *fileInputRadioButton;
    
    DragForce *theForce;
    
    Integer solarFluxID;
    Integer avgSolarFluxID;
    Integer geomagnecticIndexID;
    Integer solarFluxFileID;
    Integer inputSourceID;
    
    wxString inputSourceString;
    
    bool useFile;
    
    // Private methods
    void Initialize();
    void Update();
    
    // Methods inherited from GmatDialog
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void ResetData();
    
    // Event-handling Methods
    void OnTextChange();
    void OnRadioButtonChange(wxCommandEvent& event);
    void OnBrowse();

    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 45000,
        ID_TEXTCTRL,
        ID_BUTTON,
        ID_RADIOBUTTON
    };
};

#endif

