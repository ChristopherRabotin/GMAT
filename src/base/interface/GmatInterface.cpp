//$Header$
//------------------------------------------------------------------------------
//                              GmatInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/8/30
//
/**
 * Implements class to provide sending scripts from MATLAB to GMAT.
 */
//------------------------------------------------------------------------------

#include "GmatInterface.hpp"
#include "Moderator.hpp"         // for Instance()
#include "MessageInterface.hpp"

GmatInterface* GmatInterface::instance = NULL;

//#define DEBUG_GMAT_INTERFACE 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// static Instance()
//------------------------------------------------------------------------------
GmatInterface* GmatInterface::Instance()
{
   if (instance == NULL)
      instance = new GmatInterface();
   return instance;
}


//------------------------------------------------------------------------------
// void OpenScript()
//------------------------------------------------------------------------------
void GmatInterface::OpenScript()
{
   if (!mInStringStream)
      mInStringStream = new std::istringstream;

}


//------------------------------------------------------------------------------
// void ClearScrip()
//------------------------------------------------------------------------------
void GmatInterface::ClearScript()
{
   mStringStream.str("");
   Moderator::GetGuiInterpreter()->CloseCurrentProject();
}


//------------------------------------------------------------------------------
// void PutScript(char *str)
//------------------------------------------------------------------------------
/*
 * Appends script to a string stream.
 *
 * @param <str> string to append
 */
//------------------------------------------------------------------------------
void GmatInterface::PutScript(char *str)
{
   mStringStream << std::string(str) << std::endl;
   #if DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage("GmatInterface::PutScript() str=%s\n", str);
   #endif
}


//------------------------------------------------------------------------------
// void BuildObject()
//------------------------------------------------------------------------------
/*
 * Clears resource and build new objects from a internal string stream.
 */
//------------------------------------------------------------------------------
void GmatInterface::BuildObject()
{
  
   Moderator *moderator = Moderator::Instance();
   
   std::streambuf *streamBuf = mStringStream.rdbuf();

   // redirect mInStringStream into mStringStream
   RedirectBuffer(mInStringStream, streamBuf);
   
   #if DEBUG_GMAT_INTERFACE
   //loj: 8/31/04 Why this causes problem for long scripts? buffer overflow?
   //MessageInterface::ShowMessage
   //   ("GmatInterface::BuildObject() mStringStream.str=\n%s", mStringStream.str().c_str());
   //MessageInterface::ShowMessage
   //   ("GmatInterface::BuildObject() mInStringStream.str=\n%s\n", mInStringStream->str().c_str());
   #endif
   
   // flag to clear objects and mission sequence
   moderator->InterpretScript(mInStringStream, true);
  
   Moderator::GetGuiInterpreter()->UpdateResourceTree();
   Moderator::GetGuiInterpreter()->UpdateMissionTree();

   // empty the buffer, once objects are created
   mStringStream.str("");
}


//------------------------------------------------------------------------------
// void UpdateObject()
//------------------------------------------------------------------------------
/*
 * Build and updates objects from a internal string stream without clearing the
 * resource.
 */
//------------------------------------------------------------------------------
void GmatInterface::UpdateObject()
{
  
   Moderator *moderator = Moderator::Instance();
   
   std::streambuf *streamBuf = mStringStream.rdbuf();

   // redirect mInStringStream into mStringStream
   RedirectBuffer(mInStringStream, streamBuf);
   
   #if DEBUG_GMAT_INTERFACE
   //loj: 8/31/04 Why this causes problem for long scripts? buffer overflow?
   //MessageInterface::ShowMessage
   //   ("GmatInterface::UpdateObject() mStringStream.str=\n%s", mStringStream.str().c_str());
   //MessageInterface::ShowMessage
   //   ("GmatInterface::UpdateObject() mInStringStream.str=\n%s\n", mInStringStream->str().c_str());
   #endif

   // flag not to clear objects and mission sequence
   moderator->InterpretScript(mInStringStream, false);
   Moderator::GetGuiInterpreter()->UpdateResourceTree();
   Moderator::GetGuiInterpreter()->UpdateMissionTree();
   
   // empty the buffer, once objects are created
   mStringStream.str("");
}


//------------------------------------------------------------------------------
// void RunScript()
//------------------------------------------------------------------------------
/*
 * Executues commands from existing objects.
 */
//------------------------------------------------------------------------------
void GmatInterface::RunScript()
{
   #if DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage("GmatInterface::RunScript() entered\n");
   #endif
   
   Moderator::Instance()->RunScript();
}


//------------------------------------------------------------------------------
// char* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/*
 * @return string value of the parameter.
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetParameter(const std::string &name)
{
   #if DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::GetParameter() name=%s\n", name.c_str());
   #endif
   
   static char dataString[MAX_PARAM_VAL_STRING];
   static char *undefindString = "-123456789.123456789\0";
   //static char *tempString2 = "[123456.12345, 234567.12345, 345678.12345]\0";
   strcpy(dataString, undefindString);
   //dataString[0] = '\0';
   Parameter *param;
   
   param = Moderator::Instance()->GetParameter(name);
   if (param != NULL)
   {
      #if DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage
         ("GmatInterface::GetParameter() evaluate the parameter:%s, type=%s\n",
          param->GetName().c_str(), param->GetTypeName().c_str());
      #endif
      
      //loj: 2/16/05 param->Evaluate() causes system to crash!!
      // so just get the last value without evaluting
      //param->Evaluate(); 
      std::string str = param->ToString(); // returns last value
      str = "[" + str + "]";
      
      #if DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage
         ("GmatInterface::GetParameter() str=%s\n", str.c_str());
      #endif
      
      sprintf(dataString, "%s", str.c_str());
   }      
   
   return dataString;
}


//------------------------------------------------------------------------------
// char* GetObject(const std::string &name)
//------------------------------------------------------------------------------
/*
 * @return serialized string value of the object.
 */
//------------------------------------------------------------------------------
char* GmatInterface::GetObject(const std::string &name)
{
   #if DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::GetObject() name=%s\n", name.c_str());
   #endif

   static char dataString[MAX_OBJECT_VAL_STRING];
   static char *undefindString = "-123456789.123456789\0";
   //static char *tempString2 = "[123456.12345, 234567.12345, 345678.12345]\0";
   strcpy(dataString, undefindString);
   //dataString[0] = '\0';
   GmatBase *obj;
   
   //obj = Moderator::Instance()->GetConfiguredItem(name);
   obj = Moderator::Instance()->GetInternalObject(name);
   
   if (obj != NULL)
   {
      #if DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage
         ("GmatInterface::GetObject() get serialized string of object name:"
          "%s, type=%s\n", obj->GetName().c_str(), obj->GetTypeName().c_str());
      #endif
      
      std::string str = obj->GetGeneratingString(Gmat::MATLAB_STRUCT);
      
      #if DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage("str=%s\n", str.c_str());
      #endif
      
      sprintf(dataString, "%s", str.c_str());
   }
   else
   {
      MessageInterface::ShowMessage
         ("GmatInterface::GetObject() the pointer of object name:%s is null\n",
          name.c_str());
   }
   
   return dataString;
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// GmatInterface()
//------------------------------------------------------------------------------
GmatInterface::GmatInterface()
{
   mInStringStream = NULL;
}


//------------------------------------------------------------------------------
// GmatInterface()
//------------------------------------------------------------------------------
GmatInterface::~GmatInterface()
{
   if (mInStringStream)
      delete mInStringStream;
}
  
