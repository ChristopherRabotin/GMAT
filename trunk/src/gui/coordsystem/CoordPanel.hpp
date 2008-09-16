//$Header$
//------------------------------------------------------------------------------
//                              CoordPanel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Waka Waktola
// Created: 2005/03/11
/**
 * This class contains the Coordinate System Panel for CoordSystemConfigPanel
 * and CoordSysCreateDialog.
 */
//------------------------------------------------------------------------------
#ifndef CoordPanel_hpp
#define CoordPanel_hpp

// gui includes
#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "CoordinateSystem.hpp"

class CoordPanel : public wxPanel
{
public:
    // constructors
    CoordPanel(wxWindow *parent, bool enableAll);
    ~CoordPanel();
    
   wxString epochValue;
   wxString epochFormatValue;
   
   wxComboBox *GetOriginComboBox() {return originComboBox;}
   wxComboBox *GetTypeComboBox() {return typeComboBox;}
   wxComboBox *GetPrimaryComboBox() {return primaryComboBox;}
   wxComboBox *GetSecondaryComboBox() {return secondaryComboBox;}
   wxComboBox *GetFormatComboBox() {return formatComboBox;}
   
   wxComboBox *GetXComboBox() {return xComboBox;}
   wxComboBox *GetYComboBox() {return yComboBox;}
   wxComboBox *GetZComboBox() {return zComboBox;}
   
   bool GetShowPrimaryBody() {return mShowPrimaryBody;}
   bool GetShowSecondaryBody() {return mShowSecondaryBody;}
   bool GetShowEpoch() {return mShowEpoch;}
   bool GetShowXyz() {return mShowXyz;}
   bool GetShowUpdateInterval() {return mShowUpdate;}
   
   wxTextCtrl *GetEpochTextCtrl() {return epochTextCtrl;}
   wxTextCtrl *GetIntervalTextCtrl() {return intervalTextCtrl;}
   
   void EnableOptions(AxisSystem *axis = NULL);
   void SetDefaultAxis();
   void SetDefaultEpochRefAxis();
   void SetDefaultObjectRefAxis();
   void ShowAxisData(AxisSystem *axis);
   AxisSystem* CreateAxis();
   void ChangeEpoch(wxString &oldFormat);
   bool SaveData(const std::string &coordName, AxisSystem *axis,
                 wxString &epochFormat);

private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   
   bool mShowPrimaryBody;
   bool mShowSecondaryBody;
   bool mShowEpoch;
   bool mShowXyz;
   bool mShowUpdate;
   bool mEnableAll;
   
   wxStaticText *originStaticText;
   wxStaticText *typeStaticText;
   wxStaticText *primaryStaticText;
   wxStaticText *formatStaticText;
   wxStaticText *secondaryStaticText;
   wxStaticText *epochStaticText;

   wxStaticText *xStaticText;
   wxStaticText *yStaticText;
   wxStaticText *zStaticText;

   wxStaticText *updateStaticText;
   wxStaticText *secStaticText;

   wxTextCtrl *epochTextCtrl;
   wxTextCtrl *intervalTextCtrl;

   wxComboBox *originComboBox;
   wxComboBox *typeComboBox;
   wxComboBox *primaryComboBox;
   wxComboBox *formatComboBox;
   wxComboBox *secondaryComboBox;

   wxComboBox *xComboBox;
   wxComboBox *yComboBox;
   wxComboBox *zComboBox;

   void Create();
   void LoadData();
   
   // Layout & data handling methods
   void Setup(wxWindow *parent);
   
   bool IsValidAxis(const wxString &axisType, const wxString &priName,
                    const wxString &secName, const wxString &x,
                    const wxString &y, const wxString &z);
   bool IsValidXYZ(const wxString &x, const wxString &y, const wxString &z);
   
   // IDs for the controls and the menu commands
   enum
   {
       ID_TEXTCTRL = 46000,
       ID_COMBO,
       ID_TEXT,
   };
};

#endif // CoordPanel_hpp
