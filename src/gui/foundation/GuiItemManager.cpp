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
#include "gmatdefs.hpp"           //put this one after GUI includes
#include "Parameter.hpp"
#include "MessageInterface.hpp"
#include <algorithm>              // for sort(), set_difference()

//#define DEBUG_GUI_ITEM 1
//#define DEBUG_GUI_ITEM_PROPERTY 1

//------------------------------
// static data
//------------------------------
GuiItemManager* GuiItemManager::theInstance = NULL;

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
   }
   return theInstance;
}

//------------------------------------------------------------------------------
//  void UpdateAll()
//------------------------------------------------------------------------------
/**
 * Updates all objects.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateAll()
{
   if (theNumScProperty == 0)
      UpdatePropertyList("Spacecraft");
   
   UpdateFormation();
   UpdateSpacecraft();
   UpdateParameter();
   UpdateSolarSystem();
}

//------------------------------------------------------------------------------
//  void UpdateSpaceObject()
//------------------------------------------------------------------------------
/**
 * Updates general object gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpaceObject()
{
   UpdateSpaceObjectList();
}

//------------------------------------------------------------------------------
//  void UpdateFormation()
//------------------------------------------------------------------------------
/**
 * Updates Formation related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormation()
{
   UpdateFormationList();
   UpdateSpaceObjectList();
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
   UpdateSpaceObjectList();
}

//------------------------------------------------------------------------------
//  void UpdateParameter()
//------------------------------------------------------------------------------
/**
 * Updates parameter related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameter()
{
   UpdateParameterList();
}

//------------------------------------------------------------------------------
//  void UpdateSolarSystem()
//------------------------------------------------------------------------------
/**
 * Updates celestial body related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolarSystem()
{
   UpdateConfigBodyList();
}

//------------------------------------------------------------------------------
//  void UpdateCoordSystem()
//------------------------------------------------------------------------------
/**
 * Updates coordinate system related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCoordSystem()
{
   UpdateCoordSystemList();
}

//------------------------------------------------------------------------------
// int GetNumProperty(const wxString &objName)
//------------------------------------------------------------------------------
/*
 * @return number of properties associated with objName
 */
//------------------------------------------------------------------------------
int GuiItemManager::GetNumProperty(const wxString &objName)
{
   if (objName == "Spacecraft")
      return theNumScProperty;
   else
      return 0;
}

//------------------------------------------------------------------------------
// wxString* GetPropertyList(const wxString &objName)
//------------------------------------------------------------------------------
/*
 * @return propertie string array associated with objName
 */
//------------------------------------------------------------------------------
wxString* GuiItemManager::GetPropertyList(const wxString &objName)
{
   if (objName != "Spacecraft")
      throw GmatBaseException("There are no properties associated with " +
                              std::string(objName.c_str()));
   
   return theScPropertyList;
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
                     numSc, theSpacecraftList, wxCB_READONLY);
    
   // show first spacecraft
   theSpacecraftComboBox->SetSelection(0);
    
   return theSpacecraftComboBox;
}

//------------------------------------------------------------------------------
//  wxComboBox* GetCoordSysComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return coordinate system combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetCoordSysComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{
   // combo box for avaliable coordinate system
   
   int numCs = theNumCoordSys;
   
   if (theNumCoordSys == 0)
      numCs = 3; //loj: ComboBox is too small if 1
   
   theCoordSysComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     numCs, theCoordSysList, wxCB_READONLY);
   
   // show first coordinate system
   theCoordSysComboBox->SetSelection(0);
   
   return theCoordSysComboBox;
}

//------------------------------------------------------------------------------
//  wxComboBox* GetConfigBodyComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return celestial body combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetConfigBodyComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   int numBody = theNumConfigBody;
   
   if (theNumConfigBody == 0)
      numBody = 1;
   
   theConfigBodyComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     numBody, theConfigBodyList, wxCB_READONLY);
   
   // show Earth as a default body
   theConfigBodyComboBox->SetStringSelection("Earth");
   
   return theConfigBodyComboBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
//                                  const wxSize &size, wxArrayString &namesToExclude)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
                                                 const wxSize &size,
                                                 wxArrayString &namesToExclude)
{
#if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpaceObjectListBox() theNumSpaceObject=%d\n",
       theNumSpaceObject);
#endif
      
   wxString emptyList[] = {};

   if (namesToExclude.IsEmpty())
   {
      
#if DEBUG_GUI_ITEM
      MessageInterface::ShowMessage
         ("GuiItemManager::GetSpaceObjectListBox() namesToExclude=0\n");
#endif
      
      if (theNumSpaceObject > 0)
      {       
         theSpaceObjectListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumSpaceObject,
                          theSpaceObjectList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theSpaceObjectListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   else
   {
      int exclCount = namesToExclude.GetCount();
      int newSpaceObjCount = theNumSpaceObject - exclCount;
      
#if DEBUG_GUI_ITEM
      MessageInterface::ShowMessage
         ("GuiItemManager::GetSpaceObjectListBox() newSpaceObjCount = %d\n",
          newSpaceObjCount);
#endif

      if (newSpaceObjCount > 0)
      {
         wxString *newSpaceObjList = new wxString[newSpaceObjCount];
         bool excludeName;
         int numSpaceObj = 0;
        
         for (int i=0; i<theNumSpaceObject; i++)
         {
            excludeName = false;
            for (int j=0; j<exclCount; j++)
            {
               if (theSpaceObjectList[i] == namesToExclude[j])
               {
                  excludeName = true;
                  break;
               }
            }
            
            if (!excludeName)
               newSpaceObjList[numSpaceObj++] = theSpaceObjectList[i];
         }

         theSpaceObjectListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, newSpaceObjCount,
                          newSpaceObjList, wxLB_SINGLE|wxLB_SORT);
         delete newSpaceObjList;
      }
      else
      {
#if DEBUG_GUI_ITEM
         MessageInterface::ShowMessage
            ("GuiItemManager::GetSpaceObjectListBox() emptyList\n");
#endif
         theSpaceObjectListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }

   return theSpaceObjectListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size, wxArrayString &namesToExclude)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                wxArrayString &namesToExclude)
{
   wxString emptyList[] = {};

   if (namesToExclude.IsEmpty())
   {
      //MessageInterface::ShowMessage("GuiItemManager::GetSpacecraftListBox() namesToExclude=0\n");
      if (theNumSpacecraft > 0)
      {       
         theSpacecraftListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumSpacecraft,
                          theSpacecraftList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theSpacecraftListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   else
   {
      int exclCount = namesToExclude.GetCount();
      int newScCount = theNumSpacecraft - exclCount;
      //MessageInterface::ShowMessage("GuiItemManager::GetSpacecraftListBox() newScCount = %d\n",
      //                              newScCount);

      if (newScCount > 0)
      {
         wxString *newScList = new wxString[newScCount];
         bool excludeName;
         int numScs = 0;
        
         for (int i=0; i<theNumSpacecraft; i++)
         {
            excludeName = false;
            for (int j=0; j<exclCount; j++)
            {
               if (theSpacecraftList[i] == namesToExclude[j])
               {
                  excludeName = true;
                  break;
               }
            }
            
            if (!excludeName)
               newScList[numScs++] = theSpacecraftList[i];
         }

         theSpacecraftListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, newScCount,
                          newScList, wxLB_SINGLE|wxLB_SORT);
         delete newScList;
      }
      else
      {
         //MessageInterface::ShowMessage("GuiItemManager::GetSpacecraftListBox() emptyList\n");
         theSpacecraftListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }

   return theSpacecraftListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetPropertyListBox(wxWindow *parent, wxWindowID id, const wxSize &size,
//                                const wxString &objName)
//------------------------------------------------------------------------------
/**
 * @return Available Parameter ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetPropertyListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              const wxString &objName)
{
   if (objName != "Spacecraft")
      throw GmatBaseException("There are no properties associated with " +
                              std::string(objName.c_str()));
   
   wxString emptyList[] = {};
   int numObj = 0;
   
   numObj = theNumSpacecraft;
   
   if (numObj > 0)
   {       
      theScPropertyListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumScProperty,
                       theScPropertyList, wxLB_SINGLE|wxLB_SORT);
   }
   else
   {       
      theScPropertyListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, 0,
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   return theScPropertyListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetPlottableParameterListBox(wxWindow *parent, wxWindowID id,
//                                         const wxSize &size,
//                                         const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured PlottableParameterListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetPlottableParameterListBox(wxWindow *parent,
                                                        wxWindowID id,
                                                        const wxSize &size,
                                                        const wxString &nameToExclude)
{       
   wxString emptyList[] = {};
   wxString *newPlottableParamList;
   int numParams = 0;
    
   if (nameToExclude != "" && theNumPlottableParam >= 2)
   {
      newPlottableParamList = new wxString[theNumPlottableParam-1];
        
      for (int i=0; i<theNumPlottableParam; i++)
      {
         if (thePlottableParamList[i] != nameToExclude)
            newPlottableParamList[numParams++] = thePlottableParamList[i];
      }
      
      thePlottableParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumPlottableParam,
                       newPlottableParamList, wxLB_SINGLE|wxLB_SORT);
      
      delete newPlottableParamList;
   }
   else
   {
      if (theNumPlottableParam > 0)
      {       
         thePlottableParamListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumPlottableParam,
                          thePlottableParamList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         thePlottableParamListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   
   return thePlottableParamListBox;
}

//------------------------------------------------------------------------------
//  wxComboBox* GetUserVariableComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return configured user parameter combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox*
GuiItemManager::GetUserVariableComboBox(wxWindow *parent, wxWindowID id,
                                        const wxSize &size)
{
   // combo box for configured user parameters

   int numUserVar = theNumUserVariable;
    
   if (theNumUserVariable == 0)
      numUserVar = 1;
    
   theUserParamComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     numUserVar, theUserVarList, wxCB_READONLY);
   
   // show first spacecraft
   theUserParamComboBox->SetSelection(0);
   
   return theUserParamComboBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size,
//                                   const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured User Valiable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size,
                                                  const wxString &nameToExclude)
{       
   wxString emptyList[] = {};
   wxString *newUserVarList;
   int numParams = 0;
    
   if (nameToExclude != "" && theNumUserVariable >= 2)
   {
      newUserVarList = new wxString[theNumUserVariable-1];
        
      for (int i=0; i<theNumUserVariable; i++)
      {
         if (theUserVarList[i] != nameToExclude)
            newUserVarList[numParams++] = theUserVarList[i];
      }
      
      theUserVarListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumUserVariable,
                       newUserVarList, wxLB_SINGLE|wxLB_SORT);
      
      delete newUserVarList;
   }
   else
   {
      if (theNumUserVariable > 0)
      {       
         theUserVarListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumUserVariable,
                          theUserVarList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theUserVarListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   
   return theUserVarListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetUserArrayListBox(wxWindow *parent, wxWindowID id,
//                                const wxSize &size,
//                                const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured User Array ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserArrayListBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size,
                                               const wxString &nameToExclude)
{       
   wxString emptyList[] = {};
   wxString *newUserArrayList;
   int numParams = 0;
    
   if (nameToExclude != "" && theNumUserArray >= 2)
   {
      newUserArrayList = new wxString[theNumUserArray-1];
        
      for (int i=0; i<theNumUserArray; i++)
      {
         if (theUserVarList[i] != nameToExclude)
            newUserArrayList[numParams++] = theUserArrayList[i];
      }
      
      theUserArrayListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumUserArray,
                       newUserArrayList, wxLB_SINGLE|wxLB_SORT);
      
      delete newUserArrayList;
   }
   else
   {
      if (theNumUserArray > 0)
      {       
         theUserArrayListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumUserArray,
                          theUserArrayList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theUserArrayListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   
   return theUserArrayListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetUserParameterListBox(wxWindow *parent, wxWindowID id,
//                                    const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Configured User Array ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserParameterListBox(wxWindow *parent, wxWindowID id,
                                                   const wxSize &size)
{
   wxString emptyList[] = {};
   
   if (theNumUserParam > 0)
   {       
      theUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumUserParam,
                       theUserParamList, wxLB_SINGLE|wxLB_SORT);
   }
   else
   {       
      theUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, 0,
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   return theUserParamListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetConfigBodyListBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size,
//                                 wxArrayString &bodiesToExclude)
//------------------------------------------------------------------------------
/**
 * @return Configured ConfigBodyListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetConfigBodyListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                wxArrayString &bodiesToExclude)
{
   //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() entered\n");
    
   wxString emptyList[] = {};

   if (bodiesToExclude.IsEmpty())
   {
      //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() bodiesToExclude=0\n");
      if (theNumConfigBody > 0)
      {       
         theConfigBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumConfigBody,
                          theConfigBodyList, wxLB_SINGLE);
      }
      else
      {       
         theConfigBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
      }
   }
   else
   {
      int exclCount = bodiesToExclude.GetCount();
      int newBodyCount = theNumConfigBody - exclCount;
      //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() newBodyCount = %d\n",
      //                              newBodyCount);

      if (newBodyCount > 0)
      {
         wxString *newBodyList = new wxString[newBodyCount];
         bool excludeBody;
         int numBodies = 0;
        
         for (int i=0; i<theNumConfigBody; i++)
         {
            excludeBody = false;
            for (int j=0; j<exclCount; j++)
            {
               if (theConfigBodyList[i] == bodiesToExclude[j])
               {
                  excludeBody = true;
                  break;
               }
            }
            
            if (!excludeBody)
               newBodyList[numBodies++] = theConfigBodyList[i];
         }

         theConfigBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, newBodyCount,
                          newBodyList, wxLB_SINGLE);
         delete newBodyList;
      }
      else
      {
         //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() emptyList\n");
         theConfigBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
      }
   }

   return theConfigBodyListBox;
}

//------------------------------------------------------------------------------
// wxBoxSizer* CreateParameterSizer(...)
//------------------------------------------------------------------------------
/**
 * Creates parameter sizer.
 */
//------------------------------------------------------------------------------
wxBoxSizer* GuiItemManager::
CreateParameterSizer(wxWindow *parent,
                     wxButton **createVarButton, wxWindowID createVarButtonId,
                     wxComboBox **objectComboBox, wxWindowID objectComboBoxId,
                     wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                     wxListBox **propertyListBox, wxWindowID propertyListBoxId,
                     wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
                     wxStaticText **coordSysLabel)
{
#if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::CreateParameterSizer() entered\n");
#endif
   
   Integer borderSize = 1;
   
   //wxStaticBox
   wxStaticBox *userParamStaticBox = new wxStaticBox(parent, -1, wxT(""));
   wxStaticBox *systemParamStaticBox = new wxStaticBox(parent, -1, wxT(""));
   
   //wxStaticText
   wxStaticText *userVarStaticText =
      new wxStaticText(parent, -1, wxT("Variables"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *objectStaticText =
      new wxStaticText(parent, -1, wxT("Object"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *propertyStaticText =
      new wxStaticText(parent, -1, wxT("Property"),
                       wxDefaultPosition, wxDefaultSize, 0);   
     
   *coordSysLabel =
      new wxStaticText(parent, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxDefaultSize, 0);   
   
   // wxButton
   *createVarButton =
      new wxButton(parent, createVarButtonId, wxT("Create"),
                   wxDefaultPosition, wxSize(-1,-1), 0 );
   
   // wxComboBox
   *objectComboBox =
      GetSpacecraftComboBox(parent, objectComboBoxId, wxSize(150, 20));
   *coordSysComboBox =
      GetCoordSysComboBox(parent, coordSysComboBoxId, wxSize(150, 20));
   
   // wxListBox
   wxArrayString emptyArray;
   *userParamListBox =
      GetUserVariableListBox(parent, userParamListBoxId, wxSize(150, 50), "");
   
   *propertyListBox = 
      GetPropertyListBox(parent, propertyListBoxId, wxSize(150, 100), "Spacecraft");
   
   // wx*Sizer
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   wxStaticBoxSizer *systemParamBoxSizer =
      new wxStaticBoxSizer(systemParamStaticBox, wxVERTICAL);
   wxBoxSizer *paramBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   userParamBoxSizer->Add
      (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (*userParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (*createVarButton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   
   systemParamBoxSizer->Add
      (objectStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (*objectComboBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (propertyStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (*propertyListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (*coordSysLabel, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   systemParamBoxSizer->Add
      (*coordSysComboBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   
   paramBoxSizer->Add(userParamBoxSizer, 0,
                      wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   paramBoxSizer->Add(systemParamBoxSizer, 0,
                      wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);

   return paramBoxSizer;
}

//-------------------------------
// priavate methods
//-------------------------------

//------------------------------------------------------------------------------
//  void UpdateSpaceObjectList()
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpaceObjectList()
{
   StringArray scList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
   StringArray fmList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::FORMATION);

   int numSc = scList.size();
   int numFm = fmList.size();
   int numObj = 0;
   int soCount = 0;
   
#if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpaceObjectList() entered==========>\n");
   MessageInterface::ShowMessage("numSc=%d, scList=", numSc);
   for (int i=0; i<numSc; i++)
      MessageInterface::ShowMessage("%s ", scList[i].c_str());
   MessageInterface::ShowMessage("\nnumFm=%d, fmList=", numFm);
   for (int i=0; i<numFm; i++)
      MessageInterface::ShowMessage("%s ", fmList[i].c_str());
   MessageInterface::ShowMessage("\n");   
#endif

   //--------------------------------------
   // if any space objects are configured
   //--------------------------------------
   if ((numSc + numFm) > 0)
   {
      // if formation exists
      if (numFm > 0)
      {
         StringArray fmscListAll;
         
         //------------------------------------------
         // Merge spacecrafts in Formation
         //------------------------------------------
         for (int i=0; i<numFm; i++)
         {
            Formation *fm = (Formation*)(theGuiInterpreter->GetSpacecraft(fmList[i]));
            StringArray fmscList = fm->GetStringArrayParameter(fm->GetParameterID("Add"));
            fmscListAll.insert(fmscListAll.begin(), fmscList.begin(), fmscList.end());
         }
         
         sort(scList.begin(), scList.end());
         sort(fmscListAll.begin(), fmscListAll.end());
         
         //------------------------------------------
         // Make list of spacecrafts not in Formation
         //------------------------------------------
         StringArray result;
         set_difference(scList.begin(), scList.end(), fmscListAll.begin(),
                        fmscListAll.end(), back_inserter(result));
         
         numObj = result.size();
         
         //------------------------------------------
         // Add new list to theSpaceObjectList
         //------------------------------------------
         if (numObj > 0)  // check to see if any objects exist
         {
            for (int i=0; i<numObj; i++)
            {
               if (soCount < MAX_SPACECRAFT_SIZE)
               {
                  theSpaceObjectList[soCount] = wxString(result[i].c_str());
                  soCount++;
#if DEBUG_GUI_ITEM
                  MessageInterface::ShowMessage
                     ("theSpaceObjectList[%d]=%s\n", soCount-1,
                      theSpaceObjectList[soCount-1].c_str());
#endif
               }
            }
         }
         
         //------------------------------------------
         // Add formation to theSpaceObjectList
         //------------------------------------------
         for (int i=0; i<numFm; i++)
         {
            if (soCount < MAX_SPACECRAFT_SIZE)
            {
               theSpaceObjectList[soCount] = wxString(fmList[i].c_str());
               soCount++;
#if DEBUG_GUI_ITEM
               MessageInterface::ShowMessage
                  ("theSpaceObjectList[%d]=%s\n", soCount-1,
                   theSpaceObjectList[soCount-1].c_str());
#endif
            }
         }
      }
      // no formation, Save scList to theSpaceObjectList
      else
      {
         soCount = numSc;
         for (int i=0; i<soCount; i++)
         {
            theSpaceObjectList[i] = wxString(scList[i].c_str());
#if DEBUG_GUI_ITEM
            MessageInterface::ShowMessage
               ("theSpaceObjectList[%d]=%s\n", i, theSpaceObjectList[i].c_str());
#endif
         }
      }
   }
   //--------------------------------------
   // else no space objects are configured
   //--------------------------------------
   else
   {
      theSpaceObjectList[0] = wxString("-- None --");
   }
   
   theNumSpaceObject = soCount;
   
#if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage
      ("theNumSpaceObject=%d\n"
       "<==========GuiItemManager::UpdateSpaceObjectList() exiting\n",
       theNumSpaceObject);
#endif
}

//------------------------------------------------------------------------------
//  void UpdateFormationList()
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormationList()
{
   StringArray listForm = theGuiInterpreter->GetListOfConfiguredItems(Gmat::FORMATION);
   int numForm = listForm.size();

   if (numForm > MAX_FORMATION_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateFormationList() GUI can handle up to %d formations."
          "The number of formation configured: %d\n", MAX_FORMATION_SIZE, numForm);
      numForm = MAX_FORMATION_SIZE;
   }
   
   if (numForm > 0)  // check to see if any spacecrafts exist
   {
      for (int i=0; i<numForm; i++)
      {
         theFormationList[i] = wxString(listForm[i].c_str());
#if DEBUG_GUI_ITEM
         MessageInterface::ShowMessage
            ("GuiItemManager::UpdateFormationList() theFormationtList[%d]=%s\n",
             i, theFormationList[i].c_str());
#endif
      }
   }
   else
   {
      theFormationList[0] = wxString("-- None --");
   }
   
   theNumFormation = numForm;
}

//------------------------------------------------------------------------------
//  void UpdateSpacecraftList()
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraftList()
{
   StringArray scList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
   int numSc = scList.size();

   if (numSc > MAX_SPACECRAFT_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacecraftList() GUI can handle up to %d spacecraft."
          "The number of spacecraft configured: %d\n", MAX_SPACECRAFT_SIZE, numSc);
      numSc = MAX_SPACECRAFT_SIZE;
   }
   
   if (numSc > 0)  // check to see if any spacecrafts exist
   {
      for (int i=0; i<numSc; i++)
      {
         theSpacecraftList[i] = wxString(scList[i].c_str());
#if DEBUG_GUI_ITEM
         MessageInterface::ShowMessage
            ("GuiItemManager::UpdateSpacecraftList() theSpacecraftList[%d]=%s\n",
             i, theSpacecraftList[i].c_str());
#endif
      }
   }
   else
   {
      theSpacecraftList[0] = wxString("-- None --");
   }
   
   theNumSpacecraft = numSc;
}

//------------------------------------------------------------------------------
//  void UpdatePropertyList(const wxString &objName)
//------------------------------------------------------------------------------
/**
 * Updates available parameter list associated with objName. Currently it
 * assumes all parameters are associated with Spacecraft.
 *
 * @note Only plottable parameters (returning single value) are added to the list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePropertyList(const wxString &objName)
{
   //-----------------------------------------------------------------
   //Note: How do I know which parameter belong to which object type?
   //      Currently all paramters are associated with spacecraft.
   //-----------------------------------------------------------------
   
#if DEBUG_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage("GuiItemManager::UpdatePropertyList() " +
                                 std::string(objName.c_str()) + "\n");
#endif
   
   if (objName != "Spacecraft")
      throw GmatBaseException("There are no properties associated with " +
                              std::string(objName.c_str()));
   
   StringArray items =
      theGuiInterpreter->GetListOfFactoryItems(Gmat::PARAMETER);
   int numParams = items.size();
   
#if DEBUG_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropertyList() numParams=%d\n", numParams);
#endif

   theNumScProperty = 0;
   
   for (int i=0; i<numParams; i++)
   {
      if (theNumScProperty < MAX_PROPERTY_SIZE)
      {
         // add to list only system parameters returning single value (loj: 9/22/04)
         if (items[i].find("CartState") == std::string::npos &&
             items[i].find("KepElem") == std::string::npos &&
             items[i].find("SphElem") == std::string::npos &&
             items[i].find("Variable") == std::string::npos &&
             items[i].find("Array") == std::string::npos)
         {
            theScPropertyList[theNumScProperty] = items[i].c_str();
            theNumScProperty++;
         }
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "The number of spacecraft properties exceeds "
             "the maximum: %d", MAX_PROPERTY_SIZE);
      }
   }
   
#if DEBUG_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropertyList() theNumScProperty=%d\n", theNumScProperty);
#endif
}

//------------------------------------------------------------------------------
// void UpdateParameterList()
//------------------------------------------------------------------------------
/**
 * Updates confugured parameter list (thePlottableParamList, theSystemParamList,
 * theUserVarList, theUserArrayList).
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateParameterList()
{    
#if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateParameterList() entered.\n");
#endif
      
   StringArray items =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::PARAMETER);
   int numParamCount = items.size();
   
   Parameter *param;

   int plottableParamCount = 0;
   int userVarCount = 0;
   int userArrayCount = 0;
   int systemParamCount = 0;
   int userParamCount = 0;
   
   for (int i=0; i<numParamCount; i++)
   {
#if DEBUG_GUI_ITEM
      //11/16/04 loj: added
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateParameterList() name=%s\n", items[i].c_str());
#endif
      
      param = theGuiInterpreter->GetParameter(items[i]);

      // add if parameter plottable (returning single value)
      if (param->IsPlottable())
      {
         if (plottableParamCount < MAX_PLOT_PARAM_SIZE)
         {
            thePlottableParamList[plottableParamCount] = items[i].c_str();
            plottableParamCount++;
         }
         
         // system Parameter (object property)
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            if (systemParamCount < MAX_PROPERTY_SIZE)
            {
               theSystemParamList[systemParamCount] = items[i].c_str();
               systemParamCount++;
            }
         }
         else
         {
            // user Variable
            if (param->GetTypeName() == "Variable")
            {
               if (userArrayCount < MAX_USER_ARRAY_SIZE &&
                   userParamCount < MAX_USER_PARAM_SIZE)
               {
                  theUserVarList[userVarCount] = items[i].c_str();
                  userVarCount++;
               
                  theUserParamList[userParamCount] = items[i].c_str();
                  userParamCount++;
               }
               else
               {
                  MessageInterface::ShowMessage
                     ("GuiItemManager::UpdateParameterList() % is ignored. GUI can "
                      "handle up to %d user parameters.\n", MAX_USER_VAR_SIZE);
               }
            }
         }
      }
      else
      {
         // user Array
         if (param->GetTypeName() == "Array")
         {
            if (userArrayCount < MAX_USER_ARRAY_SIZE &&
                userParamCount < MAX_USER_PARAM_SIZE)
            {
               theUserArrayList[userArrayCount] = items[i].c_str();
               userArrayCount++;
               
               theUserParamList[userParamCount] = items[i].c_str();
               userParamCount++;
            }
            else
            {
               MessageInterface::ShowMessage
                  ("GuiItemManager::UpdateParameterList() % is ignored. GUI can "
                   "handle up to %d user parameters.\n", MAX_USER_ARRAY_SIZE);
            }
         }
      }
      //MessageInterface::ShowMessage("GuiItemManager::UpdateParameterList() " +
      //                             std::string(thePlottableParamList[i].c_str()) + "\n");
   }

   theNumPlottableParam = plottableParamCount;
   theNumSystemParam = systemParamCount;
   theNumUserVariable = userVarCount;
   theNumUserArray = userArrayCount;
   theNumUserParam = userParamCount;
}

//------------------------------------------------------------------------------
// void UpdateConfigBodyList()
//------------------------------------------------------------------------------
/**
 * Updates confugured celestial body list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateConfigBodyList()
{
   //MessageInterface::ShowMessage("GuiItemManager::UpdateConfigBodyList() entered\n");
        
   StringArray items = theSolarSystem->GetBodiesInUse();
   theNumConfigBody = items.size();
   
   if (theNumConfigBody > MAX_OBJECT_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateConfigBodyList() GUI will handle up to %d bodies."
          "The number of bodies configured: %d\n", MAX_OBJECT_SIZE, theNumConfigBody);
      theNumConfigBody = MAX_OBJECT_SIZE;
   }
   
   for (int i=0; i<theNumConfigBody; i++)
   {
      theConfigBodyList[i] = items[i].c_str();
        
      //MessageInterface::ShowMessage("GuiItemManager::UpdateConfigBodyList() " +
      //                              std::string(theConfigBodyList[i].c_str()) + "\n");
   }
}


//------------------------------------------------------------------------------
// void UpdateCoordSystemList()
//------------------------------------------------------------------------------
/**
 * Updates confugured celestial body list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCoordSystemList()
{
   //MessageInterface::ShowMessage("GuiItemManager::UpdateCoordSystemList() entered\n");
        
   StringArray items =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::COORDINATE_SYSTEM);
   theNumCoordSys = items.size();
   
   if (theNumCoordSys > MAX_OBJECT_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCoordSystemList() GUI will handle up to %d coord. sys."
          "The number of coord. sys. configured: %d\n", MAX_OBJECT_SIZE, theNumCoordSys);
      theNumCoordSys = MAX_OBJECT_SIZE;
   }

   for (int i=0; i<theNumCoordSys; i++)
   {
      theCoordSysList[i] = items[i].c_str();
        
      //MessageInterface::ShowMessage("GuiItemManager::UpdateCoordSystemList() " +
      //                              std::string(theCoordSysList[i].c_str()) + "\n");
   }
}

//------------------------------------------------------------------------------
// GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::GuiItemManager()
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
   UpdatePropertyList("Spacecraft");
   
   theNumSpaceObject = 0;
   theNumFormation = 0;
   theNumSpacecraft = 0;
   theNumScProperty = 0;
   theNumPlottableParam = 0;
   theNumConfigBody = 0;
   theNumCoordSys = 0;
   
   theSpacecraftComboBox = NULL;
   theUserParamComboBox = NULL;
   theCoordSysComboBox = NULL;
   
   theSpacecraftListBox = NULL;
   theSpaceObjectListBox = NULL;
   theFormationListBox = NULL;
   theScPropertyListBox = NULL;
   thePlottableParamListBox = NULL;
   theSystemParamListBox = NULL;
   theUserVarListBox = NULL;
   theUserArrayListBox = NULL;
   theUserParamListBox = NULL;
   theConfigBodyListBox = NULL;
}

//------------------------------------------------------------------------------
// ~GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::~GuiItemManager()
{
}
