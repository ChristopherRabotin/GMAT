//$Header$
//------------------------------------------------------------------------------
//                              FiniteBurnSetupPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: LaMont Ruley
// Created: 2004/03/04
//
/**
 * This class contains the Finite Burn Setup window.
 */
//------------------------------------------------------------------------------

#ifndef FiniteBurnSetupPanel_hpp
#define FiniteBurnSetupPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"
#include "Burn.hpp"

class FiniteBurnSetupPanel : public GmatPanel
{
public:
    // constructors
    FiniteBurnSetupPanel( wxWindow *parent, const wxString &burnName);
    
private:
    static const int MAX_PROP_ROW = 5;

    // member data
    Burn *theBurn;

    wxComboBox *frameCB;

    // member functions
    void OnTextChange(wxCommandEvent& event);
    void OnFrameComboBoxChange(wxCommandEvent& event);

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

        ID_FRAME_COMBOBOX
    };
};

#endif // FiniteBurnSetupPanel_hpp
