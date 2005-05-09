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
//#define DEBUG_GUI_ITEM_SP 1
//#define DEBUG_GUI_ITEM_CS 1

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

   UpdateCelestialPoint(); // All CelestialBodis and CalculatedPoints
   UpdateFormation();
   UpdateSpacecraft();
   UpdateParameter();
   UpdateSolarSystem();
   UpdateCoordSystem();
}


//------------------------------------------------------------------------------
//  void UpdateCelestialPoint()
//------------------------------------------------------------------------------
/**
 * Updates CelestialBody and CalculatedPoint objects.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCelestialPoint()
{
   UpdateCelestialPointList();
   UpdateSpacePointList();
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
   UpdateSpacePointList();
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
   UpdateSpacePointList();
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
   UpdateCelestialBodyList();
   UpdateCelestialPointList();
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
 * @return CelestialBody ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetConfigBodyComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   int numBody = theNumCelesBody;
   
   if (theNumCelesBody == 0)
      numBody = 1;
   
   theCelesBodyComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     numBody, theCelesBodyList, wxCB_READONLY);
   
   // show Earth as a default body
   theCelesBodyComboBox->SetStringSelection("Earth");
   
   return theCelesBodyComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSpacePointComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size, bool addVector = false)
//------------------------------------------------------------------------------
/**
 * @return configured SpacePoint object ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox*
GuiItemManager::GetSpacePointComboBox(wxWindow *parent, wxWindowID id,
                                      const wxSize &size, bool addVector)
{
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacePointComboBox() theNumSpacePoint=%d\n",
      theNumSpacePoint);
   #endif
   
   int numSpacePoint = theNumSpacePoint;
   
   if (theNumSpacePoint == 0)
      numSpacePoint = 1;

   if (addVector)
   {
      if (theNumSpacePoint == 0)
         numSpacePoint = 1;
      else
         numSpacePoint = theNumSpacePoint + 1;

      wxString *newSpacePointList = new wxString[numSpacePoint];

      // add "Vector" first
      newSpacePointList[0] = "Vector";
      
      for (int i=0; i<theNumSpacePoint; i++)
         newSpacePointList[i+1] = theSpacePointList[i];

      theSpacePointComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                        numSpacePoint, newSpacePointList, wxCB_READONLY);

      delete newSpacePointList;
   }
   else
   {
      theSpacePointComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                        numSpacePoint, theSpacePointList, wxCB_READONLY);
   }
   
   // select first item
   theSpacePointComboBox->SetSelection(0);
   
   return theSpacePointComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetCelestialPointComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size, bool addVector = false)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox*
GuiItemManager::GetCelestialPointComboBox(wxWindow *parent, wxWindowID id,
                                          const wxSize &size, bool addVector)
{
   #if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointComboBox() theNumCelesPoint=%d\n",
      theNumCelesPoint);
   #endif
   
   int numCelesPoint = theNumCelesPoint;
   
   if (theNumCelesPoint == 0)
      numCelesPoint = 1;

   if (addVector)
   {
      if (theNumCelesPoint == 0)
         numCelesPoint = 1;
      else
         numCelesPoint = theNumCelesPoint + 1;

      wxString *newCelestialPointList = new wxString[numCelesPoint];

      // add "Vector" first
      newCelestialPointList[0] = "Vector";
      
      for (int i=0; i<theNumCelesPoint; i++)
         newCelestialPointList[i+1] = theCelesPointList[i];

      theCelestialPointComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                        numCelesPoint, newCelestialPointList, wxCB_READONLY);

      delete newCelestialPointList;
   }
   else
   {
      theCelestialPointComboBox =
         new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                        numCelesPoint, theCelesPointList, wxCB_READONLY);
   }
   
   // select first item
   theCelestialPointComboBox->SetSelection(0);
   
   return theCelestialPointComboBox;
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
                     numUserVar, theUserVariableList, wxCB_READONLY);
   
   // show first spacecraft
   theUserParamComboBox->SetSelection(0);
   
   return theUserParamComboBox;
}


// ListBox
//------------------------------------------------------------------------------
// wxListBox* GetSpacePointListBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size, bool addVector = false)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpacePointListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size, bool addVector)
{
   #if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::GetSpacePointListBox() entered\n");
   #endif
   
   int numSpacePoint = theNumSpacePoint;
   
   if (theNumSpacePoint == 0)
      numSpacePoint = 1;

   if (addVector)
   {
      if (theNumSpacePoint == 0)
         numSpacePoint = 1;
      else
         numSpacePoint = theNumSpacePoint + 1;

      wxString *newSpacePointList = new wxString[numSpacePoint];

      // add "Vector" first
      newSpacePointList[0] = "Vector";
      
      for (int i=0; i<theNumSpacePoint; i++)
         newSpacePointList[i+1] = theSpacePointList[i];

      theSpacePointListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, numSpacePoint,
                       newSpacePointList, wxLB_SINGLE|wxLB_SORT);

      delete newSpacePointList;
   }
   else
   {
      theSpacePointListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, numSpacePoint,
                       theSpacePointList, wxCB_READONLY);
   }
   
   // select first item
   theSpacePointListBox->SetSelection(0);
   
   return theSpacePointListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
//                                     const wxSize &size, wxArrayString &namesToExclude)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ListBox pointer
 * excluding names in the namesToExclude array.
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
                                                    const wxSize &size,
                                                    wxArrayString &namesToExclude)
{
   #if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointListBox() theNumCelesPoint=%d\n",
       theNumCelesPoint);
   #endif
   
   wxString emptyList[] = {};

   if (namesToExclude.IsEmpty())
   {
      if (theNumCelesPoint > 0)
      {       
         theCelesPointListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumCelesPoint,
                          theCelesPointList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theCelesPointListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   else
   {
      int exclCount = namesToExclude.GetCount();
      int newCelestialPointCount = theNumCelesPoint - exclCount;
      
      if (newCelestialPointCount > 0)
      {
         wxString *newCelestialPointList = new wxString[newCelestialPointCount];
         bool excludeName;
         int numSpaceObj = 0;
        
         for (int i=0; i<theNumCelesPoint; i++)
         {
            excludeName = false;
            for (int j=0; j<exclCount; j++)
            {
               if (theCelesPointList[i].IsSameAs(namesToExclude[j]))
               {
                  excludeName = true;
                  break;
               }
            }
            
            if (!excludeName)
            {
               newCelestialPointList[numSpaceObj++] = theCelesPointList[i];
            }
         }

         theCelesPointListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, newCelestialPointCount,
                          newCelestialPointList, wxLB_SINGLE|wxLB_SORT);
         
         delete newCelestialPointList;
      }
      else
      {
         theCelesPointListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }

   return theCelesPointListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
//                                  const wxSize &size, wxArrayString &namesToExclude)
//------------------------------------------------------------------------------
/**
 * @return configured Spacecraft anf Formation object ListBox pointer
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
   for (unsigned int i=0; i<namesToExclude.GetCount(); i++)
   {
      MessageInterface::ShowMessage("namesToExclude[%d]=<%s>\n",
                                    i, namesToExclude[i].c_str());
   }
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
               if (theSpaceObjectList[i].IsSameAs(namesToExclude[j]))
               {
                  excludeName = true;
                  break;
               }
            }
            
            if (!excludeName)
            {
               #ifdef DEBUG_GUI_ITEM
               MessageInterface::ShowMessage
                  ("GuiItemManager::GetSpaceObjectListBox() so name to include:%s\n",
                   theSpaceObjectList[i].c_str());
               
               #endif
               
               newSpaceObjList[numSpaceObj++] = theSpaceObjectList[i];
            }
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
// wxListBox* GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Configured all user parameter (Varialbe, Array, String) ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size)
{
   wxString emptyList[] = {};
   wxString *allUserParamList;
   int numParams = 0;
   int allUserParamCount = theNumUserVariable + theNumUserArray + theNumUserString;
   
   allUserParamList = new wxString[allUserParamCount];
   
   for (int i=0; i<theNumUserVariable; i++)
      allUserParamList[numParams++] = theUserVariableList[i];
   
   for (int i=0; i<theNumUserString; i++)
      allUserParamList[numParams++] = theUserStringList[i];
   
   for (int i=0; i<theNumUserArray; i++)
      allUserParamList[numParams++] = theUserArrayList[i];
   
   if (allUserParamCount > 0)
   {       
      theAllUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, allUserParamCount,
                       allUserParamList, wxLB_SINGLE|wxLB_SORT);
   }
   else
   {       
      theAllUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, 0,
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   delete allUserParamList;
   
   return theAllUserParamListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size,
//                                   const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured User Variable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size,
                                                  const wxString &nameToExclude)
{
   wxString emptyList[] = {};
   wxString *newUserVariableList;
   int numParams = 0;
    
   if (nameToExclude != "" && theNumUserVariable >= 2)
   {
      newUserVariableList = new wxString[theNumUserVariable-1];
        
      for (int i=0; i<theNumUserVariable; i++)
      {
         if (theUserVariableList[i] != nameToExclude)
            newUserVariableList[numParams++] = theUserVariableList[i];
      }
      
      theUserVariableListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumUserVariable,
                       newUserVariableList, wxLB_SINGLE|wxLB_SORT);
      
      delete newUserVariableList;
   }
   else
   {
      if (theNumUserVariable > 0)
      {       
         theUserVariableListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumUserVariable,
                          theUserVariableList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {       
         theUserVariableListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   
   return theUserVariableListBox;
}

//------------------------------------------------------------------------------
// wxListBox* GetUserStringListBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size,
//                                 const wxString &nameToExclude = "")
//------------------------------------------------------------------------------
/**
 * @return Configured User Valiable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserStringListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                const wxString &nameToExclude)
{       
   wxString emptyList[] = {};
   wxString *newUserStringList;
   int numParams = 0;
    
   if (nameToExclude != "" && theNumUserString >= 2)
   {
      newUserStringList = new wxString[theNumUserString-1];
        
      for (int i=0; i<theNumUserString; i++)
      {
         if (theUserStringList[i] != nameToExclude)
            newUserStringList[numParams++] = theUserStringList[i];
      }
      
      theUserStringListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumUserString,
                       newUserStringList, wxLB_SINGLE|wxLB_SORT);
      
      delete newUserStringList;
   }
   else
   {
      if (theNumUserString > 0)
      {       
         theUserStringListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumUserString,
                          theUserStringList, wxLB_SINGLE|wxLB_SORT);
      }
      else
      {
         theUserStringListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE|wxLB_SORT);
      }
   }
   
   return theUserStringListBox;
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
         if (theUserArrayList[i] != nameToExclude)
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
      if (theNumCelesBody > 0)
      {       
         theCelesBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, theNumCelesBody,
                          theCelesBodyList, wxLB_SINGLE);
      }
      else
      {       
         theCelesBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
      }
   }
   else
   {
      int exclCount = bodiesToExclude.GetCount();
      int newBodyCount = theNumCelesBody - exclCount;
      //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() newBodyCount = %d\n",
      //                              newBodyCount);

      if (newBodyCount > 0)
      {
         wxString *newBodyList = new wxString[newBodyCount];
         bool excludeBody;
         int numBodies = 0;
        
         for (int i=0; i<theNumCelesBody; i++)
         {
            excludeBody = false;
            for (int j=0; j<exclCount; j++)
            {
               if (theCelesBodyList[i] == bodiesToExclude[j])
               {
                  excludeBody = true;
                  break;
               }
            }
            
            if (!excludeBody)
               newBodyList[numBodies++] = theCelesBodyList[i];
         }

         theCelesBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, newBodyCount,
                          newBodyList, wxLB_SINGLE);
         delete newBodyList;
      }
      else
      {
         //MessageInterface::ShowMessage("GuiItemManager::GetConfigBodyListBox() emptyList\n");
         theCelesBodyListBox =
            new wxListBox(parent, id, wxDefaultPosition, size, 0,
                          emptyList, wxLB_SINGLE);
      }
   }

   return theCelesBodyListBox;
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
                     wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                     wxButton **createVarButton, wxWindowID createVarButtonId,
                     wxComboBox **objectComboBox, wxWindowID objectComboBoxId,
                     wxListBox **propertyListBox, wxWindowID propertyListBoxId,
                     wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
                     wxComboBox **originComboBox, wxWindowID originComboBoxId,
                     wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
                     bool showArrayAndString)
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

   //loj: 1/19/05 Changed ListBox width to 170
   // wxComboBox
   *objectComboBox =
      GetSpacecraftComboBox(parent, objectComboBoxId, wxSize(170, 20));
   *coordSysComboBox =
      GetCoordSysComboBox(parent, coordSysComboBoxId, wxSize(170, 20));
   *originComboBox =
      GetConfigBodyComboBox(parent, coordSysComboBoxId, wxSize(170, 20));
   
   // wxListBox
   wxArrayString emptyArray;
   if (showArrayAndString)
   {
      *userParamListBox =
         GetAllUserParameterListBox(parent, userParamListBoxId, wxSize(170, 50));
   }
   else
   {
      *userParamListBox =
         GetUserVariableListBox(parent, userParamListBoxId, wxSize(170, 50), "");
   }
   
   //loj: 1/19/05 Chagned height to 80 from 100
   *propertyListBox = 
      GetPropertyListBox(parent, propertyListBoxId, wxSize(170, 80), "Spacecraft");
   
   // wx*Sizer
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   wxStaticBoxSizer *systemParamBoxSizer =
      new wxStaticBoxSizer(systemParamStaticBox, wxVERTICAL);
   wxBoxSizer *paramBoxSizer = new wxBoxSizer(wxVERTICAL);
   *coordSysBoxSizer = new wxBoxSizer(wxVERTICAL);

   (*coordSysBoxSizer)->Add(*coordSysLabel, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
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
      (*coordSysBoxSizer, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   
   paramBoxSizer->Add(userParamBoxSizer, 0,
                      wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   paramBoxSizer->Add(systemParamBoxSizer, 0,
                      wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);

   return paramBoxSizer;
}

//------------------------------------------------------------------------------
// wxBoxSizer* CreateUserVarSizer(...)
//------------------------------------------------------------------------------
/**
 * Creates parameter sizer.
 */
//------------------------------------------------------------------------------
wxBoxSizer* GuiItemManager::
CreateUserVarSizer(wxWindow *parent,
                   wxListBox **userParamListBox, wxWindowID userParamListBoxId,
                   wxButton **createVarButton, wxWindowID createVarButtonId,
                   bool showArrayAndString)
{
   #if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::CreateUserVarSizer() entered\n");
   #endif
   
   Integer borderSize = 1;
   
   //wxStaticBox
   wxStaticBox *userParamStaticBox = new wxStaticBox(parent, -1, wxT(""));
   
   //wxStaticText
   wxStaticText *userVarStaticText =
      new wxStaticText(parent, -1, wxT("Variables"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   // wxButton
   *createVarButton =
      new wxButton(parent, createVarButtonId, wxT("Create"),
                   wxDefaultPosition, wxSize(-1,-1), 0 );
   
   // wxListBox
   wxArrayString emptyArray;

   if (showArrayAndString)
   {
      *userParamListBox =
         GetAllUserParameterListBox(parent, userParamListBoxId, wxSize(170, 50));
   }
   else
   {
      *userParamListBox =
         GetUserVariableListBox(parent, userParamListBoxId, wxSize(170, 50), "");
   }
   
   // wx*Sizer
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   wxBoxSizer *paramBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   userParamBoxSizer->Add
      (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (*userParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   userParamBoxSizer->Add
      (*createVarButton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, borderSize);
   
   
   paramBoxSizer->Add(userParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, borderSize);
   
   return paramBoxSizer;
}

//-------------------------------
// priavate methods
//-------------------------------

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
         // add to list only system parameters returning single value
         if (items[i].find("CartState") == std::string::npos &&
             items[i].find("KepElem") == std::string::npos &&
             items[i].find("SphElem") == std::string::npos &&
             items[i].find("Variable") == std::string::npos &&
             items[i].find("Array") == std::string::npos &&
             items[i].find("String") == std::string::npos) //loj: 1/19/05 Added
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
 * theUserVariableList, theUserStringList, theUserArrayList).
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
   int userStringCount = 0;
   int userArrayCount = 0;
   int systemParamCount = 0;
   int userParamCount = 0;
   
   for (int i=0; i<numParamCount; i++)
   {
      #if DEBUG_GUI_ITEM
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
                  theUserVariableList[userVarCount] = items[i].c_str();
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
      else // not plottable parameters
      {
         // user String
         if (param->GetTypeName() == "String")
         {
            if (userArrayCount < MAX_USER_STRING_SIZE &&
                userParamCount < MAX_USER_PARAM_SIZE)
            {
               theUserStringList[userStringCount] = items[i].c_str();
               userStringCount++;
               
               theUserParamList[userParamCount] = items[i].c_str();
               userParamCount++;
            }
            else
            {
               MessageInterface::ShowMessage
                  ("GuiItemManager::UpdateParameterList() % is ignored. GUI can "
                   "handle up to %d user parameters.\n", MAX_USER_STRING_SIZE);
            }
         }
         // user Array
         else if (param->GetTypeName() == "Array")
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
   theNumUserString = userStringCount;
   theNumUserArray = userArrayCount;
   theNumUserParam = userParamCount;
}


//------------------------------------------------------------------------------
//  void UpdateSpacecraftList()
//------------------------------------------------------------------------------
/**
 * updates Spacecraft list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacecraftList()
{
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateSpacecraftList() entered\n");
   #endif
   
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
         
         #if DEBUG_GUI_ITEM_SP
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
//  void UpdateFormationList()
//------------------------------------------------------------------------------
/**
 * updates Formation list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormationList()
{
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateFormationList() entered\n");
   #endif
   
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
//  void UpdateSpaceObjectList()
//------------------------------------------------------------------------------
/**
 * updates Spacecraft and Formation list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpaceObjectList()
{
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateSpaceObjectList() entered\n");
   #endif
   
   StringArray scList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACECRAFT);
   StringArray fmList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::FORMATION);

   int numSc = scList.size();
   int numFm = fmList.size();
   int numObj = 0;
   int soCount = 0;
   
   #if DEBUG_GUI_ITEM_SP > 1
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpaceObjectList() ==========>\n");
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
                  
                  #if DEBUG_GUI_ITEM_SP > 1
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
               
               #if DEBUG_GUI_ITEM_SP > 1
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
            
            #if DEBUG_GUI_ITEM_SP > 1
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
   
   #if DEBUG_GUI_ITEM_SP > 1
   MessageInterface::ShowMessage
      ("theNumSpaceObject=%d\n"
       "<==========GuiItemManager::UpdateSpaceObjectList() exiting\n",
       theNumSpaceObject);
   #endif
}


//------------------------------------------------------------------------------
// void UpdateCelestialBodyList()
//------------------------------------------------------------------------------
/**
 * Updates confugured CelestialBody list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCelestialBodyList()
{
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateCelestialBodyList() entered\n");
   #endif
   
   //StringArray items = theSolarSystem->GetBodiesInUse();
   StringArray items = theGuiInterpreter->GetListOfConfiguredItems(Gmat::CELESTIAL_BODY);
   theNumCelesBody = items.size();
   
   if (theNumCelesBody > MAX_CELES_BODY_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCelestialBodyList() GUI will handle up to %d bodies."
          "The number of bodies configured: %d\n", MAX_CELES_BODY_SIZE, theNumCelesBody);
      theNumCelesBody = MAX_CELES_BODY_SIZE;
   }
   
   for (int i=0; i<theNumCelesBody; i++)
   {
      theCelesBodyList[i] = items[i].c_str();
        
      #if DEBUG_GUI_ITEM > 1
      MessageInterface::ShowMessage("GuiItemManager::UpdateCelestialBodyList() " +
                                    std::string(theCelesBodyList[i].c_str()) + "\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void UpdateCelestialPointList()
//------------------------------------------------------------------------------
/**
 * Updates confugured CelestialBody and CalculatedPoint list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCelestialPointList()
{
   StringArray celesBodyList =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::CELESTIAL_BODY);
   //StringArray celesBodyList = theSolarSystem->GetBodiesInUse();
   StringArray calPointList =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::CALCULATED_POINT);
   
   theNumCelesBody = celesBodyList.size();
   theNumCalPoint = calPointList.size();
   theNumCelesPoint = theNumCelesBody + theNumCalPoint;
   
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateCelestialPointList() theNumCelesBody=%d, "
       "theNumCalPoint=%d, theNumCelesPoint=%d\n", theNumCelesBody, theNumCalPoint,
       theNumCelesPoint);
   #endif
   
   if (theNumCelesPoint > MAX_CELES_POINT_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCelestialPointList() GUI will handle up to %d bodies."
          "The number of bodies configured: %d\n", MAX_CELES_POINT_SIZE,
          theNumCelesPoint);
      theNumCelesPoint = MAX_CELES_POINT_SIZE;
   }

   // update CelestialBody list
   for (int i=0; i<theNumCelesBody; i++)
      theCelesBodyList[i] = celesBodyList[i].c_str();
   
   // update CalculatedPoint list
   for (int i=0; i<theNumCalPoint; i++)
      theCalPointList[i] = calPointList[i].c_str();
   
   // add CelestialBody to list
   for (int i=0; i<theNumCelesBody; i++)
      theCelesPointList[i] = theCelesBodyList[i];
   
   // add CalculatedPoint to list
   for (int i=0; i<theNumCalPoint; i++)
      theCelesPointList[theNumCelesBody+i] = theCalPointList[i];
   
}


//------------------------------------------------------------------------------
// void UpdateSpacePointList()
//------------------------------------------------------------------------------
/**
 * Updates configured SpacePoint list (Spacecraft, CelestialBody, CalculatedPoint)
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSpacePointList()
{
   #if DEBUG_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateSpacePointList() entered\n");
   #endif
   
   StringArray spList = theGuiInterpreter->GetListOfConfiguredItems(Gmat::SPACE_POINT);
   theNumSpacePoint = spList.size();

   if (theNumSpacePoint > MAX_SPACE_POINT_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacePointList() GUI will handle up to %d SpacePoints."
          "The number of SpacePoints configured: %d\n", MAX_SPACE_POINT_SIZE,
          theNumSpacePoint);
      theNumSpacePoint = MAX_SPACE_POINT_SIZE;
   }
   
   for (int i=0; i<theNumSpacePoint; i++)
   {
      theSpacePointList[i] = spList[i].c_str();
      
      #if DEBUG_GUI_ITEM_SP > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacePointList() theSpacePointList[%d]=%s\n",
          i, theSpacePointList[i].c_str());
      #endif
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
   #if DEBUG_GUI_ITEM_CS
   MessageInterface::ShowMessage("GuiItemManager::UpdateCoordSystemList() entered\n");
   #endif
   
   StringArray items =
      theGuiInterpreter->GetListOfConfiguredItems(Gmat::COORDINATE_SYSTEM);
   theNumCoordSys = items.size();
   
   if (theNumCoordSys > MAX_COORD_SYS_SIZE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCoordSystemList() GUI will handle up to %d coord. sys."
          "The number of coord. sys. configured: %d\n", MAX_COORD_SYS_SIZE, theNumCoordSys);
      theNumCoordSys = MAX_COORD_SYS_SIZE;
   }

   for (int i=0; i<theNumCoordSys; i++)
   {
      theCoordSysList[i] = items[i].c_str();

      #if DEBUG_GUI_ITEM_CS > 1
      MessageInterface::ShowMessage("GuiItemManager::UpdateCoordSystemList() " +
                                    std::string(theCoordSysList[i].c_str()) + "\n");
      #endif
   }
}

//------------------------------------------------------------------------------
// GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::GuiItemManager()
{
   #if DEBUG_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::GuiItemManager() entered\n");
   #endif
   
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theSolarSystem = theGuiInterpreter->GetDefaultSolarSystem();
   UpdatePropertyList("Spacecraft");
   
   theNumScProperty = 0;
   theNumSpaceObject = 0;
   theNumFormation = 0;
   theNumSpacecraft = 0;
   theNumCoordSys = 0;
   theNumPlottableParam = 0;
   theNumSystemParam = 0;
   theNumUserVariable = 0;
   theNumUserString = 0;
   theNumUserArray = 0;
   theNumUserParam = 0;
   theNumCelesBody = 0;
   theNumCelesPoint = 0;
   theNumCalPoint = 0;
   theNumSpacePoint = 0;
   
   theSpacecraftComboBox = NULL;
   theUserParamComboBox = NULL;
   theCoordSysComboBox = NULL;
   
   theSpacecraftListBox = NULL;
   theSpaceObjectListBox = NULL;
   theFormationListBox = NULL;
   theScPropertyListBox = NULL;
   thePlottableParamListBox = NULL;
   theSystemParamListBox = NULL;
   theAllUserParamListBox = NULL;
   theUserVariableListBox = NULL;
   theUserStringListBox = NULL;
   theUserArrayListBox = NULL;
   theUserParamListBox = NULL;
   theCelesBodyListBox = NULL;
   theSpacePointComboBox = NULL;
}

//------------------------------------------------------------------------------
// ~GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::~GuiItemManager()
{
}
