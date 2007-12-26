//$Id$
//------------------------------------------------------------------------------
//                              GuiItemManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
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
#include "Array.hpp"
#include "ParameterInfo.hpp"
#include "Hardware.hpp"
#include "GmatGlobal.hpp"         // for GetDataPrecision()
#include "MessageInterface.hpp"
#include <algorithm>              // for sort(), set_difference()
#include <sstream>
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

//#define DBGLVL_GUI_ITEM 1
//#define DBGLVL_GUI_ITEM_UPDATE 1
//#define DBGLVL_GUI_ITEM_UNREG 1
//#define DBGLVL_GUI_ITEM_PARAM 2
//#define DBGLVL_GUI_ITEM_PROPERTY 2
//#define DBGLVL_GUI_ITEM_SP 1
//#define DBGLVL_GUI_ITEM_SO 1
//#define DBGLVL_GUI_ITEM_SC 1
//#define DBGLVL_GUI_ITEM_CS 2
//#define DBGLVL_GUI_ITEM_HW 2
//#define DBGLVL_GUI_ITEM_BURN 2
//#define DBGLVL_GUI_ITEM_SUBS 2
//#define DBGLVL_GUI_ITEM_SOLVER 1
//#define DBGLVL_GUI_ITEM_ALL_OBJECT 1

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
// wxString ToWxString(Real rval)
//------------------------------------------------------------------------------
wxString GuiItemManager::ToWxString(Real rval)
{
   wxString str;
   std::stringstream ss;
   ss.precision(theDataPrecision);
   
   ss << rval;
   str.Printf("%s", ss.str().c_str());
   return str;
}


//------------------------------------------------------------------------------
// int IsValidVariable(const std::string &varName, Gmat::ObjectType ownerType,
//                     bool allowNumber = false, bool allowNonPlottable = true)
//------------------------------------------------------------------------------
/*
 * Checks if input variable is a Variable, Array element, or plottable
 * parameter of input owner type. The plottable parameter returns Real number.
 *
 * @param  varName  input variable name
 * @param  ownerType  input owner type (such as Gmat::SPACECRAFT)
 * @param  allowNumber  true if varName can be a Real number 
 * @param  allowNonPlottable  true if varName can be a non-plottable
 *
 * @return -1 if varName NOT found in the configuration
 *          0 if varName found BUT is not one of Variable, Array element, or parameter
 *          1 if varName found AND is one of Variable, Array element, or parameter
 *          2 if number is allowed and varName is Real number
 *          3 if varName contains undefined object of Parameter type
 *          4 if varName contains valid Parameter type
 */
//------------------------------------------------------------------------------
int GuiItemManager::IsValidVariable(const std::string &varName,
                                    Gmat::ObjectType ownerType, bool allowNumber,
                                    bool allowNonPlottable)
{
   if (allowNumber)
   {
      Real rval;
      if (GmatStringUtil::ToReal(varName.c_str(), rval))
         return 2;
   }
   
   GmatBase *obj = theGuiInterpreter->GetConfiguredObject(varName);
   
   // If name has a system Parameter type and owner object exist, create
   if (obj == NULL)
   {
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(varName, type, ownerName, depObj);

      #if DBGLVL_GUI_ITEM_VALIDATE
      MessageInterface::ShowMessage
         ("GuiItemManager::IsValidVariable() varName=<%s>, type=<%s>, "
          "ownerName=<%s>, depObj=<%s>\n", varName.c_str(), type.c_str(),
          ownerName.c_str(), depObj.c_str());
      #endif
      
      if (type != "")
      {
         if (theGuiInterpreter->IsParameter(type))
         {
            if (theGuiInterpreter->GetConfiguredObject(ownerName))
            {
               theGuiInterpreter->CreateSystemParameter(varName);
               return 1;
            }
            else
               return 3;
         }
         else
            return 4;
      }
      else
         return -1;
   }
   
   Parameter *param = (Parameter*)obj;   
   bool isValid = false;
   
   if (param->IsOfType("Variable"))
   {
      isValid = true;
   }
   else if (param->IsOfType("Array"))
   {
      // check to see if it is array element or whole array of 1x1
      if (GmatStringUtil::IsParenPartOfArray(varName))
      {
         isValid = true;
      }
      else
      {
         Array *arr = (Array*)param;
         if (arr->GetRowCount() == 1 && arr->GetColCount() == 1)
            isValid = true;
      }
   }
   else if (param->GetKey() == GmatParam::SYSTEM_PARAM)
   {
      if (allowNonPlottable)
      {
         // check to see if it is parameter of owenerType
         if (param->GetOwnerType() == ownerType)
            isValid = true;
      }
      else
      {
         // check to see if it is parameter of owenerType and plottable
         if (param->GetOwnerType() == ownerType && param->IsPlottable())
            isValid = true;
      }
   }
   
   if (isValid)
      return 1;
   else
      return 0;
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
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("==================> GuiItemManager::UpdateAll()\n");
   #endif
   
   UpdateCelestialPoint(); // All CelestialBodies and CalculatedPoints
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateCelestialPoint()\n");
   #endif
   
   UpdateFormation();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateFormation()\n");
   #endif
   
   UpdateSpacecraft();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateSpacecraft()\n");
   #endif
   
   UpdateBurn();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateBurn()\n");
   #endif
   
   UpdateParameter();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateParameter()\n");
   #endif
   
   UpdateSolarSystem();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateSolarSystem()\n");
   #endif
   
   UpdateCoordSystem();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateCoordSystem()\n");
   #endif
   
   UpdateHardware();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateHardware()\n");
   #endif
   
   UpdateFunction();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateFunction()\n");
   #endif
   
   UpdateSubscriber();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateSubscriber()\n");
   #endif
   
   UpdateSolver();
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> after UpdateSolver()\n");
   #endif
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
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateCelestialPoint\n");
   #endif
   
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
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateFormation\n");
   #endif
   
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
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSpacecraft\n");
   #endif
   
   UpdateSpacecraftList();
   UpdateSpaceObjectList();
   UpdateSpacePointList();
}


//------------------------------------------------------------------------------
//  void UpdateBurn()
//------------------------------------------------------------------------------
/**
 * Updates burn related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateBurn()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateBurn\n");
   #endif
   
   UpdateBurnList();
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
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateParameter\n");
   #endif
   
   UpdateParameterList();
}

//------------------------------------------------------------------------------
//  void UpdateSolarSystem()
//------------------------------------------------------------------------------
/**
 * Updates SolarSystem related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolarSystem()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSolarSystem\n");
   #endif
   
   UpdateCelestialBodyList();
   UpdateCelestialPointList();
}


//------------------------------------------------------------------------------
//  void UpdateCoordSystem()
//------------------------------------------------------------------------------
/**
 * Updates CoordinateSystem related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCoordSystem()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateCoordSystem\n");
   #endif
   
   UpdateCoordSystemList();
}


//------------------------------------------------------------------------------
//  void UpdateHardware()
//------------------------------------------------------------------------------
/**
 * Updates hardware related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateHardware()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateHardware\n");
   #endif
   
   UpdateHardwareList();
}


//------------------------------------------------------------------------------
//  void UpdateFunction()
//------------------------------------------------------------------------------
/**
 * Updates Function related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFunction()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateFunction\n");
   #endif
   
   UpdateFunctionList();
}


//------------------------------------------------------------------------------
//  void UpdateSubscriber()
//------------------------------------------------------------------------------
/**
 * Updates Subscriber related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSubscriber()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSubscriber\n");
   #endif
   
   UpdateSubscriberList();
}


//------------------------------------------------------------------------------
//  void UpdateSolver()
//------------------------------------------------------------------------------
/**
 * Updates Solver related gui components.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolver()
{
   #if DBGLVL_GUI_ITEM_UPDATE
   MessageInterface::ShowMessage("===> UpdateSolver\n");
   #endif
   
   UpdateSolverList();
}


//------------------------------------------------------------------------------
// void AddToResourceUpdateListeners(GmatPanel *panel)
//------------------------------------------------------------------------------
void GuiItemManager::AddToResourceUpdateListeners(GmatPanel *panel)
{
   mResourceUpdateListeners.push_back(panel);
}


//------------------------------------------------------------------------------
// void RemoveFromResourceUpdateListeners(GmatPanel *panel)
//------------------------------------------------------------------------------
void GuiItemManager::RemoveFromResourceUpdateListeners(GmatPanel *panel)
{
   std::vector<GmatPanel*>::iterator pos1 =
      find(mResourceUpdateListeners.begin(), mResourceUpdateListeners.end(), panel);
   
   if (pos1 != mResourceUpdateListeners.end())
      mResourceUpdateListeners.erase(pos1);
}


//------------------------------------------------------------------------------
// virtual bool PrepareObjectNameChange()
//------------------------------------------------------------------------------
bool GuiItemManager::PrepareObjectNameChange()
{
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
      if (!mResourceUpdateListeners[i]->PrepareObjectNameChange())
         return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// virtual void NotifyObjectNameChange(Gmat::ObjectType type,
//                                     const wxString &oldName,
//                                     const wxString &newName)
//------------------------------------------------------------------------------
void GuiItemManager::NotifyObjectNameChange(Gmat::ObjectType type,
                                            const wxString &oldName,
                                            const wxString &newName)
{
   for (UnsignedInt i=0; i<mResourceUpdateListeners.size(); i++)
   {
      mResourceUpdateListeners[i]->ObjectNameChanged(type, oldName, newName);
   }
}


//------------------------------------------------------------------------------
// void UnregisterListBox(const wxString &type, wxListBox *lb)
//                        wxArrayString *excList)
//------------------------------------------------------------------------------
void GuiItemManager::UnregisterListBox(const wxString &type, wxListBox *lb,
                                       wxArrayString *excList)
{
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterListBox() lb=%d, excList=%d\n", lb, excList);
   #endif
   
   if (type == "SpaceObject")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mSpaceObjectLBList.begin(), mSpaceObjectLBList.end(), lb);
      
      if (pos1 != mSpaceObjectLBList.end())
         mSpaceObjectLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpaceObjectExcList.begin(), mSpaceObjectExcList.end(), excList);
      
      if (pos2 != mSpaceObjectExcList.end())
         mSpaceObjectExcList.erase(pos2);
   }
   else if (type == "Spacecraft")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mSpacecraftLBList.begin(), mSpacecraftLBList.end(), lb);
      
      if (pos1 != mSpacecraftLBList.end())
         mSpacecraftLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpacecraftExcList.begin(), mSpacecraftExcList.end(), excList);
      
      if (pos2 != mSpacecraftExcList.end())
         mSpacecraftExcList.erase(pos2);
   }
   else if (type == "ImpulsiveBurn")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mImpBurnLBList.begin(), mImpBurnLBList.end(), lb);
      
      if (pos1 != mImpBurnLBList.end())
         mImpBurnLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mImpBurnExcList.begin(), mImpBurnExcList.end(), excList);
      
      if (pos2 != mImpBurnExcList.end())
         mImpBurnExcList.erase(pos2);
   }
   else if (type == "FuelTank")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mFuelTankLBList.begin(), mFuelTankLBList.end(), lb);
      
      if (pos1 != mFuelTankLBList.end())
         mFuelTankLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mFuelTankExcList.begin(), mFuelTankExcList.end(), excList);
      
      if (pos2 != mFuelTankExcList.end())
         mFuelTankExcList.erase(pos2);
   }
   else if (type == "Thruster")
   {
      std::vector<wxListBox*>::iterator pos1 =
         find(mThrusterLBList.begin(), mThrusterLBList.end(), lb);
      
      if (pos1 != mThrusterLBList.end())
         mThrusterLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mThrusterExcList.begin(), mThrusterExcList.end(), excList);
      
      if (pos2 != mThrusterExcList.end())
         mThrusterExcList.erase(pos2);
   }
}


//------------------------------------------------------------------------------
// void UnregisterCheckListBox(const wxString &type, wxCheckListBox *lb)
//                             wxArrayString *excList)
//------------------------------------------------------------------------------
void GuiItemManager::UnregisterCheckListBox(const wxString &type, wxCheckListBox *clb,
                                            wxArrayString *excList)
{
   #if DBGLVL_GUI_ITEM_UNREG
   MessageInterface::ShowMessage
      ("GuiItemManager::UnregisterCheckListBox() clb=%d, excList=%d\n", clb, excList);
   #endif
   
   if (type == "Subscriber")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mSubscriberCLBList.begin(), mSubscriberCLBList.end(), clb);
      
      if (pos1 != mSubscriberCLBList.end())
         mSubscriberCLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSubscriberExcList.begin(), mSubscriberExcList.end(), excList);
      
      if (pos2 != mSubscriberExcList.end())
         mSubscriberExcList.erase(pos2);
   }
   if (type == "Spacecraft")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mSpacecraftCLBList.begin(), mSpacecraftCLBList.end(), clb);
      
      if (pos1 != mSpacecraftCLBList.end())
         mSpacecraftCLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mSpacecraftExcList.begin(), mSpacecraftExcList.end(), excList);
      
      if (pos2 != mSpacecraftExcList.end())
         mSpacecraftExcList.erase(pos2);
   }
   else if (type == "AllObject")
   {
      std::vector<wxCheckListBox*>::iterator pos1 =
         find(mAllObjectCLBList.begin(), mAllObjectCLBList.end(), clb);
      
      if (pos1 != mAllObjectCLBList.end())
         mAllObjectCLBList.erase(pos1);
      
      std::vector<wxArrayString*>::iterator pos2 =
         find(mAllObjectExcList.begin(), mAllObjectExcList.end(), excList);
      
      if (pos2 != mAllObjectExcList.end())
         mAllObjectExcList.erase(pos2);
   }
}


//------------------------------------------------------------------------------
// void UnregisterComboBox(const wxString &type, wxComboBox *cb)
//------------------------------------------------------------------------------
void GuiItemManager::UnregisterComboBox(const wxString &type, wxComboBox *cb)
{
   if (type == "SpacePoint")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSpacePointCBList.begin(), mSpacePointCBList.end(), cb);
      
      if (pos != mSpacePointCBList.end())
         mSpacePointCBList.erase(pos);
   }
   else if (type == "Spacecraft")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSpacecraftCBList.begin(), mSpacecraftCBList.end(), cb);
      
      if (pos != mSpacecraftCBList.end())
         mSpacecraftCBList.erase(pos);
   }
   else if (type == "ImpulsiveBurn")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mImpBurnCBList.begin(), mImpBurnCBList.end(), cb);
      
      if (pos != mImpBurnCBList.end())
         mImpBurnCBList.erase(pos);
   }
   else if (type == "FiniteBurn")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mFiniteBurnCBList.begin(), mFiniteBurnCBList.end(), cb);
      
      if (pos != mFiniteBurnCBList.end())
         mFiniteBurnCBList.erase(pos);
   }
   else if (type == "CoordinateSystem")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mCoordSysCBList.begin(), mCoordSysCBList.end(), cb);
      
      if (pos != mCoordSysCBList.end())
         mCoordSysCBList.erase(pos);
   }
   else if (type == "Function")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mFunctionCBList.begin(), mFunctionCBList.end(), cb);
      
      if (pos != mFunctionCBList.end())
         mFunctionCBList.erase(pos);
   }
   else if (type == "FuelTank")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mFuelTankCBList.begin(), mFuelTankCBList.end(), cb);
      
      if (pos != mFuelTankCBList.end())
         mFuelTankCBList.erase(pos);
   }
   else if (type == "Thruster")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mThrusterCBList.begin(), mThrusterCBList.end(), cb);
      
      if (pos != mThrusterCBList.end())
         mThrusterCBList.erase(pos);
   }
   else if (type == "Subscriber")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSubscriberCBList.begin(), mSubscriberCBList.end(), cb);
      
      if (pos != mSubscriberCBList.end())
         mSubscriberCBList.erase(pos);
   }
   else if (type == "ReportFile")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mReportFileCBList.begin(), mReportFileCBList.end(), cb);
      
      if (pos != mReportFileCBList.end())
         mReportFileCBList.erase(pos);
   }
   else if (type == "Solver")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mSolverCBList.begin(), mSolverCBList.end(), cb);
      
      if (pos != mSolverCBList.end())
         mSolverCBList.erase(pos);
   }
   else if (type == "BoundarySolver")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mBoundarySolverCBList.begin(), mBoundarySolverCBList.end(), cb);
      
      if (pos != mBoundarySolverCBList.end())
         mBoundarySolverCBList.erase(pos);
   }
   else if (type == "Optimizer")
   {
      std::vector<wxComboBox*>::iterator pos =
         find(mOptimizerCBList.begin(), mOptimizerCBList.end(), cb);
      
      if (pos != mOptimizerCBList.end())
         mOptimizerCBList.erase(pos);
   }
   
}


//------------------------------------------------------------------------------
// wxArrayString GetPropertyList(const wxString &objType,
//                               int showOption = SHOW_PLOTTABLE))
//------------------------------------------------------------------------------
wxArrayString GuiItemManager::GetPropertyList(const wxString &objType,
                                              int showOption)
{
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::GetPropertyList() objType=%s\n", objType.c_str());
   #endif
   
   wxArrayString array;
   
   if (objType == "Spacecraft")
   {
      ParameterInfo *theParamInfo = ParameterInfo::Instance();
      for (int i=0; i<theNumScProperty; i++)
      {
         if (showOption == SHOW_REPORTABLE)
         {
            if (theParamInfo->IsReportable(theScPropertyList[i].c_str()))
            {
               #if DBGLVL_GUI_ITEM_PROPERTY > 1
               MessageInterface::ShowMessage
                  ("GetPropertyList() Adding %s\n", theScPropertyList[i].c_str());
               #endif
               
               array.Add(theScPropertyList[i]);
            }
         }
         else if (showOption == SHOW_PLOTTABLE)
         {
            if (theParamInfo->IsPlottable(theScPropertyList[i].c_str()))
            {
               #if DBGLVL_GUI_ITEM_PROPERTY > 1
               MessageInterface::ShowMessage
                  ("GetPropertyList() Adding %s\n", theScPropertyList[i].c_str());
               #endif
               
               array.Add(theScPropertyList[i]);
            }
         }
         else if (showOption == SHOW_SETTABLE)
         {
            if (theParamInfo->IsSettable(theScPropertyList[i].c_str()))
            {
               #if DBGLVL_GUI_ITEM_PROPERTY > 1
               MessageInterface::ShowMessage
                  ("GetPropertyList() Adding %s\n", theScPropertyList[i].c_str());
               #endif
               
               array.Add(theScPropertyList[i]);
            }
         }
      }
      
      return array;
   }
   else if (objType == "ImpulsiveBurn")
   {
      // for now all impulsive burn parameters are reportable
      return wxArrayString(theNumImpBurnProperty, theImpBurnPropertyList);
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Property list for %s is not available at this time\n",
          objType.c_str());
      
      return array;
   }
}


//------------------------------------------------------------------------------
// int GetNumProperty(const wxString &objType)
//------------------------------------------------------------------------------
/*
 * @return number of properties associated with objType
 */
//------------------------------------------------------------------------------
int GuiItemManager::GetNumProperty(const wxString &objType)
{
   if (objType == "Spacecraft")
      return theNumScProperty;
   else if (objType == "ImpulsiveBurn")
      return theNumImpBurnProperty;
   else if (objType == "FiniteBurn")
      return theNumFiniteBurnProperty;
   else
      return 0;
}


//------------------------------------------------------------------------------
// wxString* GetPropertyList(const wxString &objType)
//------------------------------------------------------------------------------
/*
 * @return propertie string array associated with objType
 */
//------------------------------------------------------------------------------
wxString* GuiItemManager::GetPropertyList(const wxString &objType)
{
   if (objType == "Spacecraft")
      return theScPropertyList;
   else if (objType == "ImpulsiveBurn")
      return theImpBurnPropertyList;
   else if (objType == "FiniteBurn")
      return theFiniteBurnPropertyList;
   else
      throw GmatBaseException("There are no properties associated with " +
                              std::string(objType.c_str()));

//    if (objType != "Spacecraft")
//       throw GmatBaseException("There are no properties associated with " +
//                               std::string(objType.c_str()));
   
//    return theScPropertyList;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetObjectTypeComboBox(wxWindow *parent, const wxSize &size, ...)
//------------------------------------------------------------------------------
/**
 * @return object type combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetObjectTypeComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size,
                                                  const wxArrayString objectTypeList)
{
   wxArrayString emptyList;
   wxComboBox *cb =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     emptyList, wxCB_READONLY);
   
   for (unsigned int i=0; i<objectTypeList.size(); i++)
   {
      if (objectTypeList[i] == "Spacecraft")// && theNumSpacecraft > 0)
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "ImpulsiveBurn")// && theNumImpBurn > 0)
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "FiniteBurn")// && theNumFiniteBurn > 0)
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "Array")// && theNumUserArray > 0)
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "Variable")// && theNumUserVariable > 0)
         cb->Append(objectTypeList[i]);
      else if (objectTypeList[i] == "String")// && theNumUserString > 0)
         cb->Append(objectTypeList[i]);
   }
   
   cb->SetSelection(0);
   
   return cb;
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
   wxComboBox *spacecraftComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumSpacecraft, theSpacecraftList, wxCB_READONLY);
   
   if (theNumSpacecraft == 0)
      spacecraftComboBox->Append("No Spacecrafts Available");
   
   // show first spacecraft
   spacecraftComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSpacecraftCBList.push_back(spacecraftComboBox);
   #if DBGLVL_GUI_ITEM_SC
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacecraftComboBox() Spacecraft:%d registered\n",
       spacecraftComboBox);
   #endif
   
   return spacecraftComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetImpBurnComboBox(wxWindow *parent, wxWindowID id, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return burn combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetImpBurnComboBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size)
{
   wxComboBox *burnComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumImpBurn, theImpBurnList, wxCB_READONLY);
   
   if (theNumImpBurn == 0)
      burnComboBox->Append("No Impulsive Burns Available");
   
   // show first burn
   burnComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mImpBurnCBList.push_back(burnComboBox);
   
   return burnComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetFiniteBurnComboBox(wxWindow *parent, wxWindowID id,
//                                    const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return burn combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetFiniteBurnComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *burnComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumFiniteBurn, theFiniteBurnList, wxCB_READONLY);

   if (theNumFiniteBurn == 0)
      burnComboBox->Append("No Finite Burns Available");
   
   // show first burn
   burnComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFiniteBurnCBList.push_back(burnComboBox);
   
   return burnComboBox;
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
   
   wxComboBox *coordSysComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     numCs, theCoordSysList, wxCB_READONLY);
   
   // show first coordinate system
   coordSysComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mCoordSysCBList.push_back(coordSysComboBox);
   
   return coordSysComboBox;
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
   
   wxComboBox *celesBodyComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     numBody, theCelesBodyList, wxCB_READONLY);
   
   // show Earth as a default body
   celesBodyComboBox->SetStringSelection("Earth");
   
   return celesBodyComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetFunctionComboBox(wxWindow *parent, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return CelestialBody ComboBox pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetFunctionComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{   
   wxComboBox *functionComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumFunction, theFunctionList, wxCB_READONLY);
   
   if (theNumFunction == 0)
      functionComboBox->Append("No Functions Available");
   
   functionComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFunctionCBList.push_back(functionComboBox);
   
   return functionComboBox;
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
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpacePointComboBox() theNumSpacePoint=%d\n",
      theNumSpacePoint);
   #endif
   
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxComboBox *spacePointComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size, //0,
                     emptyList, wxCB_READONLY);
   
   if (addVector)
      spacePointComboBox->Append("Vector");
   
   for (int i=0; i<theNumSpacePoint; i++)
      spacePointComboBox->Append(theSpacePointList[i]);
   
   // select first item
   //loj: 6/21/05 commented out.
   // We don't want to show object selected for CoordinateSystem.
   //spacePointComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacePointCBList.push_back(spacePointComboBox);
   
   return spacePointComboBox;
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
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointComboBox() theNumCelesPoint=%d\n",
      theNumCelesPoint);
   #endif
      
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxComboBox * celestialPointComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size, // 0,
                     emptyList, wxCB_READONLY);
   
   if (addVector)
      celestialPointComboBox->Append("Vector");
   
   for (int i=0; i<theNumCelesPoint; i++)
      celestialPointComboBox->Append(theCelesPointList[i]);
   
   // select first item
   celestialPointComboBox->SetSelection(0);
   
   return celestialPointComboBox;
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
   wxComboBox *userParamComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumUserVariable, theUserVariableList, wxCB_READONLY);
   
   if (theNumUserVariable == 0)
      userParamComboBox->Append("No Variables Available");
   
   // show first parameter
   userParamComboBox->SetSelection(0);
   
   return userParamComboBox;
}


//------------------------------------------------------------------------------
//  wxComboBox* GetFuelTankComboBox(wxWindow *parent, wxWindowID id, const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return fuel tank combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetFuelTankComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{
   wxComboBox *fuelTankComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumFuelTank, theFuelTankList, wxCB_READONLY);
   
   if (theNumFuelTank == 0)
      fuelTankComboBox->Append("No Fuel Tanks Available");
   
   // show first burn
   fuelTankComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFuelTankCBList.push_back(fuelTankComboBox);
   
   return fuelTankComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetThrusterComboBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Thruster combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetThrusterComboBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size)
{
   wxComboBox *thrusterComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumThruster, theThrusterList, wxCB_READONLY);
   
   if (theNumThruster == 0)
      thrusterComboBox->Append("No Thrusters Available");
   
   // show first Thruster
   thrusterComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mThrusterCBList.push_back(thrusterComboBox);
   
   return thrusterComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSubscriberComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Subscriber combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSubscriberComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *subsComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumSubscriber, theSubscriberList, wxCB_READONLY);
   
   if (theNumSubscriber == 0)
      subsComboBox->Append("No Subscribers Available");
   
   // show first Subscriber
   subsComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSubscriberCBList.push_back(subsComboBox);
   
   return subsComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetReportFileComboBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return ReportFile combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetReportFileComboBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size)
{
   wxComboBox *rfComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumReportFile, theReportFileList, wxCB_READONLY);
   
   if (theNumReportFile == 0)
      rfComboBox->Append("No Report Files Available");
   
   // show first ReportFile
   rfComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mReportFileCBList.push_back(rfComboBox);
   
   return rfComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetSolverComboBox(wxWindow *parent, wxWindowID id,
//                               const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Solver combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetSolverComboBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size)
{
   wxComboBox *solverComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumSolver, theSolverList, wxCB_READONLY);
   
   if (theNumSolver == 0)
      solverComboBox->Append("No Solvers Available");
   
   // show first Solver
   solverComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mSolverCBList.push_back(solverComboBox);
   
   return solverComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetBoundarySolverComboBox(wxWindow *parent, wxWindowID id,
//                                      const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return BoundarySolver combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetBoundarySolverComboBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size)
{
   wxComboBox *bvsComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumBoundarySolver, theBoundarySolverList, wxCB_READONLY);
   
   // Is this correct name?
   if (theNumBoundarySolver == 0)
      bvsComboBox->Append("No Differential Correctors Available");
   
   // show first Boundary Solver
   bvsComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mBoundarySolverCBList.push_back(bvsComboBox);
   
   return bvsComboBox;
}


//------------------------------------------------------------------------------
// wxComboBox* GetOptimizerComboBox(wxWindow *parent, wxWindowID id,
//                                  const wxSize &size)
//------------------------------------------------------------------------------
/**
 * @return Optimizer combo box pointer
 */
//------------------------------------------------------------------------------
wxComboBox* GuiItemManager::GetOptimizerComboBox(wxWindow *parent, wxWindowID id,
                                                 const wxSize &size)
{
   wxComboBox *optimizerComboBox =
      new wxComboBox(parent, id, wxT(""), wxDefaultPosition, size,
                     theNumOptimizer, theOptimizerList, wxCB_READONLY);
   
   // Is this correct name?
   if (theNumOptimizer == 0)
      optimizerComboBox->Append("No Optimizers Available");
   
   // show first Optimizer
   optimizerComboBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mOptimizerCBList.push_back(optimizerComboBox);
   
   return optimizerComboBox;
}


// CheckListBox
//------------------------------------------------------------------------------
// wxCheckListBox* GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
//                                           const wxSize &size, wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Available Subscriber ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetSubscriberCheckListBox(wxWindow *parent, wxWindowID id,
                                                          const wxSize &size,
                                                          wxArrayString *excList)
{
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, //0,
                         emptyList, wxLB_SINGLE|wxLB_SORT);

   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSubscriber; i++)
      {
         if (excList->Index(theSubscriberList[i]) == wxNOT_FOUND)
            checkListBox->Append(theSubscriberList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumSubscriber; i++)
         checkListBox->Append(theSubscriberList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSubscriberCLBList.push_back(checkListBox);
   mSubscriberExcList.push_back(excList);
   
   checkListBox->SetSelection(0);
   return checkListBox;
}


//------------------------------------------------------------------------------
// wxCheckListBox* GetSpacecraftCheckListBox(wxWindow *parent, wxWindowID id,
//                                           const wxSize &size, wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxCheckListBox* GuiItemManager::GetSpacecraftCheckListBox(wxWindow *parent, wxWindowID id,
                                                          const wxSize &size,
                                                          wxArrayString *excList)
{
   //causing VCC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size, //0,
                         emptyList, wxLB_SINGLE|wxLB_SORT);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSpacecraft; i++)
      {
         if (excList->Index(theSpacecraftList[i]) == wxNOT_FOUND)
            checkListBox->Append(theSpacecraftList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumSpacecraft; i++)
         checkListBox->Append(theSpacecraftList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacecraftCLBList.push_back(checkListBox);
   mSpacecraftExcList.push_back(excList);
   
   //We don't need to set selection (loj: 2007.06.26)
   //checkListBox->SetSelection(0);
   return checkListBox;
}


//------------------------------------------------------------------------------
// wxCheckListBox* GetAllObjectCheckListBox(wxWindow *parent, wxWindowID id,
//                                          const wxSize &size, wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Available All Object ListBox pointer.
 */
//------------------------------------------------------------------------------
wxCheckListBox*
GuiItemManager::GetAllObjectCheckListBox(wxWindow *parent, wxWindowID id,
                                         const wxSize &size, wxArrayString *excList)
{
   wxArrayString emptyList;
   wxCheckListBox *checkListBox =
      new wxCheckListBox(parent, id, wxDefaultPosition, size,
                         emptyList, wxLB_SINGLE|wxLB_SORT);
   
   //---------------------------------------------
   // Get all object list
   //---------------------------------------------
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumAllObject; i++)
      {
         if (excList->Index(theAllObjectList[i]) == wxNOT_FOUND)
            checkListBox->Append(theAllObjectList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumAllObject; i++)
         checkListBox->Append(theAllObjectList[i]);
   }
   
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mAllObjectCLBList.push_back(checkListBox);
   mAllObjectExcList.push_back(excList);
   
   checkListBox->SetSelection(0);
   return checkListBox;
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
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::GetSpacePointListBox() entered\n");
   #endif
   
   int numSpacePoint = theNumSpacePoint;
   
   if (theNumSpacePoint == 0)
      numSpacePoint = 1;

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxListBox *spacePointListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE|wxLB_SORT);
   
   if (addVector)
      spacePointListBox->Append("Vector");
   
   for (int i=0; i<theNumSpacePoint; i++)
      spacePointListBox->Append(theSpacePointList[i]);
      
   // select first item
   spacePointListBox->SetSelection(0);

   return spacePointListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
//                                     const wxSize &size, wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return configured CelestialBody and CalculatedPoint object ListBox pointer
 * excluding names in the excList array.
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetCelestialPointListBox(wxWindow *parent, wxWindowID id,
                                                    const wxSize &size,
                                                    wxArrayString &excList)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage
      ("GuiItemManager::GetCelestialPointListBox() theNumCelesPoint=%d\n",
       theNumCelesPoint);
   #endif
   
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxListBox *celesPointListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE|wxLB_SORT);
   
   if (excList.GetCount() > 0)
   {
      for (int i=0; i<theNumCelesPoint; i++)
      {
         if (excList.Index(theCelesPointList[i].c_str()) == wxNOT_FOUND)
            celesPointListBox->Append(theCelesPointList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumCelesPoint; i++)
         celesPointListBox->Append(theCelesPointList[i]);
   }
   
   
   celesPointListBox->SetSelection(0);
   return celesPointListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetSpaceObjectListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return configured Spacecraft and Formation object ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpaceObjectListBox(wxWindow *parent, wxWindowID id,
                                                 const wxSize &size,
                                                 wxArrayString *excList,
                                                 bool includeFormation)
{
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpaceObjectListBox() theNumSpaceObject=%d\n",
       theNumSpaceObject);
   for (unsigned int i=0; i<excList->GetCount(); i++)
   {
      MessageInterface::ShowMessage("excList[%d]=<%s>\n",
                                    i, (*excList)[i].c_str());
   }
   #endif
   
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxListBox *spaceObjectListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE|wxLB_SORT);

   // get Formation list
   StringArray fmList =
      theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSpaceObject; i++)
      {
         if (excList->Index(theSpaceObjectList[i]) == wxNOT_FOUND)
            spaceObjectListBox->Append(theSpaceObjectList[i]);
      }

      // if include formation
      if (includeFormation)
      {
         for (unsigned int i=0; i<fmList.size(); i++)
            if (excList->Index(fmList[i].c_str()) == wxNOT_FOUND)
               spaceObjectListBox->Append(fmList[i].c_str());
      }
   }
   else
   {
      for (int i=0; i<theNumSpaceObject; i++)
         spaceObjectListBox->Append(theSpaceObjectList[i]);

      // if include formation
      if (includeFormation)
         for (unsigned int i=0; i<fmList.size(); i++)
            spaceObjectListBox->Append(fmList[i].c_str());
      
   }
   
   spaceObjectListBox->SetSelection(0);
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpaceObjectLBList.push_back(spaceObjectListBox);
   mSpaceObjectExcList.push_back(excList);
   
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage
      ("GuiItemManager::GetSpaceObjectListBox() size=%d, addr=%d\n",
       mSpaceObjectLBList.size(), spaceObjectListBox);
   #endif
   
   return spaceObjectListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetSpacecraftListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Available Spacecraft ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetSpacecraftListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                wxArrayString *excList,
                                                bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *spacecraftListBox = NULL;
   
   if (multiSelect)
   {
      spacecraftListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                        emptyList, wxLB_MULTIPLE|wxLB_SORT);
   }
   else
   {
      spacecraftListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                        emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumSpacecraft; i++)
      {
         if (excList->Index(theSpacecraftList[i]) == wxNOT_FOUND)
            spacecraftListBox->Append(theSpacecraftList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumSpacecraft; i++)
         spacecraftListBox->Append(theSpacecraftList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mSpacecraftLBList.push_back(spacecraftListBox);
   mSpacecraftExcList.push_back(excList);
   
   if (!multiSelect)
      spacecraftListBox->SetSelection(0);
   
   return spacecraftListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetImpBurnListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Available ImpBurn ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetImpBurnListBox(wxWindow *parent, wxWindowID id,
                                             const wxSize &size,
                                             wxArrayString *excList,
                                             bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *impBurnListBox = NULL;
   
   if (multiSelect)
   {
      impBurnListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                     emptyList, wxLB_MULTIPLE|wxLB_SORT);
   }
   else
   {
      impBurnListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                     emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumImpBurn; i++)
      {
         if (excList->Index(theImpBurnList[i]) == wxNOT_FOUND)
            impBurnListBox->Append(theImpBurnList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumImpBurn; i++)
         impBurnListBox->Append(theImpBurnList[i]);
   }
   
   //---------------------------------------------
   // register to update list
   //---------------------------------------------
   mImpBurnLBList.push_back(impBurnListBox);
   mImpBurnExcList.push_back(excList);
   
   if (!multiSelect)
      impBurnListBox->SetSelection(0);
   
   return impBurnListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetPropertyListBox(wxWindow *parent, wxWindowID id, const wxSize &size,
//                               const wxString &objType, int showOption,
//                               bool multiSelect = false)
//------------------------------------------------------------------------------
/**
 * @return Available Parameter ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetPropertyListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              const wxString &objType,
                                              int showOption, bool multiSelect)
{
   #ifdef DEBUG_PROPERTY_LISTBOX
   MessageInterface::ShowMessage
      ("GuiItemManager::GetPropertyListBox() showOption=%d, multiSelect=%d\n",
       showOption, multiSelect);
   #endif
   
   ParameterInfo *theParamInfo = ParameterInfo::Instance();
   wxArrayString emptyList;
   wxListBox *propertyListBox = NULL;
   
   if (multiSelect)
   {
      propertyListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                      emptyList, wxLB_MULTIPLE|wxLB_SORT);
   }
   else
   {
      propertyListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                      emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   // now append properties
   if (objType == "Spacecraft")
   {
      if (showOption == SHOW_PLOTTABLE)
      {
         for (int i=0; i<theNumScProperty; i++)
         {
            if (theParamInfo->IsPlottable(theScPropertyList[i].c_str()))
               propertyListBox->Append(theScPropertyList[i]);
         }
      }
      else if (showOption == SHOW_REPORTABLE)
      {
         for (int i=0; i<theNumScProperty; i++)
         {
            if (theParamInfo->IsReportable(theScPropertyList[i].c_str()))
               propertyListBox->Append(theScPropertyList[i]);
         }
      }
      else if (showOption == SHOW_SETTABLE)
      {
         for (int i=0; i<theNumScProperty; i++)
         {
            if (theParamInfo->IsSettable(theScPropertyList[i].c_str()))
               propertyListBox->Append(theScPropertyList[i]);
         }
      }
   }
   else if (objType == "ImpulsiveBurn")
   {
      for (int i=0; i<theNumImpBurnProperty; i++)
         propertyListBox->Append(theImpBurnPropertyList[i]);
   }
   else if (objType == "FiniteBurn")
   {
      for (int i=0; i<theNumFiniteBurnProperty; i++)
         propertyListBox->Append(theFiniteBurnPropertyList[i]);
   }
   else
   {
      throw GmatBaseException("There are no properties associated with " +
                              std::string(objType.c_str()));
   }
   
   if (!multiSelect)
      propertyListBox->SetSelection(0);
   
   return propertyListBox;
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
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   
   wxListBox *plottableParamListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE|wxLB_SORT);

   if (nameToExclude != "")
   {
      for (int i=0; i<theNumPlottableParam; i++)
         if (thePlottableParamList[i] != nameToExclude)
            plottableParamListBox->Append(thePlottableParamList[i]);
   }
   else
   {
      for (int i=0; i<theNumPlottableParam; i++)
         plottableParamListBox->Append(thePlottableParamList[i]);
   }
   
   plottableParamListBox->SetSelection(theNumPlottableParam - 1);
   return plottableParamListBox;
   
   
}

//------------------------------------------------------------------------------
// wxListBox* GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
//                                   const wxSize &size, bool showArray)
//------------------------------------------------------------------------------
/**
 * @return Configured all user parameter (Varialbe, Array, String) ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetAllUserParameterListBox(wxWindow *parent, wxWindowID id,
                                                      const wxSize &size, bool showArray)
{
   wxString *allUserParamList;
   int numParams = 0;
   int allUserParamCount = 0;
   wxListBox *allUserParamListBox = NULL;

   if (showArray)
      allUserParamCount = theNumUserVariable + theNumUserArray + theNumUserString;
   else
      allUserParamCount = theNumUserVariable + theNumUserString;
      
   // 5/16/05 - arg:  moved create and delete allUserParamList into the if-stmt
   // to prevent crashing when trying to delete an empty array
   if (allUserParamCount > 0)
   {
      allUserParamList = new wxString[allUserParamCount];

      for (int i=0; i<theNumUserVariable; i++)
         allUserParamList[numParams++] = theUserVariableList[i];

      for (int i=0; i<theNumUserString; i++)
         allUserParamList[numParams++] = theUserStringList[i];

      if (showArray)
      {
         for (int i=0; i<theNumUserArray; i++)
            allUserParamList[numParams++] = theUserArrayList[i];
      }
      
      allUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, allUserParamCount,
                       allUserParamList, wxLB_SINGLE|wxLB_SORT);
                       
      delete [] allUserParamList;
   }
   else
   {
      //causing VC++ error => wxString emptyList[] = {};
      wxArrayString emptyList;
      
      allUserParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, //0,
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   return allUserParamListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserVariableListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Configured User Variable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserVariableListBox(wxWindow *parent, wxWindowID id,
                                                  const wxSize &size,
                                                  const wxString &nameToExclude,
                                                  bool multiSelect)
{   
   wxArrayString emptyList;
   wxListBox *userVariableListBox = NULL;

   // create ListBox
   if (multiSelect)
   {
      userVariableListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                          emptyList, wxLB_MULTIPLE|wxLB_SORT);
   }
   else
   {      
      userVariableListBox = new wxListBox(parent, id, wxDefaultPosition, size, 
                                          emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   // add to ListBox
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumUserVariable; i++)
         if (theUserVariableList[i] != nameToExclude)
            userVariableListBox->Append(theUserVariableList[i]);
   }
   else
   {
      for (int i=0; i<theNumUserVariable; i++)
         userVariableListBox->Append(theUserVariableList[i]);
   }
   
   if (!multiSelect && userVariableListBox->GetCount() > 0)
      userVariableListBox->SetSelection(userVariableListBox->GetCount() - 1);
   
   return userVariableListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserStringListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Configured User Valiable ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserStringListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                const wxString &nameToExclude,
                                                bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *userStringListBox = NULL;
   
   // create ListBox
   if (multiSelect)
   {
      userStringListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                        emptyList, wxLB_MULTIPLE|wxLB_SORT);
   }
   else
   {
      userStringListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                        emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   // add to ListBox
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumUserString; i++)
         if (theUserStringList[i] != nameToExclude)
            userStringListBox->Append(theUserStringList[i]);
   }
   else
   {
      for (int i=0; i<theNumUserString; i++)
         userStringListBox->Append(theUserStringList[i]);
   }
   
   if (!multiSelect && userStringListBox->GetCount() > 0)
      userStringListBox->SetSelection(userStringListBox->GetCount() - 1);
   
   return userStringListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetUserArrayListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
/**
 * @return Configured User Array ListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetUserArrayListBox(wxWindow *parent, wxWindowID id,
                                               const wxSize &size,
                                               const wxString &nameToExclude,
                                               bool multiSelect)
{
   wxArrayString emptyList;
   wxListBox *userArrayListBox = NULL;
   
   // create ListBox
   if (multiSelect)
   {
      userArrayListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                       emptyList, wxLB_MULTIPLE|wxLB_SORT);
   }
   else
   {
      userArrayListBox = new wxListBox(parent, id, wxDefaultPosition, size,
                                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   // add to ListBox
   if (nameToExclude != "")
   {
      for (int i=0; i<theNumUserArray; i++)
         if (theUserArrayList[i] != nameToExclude)
            userArrayListBox->Append(theUserArrayList[i]);
   }
   else
   {
      for (int i=0; i<theNumUserArray; i++)
         userArrayListBox->Append(theUserArrayList[i]);
   }
   
   if (!multiSelect && userArrayListBox->GetCount() > 0)
      userArrayListBox->SetSelection(userArrayListBox->GetCount() - 1);
   
   return userArrayListBox;
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
   wxArrayString emptyList;
   wxListBox *userParamListBox = NULL;
   
   if (theNumUserParam > 0)
   {       
      userParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size, theNumUserParam,
                       theUserParamList, wxLB_SINGLE|wxLB_SORT);
   }
   else
   {       
      userParamListBox =
         new wxListBox(parent, id, wxDefaultPosition, size,
                       emptyList, wxLB_SINGLE|wxLB_SORT);
   }
   
   return userParamListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetConfigBodyListBox(wxWindow *parent, wxWindowID id,
//                                 const wxSize &size,
//                                 wxArrayString &excList)
//------------------------------------------------------------------------------
/**
 * @return Configured ConfigBodyListBox pointer
 */
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetConfigBodyListBox(wxWindow *parent, wxWindowID id,
                                                const wxSize &size,
                                                wxArrayString &excList)
{    
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxListBox *celesBodyListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE);

   if (excList.GetCount() > 0)
   {
      for (int i=0; i<theNumCelesBody; i++)
      {
         if (excList.Index(theCelesBodyList[i].c_str()) == wxNOT_FOUND)
            celesBodyListBox->Append(theCelesBodyList[i]);
      }
   }
   else
   {
      for (int i=0; i<theNumCelesBody; i++)
         celesBodyListBox->Append(theCelesBodyList[i]);
   }
   
   celesBodyListBox->SetSelection(0);
   return celesBodyListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetFuelTankListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetFuelTankListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              wxArrayString *excList)
{
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::GetFuelTankListBox() theNumFuelTank=%d, "
       "Exclude count=%d\n", theNumFuelTank, excList->GetCount());
   #endif
   
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxListBox *fuelTankListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE|wxLB_SORT);

//    //-------------------------------------------------------
//    // exclude FuelTanks used in other spacecrafts
//    //-------------------------------------------------------
//    Spacecraft *sc = NULL;
//    StringArray tankNames, allTankNames;
//    int paramID;
   
//    for (int i=0; i<theNumSpacecraft; i++)
//    {
//       sc = theGuiInterpreter->GetSpacecraft(std::string(theSpacecraftList[i].c_str()));
//       paramID = sc->GetParameterID("Tanks");
//       tankNames = sc->GetStringArrayParameter(paramID);
//       allTankNames.insert(allTankNames.end(), tankNames.begin(), tankNames.end());
//    }
   
//    if (allTankNames.size() > 0)
//    {
//       for (int i=0; i<theNumFuelTank; i++)
//       {
//          StringArray::iterator pos =
//             find(allTankNames.begin(), allTankNames.end(),
//                  std::string(theFuelTankList[i].c_str()));
         
//          if (pos == allTankNames.end())
//             fuelTankListBox->Append(theFuelTankList[i]);
//       }
//    }

   // It's ok to have the same FuleTank in more than one spacecraft since
   // the Sandbox will clone it.
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumFuelTank; i++)
         if (excList->Index(theFuelTankList[i].c_str()) == wxNOT_FOUND)
            fuelTankListBox->Append(theFuelTankList[i]);

   }
   else
   {
      for (int i=0; i<theNumFuelTank; i++)
         fuelTankListBox->Append(theFuelTankList[i]);
   }
   
   fuelTankListBox->SetSelection(0);
   
   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mFuelTankLBList.push_back(fuelTankListBox);
   mFuelTankExcList.push_back(excList);
   
   return fuelTankListBox;
}


//------------------------------------------------------------------------------
// wxListBox* GetThrusterListBox(wxWindow *parent, wxWindowID id, ...)
//------------------------------------------------------------------------------
wxListBox* GuiItemManager::GetThrusterListBox(wxWindow *parent, wxWindowID id,
                                              const wxSize &size,
                                              wxArrayString *excList)
{
   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;
   wxListBox *thrusterListBox =
      new wxListBox(parent, id, wxDefaultPosition, size, //0,
                    emptyList, wxLB_SINGLE|wxLB_SORT);
      
   // It's ok to have the same Thruster in more than one spacecraft isince
   // the Sandbox will clone it
   if (excList != NULL && excList->GetCount() > 0)
   {
      for (int i=0; i<theNumThruster; i++)
         if (excList->Index(theThrusterList[i].c_str()) == wxNOT_FOUND)
            thrusterListBox->Append(theThrusterList[i]);
            //thrusterListBox->Append(theFuelTankList[i]);

   }
   else
   {
      for (int i=0; i<theNumThruster; i++)
         thrusterListBox->Append(theThrusterList[i]);
   }
   
   thrusterListBox->SetSelection(0);

   //---------------------------------------------
   // register for update
   //---------------------------------------------
   mThrusterLBList.push_back(thrusterListBox);
   mThrusterExcList.push_back(excList);
   
   return thrusterListBox;
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
                     wxComboBox **objectTypeComboBox, wxWindowID objectTypeComboBoxId, 
                     wxComboBox **spacecraftComboBox, wxWindowID spacecraftComboBoxId,
                     wxComboBox **impBurnComboBox, wxWindowID impBurnComboBoxId,
                     wxListBox **propertyListBox, wxWindowID propertyListBoxId,
                     wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
                     wxComboBox **originComboBox, wxWindowID originComboBoxId,
                     wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
                     const wxArrayString &objectTypeList, int showOption,
                     bool showVariable, bool showArray, const wxString &objectType)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::CreateParameterSizer() entered\n");
   #endif
   
   int bsize = 1;
   
   //wxStaticText
   wxStaticText *userVarStaticText = NULL;
   
   if (showVariable || showArray)
   {
      userVarStaticText =
         new wxStaticText(parent, -1, wxT("Variables"),
                          wxDefaultPosition, wxDefaultSize, 0);
   }
   
   wxStaticText *objectTypeStaticText =
      new wxStaticText(parent, -1, wxT("Object Type"),
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
   if (showVariable || showArray)
   {
      *createVarButton =
         new wxButton(parent, createVarButtonId, wxT("Create"),
                      wxDefaultPosition, wxSize(-1,-1), 0 );
   }
   
   // Object type ComboBox
   *objectTypeComboBox =
      GetObjectTypeComboBox(parent, objectTypeComboBoxId, wxSize(170, 20),
                            objectTypeList);
   (*objectTypeComboBox)->SetValue(objectType);
   
   // Spacecraft ComboBox
   *spacecraftComboBox =
      GetSpacecraftComboBox(parent, spacecraftComboBoxId, wxSize(170, 20));
   
   // ImpulsiveBurn ComboBox
   *impBurnComboBox =
      GetImpBurnComboBox(parent, impBurnComboBoxId, wxSize(170, 20));
   
   // Coordinate System ComboBox
   *coordSysComboBox =
      GetCoordSysComboBox(parent, coordSysComboBoxId, wxSize(170, 20));
   
   // Origin ComboBox
   *originComboBox =
      GetConfigBodyComboBox(parent, originComboBoxId, wxSize(170, 20));
   
   //-----------------------------------------------------------------
   // user parameter
   //-----------------------------------------------------------------
   *userParamListBox = NULL;
   
   if (showVariable || showArray)
   {
      if (showOption == SHOW_REPORTABLE)
      {
         *userParamListBox =
            GetAllUserParameterListBox(parent, userParamListBoxId, 
                                       wxSize(170, 50), showArray);
      }
      else if (showOption == SHOW_PLOTTABLE)
      {
         *userParamListBox =
            GetUserVariableListBox(parent, userParamListBoxId,
                                   wxSize(170, 50), "");
      }
   }
   
   //-----------------------------------------------------------------
   // property
   //-----------------------------------------------------------------
   *propertyListBox = 
      GetPropertyListBox(parent, propertyListBoxId, wxSize(170, 80), objectType,
                         showOption);
   
   #ifdef __WXMAC__
   //-------------------------------------------------------
   wxBoxSizer *userParamBoxSizer = NULL;
   if (showVariable || showArray)
      userParamBoxSizer = new wxBoxSizer(wxVERTICAL);
   wxBoxSizer *systemParamBoxSizer = new wxBoxSizer(wxVERTICAL);
   #else
   //-------------------------------------------------------
   wxStaticBoxSizer *userParamBoxSizer = NULL;
   if (showVariable || showArray)
      userParamBoxSizer = new wxStaticBoxSizer(wxVERTICAL, parent, "");
   wxStaticBoxSizer *systemParamBoxSizer =
      new wxStaticBoxSizer(wxVERTICAL, parent, "");
   #endif
   //-------------------------------------------------------
   
   wxBoxSizer *paramBoxSizer = new wxBoxSizer(wxVERTICAL);
   *coordSysBoxSizer = new wxBoxSizer(wxVERTICAL);
   
   (*coordSysBoxSizer)->Add(*coordSysLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   if (showVariable || showArray)
   {
      userParamBoxSizer->Add
         (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
      userParamBoxSizer->Add
         (*userParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
      userParamBoxSizer->Add
         (*createVarButton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   }
   
   systemParamBoxSizer->Add
      (objectTypeStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   systemParamBoxSizer->Add
        (*objectTypeComboBox, 0, wxGROW|wxALIGN_CENTER|wxBOTTOM|wxALL, bsize);
   systemParamBoxSizer->Add
      (objectStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   
   #ifdef __WXMAC__
   systemParamBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   #endif
   
   systemParamBoxSizer->Add
      (*spacecraftComboBox, 0, wxGROW|wxALIGN_CENTER|wxBOTTOM|wxALL, bsize);
   systemParamBoxSizer->Add
      (*impBurnComboBox, 0, wxGROW|wxALIGN_CENTER|wxBOTTOM|wxALL, bsize);
   
   // Show desired object type   
   if (objectType == "ImpulsiveBurn")
      systemParamBoxSizer->Hide(*spacecraftComboBox);
   else
      systemParamBoxSizer->Hide(*impBurnComboBox);
   
   systemParamBoxSizer->Add
      (propertyStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   systemParamBoxSizer->Add
      (*propertyListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   systemParamBoxSizer->Add
      (*coordSysBoxSizer, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   
   if (showVariable || showArray)
   {
      paramBoxSizer->Add(userParamBoxSizer, 0,
                         wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   }
   
   #ifdef __WXMAC__
   paramBoxSizer->Add(30, 20, 0, wxALIGN_CENTRE|wxALL, bsize);
   #endif
   
   paramBoxSizer->Add(systemParamBoxSizer, 0,
                      wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   
   return paramBoxSizer;
}


//------------------------------------------------------------------------------
// Wxboxsizer* Create3ColParameterSizer(...)
//------------------------------------------------------------------------------
/**
 * Creates new parameter sizer.
 */
//------------------------------------------------------------------------------
wxSizer* GuiItemManager::Create3ColParameterSizer
   (wxWindow *parent,
    wxCheckBox **entireObjCheckBox, wxWindowID entireObjCheckBoxId,
    wxComboBox **objectTypeComboBox, wxWindowID objectTypeComboBoxId, 
    wxListBox **objectListBox, wxWindowID objectListBoxId,
    wxStaticText **rowStaticText, wxWindowID rowStaticTextId,
    wxStaticText **colStaticText, wxWindowID colStaticTextId,
    wxTextCtrl **rowTextCtrl, wxWindowID rowTextCtrlId,
    wxTextCtrl **colTextCtrl, wxWindowID colTextCtrlId,
    wxListBox **propertyListBox, wxWindowID propertyListBoxId,
    wxComboBox **coordSysComboBox, wxWindowID coordSysComboBoxId,
    wxComboBox **originComboBox, wxWindowID originComboBoxId,
    wxStaticText **coordSysLabel, wxBoxSizer **coordSysBoxSizer,
    wxButton **upButton, wxWindowID upButtonId,
    wxButton **downButton, wxWindowID downButtonId,
    wxButton **addButton, wxWindowID addButtonId,
    wxButton **removeButton, wxWindowID removeButtonId,
    wxButton **addAllButton, wxWindowID addAllButtonId,
    wxButton **removeAllButton, wxWindowID removeAllButtonId,
    wxListBox **selectedListBox, wxWindowID selectedListBoxId,
    const wxArrayString &objectTypeList, int showOption,
    bool allowMultiSelect, bool showString, bool allowWholeObject,
    bool showSysParam, bool showVariable, bool showArray,
    const wxString &objectType)
{
   #if DEBUG_PARAM_SIZER
   MessageInterface::ShowMessage("GuiItemManager::Create3ColParameterSizer() entered\n");
   #endif
   
   int bsize = 1;
   
   //-----------------------------------------------------------------
   // Object type and list
   //-----------------------------------------------------------------
   wxStaticText *objectTypeStaticText =
      new wxStaticText(parent, -1, wxT("Object Type"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxStaticText *objectStaticText =
      new wxStaticText(parent, -1, wxT("Object List"),
                       wxDefaultPosition, wxDefaultSize, 0);   
   
   *entireObjCheckBox =
      new wxCheckBox(parent, entireObjCheckBoxId, wxT("Select Entire Object"));

   if (!allowWholeObject)
      (*entireObjCheckBox)->Disable();
   
   wxArrayString tmpObjTypeList;
   
   // add more object types to the list
   if (showSysParam)
      tmpObjTypeList = objectTypeList;
   
   if (showVariable)
      tmpObjTypeList.Add("Variable");
   if (showArray)
      tmpObjTypeList.Add("Array");
   if (showString)
      tmpObjTypeList.Add("String");
   
   *objectTypeComboBox =
      GetObjectTypeComboBox(parent, objectTypeComboBoxId, wxSize(170, 20),
                            tmpObjTypeList);
   
   // set default object type and get appropriate ListBox
   (*objectTypeComboBox)->SetValue(objectType);
   
   if (objectType == "Spacecraft")
   {
      // create Spacecraft ListBox
      *objectListBox =
         GetSpacecraftListBox(parent, objectListBoxId, wxSize(170, 163), NULL,
                              allowMultiSelect);
   }
   else if (objectType == "ImpulsiveBurn")
   {
      // create ImpulsiveBurn ListBox
      *objectListBox =
         GetImpBurnListBox(parent, objectListBoxId, wxSize(170, 163), NULL,
                           allowMultiSelect);
   }
   else
   {
      // create Variable ListBox
      *objectListBox =
         GetUserVariableListBox(parent, objectListBoxId, wxSize(170, 163), "",
                                allowMultiSelect);
      
      // set object type to Variable
      (*objectTypeComboBox)->SetValue("Variable");
   }
   
   // select first item
   //(*objectListBox)->SetSelection(0);
   
   //-----------------------------------------------------------------
   // Array row and column
   //-----------------------------------------------------------------
   *rowStaticText = new wxStaticText(parent, -1, wxT("Row [xx]"));
   *colStaticText = new wxStaticText(parent, -1, wxT("Col [xx]"));
   
   *rowTextCtrl =
      new wxTextCtrl(parent, -1, wxT("1"), wxDefaultPosition, wxSize(40, 20));
   
   *colTextCtrl =
      new wxTextCtrl(parent, -1, wxT("1"), wxDefaultPosition, wxSize(40, 20));
   
   //----- arrayIndexSizer
   wxFlexGridSizer *arrayIndexSizer = new wxFlexGridSizer(3);
   arrayIndexSizer->Add(*rowStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrayIndexSizer->Add(10, 10);
   arrayIndexSizer->Add(*colStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrayIndexSizer->Add(*rowTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrayIndexSizer->Add(10, 10);
   arrayIndexSizer->Add(*colTextCtrl, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //----- objectListSizer
   GmatStaticBoxSizer *objectListSizer =
      new GmatStaticBoxSizer(wxVERTICAL, parent, "");
   
   objectListSizer->Add(objectTypeStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objectListSizer->Add(*objectTypeComboBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objectListSizer->Add(10, 5);
   objectListSizer->Add(*entireObjCheckBox, 0, wxALIGN_CENTER|wxALL, bsize);
   objectListSizer->Add(10, 10);
   objectListSizer->Add(objectStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   objectListSizer->Add(*objectListBox, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   objectListSizer->Add(20, 2);
   objectListSizer->Add(arrayIndexSizer, 0, wxALIGN_CENTER|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Object properties
   //-----------------------------------------------------------------
   
   wxStaticText *propertyStaticText =
      new wxStaticText(parent, -1, wxT("Object Properties"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   *propertyListBox = 
      GetPropertyListBox(parent, propertyListBoxId, wxSize(170, 230), objectType,
                         showOption, allowMultiSelect);
   
   *coordSysLabel =
      new wxStaticText(parent, -1, wxT("Coordinate System"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   *coordSysComboBox =
      GetCoordSysComboBox(parent, coordSysComboBoxId, wxSize(170, 20));
   
   *originComboBox =
      GetConfigBodyComboBox(parent, originComboBoxId, wxSize(170, 20));
   
   //----- coordSysBoxSizer
   *coordSysBoxSizer = new wxBoxSizer(wxVERTICAL);   
   (*coordSysBoxSizer)->Add(*coordSysLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //----- propertySizer
   GmatStaticBoxSizer *propertySizer =
      new GmatStaticBoxSizer(wxVERTICAL, parent, "");
   
   propertySizer->Add(propertyStaticText, 0, wxALIGN_CENTRE|wxALL, bsize);
   propertySizer->Add(*propertyListBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   propertySizer->Add(*coordSysBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Arrows
   //-----------------------------------------------------------------
   wxSize buttonSize(25, 20);
   
   #ifdef __WXMAC__
   buttonSize.Set(40, 20);
   #endif
   
   *upButton = new wxButton
      (parent, addButtonId, wxT("UP"), wxDefaultPosition, buttonSize, 0);
   (*upButton)->SetToolTip("Move Up");
   if (!allowMultiSelect)
      (*upButton)->Disable();
   
   *downButton = new wxButton
      (parent, addButtonId, wxT("DN"), wxDefaultPosition, buttonSize, 0);
   (*downButton)->SetToolTip("Move Down");
   if (!allowMultiSelect)
      (*downButton)->Disable();
   
   *addButton = new wxButton
      (parent, addButtonId, wxT("->"), wxDefaultPosition, buttonSize, 0);
   (*addButton)->SetToolTip("Add Selected Item(s)");
   
   *removeButton = new wxButton
      (parent, removeButtonId, wxT("<-"), wxDefaultPosition, buttonSize, 0);
   (*removeButton)->SetToolTip("Remove Selected Item");
   
   *addAllButton = new wxButton
      (parent, removeAllButtonId, wxT("=>"), wxDefaultPosition, buttonSize, 0);
   (*addAllButton)->SetToolTip("Add All Items");
   if (!allowMultiSelect)
      (*addAllButton)->Disable();
   
   *removeAllButton = new wxButton
      (parent, removeAllButtonId, wxT("<="), wxDefaultPosition, buttonSize, 0);
   (*removeAllButton)->SetToolTip("Remove All Items");
   
   //----- arrowButtonsBoxSizer
   wxBoxSizer *arrowButtonsSizer = new wxBoxSizer(wxVERTICAL);
   arrowButtonsSizer->Add(*upButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(*downButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(10, 15);
   arrowButtonsSizer->Add(*addButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(*removeButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(10, 15);
   arrowButtonsSizer->Add(*addAllButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   arrowButtonsSizer->Add(*removeAllButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   //-----------------------------------------------------------------
   // Selected values
   //-----------------------------------------------------------------
   wxStaticText *selectedLabel =
      new wxStaticText(parent, -1, wxT("Selected Value(s)"),
                       wxDefaultPosition, wxDefaultSize, 0);
   
   wxArrayString emptyList;
   
   // make single selection so we can do move up and down
   *selectedListBox =
      new wxListBox(parent, -1, wxDefaultPosition, wxSize(200, 270),
                    emptyList, wxLB_SINGLE);
   
   //----- selectedSizer
   GmatStaticBoxSizer *selectedSizer =
      new GmatStaticBoxSizer(wxVERTICAL, parent, "");
   
   selectedSizer->Add(selectedLabel, 0, wxALIGN_CENTRE|wxALL, bsize);
   selectedSizer->Add(*selectedListBox, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   
   //-----------------------------------------------------------------
   // Sizer to return
   //-----------------------------------------------------------------
   //----- paramSizer
   wxFlexGridSizer *paramSizer = new wxFlexGridSizer(4);
   paramSizer->Add(objectListSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   paramSizer->Add(propertySizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   paramSizer->Add(arrowButtonsSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   paramSizer->Add(selectedSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   return paramSizer;
}


//------------------------------------------------------------------------------
// wxSizer* CreateUserVarSizer(...)
//------------------------------------------------------------------------------
/**
 * Creates parameter sizer.
 */
//------------------------------------------------------------------------------
wxSizer* GuiItemManager::CreateUserVarSizer
   (wxWindow *parent,
    wxListBox **userParamListBox, wxWindowID userParamListBoxId,
    wxButton **createVarButton, wxWindowID createVarButtonId,
    int showOption, bool showArray)
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::CreateUserVarSizer() entered\n");
   #endif
   
   int bsize = 1;
   
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
   
   if (showOption == SHOW_REPORTABLE)
   {
      *userParamListBox =
         GetAllUserParameterListBox(parent, userParamListBoxId, wxSize(170, 50), showArray);
   }
   else
   {
      *userParamListBox =
         GetUserVariableListBox(parent, userParamListBoxId, wxSize(170, 50), "");
   }
   
   //----- userParamBoxSizer
   wxStaticBoxSizer *userParamBoxSizer =
      new wxStaticBoxSizer(userParamStaticBox, wxVERTICAL);
   
   userParamBoxSizer->Add
      (userVarStaticText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   userParamBoxSizer->Add
      (*userParamListBox, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   userParamBoxSizer->Add
      (*createVarButton, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT|wxBOTTOM, bsize);
   
   
   //----- paramBoxSizer
   wxBoxSizer *paramSizer = new wxBoxSizer(wxVERTICAL);
   paramSizer->Add(userParamBoxSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   return paramSizer;
}

//-------------------------------
// priavate methods
//-------------------------------

//------------------------------------------------------------------------------
//  void UpdatePropertyList()
//------------------------------------------------------------------------------
/**
 * Updates available parameter list associated with Spacecraft and ImpulsiveBurn.
 *
 * @note Only plottable parameters (returning single value) are added to the list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdatePropertyList()
{
   
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage("GuiItemManager::UpdatePropertyList()\n");
   #endif
   
   StringArray items =
      theGuiInterpreter->GetListOfFactoryItems(Gmat::PARAMETER);
   int numParams = items.size();
   
   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropertyList() numParams=%d\n", numParams);
   #endif
   
   theNumScProperty = 0;
   //theNumImpBurnProperty = 0; //loj: 6/29/06 Until BurnParameter is defined
   theNumImpBurnProperty = 0;
   Gmat::ObjectType objectType;
   ParameterInfo *theParamInfo = ParameterInfo::Instance();
   
   for (int i=0; i<numParams; i++)
   {
      // skip CurrA1Mjd from the GUI - The parameter CurrA1MJD will be removed (loj: 5/3/06)
      if (items[i] == "CurrA1MJD")
         continue;
      
      // add only reportable parameters (Real, String for now) to list
      // we may want add Rvector6 later, then Rvec6Var needs to be set reportable true
      if (theParamInfo->IsReportable(items[i]))
      {
         objectType = theParamInfo->GetObjectType(items[i]);
         
         #if DBGLVL_GUI_ITEM_PROPERTY > 1
         MessageInterface::ShowMessage
            ("===> param name=%s, objectType=%d\n",  items[i].c_str(), objectType);
         #endif
         
         if (objectType == Gmat::SPACECRAFT)
         {
            // update Spacecraft property list
            if (theNumScProperty < MAX_SC_PROPERTY)
            {
               theScPropertyList[theNumScProperty] = items[i].c_str();
               theNumScProperty++;
            }
            else
            {
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "The number of Spacecraft properties exceeds "
                   "the maximum: %d", MAX_SC_PROPERTY);
            }
         }
         else if (objectType == Gmat::IMPULSIVE_BURN)
         {
            // update ImpulsiveBurn property list
            if (theNumImpBurnProperty < MAX_IB_PROPERTY)
            {
               theImpBurnPropertyList[theNumImpBurnProperty] = items[i].c_str();
               theNumImpBurnProperty++;
            }
            else
            {
               MessageInterface::PopupMessage
                  (Gmat::WARNING_, "The number of ImpulsiveBurn properties exceeds "
                   "the maximum: %d", MAX_IB_PROPERTY);
            }
         }
      }
   }

   #if DBGLVL_GUI_ITEM_PROPERTY
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdatePropertyList() theNumScProperty=%d, "
       "theNumImpBurnProperty=%d\n", theNumScProperty, theNumImpBurnProperty);
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
   #if DBGLVL_GUI_ITEM_PARAM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateParameterList() entered\ntheNumPlottableParam=%d, "
       "theNumSystemParam=%d, theNumUserVariable=%d\ntheNumUserString=%d, "
       "theNumUserArray=%d, theNumUserParam=%d\n", theNumPlottableParam,
       theNumSystemParam, theNumUserVariable, theNumUserString,
       theNumUserArray, theNumUserParam);
   #endif
   
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::PARAMETER);
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
      param = (Parameter*)theGuiInterpreter->GetConfiguredObject(items[i]);
      
      #if DBGLVL_GUI_ITEM_PARAM > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateParameterList() items[%d]=%s, type=%s\n",
          items[i].c_str(), param->GetTypeName().c_str());
      #endif
      
      // add if parameter plottable (returning single value)
      if (param->IsPlottable())
      {
         if (plottableParamCount < MAX_PLOT_PARAM)
         {
            thePlottableParamList[plottableParamCount] = items[i].c_str();
            plottableParamCount++;
         }
         
         // system Parameter (object property)
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            if (systemParamCount < MAX_PROPERTY)
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
               if (userArrayCount < MAX_USER_ARRAY &&
                   userParamCount < MAX_USER_PARAM)
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
                      "handle up to %d Variables.\n", param->GetName().c_str(),
                      MAX_USER_VAR);
               }
            }
         }
      }
      else // not plottable parameters
      {
         // user String
         if (param->GetTypeName() == "String")
         //if (param->GetReturnType() == Gmat::STRING_TYPE)
         {
            if (userArrayCount < MAX_USER_STRING &&
                userParamCount < MAX_USER_PARAM)
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
                   "handle up to %d Strings.\n", param->GetName().c_str(),
                   MAX_USER_STRING);
            }
         }
         // user Array
         else if (param->GetTypeName() == "Array")
         {
            if (userArrayCount < MAX_USER_ARRAY &&
                userParamCount < MAX_USER_PARAM)
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
                   "handle up to %d Arrays.\n", param->GetName().c_str(),
                   MAX_USER_ARRAY);
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

   #if DBGLVL_GUI_ITEM_PARAM
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateParameterList() exiting\ntheNumPlottableParam=%d, "
       "theNumSystemParam=%d, theNumUserVariable=%d\ntheNumUserString=%d, "
       "theNumUserArray=%d, theNumUserParam=%d\n", theNumPlottableParam,
       theNumSystemParam, theNumUserVariable, theNumUserString,
       theNumUserArray, theNumUserParam);
   #endif
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
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() entered. theNumSpacecraft=%d\n",
       theNumSpacecraft);
   #endif
   
   StringArray scList = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
   int numSc = scList.size();

   if (numSc > MAX_SPACECRAFT)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacecraftList() GUI can handle up to %d spacecraft."
          "The number of spacecraft configured: %d\n", MAX_SPACECRAFT, numSc);
      numSc = MAX_SPACECRAFT;
   }

   wxArrayString scNames;
   
   if (numSc > 0)  // check to see if any spacecrafts exist
   {
      for (int i=0; i<numSc; i++)
      {
         theSpacecraftList[i] = wxString(scList[i].c_str());
         scNames.Add(scList[i].c_str());
         
         #if DBGLVL_GUI_ITEM_SP
         MessageInterface::ShowMessage
            ("GuiItemManager::UpdateSpacecraftList() theSpacecraftList[%d]=%s\n",
             i, theSpacecraftList[i].c_str());
         #endif
      }
   }
   
   theNumSpacecraft = numSc;
   
   //-------------------------------------------------------
   // update registered Spacecraft ListBox
   //-------------------------------------------------------
   std::vector<wxArrayString*>::iterator exPos = mSpacecraftExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mSpacecraftLBList.begin();
        pos != mSpacecraftLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumSpacecraft; i++)
      {
         if (excList->Index(theSpacecraftList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theSpacecraftList[i]);
      }
      
      //(*pos)->SetSelection((*pos)->GetCount() - 1);
   }
   
   //-------------------------------------------------------
   // update registered Spacecraft ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mSpacecraftCBList.begin();
        pos != mSpacecraftCBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;
      
      int sel = (*pos)->GetSelection();
      
      (*pos)->Clear();
      (*pos)->Append(scNames);
      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Spacecraft CheckListBox
   //-------------------------------------------------------
   wxArrayString itemCheckedArray;
   for (std::vector<wxCheckListBox*>::iterator pos = mSpacecraftCLBList.begin();
        pos != mSpacecraftCLBList.end(); ++pos)
   {
      if ((*pos) == NULL)
         continue;

      itemCheckedArray.Clear();
      
      // save checked item
      int count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if ((*pos)->IsChecked(i))
            itemCheckedArray.Add((*pos)->GetString(i));
      
      (*pos)->Clear();
      (*pos)->Append(scNames);
      
      // restore checked item
      count = (*pos)->GetCount();
      for (int i=0; i<count; i++)
         if (itemCheckedArray.Index((*pos)->GetString(i)) != wxNOT_FOUND)
            (*pos)->Check(i);
   }
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacecraftList() exiting. theNumSpacecraft=%d\n",
       theNumSpacecraft);
   #endif
   
   
   AddToAllObjectList();
   
} // end UpdateSpacecraftList()


//------------------------------------------------------------------------------
//  void UpdateFormationList()
//------------------------------------------------------------------------------
/**
 * updates Formation list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFormationList()
{
   StringArray listForm = theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   int numForm = listForm.size();

   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateFormationList() numForm=%d\n", numForm);
   #endif
   
   if (numForm > MAX_FORMATION)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateFormationList() GUI can handle up to %d formations."
          "The number of formation configured: %d\n", MAX_FORMATION, numForm);
      numForm = MAX_FORMATION;
   }
   
   if (numForm > 0)  // check to see if any spacecrafts exist
   {
      for (int i=0; i<numForm; i++)
      {
         theFormationList[i] = wxString(listForm[i].c_str());
         #if DBGLVL_GUI_ITEM
         MessageInterface::ShowMessage
            ("GuiItemManager::UpdateFormationList() theFormationtList[%d]=%s\n",
             i, theFormationList[i].c_str());
         #endif
      }
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
   #if DBGLVL_GUI_ITEM_SO
   MessageInterface::ShowMessage("GuiItemManager::UpdateSpaceObjectList() entered\n");
   #endif
   
   StringArray scList = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
   StringArray fmList = theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);

   int numSc = scList.size();
   int numFm = fmList.size();
   int numObj = 0;
   int soCount = 0;
   
   #if DBGLVL_GUI_ITEM_SO > 1
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
            //Formation *fm = (Formation*)(theGuiInterpreter->GetSpacecraft(fmList[i]));
            GmatBase *fm = theGuiInterpreter->GetConfiguredObject(fmList[i]);
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
               if (soCount < MAX_SPACECRAFT)
               {
                  theSpaceObjectList[soCount] = wxString(result[i].c_str());
                  soCount++;
                  
                  #if DBGLVL_GUI_ITEM_SO > 1
                  MessageInterface::ShowMessage
                     ("theSpaceObjectList[%d]=%s\n", soCount-1,
                      theSpaceObjectList[soCount-1].c_str());
                  #endif
               }
            }
         }
         
         //loj: 7/14/05
         // Do not add Formation to theSpaceObjectList until it can handle not including
         // each other; e.g) form1 includes form2 and form2 includes form1.
         //loj: 7/14/05 Added formation option in the GetSpaceObjectListBox()
         
//          //------------------------------------------
//          // Add formation to theSpaceObjectList
//          //------------------------------------------
//          for (int i=0; i<numFm; i++)
//          {
//             if (soCount < MAX_SPACECRAFT)
//             {
//                theSpaceObjectList[soCount] = wxString(fmList[i].c_str());
//                soCount++;
               
//                #if DBGLVL_GUI_ITEM_SO > 1
//                MessageInterface::ShowMessage
//                   ("theSpaceObjectList[%d]=%s\n", soCount-1,
//                    theSpaceObjectList[soCount-1].c_str());
//                #endif
//             }
//          }
      }
      // no formation, Save scList to theSpaceObjectList
      else
      {
         soCount = numSc;
         for (int i=0; i<soCount; i++)
         {
            theSpaceObjectList[i] = wxString(scList[i].c_str());
            
            #if DBGLVL_GUI_ITEM_SO > 1
            MessageInterface::ShowMessage
               ("theSpaceObjectList[%d]=%s\n", i, theSpaceObjectList[i].c_str());
            #endif
         }
      }
   }
   
   theNumSpaceObject = soCount;
   
   //-------------------------------------------------------
   // update registered SpaceObjectListBox
   //-------------------------------------------------------
   std::vector<wxArrayString*>::iterator exPos = mSpaceObjectExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mSpaceObjectLBList.begin();
        pos != mSpaceObjectLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumSpaceObject; i++)
      {
         if (excList->Index(theSpaceObjectList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theSpaceObjectList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);

   }
   
   #if DBGLVL_GUI_ITEM_SO
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
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage("GuiItemManager::UpdateCelestialBodyList() entered\n");
   #endif
      
   StringArray items = theGuiInterpreter->GetListOfObjects(Gmat::CELESTIAL_BODY);
   theNumCelesBody = items.size();
   
   if (theNumCelesBody > MAX_CELES_BODY)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCelestialBodyList() GUI can handle up to %d bodies."
          "The number of bodies configured: %d\n", MAX_CELES_BODY, theNumCelesBody);
      theNumCelesBody = MAX_CELES_BODY;
   }
   
   for (int i=0; i<theNumCelesBody; i++)
   {
      theCelesBodyList[i] = items[i].c_str();
        
      #if DBGLVL_GUI_ITEM > 1
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
      theGuiInterpreter->GetListOfObjects(Gmat::CELESTIAL_BODY);
   StringArray calPointList =
      theGuiInterpreter->GetListOfObjects(Gmat::CALCULATED_POINT);
   
   theNumCelesBody = celesBodyList.size();
   theNumCalPoint = calPointList.size();
   theNumCelesPoint = theNumCelesBody + theNumCalPoint;
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateCelestialPointList() theNumCelesBody=%d, "
       "theNumCalPoint=%d, theNumCelesPoint=%d\n", theNumCelesBody, theNumCalPoint,
       theNumCelesPoint);
   #endif
   
   if (theNumCelesPoint > MAX_CELES_POINT)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCelestialPointList() GUI can handle up to %d bodies."
          "The number of bodies configured: %d\n", MAX_CELES_POINT,
          theNumCelesPoint);
      theNumCelesPoint = MAX_CELES_POINT;
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
   StringArray spList = theGuiInterpreter->GetListOfObjects(Gmat::SPACE_POINT);
   theNumSpacePoint = spList.size();
   
   #if DBGLVL_GUI_ITEM_SP
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSpacePointList() theNumSpacePoint=%d\n", theNumSpacePoint);
   #endif
   
   if (theNumSpacePoint > MAX_SPACE_POINT)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacePointList() GUI can handle up to %d SpacePoints."
          "The number of SpacePoints configured: %d\n", MAX_SPACE_POINT,
          theNumSpacePoint);
      theNumSpacePoint = MAX_SPACE_POINT;
   }
   
   wxArrayString spNames;
   
   for (int i=0; i<theNumSpacePoint; i++)
   {
      theSpacePointList[i] = spList[i].c_str();
      spNames.Add(spList[i].c_str());
      
      #if DBGLVL_GUI_ITEM_SP > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSpacePointList() theSpacePointList[%d]=%s\n",
          i, theSpacePointList[i].c_str());
      #endif
   }
   
   //-------------------------------------------------------
   // update registered SpacePoint ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mSpacePointCBList.begin();
        pos != mSpacePointCBList.end(); ++pos)
   {
      wxString str = (*pos)->GetStringSelection();
      
      if ((*pos)->FindString("Vector") == wxNOT_FOUND)
      {
         (*pos)->Clear();
      }
      else
      {
         (*pos)->Clear();
         (*pos)->Append("Vector");
      }
      
      (*pos)->Append(spNames);
      
      (*pos)->SetStringSelection(str);
   }
}


//------------------------------------------------------------------------------
// void UpdateBurnList()
//------------------------------------------------------------------------------
/**
 * Updates confugured burn list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateBurnList()
{
   StringArray items = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
   int numBurn = items.size();
   
   #if DBGLVL_GUI_ITEM_BURN
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateBurnList() numBurn=%d\n", numBurn);
   #endif
   
   if (numBurn > MAX_BURN)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateBurnList() GUI can handle up to %d burns."
          "The number of burns configured: %d\n", MAX_BURN, numBurn);
      numBurn = MAX_BURN;
   }
   
   wxArrayString impBurnNames;
   wxArrayString finiteBurnNames;
   theNumImpBurn = 0;
   theNumFiniteBurn = 0;
   GmatBase *obj;
   
   for (int i=0; i<numBurn; i++)
   {
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->GetTypeName() == "ImpulsiveBurn")
      {
         theImpBurnList[theNumImpBurn] = items[i].c_str();
         impBurnNames.Add(items[i].c_str());
         theNumImpBurn++;
         #if DBGLVL_GUI_ITEM_BURN > 1
         MessageInterface::ShowMessage
            ("   adding to theImpBurnList: %s\n", theImpBurnList[i].c_str());
         #endif
      }
      else if (obj->GetTypeName() == "FiniteBurn")
      {
         theFiniteBurnList[theNumFiniteBurn] = items[i].c_str();
         finiteBurnNames.Add(items[i].c_str());
         theNumFiniteBurn++;
         #if DBGLVL_GUI_ITEM_BURN > 1
         MessageInterface::ShowMessage
            ("   adding to theFiniteBurnList: %s\n", theFiniteBurnList[i].c_str());
         #endif
      }
   }
   
   //-------------------------------------------------------
   // update registered ImpulsiveBurn ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mImpBurnCBList.begin();
        pos != mImpBurnCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      
      (*pos)->Clear();
      (*pos)->Append(impBurnNames);
      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered FiniteBurn ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mFiniteBurnCBList.begin();
        pos != mFiniteBurnCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();

      (*pos)->Clear();
      (*pos)->Append(finiteBurnNames);
      
      (*pos)->SetSelection(sel);
   }


   AddToAllObjectList();
}


//------------------------------------------------------------------------------
// void UpdateCoordSystemList()
//------------------------------------------------------------------------------
/**
 * Updates confugured coordinate system list
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateCoordSystemList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   theNumCoordSys = items.size();
   
   #if DBGLVL_GUI_ITEM_CS
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateCoordSystemList() theNumCoordSys=%d\n",
       theNumCoordSys);
   #endif
   
   if (theNumCoordSys > MAX_COORD_SYS)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateCoordSystemList() GUI can handle up to %d coord. sys."
          "The number of coord. sys. configured: %d\n", MAX_COORD_SYS, theNumCoordSys);
      theNumCoordSys = MAX_COORD_SYS;
   }

   wxArrayString coordSysNames;
   
   for (int i=0; i<theNumCoordSys; i++)
   {
      theCoordSysList[i] = items[i].c_str();
      coordSysNames.Add(items[i].c_str());

      #if DBGLVL_GUI_ITEM_CS > 1
      MessageInterface::ShowMessage("GuiItemManager::UpdateCoordSystemList() " +
                                    std::string(theCoordSysList[i].c_str()) + "\n");
      #endif
   }

   
   //-------------------------------------------------------
   // update registered CoordinateSystem ComboBox
   //-------------------------------------------------------
   int sel;
   
   for (std::vector<wxComboBox*>::iterator pos = mCoordSysCBList.begin();
        pos != mCoordSysCBList.end(); ++pos)
   {
      // How can I catch bad pointer?
      
      if ((*pos)->GetParent() != NULL)
      {
         sel = (*pos)->GetSelection();
         
         (*pos)->Clear();
         (*pos)->Append(coordSysNames);
         
         (*pos)->SetSelection(sel);
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateHardwareList()
//------------------------------------------------------------------------------
/**
 * Updates configured hardware list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateHardwareList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::HARDWARE);
   int numHardware = items.size();
   
   #if DBGLVL_GUI_ITEM_HW
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateHardwareList() numHardware=%d\n", numHardware);
   #endif
   
   if (numHardware > MAX_HARDWARE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateHardwareList() GUI can handle up to %d FuelTank."
          "The number of FuelTank configured: %d\n", MAX_HARDWARE, theNumFuelTank);
      numHardware = MAX_HARDWARE;
   }

   theNumFuelTank = 0;
   theNumThruster = 0;
   GmatBase *hw;
   wxArrayString tankNames;
   wxArrayString thrusterNames;
   
   for (int i=0; i<numHardware; i++)
   {
      hw = theGuiInterpreter->GetConfiguredObject(items[i]);
      
      if (hw->IsOfType(Gmat::FUEL_TANK))
      {
         theFuelTankList[theNumFuelTank++] = items[i].c_str();
         tankNames.Add(items[i].c_str());
      }
      else if (hw->IsOfType(Gmat::THRUSTER))
      {
         theThrusterList[theNumThruster++] = items[i].c_str();
         thrusterNames.Add(items[i].c_str());
      }
      
      #if DBGLVL_GUI_ITEM_HW > 1
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateHardwareList() " + items[i] + "\n");
      #endif
   }
   
   //-------------------------------------------------------
   // update registered FuelTank ListBox
   //-------------------------------------------------------   
   // It's ok to have the same FuelTank in more than one spacecraft since
   // the Sandbox will clone it.
   std::vector<wxArrayString*>::iterator exPos = mFuelTankExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mFuelTankLBList.begin();
        pos != mFuelTankLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumFuelTank; i++)
      {
         if (excList->Index(theFuelTankList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theFuelTankList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);
   }
   
   //-------------------------------------------------------
   // update registered Thruster ListBox
   //-------------------------------------------------------
   // It's ok to have the same Thruster in more than one spacecraft since
   // the Sandbox will clone it.
   exPos = mThrusterExcList.begin();
   
   for (std::vector<wxListBox*>::iterator pos = mThrusterLBList.begin();
        pos != mThrusterLBList.end(); ++pos)
   {
      wxArrayString *excList = *exPos++;
      (*pos)->Clear();
      
      for (int i=0; i<theNumThruster; i++)
      {
         if (excList->Index(theThrusterList[i].c_str()) == wxNOT_FOUND)
            (*pos)->Append(theThrusterList[i]);
      }
      
      (*pos)->SetSelection((*pos)->GetCount() - 1);
   }
   
   //-------------------------------------------------------
   // update registered FuelTank ComboBox
   //-------------------------------------------------------
   
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mFuelTankCBList.begin();
        pos != mFuelTankCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      
      (*pos)->Clear();
      (*pos)->Append(tankNames);
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Thruster ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mThrusterCBList.begin();
        pos != mThrusterCBList.end(); ++pos)
   {
      sel = (*pos)->GetSelection();
      
      (*pos)->Clear();
      (*pos)->Append(thrusterNames);
      (*pos)->SetSelection(sel);
   }
   
   AddToAllObjectList();
   
} // end UpdateHardwareList()


//------------------------------------------------------------------------------
// void UpdateFunctionList()
//------------------------------------------------------------------------------
/**
 * Updates configured Function list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateFunctionList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::FUNCTION);
   theNumFunction = items.size();
   
   #if DBGLVL_GUI_ITEM_FN
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateFunctionList() theNumFunction=%d\n", theNumFunction);
   #endif
   
   if (theNumFunction > MAX_FUNCTION)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateFunctionList() GUI can handle up to %d Function."
          "The number of Function configured: %d\n", MAX_FUNCTION, theNumFunction);
      theNumFunction = MAX_FUNCTION;
   }
   
   wxArrayString functionNames;
   
   for (int i=0; i<theNumFunction; i++)
   {
      theFunctionList[i] = items[i].c_str();
      functionNames.Add(items[i].c_str());

      #if DBGLVL_GUI_ITEM_FN > 1
      MessageInterface::ShowMessage("GuiItemManager::UpdateFunctionList() " +
                                    std::string(theFunctionList[i].c_str()) + "\n");
      #endif
   }
   
   //-------------------------------------------------------
   // update registered Function ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mFunctionCBList.begin();
        pos != mFunctionCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();

      (*pos)->Clear();
      (*pos)->Append(functionNames);
      
      (*pos)->SetSelection(sel);
   }
   
   AddToAllObjectList();
}


//------------------------------------------------------------------------------
// void UpdateSubscriberList()
//------------------------------------------------------------------------------
/**
 * Updates configured Subscriber list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSubscriberList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   theNumSubscriber = items.size();
   theNumReportFile = 0;
   
   #if DBGLVL_GUI_ITEM_SUBS
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSubscriberList() theNumSubscriber=%d\n", theNumSubscriber);
   #endif
   
   if (theNumSubscriber > MAX_SUBSCRIBER)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSubscriberList() GUI can handle up to %d Subscriber."
          "The number of Subscriber configured: %d\n", MAX_SUBSCRIBER, theNumSubscriber);
      theNumSubscriber = MAX_SUBSCRIBER;
   }
   
   wxArrayString subsNames;
   wxArrayString rfNames;
   GmatBase *obj;

   // Update Subscriber list
   for (int i=0; i<theNumSubscriber; i++)
   {
      theSubscriberList[i] = items[i].c_str();
      subsNames.Add(items[i].c_str());

      #if DBGLVL_GUI_ITEM_SUBS > 1
      MessageInterface::ShowMessage("GuiItemManager::UpdateSubscriberList() " +
                                    std::string(theSubscriberList[i].c_str()) + "\n");
      #endif
   }

   int numReportFile = 0;
   
   // Update ReportFile list
   for (int i=0; i<theNumSubscriber; i++)
   {
      // check for ReportFile
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->GetTypeName() == "ReportFile")
      {
         if (i < MAX_REPORT_FILE)
         {
            theReportFileList[theNumReportFile] = items[i].c_str();
            rfNames.Add(items[i].c_str());
            theNumReportFile++;
         }
         
         numReportFile++;
      }
   }
   
   if (numReportFile > MAX_REPORT_FILE)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSubscriberList() GUI can handle up to %d ReportFile."
          "The number of ReportFile configured: %d\n", MAX_REPORT_FILE, numReportFile);
   }
   
   
   //-------------------------------------------------------
   // update registered Subscriber ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mSubscriberCBList.begin();
        pos != mSubscriberCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();

      (*pos)->Clear();
      (*pos)->Append(subsNames);
      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered ReportFile ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mReportFileCBList.begin();
        pos != mReportFileCBList.end(); ++pos)
   {      
       sel = (*pos)->GetSelection();

      (*pos)->Clear();
      (*pos)->Append(rfNames);
      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Subscriber CheckListBox
   //-------------------------------------------------------
   for (std::vector<wxCheckListBox*>::iterator pos = mSubscriberCLBList.begin();
        pos != mSubscriberCLBList.end(); ++pos)
   {
      int guiCount = (*pos)->GetCount();
      bool found = false;
      wxString item;
      
      // if deleted item remove from the list
      for (int i=0; i<guiCount; i++)
      {
         found = false;
         for (int j=0; j<theNumSubscriber; j++)
         {
            item = (*pos)->GetString(i);
            if (item == theSubscriberList[j])
            {
               found = true;
               break;
            }
         }
         
         if (!found)
         {
            (*pos)->Delete(i);
            guiCount--;
         }
      }
      
      // if new item add to the list
      for (int i=0; i<theNumSubscriber; i++)
         if ((*pos)->FindString(theSubscriberList[i]) == wxNOT_FOUND)
            (*pos)->Append(theSubscriberList[i]);
      
   }
   
   AddToAllObjectList();
}


//------------------------------------------------------------------------------
// void UpdateSolverList()
//------------------------------------------------------------------------------
/**
 * Updates configured Suolver list.
 */
//------------------------------------------------------------------------------
void GuiItemManager::UpdateSolverList()
{
   StringArray items =
      theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   
   theNumSolver = items.size();
   theNumBoundarySolver = 0;
   theNumOptimizer = 0;
   
   #if DBGLVL_GUI_ITEM_SOLVER
   MessageInterface::ShowMessage
      ("GuiItemManager::UpdateSolverList() theNumSolver=%d\n", theNumSolver);
   #endif
   
   if (theNumSolver > MAX_SOLVER)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSolverList() GUI can handle up to %d Solver."
          "The number of Solvers configured: %d\n", MAX_SOLVER, theNumSolver);
      theNumSolver = MAX_SOLVER;
   }
   
   wxArrayString solverNames;
   wxArrayString bvsNames;
   wxArrayString optNames;
   GmatBase *obj;
   
   //-------------------------------------------------------
   // Update Solver list
   //-------------------------------------------------------
   for (int i=0; i<theNumSolver; i++)
   {
      theSolverList[i] = items[i].c_str();
      solverNames.Add(items[i].c_str());
      
      #if DBGLVL_GUI_ITEM_SOLVER > 1
      MessageInterface::ShowMessage
         ("     theSolverList[%d]=%s\n", theSolverList[i].c_str());
      #endif
   }
   
   //-------------------------------------------------------
   // Update Boundary Value Solver list
   //-------------------------------------------------------
   int numBoundarySolver = 0;
   
   for (int i=0; i<theNumSolver; i++)
   {
      // check for Optimizer
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->GetTypeName() == "DifferentialCorrector")
      {
         if (i < MAX_BOUNDARY_SOLVER)
         {
            #if DBGLVL_GUI_ITEM_SOLVER
            MessageInterface::ShowMessage
               ("   Adding %s to theBoundarySolverList\n", items[i].c_str());
            #endif
            
            theBoundarySolverList[theNumBoundarySolver] = items[i].c_str();
            bvsNames.Add(items[i].c_str());
            theNumBoundarySolver++;
         }
         
         numBoundarySolver++;
      }
   }
   
   if (numBoundarySolver > MAX_BOUNDARY_SOLVER)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSolverList() GUI can handle up to %d Boundary "
          "Value Solver. The number of Boundary Value Solver configured: %d\n",
          MAX_BOUNDARY_SOLVER, numBoundarySolver);
   }
   
   //-------------------------------------------------------
   // Update Optimizer list
   //-------------------------------------------------------
   int numOptimizer = 0;
   
   for (int i=0; i<theNumSolver; i++)
   {
      // check for Optimizer
      obj = theGuiInterpreter->GetConfiguredObject(items[i]);
      if (obj->GetTypeName() == "FminconOptimizer")
      {
         if (i < MAX_OPTIMIZER)
         {
            #if DBGLVL_GUI_ITEM_SOLVER
            MessageInterface::ShowMessage
               ("   Adding %s to theOptimizerList\n", items[i].c_str());
            #endif
            
            theOptimizerList[theNumOptimizer] = items[i].c_str();
            optNames.Add(items[i].c_str());
            theNumOptimizer++;
         }
         
         numOptimizer++;
      }
   }
   
   if (numOptimizer > MAX_OPTIMIZER)
   {
      MessageInterface::ShowMessage
         ("GuiItemManager::UpdateSolverList() GUI can handle up to %d Optimizer."
          "The number of Optimizer configured: %d\n", MAX_OPTIMIZER, numOptimizer);
   }
   
   
   //-------------------------------------------------------
   // update registered Solver ComboBox
   //-------------------------------------------------------
   int sel;
   for (std::vector<wxComboBox*>::iterator pos = mSolverCBList.begin();
        pos != mSolverCBList.end(); ++pos)
   {
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(solverNames);
      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Boundary Solver ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mBoundarySolverCBList.begin();
        pos != mBoundarySolverCBList.end(); ++pos)
   {
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(bvsNames);
      
      (*pos)->SetSelection(sel);
   }
   
   //-------------------------------------------------------
   // update registered Optimizer ComboBox
   //-------------------------------------------------------
   for (std::vector<wxComboBox*>::iterator pos = mOptimizerCBList.begin();
        pos != mOptimizerCBList.end(); ++pos)
   {
       sel = (*pos)->GetSelection();
       
      (*pos)->Clear();
      (*pos)->Append(optNames);
      
      (*pos)->SetSelection(sel);
   }
   
   AddToAllObjectList();
}


//------------------------------------------------------------------------------
// void AddToAllObjectList()
//------------------------------------------------------------------------------
/**
 * Currently it adds the following objects:
 *    Spacecraft, ImpulsiveBurn, FiniteBurn objects.
 *
 * If more object types are added, make sure to increase MAX_ALL_OBJECT and
 * add to call AddToAllObjectList() in the Update*List().
 */
//------------------------------------------------------------------------------
void GuiItemManager::AddToAllObjectList()
{
   theNumAllObject = 0;
   
   // Add Spacecraft object to the list
   for (int i=0; i<theNumSpacecraft; i++)
   {
      theAllObjectList[theNumAllObject] = theSpacecraftList[i];
      theNumAllObject++;
   }
   
   // Add FuelTank object to the list
   for (int i=0; i<theNumFuelTank; i++)
   {
      theAllObjectList[theNumAllObject] = theFuelTankList[i];
      theNumAllObject++;
   }
   
   // Add Thruster object to the list
   for (int i=0; i<theNumThruster; i++)
   {
      theAllObjectList[theNumAllObject] = theThrusterList[i];
      theNumAllObject++;
   }
   
   // Add ImpulsiveBurn objects to the list
   for (int i=0; i<theNumImpBurn; i++)
   {
      theAllObjectList[theNumAllObject] = theImpBurnList[i];
      theNumAllObject++;      
   }
   
   // Add FiniteBurn objects to the list
   for (int i=0; i<theNumFiniteBurn; i++)
   {
      theAllObjectList[theNumAllObject] = theFiniteBurnList[i];
      theNumAllObject++;      
   }
   
   // Add Solver objects to the list
   for (int i=0; i<theNumSolver; i++)
   {
      theAllObjectList[theNumAllObject] = theSolverList[i];
      theNumAllObject++;      
   }
   
   // Add Subscriber objects to the list
   for (int i=0; i<theNumSubscriber; i++)
   {
      theAllObjectList[theNumAllObject] = theSubscriberList[i];
      theNumAllObject++;      
   }   
   
   // Add Function objects to the list
   for (int i=0; i<theNumFunction; i++)
   {
      theAllObjectList[theNumAllObject] = theFunctionList[i];
      theNumAllObject++;      
   }
   
   
   #if DBGLVL_GUI_ITEM_ALL_OBJECT
   MessageInterface::ShowMessage
      ("GuiItemManager::AddToAllObjectList()\n   "
       "theNumSpacecraft = %d\n   theNumFuelTank   = %d\n   "
       "theNumThruster   = %d\n   theNumImpBurn    = %d\n   "
       "theNumFiniteBurn = %d\n   theNumSolver     = %d\n   "
       "theNumAllObject  = %d\n", theNumSpacecraft, theNumFuelTank,
       theNumThruster, theNumImpBurn, theNumFiniteBurn, theNumSolver,
       theNumAllObject);
   #endif
   
   //-------------------------------------------------------
   // update registered All Object CheckListBox
   //-------------------------------------------------------
   for (std::vector<wxCheckListBox*>::iterator pos = mAllObjectCLBList.begin();
        pos != mAllObjectCLBList.end(); ++pos)
   {
      int guiCount = (*pos)->GetCount();
      bool found = false;
      wxString item;
      
      // if deleted item remove from the list
      for (int i=0; i<guiCount; i++)
      {
         found = false;
         for (int j=0; j<theNumAllObject; j++)
         {
            item = (*pos)->GetString(i);
            if (item == theAllObjectList[j])
            {
               found = true;
               break;
            }
         }
         
         if (!found)
         {
            (*pos)->Delete(i);
            guiCount--;
         }
      }
      
      // if new item add to the list
      for (int i=0; i<theNumAllObject; i++)
         if ((*pos)->FindString(theAllObjectList[i]) == wxNOT_FOUND)
            (*pos)->Append(theAllObjectList[i]);
      
   }
}


//------------------------------------------------------------------------------
// GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::GuiItemManager()
{
   #if DBGLVL_GUI_ITEM
   MessageInterface::ShowMessage("GuiItemManager::GuiItemManager() entered\n");
   #endif
   
   theDataPrecision = GmatGlobal::Instance()->GetDataPrecision();
   
   theGuiInterpreter = GmatAppData::GetGuiInterpreter();
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   
   theNumScProperty = 0;
   theNumImpBurnProperty = 0;
   theNumFiniteBurnProperty = 0;
   theNumSpaceObject = 0;
   theNumFormation = 0;
   theNumSpacecraft = 0;
   theNumImpBurn = 0;
   theNumFiniteBurn = 0;
   theNumCoordSys = 0;
   theNumFunction = 0;
   theNumFuelTank = 0;
   theNumThruster = 0;
   theNumPlottableParam = 0;
   theNumSystemParam = 0;
   theNumUserVariable = 0;
   theNumUserString = 0;
   theNumUserArray = 0;
   theNumUserParam = 0;
   theNumCelesBody = 0;
   theNumCelesPoint = 0;
   theNumCalPoint = 0;
   theNumSubscriber = 0;
   theNumSolver = 0;
   theNumBoundarySolver = 0;
   theNumOptimizer = 0;
   theNumSpacePoint = 0;
      
   // update property list
   UpdatePropertyList();
}


//------------------------------------------------------------------------------
// ~GuiItemManager()
//------------------------------------------------------------------------------
GuiItemManager::~GuiItemManager()
{
}
