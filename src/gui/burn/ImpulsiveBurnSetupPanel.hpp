//$Header$
//------------------------------------------------------------------------------
//                              ImpulsiveBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/02/04
//
/**
 * This class contains the Impulsive Burn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef ImpulsiveBurnSetupPanel_hpp
#define ImpulsiveBurnSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "Burn.hpp"

class ImpulsiveBurnSetupPanel : public GmatPanel
{
public:
    // constructors
    ImpulsiveBurnSetupPanel( wxWindow *parent, const wxString &burnName);
    
private:
    // member data
    GuiInterpreter *theGuiInterpreter;
    Burn *theBurn;

    wxComboBox *frameCB;
    wxComboBox *formatCB;
    
    wxPanel *vectorPanel;

    wxStaticText *description1;
    wxStaticText *description2;
    wxStaticText *description3;
    wxStaticText *label1;
    wxStaticText *label2;
    wxStaticText *label3;
    
    wxTextCtrl *textCtrl1;
    wxTextCtrl *textCtrl2;
    wxTextCtrl *textCtrl3;

    // member functions
    void AddVector(wxWindow *parent);
    void LabelsUnits();
    void OnTextChange(wxCommandEvent& event);
    void OnFrameComboBoxChange(wxCommandEvent& event);
    void OnFormatComboBoxChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 81000,
        ID_TEXTCTRL,

        ID_FORMAT_COMBOBOX,
        ID_FRAME_COMBOBOX,

        ID_STATIC_VECTOR
    };
};

#endif // ImpulsiveBurnSetupPanel_hpp
