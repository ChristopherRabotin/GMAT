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
   UpdateObject();
   UpdateSpacecraft();
   UpdateParameter();
   UpdateSolarSystem();
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
   UpdateObjectList();
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
// wxListBox* GetSpacecraftListBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpacecraftListBox(wxWindow *parent, const wxSize &size)
{

   // Spacecraft
   //loj: 2/23/04 add other objects in the future build
    
   int numObj = theNumSpacecraft;
    
   if (theNumSpacecraft == 0)
      numObj = 1;
    
   wxString emptyList[] = {};
        
   if (numObj > 0)
   {       
      theSpacecraftListBox =
         new wxListBox(parent, -1, wxDefaultPosition, size, numObj,
                       theSpacecraftList, wxLB_SINGLE);
   }
   else
   {       
      theSpacecraftListBox =
         new wxListBox(parent, -1, wxDefaultPosition, size, 0,
                       emptyList, wxLB_SINGLE);
   }
   
   return theSpacecraftListBox;
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

      delete newConfigParamList;
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

//------------------------------------------------------------------------------
// wxListBox* GetConfigBodyListBox(wxWindow *parent, const wxSize &size,
//                                 wxArrayString &bodiesToExclude)
//------------------------------------------------------------------------------
/**
 * @return Configured ConfigBodyListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetConfigBodyListBox(wxWindow *parent, const wxSize &size,
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
            new wxListBox(parent, -1, wxDefaultPosition, size, theNumConfigBody,
                          theConfigBodyList, wxLB_SINGLE);
      }
      else
      {       
         theConfigBodyListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, 0,
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
            new wxListBox(parent, -1, wxDefaultPosition, size, newBodyCount,
                          newBodyList, wxLB_SINGLE);
         delete newBodyList;
      }
      else
      {
         //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() emptyList\n");
         theConfigBodyListBox =
            new wxListBox(parent, -1, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
      }
   }

   return theConfigBodyListBox;
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
    
   if (numObj > 0)  // check to see if any objects exist
   {
      for (int i=0; i<numObj; i++)
      {
         theObjectList[i] = wxString(listSc[i].c_str());
         //MessageInterface::ShowMessage("GuiItemManager::UpdateObjectList() " +
         //                              std::string(theObjectList[i].c_str()) + "\n");
      }
   }
   else
   {
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

   if (numSc > 0)  // check to see if any spacecrafts exist
   {
      for (int i=0; i<numSc; i++)
      {
         theSpacecraftList[i] = wxString(listSc[i].c_str());
         //MessageInterface::ShowMessage("GuiItemManager::UpdateSpacecraftList() " +
         //                              std::string(theSpacecraftList[i].c_str()) + "\n");
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

      //loj: 3/12/04 Parameters does not have "Param" suffix anymore
      //          if (theParamList[i].Contains("Param"))
      //          {
      //              //if "Param" is suffix
      //              if (theParamList[i].First("Param") == len-5)
      //              {
      //                  // remove "Param" from the parameter name
      //                  // Note: When a new Parameter is created "Param" should be appended
      //                  //       That's the constructor name
      //                  theParamList[i].Remove(len-5, 5);
                
      //                  //MessageInterface::ShowMessage("GuiItemManager::UpdateParameterList() " +
      //                  //                              std::string(theParamList[i].c_str()) + "\n");
      //              }
      //          }
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

   theNumObject = 0;
   theNumSpacecraft = 0;
   theNumParam = 0;
   theNumConfigParam = 0;
   theNumConfigBody = 0;

   theSpacecraftComboBox = NULL;
   theSpacecraftListBox = NULL;
   theObjectListBox = NULL;
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
