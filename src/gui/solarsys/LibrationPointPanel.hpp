//$Header$
//------------------------------------------------------------------------------
//                              LibrationPointPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Allison Greene
// Created: 2005/04/21
/**
 * This class allows user to specify LibrationPoint bodies
 */
//------------------------------------------------------------------------------
#ifndef LibrationPointPanel_hpp
#define LibrationPointPanel_hpp

#include "gmatwxdefs.hpp"
#include "GmatPanel.hpp"
#include "GuiInterpreter.hpp"
#include "LibrationPoint.hpp"


class LibrationPointPanel: public GmatPanel
{
public:
    LibrationPointPanel(wxWindow *parent, const wxString &name);
    ~LibrationPointPanel();
    
   
private:
    // member data
    LibrationPoint *theLibrationPt;

    wxComboBox *primaryBodyCB;
    wxComboBox *secondaryBodyCB;
    wxComboBox *librationPtCB;
    
    // member functions
    void OnComboBoxChange(wxCommandEvent& event);

    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
        
    DECLARE_EVENT_TABLE();
    
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 6150,
        ID_COMBOBOX
    };
};
#endif



