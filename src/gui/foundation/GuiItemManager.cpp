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
GuiItemManager* GuiItemManager::theInstance = NULL;
GuiInterpreter* GuiItemManager::theGuiInterpreter = NULL;

int GuiItemManager::theNumObject = 0;
int GuiItemManager::theNumSpacecraft = 0;
int GuiItemManager::theNumParam = 0;
int GuiItemManager::theNumConfigParam = 0;

wxString* GuiItemManager::theObjectList = NULL;
wxString* GuiItemManager::theSpacecraftList = NULL;
wxString* GuiItemManager::theParamList = NULL;
wxString* GuiItemManager::theConfigParamList = NULL;

wxComboBox* GuiItemManager::theSpacecraftComboBox = NULL;
wxListBox* GuiItemManager::theObjectListBox = NULL;
wxListBox* GuiItemManager::theParamListBox = NULL;
wxListBox* GuiItemManager::theConfigParamListBox = NULL;

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
//  void UpdateObject()
//------------------------------------------------------------------------------
/**
 * Updates spacecraft related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateObject()
{
    UpdateObjectList();
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
//  void UpdateParameter()
//------------------------------------------------------------------------------
/**
 * Updates parameter related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameter(const wxString &objName)
{
    UpdateParameterList(objName);
    UpdateConfigParameterList(objName);
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
    
    theSpacecraftComboBox =
        new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                       numSc, theSpacecraftList, wxCB_DROPDOWN);
    
    // show first spacecraft
    theSpacecraftComboBox->SetSelection(0);
    
    return theSpacecraftComboBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetObjectListBox(wxWindow *parent, const wxSize &size,
//                             const wxString &objName, int numObj)
//------------------------------------------------------------------------------
/**
 * @return Available Object ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetObjectListBox(wxWindow *parent, const wxSize &size)
    //const wxString &objName, int numObj)
{

    // Spacecraft
    //loj: 2/23/04 add other objects in the future build
    
    int numObj = theNumObject;
    
    if (theNumObject == 0)
        numObj = 1;
    
    wxString emptyList[] = {};
        
    if (numObj > 0)
    {       
        theObjectListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, numObj,
                          theObjectList, wxLB_SINGLE);
    }
    else
    {       
        theObjectListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
    }
   
    return theObjectListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetParameterListBox(wxWindow *parent, const wxSize &size,
//                                const wxString &objName, int numObj)
//------------------------------------------------------------------------------
/**
 * @return Available Parameter ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetParameterListBox(wxWindow *parent, const wxSize &size,
                                               const wxString &objName, int numObj)
{
        
    wxString emptyList[] = {};
        
    if (numObj > 0)
    {       
        theParamListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, theNumParam,
                          theParamList, wxLB_SINGLE);
    }
    else
    {       
        theParamListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
    }
   
    return theParamListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetConfigParameterListBox(wxWindow *parent, const wxSize &size,
//                                      const wxString &nameToExclude)
//------------------------------------------------------------------------------
/**
 * @return Configured ConfigParameterListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetConfigParameterListBox(wxWindow *parent,
                                                     const wxSize &size,
                                                     const wxString &nameToExclude)
{
        
    wxString emptyList[] = {};
    wxString *newConfigParamList;
    int numParams = 0;
    
    if (nameToExclude != "" && theNumConfigParam >= 2)
    {
        newConfigParamList = new wxString[theNumConfigParam-1];
        
        for (int i=0; i<theNumConfigParam; i++)
        {
            if (theConfigParamList[i] != nameToExclude)
                newConfigParamList[numParams++] = theConfigParamList[i];
        }
        
        theConfigParamListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, theNumConfigParam-1,
                          newConfigParamList, wxLB_SINGLE);
    }
    else
    {
        if (theNumConfigParam > 0)
        {       
            theConfigParamListBox =
                new wxListBox(parent, -1, wxDefaultPosition, size, theNumConfigParam,
                              theConfigParamList, wxLB_SINGLE);
        }
        else
        {       
            theConfigParamListBox =
                new wxListBox(parent, -1, wxDefaultPosition, size, 0,
                              emptyList, wxLB_SINGLE);
        }
    }
    
    return theConfigParamListBox;
}


//-------------------------------
// priavate methods
//-------------------------------

//------------------------------------------------------------------------------
//  void UpdateObjectList(bool firstTime)
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateObjectList(bool firstTime)
{
    // Spacecraft
    //loj: add other objects later
    
    StringArray &listSc = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
    int numObj = listSc.size();

    if (theObjectList != NULL)
    {
        delete theObjectList;
        theObjectList = NULL;
    }
    
    if (numObj > 0)  // check to see if any objects exist
    {
        theObjectList = new wxString[numObj];
        
        for (int i=0; i<numObj; i++)
        {
            theObjectList[i] = wxString(listSc[i].c_str());
            //MessageInterface::ShowMessage("GuiItemManager::UpdateObjectList() " +
            //                              std::string(theObjectList[i].c_str()) + "\n");
        }
    }
    else
    {
        theObjectList = new wxString[1];
        theObjectList[0] = wxString("-- None --");
    }
        
    theNumObject = numObj;
}

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

    if (theSpacecraftList != NULL)
    {
        //delete theSpacecraftList;
        //theSpacecraftList = NULL;
        theSpacecraftList->Clear();
        
    }
    
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
    else
    {
        theSpacecraftList = new wxString[1];
        theSpacecraftList[0] = wxString("-- None --");
    }
        
    theNumSpacecraft = numSc;
}

//------------------------------------------------------------------------------
//  void UpdateParameterList(const wxString &objName, bool firstTime = false)
//------------------------------------------------------------------------------
/**
 * Updates available parameter list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameterList(const wxString &objName, bool firstTime)
{
    //MessageInterface::ShowMessage("GuiItemManager::UpdateParameterList() " +
    //                              std::string(objName.c_str()) + "\n");
    
    if (theParamList != NULL)
    {
        delete theParamList;
        theParamList = NULL;
    }

    //loj: How do I know which parameter belong to which object type?
    //loj: for B2 show all paramters
    StringArray items =
        theGuiInterpreter->GetListOfFactoryItems(Gmat::PARAMETER);
    theNumParam = items.size();

    theParamList = new wxString[theNumParam];
    
    int len;
    for (int i=0; i<theNumParam; i++)
    {
        theParamList[i] = items[i].c_str();
        len = theParamList[i].Length();

        // remove "Param" from the parameter name
        // Note: When a new Parameter is created "Param" should be appended
        //       That's the constructor name
        theParamList[i].Remove(len-5, 5);
        
        //MessageInterface::ShowMessage("GuiItemManager::UpdateParameterList() " +
        //                              std::string(theParamList[i].c_str()) + "\n");
    }
}

//------------------------------------------------------------------------------
//  void UpdateConfigParameterList(const wxString &objName = "", bool firstTime = false)
//------------------------------------------------------------------------------
/**
 * Updates confugured parameter list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateConfigParameterList(const wxString &objName, bool firstTime)
{
    //MessageInterface::ShowMessage("GuiItemManager::UpdateConfigParameterList() " +
    //                              std::string(objName.c_str()) + "\n");
    
    if (theConfigParamList != NULL)
    {
        delete theConfigParamList;
        theConfigParamList = NULL;
    }

    StringArray items =
        theGuiInterpreter->GetListOfConfiguredItems(Gmat::PARAMETER);
    theNumConfigParam = items.size();

    theConfigParamList = new wxString[theNumConfigParam];
    
    for (int i=0; i<theNumConfigParam; i++)
    {
        if (objName == "")
            theConfigParamList[i] = items[i].c_str();
        else
            theConfigParamList[i] = objName + "." + items[i].c_str();
        
        //MessageInterface::ShowMessage("GuiItemManager::UpdateConfigParameterList() " +
        //                             std::string(theConfigParamList[i].c_str()) + "\n");
    }
}

