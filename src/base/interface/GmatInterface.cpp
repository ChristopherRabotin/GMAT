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

   //ClearScript();
}

//------------------------------------------------------------------------------
// void ClearScrip()
//------------------------------------------------------------------------------
void GmatInterface::ClearScript()
{
   //Moderator *moderator = Moderator::Instance();
   mStringStream.str("");
   //moderator->ClearResource();
   //moderator->ClearCommandSeq();
   //Moderator::GetGuiInterpreter()->UpdateResourceTree();
   //Moderator::GetGuiInterpreter()->UpdateMissionTree();
   Moderator::GetGuiInterpreter()->CloseCurrentProject();
}

//------------------------------------------------------------------------------
// void PutScript(char *str)
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
void GmatInterface::RunScript()
{
   #if DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage("GmatInterface::RunScript() entered\n");
   #endif
   
   Moderator::Instance()->RunScript();
}

//------------------------------------------------------------------------------
// char* GetParameterData(const std::string &name)
//------------------------------------------------------------------------------
char* GmatInterface::GetParameterData(const std::string &name)
{
   #if DEBUG_GMAT_INTERFACE
   MessageInterface::ShowMessage
      ("GmatInterface::GetParameterData() name=%s\n", name.c_str());
   #endif

   static char dataString[MAX_DATA_STRING];
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
         ("Now evaluate the parameter:%s, type=%s\n",
          param->GetName().c_str(), param->GetTypeName().c_str());
      #endif
      
      //loj: 2/16/05 param->Evaluate() causes system to crash!!
      // so just get the last value without evaluting
      //param->Evaluate(); 
      std::string str = param->ToString(); // returns last value
      str = "[" + str + "]";
      
      #if DEBUG_GMAT_INTERFACE
      MessageInterface::ShowMessage("str=%s\n", str.c_str());
      #endif
      
      sprintf(dataString, "%s", str.c_str());
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
  
