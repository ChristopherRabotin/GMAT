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
#include "MessageInterface.hpp"
#include <algorithm>              // for sort(), set_difference()

#define DEBUG_GUI_ITEM 0

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
void GuiItemManager::UpdateParameter(const wxString &objName)
{
   UpdateParameterList(objName);
   UpdateConfigParameterList(objName);
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
// wxListBox* GetParameterListBox(wxWindow *parent, wxWindowID id, const wxSize &size,
//                                const wxString &objName, int numObj)
//------------------------------------------------------------------------------
/**
 * @return Available Parameter ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetParameterListBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size,
                                               const wxString &objName, int numObj)
{
   wxString emptyList[] = {};
        
   if (numObj > 0)
   {       
      theParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumParam,
                       theParamList, wxLB_SINGLE|wxLB_SORT);
   }
   else
   {       
      theParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, 0,
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   return theParamListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetConfigParameterListBox(wxWindow *parent, wxWindowID id,
//                                      const wxSize &size,
//                                      const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured ConfigParameterListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetConfigParameterListBox(wxWindow *parent,
                                                     wxWindowID id,
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
         new wxListBox(parent, id, wxDefaultPosition, size, theNumConfigParam,
                       newConfigParamList, wxLB_SINGLE|wxLB_SORT);
      
      delete newConfigParamList;
   }
   else
   {
      if (theNumConfigParam > 0)
      {       
         theConfigParamListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumConfigParam,
                          theConfigParamList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theConfigParamListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   
   return theConfigParamListBox;
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

//-------------------------------
// priavate methods
//-------------------------------

//------------------------------------------------------------------------------
//  void UpdateSpaceObjectList(bool firstTime)
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpaceObjectList(bool firstTime)
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
               theSpaceObjectList[soCount] = wxString(result[i].c_str());
               soCount++;
#if DEBUG_GUI_ITEM
               MessageInterface::ShowMessage
                  ("theSpaceObjectList[%d]=%s\n", soCount-1,
                   theSpaceObjectList[soCount-1].c_str());
#endif
            }
         }
         
         //------------------------------------------
         // Add formation to theSpaceObjectList
         //------------------------------------------
         for (int i=0; i<numFm; i++)
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
//  void UpdateFormationList(bool firstTime)
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormationList(bool firstTime)
{
   StringArray listForm = theGuiInterpreter->GetListOfConfiguredItems(Gmat::FORMATION);
   int numForm = listForm.size();

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
//  void UpdateSpacecraftList(bool firstTime)
//------------------------------------------------------------------------------
/**
 * updates spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraftList(bool firstTime)
{
   StringArray scList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
   int numSc = scList.size();

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
    
    //loj: How do I know which parameter belong to which object type?
    //loj: for B2 show all paramters
   StringArray items =
      theGuiInterpreter->GetListOfFactoryItems(Gmat::PARAMETER);
   theNumParam = items.size();

   int len;
   for (int i=0; i<theNumParam; i++)
   {
      theParamList[i] = items[i].c_str();
      len = theParamList[i].Length();
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
    
   StringArray items =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::PARAMETER);
   theNumConfigParam = items.size();

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

//------------------------------------------------------------------------------
//  void UpdateConfigBodyList(bool firstTime = false)
//------------------------------------------------------------------------------
/**
 * Updates confugured celestial body list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateConfigBodyList(bool firstTime)
{
   //MessageInterface::ShowMessage("GuiItemManager::UpdateConfigBodyList() entered\n");
        
   StringArray items = theSolarSystem->GetBodiesInUse();
   theNumConfigBody = items.size();

   for (int i=0; i<theNumConfigBody; i++)
   {
      theConfigBodyList[i] = items[i].c_str();
        
      //MessageInterface::ShowMessage("GuiItemManager::UpdateConfigBodyList() " +
      //                              std::string(theConfigBodyList[i].c_str()) + "\n");
   }
}

//------------------------------------------------------------------------------
// GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::GuiItemManager()
{
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();

   theNumSpaceObject = 0;
   theNumFormation = 0;
   theNumSpacecraft = 0;
   theNumParam = 0;
   theNumConfigParam = 0;
   theNumConfigBody = 0;

   theSpacecraftComboBox = NULL;
   theSpacecraftListBox = NULL;
   theSpaceObjectListBox = NULL;
   theParamListBox = NULL;
   theConfigParamListBox = NULL;
   theConfigBodyListBox = NULL;
}

//------------------------------------------------------------------------------
// ~GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::~GuiItemManager()
{
}
