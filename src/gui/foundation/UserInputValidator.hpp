//$Id$
//------------------------------------------------------------------------------
//                              UserInputValidator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun
// Created: 2004/02/02
//
/** Validates user input entered via the GUI.
 * Declares UserInputValidator class.
 */
//------------------------------------------------------------------------------

#ifndef UserInputValidator_hpp
#define UserInputValidator_hpp

#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatBase.hpp"

class UserInputValidator
{
public:
   
   // constructors
   UserInputValidator();
   ~UserInputValidator();
   
   void SetObject(GmatBase *obj);
   void SetGuiManager(GuiItemManager *manager);
   void SetWindow(wxWindow *window);
   bool IsInputValid();
   
   bool IsValidName(const wxString &name);
   bool CheckFileName(const std::string &str, const std::string &field,
                      bool onlyMsg = false);
   
   bool CheckReal(Real &rvalue, const std::string &str,
                  const std::string &field, const std::string &expRange,
                  bool onlyMsg = false, bool checkRange = false, 
                  bool positive = false, bool zeroOk = false);
   
   bool CheckInteger(Integer &ivalue, const std::string &str,
                     const std::string &field, const std::string &expRange,
                     bool onlyMsg = false, bool checkRange = false,
                     bool positive = false, bool zeroOk = false);
   
   bool CheckIntegerRange(Integer &ivalue, const std::string &str, 
                          const std::string &field,
                          Integer lower, Integer upper,
                          bool checkLower = true, bool checkUpper = true,
                          bool includeLower = false,
                          bool includeUpper = false);
   
   bool CheckVariable(const std::string &varName, ObjectTypeArray ownerTypes,
                      const std::string &field, const std::string &expRange,
                      bool allowNumber = true, bool allowNonPlottable = false,
                      bool allowObjectProperty = false, bool allowWholeArray = false);
   
   bool CheckRealRange(const std::string &sValue, Real value, const std::string &field,
                       Real lower, Real upper,
                       bool checkLower = true, bool checkUpper = true,
                       bool includeLower = false,
                       bool includeUpper = false, bool isInteger = false);
   
   bool CheckTimeFormatAndValue(const std::string &format, const std::string& value,
                                const std::string &field,  bool checkRange = false);

   wxArrayString ToWxArrayString(const StringArray &array);
   wxString ToWxString(const wxArrayString &names);
   wxString ToWxString(const std::string &stdStr);
   wxString ToWxString(Real realVal);
   wxString ToWxString(Integer intVal);
   Real     ToReal(const wxString &wxStr);
   Integer  ToInteger(const wxString &wxStr);
   
protected:
   static std::string lessOrEq;
   static std::string lessThan;
   static std::string moreOrEq;
   static std::string moreThan;
   
   GmatBase       *mObject;
   GuiItemManager *mGuiManager;
   wxWindow       *mWindow;
   bool           mIsInputValid;;
   std::string    mObjectName;
   std::string    mMsgFormat;
   
   void SetErrorFlag();
   
};

#endif // UserInputValidator_hpp
