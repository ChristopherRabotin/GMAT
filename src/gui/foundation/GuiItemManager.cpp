//$Header$
//------------------------------------------------------------------------------
//                              GuiItemManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/02/06
//
/**
 * Implements GuiItemManager class.
 */
//------------------------------------------------------------------------------

#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "gmatdefs.hpp" //put this one after GUI includes
#include "MessageInterface.hpp"

//------------------------------
// static data
//------------------------------
int GuiItemManager::theNumSpacecraft = 0;
int GuiItemManager::theNumParameter = 0;
GuiItemManager* GuiItemManager::theInstance = NULL;
GuiInterpreter* GuiItemManager::theGuiInterpreter = NULL;
wxString* GuiItemManager::theParameterList = NULL;
wxString* GuiItemManager::theSpacecraftList = NULL;
wxComboBox* GuiItemManager::theSpacecraftComboBox = NULL;
wxListBox* GuiItemManager::theParameterListBox = NULL;

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
//  GuiItemManager* GetInstance()
//------------------------------------------------------------------------------
/**
 * @return Instance of the GuiItemManager.
 */
//------------------------------------------------------------------------------
GuiItemManager* GuiItemManager::GetInstance()
{
   if (theInstance == NULL)
   {
       theInstance = new GuiItemManager();
       theGuiInterpreter = GmatAppData::GetGuiInterpreter();
       UpdateSpacecraftList(true);
   }
   return theInstance;
}

//------------------------------------------------------------------------------
//  void UpdateSpacecraft()
//------------------------------------------------------------------------------
/**
 * Updates spacecraft related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraft()
{
    UpdateSpacecraftList();
}

//------------------------------------------------------------------------------
//  void UpdateParameter(const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Updates spacecraft related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameter(const wxString &objName)
{
    UpdateParameterList(objName);
}

//------------------------------------------------------------------------------
//  wxComboBox* GetSpacecraftComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return spacecraft combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSpacecraftComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
    // combo box for avaliable spacecrafts

    int numSc = theNumSpacecraft;
    if (theNumSpacecraft == 0)
        numSc = 1;

    if (theSpacecraftComboBox != NULL)
        delete theSpacecraftComboBox;
    
    theSpacecraftComboBox =
        new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                       numSc, theSpacecraftList, wxCB_DROPDOWN);
    
    // show first spacecraft
    theSpacecraftComboBox->SetSelection(0);
    
    return theSpacecraftComboBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetParameterListBox(wxWindow *parent, const wxSize &size,
//                                const wxString &objName, int numObj)
//------------------------------------------------------------------------------
/**
 * @return Parameter ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetParameterListBox(wxWindow *parent, const wxSize &size,
                                               const wxString &objName, int numObj)
{
    
    if (theParameterListBox != NULL)
        delete theParameterListBox;
    
    if (numObj > 0)
    {       
        theParameterListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, theNumParameter,
                          theParameterList, wxLB_SINGLE);
    }
   
    return theParameterListBox;
}


//-------------------------------
// priavate methods
//-------------------------------

//------------------------------------------------------------------------------
//  void UpdateSpacecraftList(bool firstTime)
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraftList(bool firstTime)
{
    StringArray &listSc = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    int numSc = listSc.size();

    if (firstTime)
    {
        // combo box for avaliable spacecrafts
        theSpacecraftList = new wxString[1];
        theSpacecraftList[0] = wxString("No Spacecrafts Available");
    }
    else
    {
        if (theSpacecraftList != NULL)
            delete theSpacecraftList;
        
        if (numSc > 0)  // check to see if any spacecrafts exist
        {
            theSpacecraftList = new wxString[numSc];
        
            for (int i=0; i<numSc; i++)
            {
                theSpacecraftList[i] = wxString(listSc[i].c_str());
                //MessageInterface::ShowMessage("GuiItemManager::UpdateSpacecraftList() " +
                //                              std::string(theSpacecraftList[i].c_str()) + "\n");
             }
        }

        theNumSpacecraft = numSc;
    }
}

//------------------------------------------------------------------------------
//  void UpdateParameterList(const wxString &objName, bool firstTime = false)
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameterList(const wxString &objName, bool firstTime)
{
    MessageInterface::ShowMessage("GuiItemManager::UpdateParameterList() " +
                                  std::string(objName.c_str()) + "\n");
    
    if (theParameterList != NULL)
    {
        delete theParameterList;
        theParameterList = NULL;
    }

    StringArray items =
        theGuiInterpreter->GetListOfConfiguredItems(Gmat::PARAMETER);
    theNumParameter = items.size();

    theParameterList = new wxString[theNumParameter];
    
    for (int i=0; i<theNumParameter; i++)
    {
        theParameterList[i] = objName + "." + items[i].c_str();
        MessageInterface::ShowMessage("GuiItemManager::UpdateParameterList() " +
                                      std::string(theParameterList[i].c_str()) + "\n");
    }
}
