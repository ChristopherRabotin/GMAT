//$Header$
//------------------------------------------------------------------------------
//                              JacchiaRobertsDialog
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2004/04/06
// Modified:
/**
 * This class allows user to edit Jacchia-Roberts drag parameters.
 */
//------------------------------------------------------------------------------

#ifndef JacchiaRobertsDialog_hpp
#define JacchiaRobertsDialog_hpp

#include "gmatwxdefs.hpp"
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatAppData.hpp"
#include "GmatDialog.hpp"

#include "DragForce.hpp"

class JacchiaRobertsDialog : public GmatDialog
{
public:
    JacchiaRobertsDialog(wxWindow *parent, DragForce *dragForce);
    ~JacchiaRobertsDialog();
    DragForce* GetForce();
private:   
    wxStaticText *solarFluxStaticText;
    wxStaticText *fileNameStaticText;
    
    wxTextCtrl *solarFluxTextCtrl;
    wxTextCtrl *fileNameTextCtrl;
    
    wxButton *browseButton;
    
    DragForce *theForce;
    
    // Private methods
    void Initialize();
    
    // Methods inherited from GmatDialog
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    virtual void ResetData();
    
    // Event-handling Methods
    void OnTextChange();
    void OnBrowse();

    DECLARE_EVENT_TABLE();

    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 45000,
        ID_TEXTCTRL,
        ID_BUTTON
    };
};

#endif

