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
//#include "MessageInterface.hpp"

//------------------------------
// static data
//------------------------------
int GuiItemManager::theNumSpacecraft = 0;
GuiItemManager* GuiItemManager::theInstance = NULL;
GuiInterpreter* GuiItemManager::theGuiInterpreter = NULL;
wxComboBox* GuiItemManager::theSpacecraftComboBox = NULL;
wxString* GuiItemManager::theSpacecraftList = NULL;

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
//  wxComboBox* GetSpacecraftComboBox(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * @return spacecraft combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSpacecraftComboBox(wxWindow *parent, int eventId)
{
    // combo box for avaliable spacecrafts

    int numSc = theNumSpacecraft;
    if (theNumSpacecraft == 0)
        numSc = 1;

    if (theSpacecraftComboBox != NULL)
        delete theSpacecraftComboBox;
    
    theSpacecraftComboBox =
        new wxComboBox(parent, eventId, wxT(""), wxDefaultPosition, 
                       wxSize(100, -1), numSc, theSpacecraftList, wxCB_DROPDOWN);
    
    // show first spacecraft
    theSpacecraftComboBox->SetSelection(0);
    
    return theSpacecraftComboBox;
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
    
    if ((theNumSpacecraft != numSc) || firstTime)
    {
        if (theSpacecraftList != NULL)
            delete theSpacecraftList;
        
        if (numSc > 0)  // check to see if any spacecrafts exist
        {
            theSpacecraftList = new wxString[numSc];
        
            for (int i=0; i<numSc; i++)
                theSpacecraftList[i] = wxString(listSc[i].c_str());
        }
        else
        {
            // combo box for avaliable spacecrafts
            theSpacecraftList = new wxString[1];
            theSpacecraftList[0] = wxString("No Spacecrafts Available");
        }

        theNumSpacecraft = numSc;
        //MessageInterface::ShowMessage("GuiItemManager::UpdateSpacecraftList() " +
        //                              std::string(theSpacecraftList[0].c_str()) + "\n");
    }
}
