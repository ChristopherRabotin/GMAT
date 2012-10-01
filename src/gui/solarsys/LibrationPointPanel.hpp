//$Id$
//------------------------------------------------------------------------------
//                              LibrationPointPanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Allison Greene
// Created: 2005/04/21
/**
 * This class allows user to configure a LibrationPoint.
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
   virtual bool RefreshObjects(Gmat::ObjectType type = Gmat::UNKNOWN_OBJECT);
    
   
private:
    // member data
    LibrationPoint *theLibrationPt;

    wxComboBox *primaryBodyCB;
    wxComboBox *secondaryBodyCB;
    wxComboBox *librationPtCB;
    
    // member functions
    void OnComboBoxChange(wxCommandEvent& event);
	void UpdateComboBoxes();

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



