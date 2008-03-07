//$Id$
//------------------------------------------------------------------------------
//                                  MatlabWs
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/08
//
/**
 * Implements MatlabWs class which evaluates parameters to be sent to MATLAB
 * workspace.
 */
//------------------------------------------------------------------------------

#include "MatlabWs.hpp"
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "StringUtil.hpp"          // for ToString()

#ifdef __USE_MATLAB__
#include "MatlabInterface.hpp"
#endif

//#define DEBUG_MATLABWS_PARAM
//#define DEBUG_MATLABWS_INIT
//#define DEBUG_MATLABWS_SET
//#define DBGLVL_MATLABWS_RUN 2

//---------------------------------
// static data
//---------------------------------
const std::string
MatlabWs::PARAMETER_TEXT[MatlabWsParamCount - SubscriberParamCount] =
{
   "Add",
   "UpdateFrequency"
};

const Gmat::ParameterType
MatlabWs::PARAMETER_TYPE[MatlabWsParamCount - SubscriberParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,    //"Add"
   Gmat::INTEGER_TYPE,        //"UpdateFrequency"
};


//------------------------------------------------------------------------------
// MatlabWs(const std::string &name, Parameter *firstParam)
//------------------------------------------------------------------------------
MatlabWs::MatlabWs(const std::string &name, Parameter *firstParam) :
   Subscriber("MatlabWS", name)
{
   mNumParams = 0;
   mDataCount = 0;
   mSendCount = 0;
   
   if (firstParam != NULL)
      AddParameter(firstParam->GetName(), 0);
   
   mUpdateFrequency = 1;
   parameterCount = MatlabWsParamCount;
}


//------------------------------------------------------------------------------
// ~MatlabWs(void)
//------------------------------------------------------------------------------
MatlabWs::~MatlabWs(void)
{
}


//------------------------------------------------------------------------------
// MatlabWs(const MatlabWs &copy)
//------------------------------------------------------------------------------
MatlabWs::MatlabWs(const MatlabWs &copy) :
   Subscriber(copy)
{
   mParams = copy.mParams; 
   mNumParams = copy.mNumParams;
   mParamNames = copy.mParamNames;
   mUpdateFrequency = copy.mUpdateFrequency;
   mAllRefObjectNames = copy.mAllRefObjectNames;
   mDataCount = 0;
   mSendCount = 0;
}


//------------------------------------------------------------------------------
// MatlabWs& MatlabWs::operator=(const MatlabWs& right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
MatlabWs& MatlabWs::operator=(const MatlabWs& right)
{
   if (this == &right)
      return *this;
   
   Subscriber::operator=(right);
   
   mParams = right.mParams; 
   mNumParams = right.mNumParams;
   mParamNames = right.mParamNames;
   mUpdateFrequency = right.mUpdateFrequency;
   mAllRefObjectNames = right.mAllRefObjectNames;
   mDataCount = 0;
   mSendCount = 0;
   
   return *this;
}

//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool MatlabWs::Initialize()
{
   #ifdef DEBUG_MATLABWS_INIT
   MessageInterface::ShowMessage
      ("MatlabWs::Initialize() entered. mUpdateFrequency=%d, mDataCount=%d, "
       "mSendCount=%d\n", mUpdateFrequency, mDataCount, mSendCount);
   #endif
   
   Subscriber::Initialize();
   
   if (active)
   {
      if (mNumParams == 0)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** The MatlabWS named \"%s\" will not be created.\n"
             "No parameters were added to MatlabWS.\n", GetName().c_str());
         
         active = false;
         return false;
      }
   }
   
   //-----------------------------------------------------------------
   // Open Matlab engine, clear parameters in the workspace
   //-----------------------------------------------------------------
   #ifdef __USE_MATLAB__
   
   MatlabInterface::Open();
   
   for (int i=0; i < mNumParams; i++)
   {
      std::string matlabStr = "clear " + mParamNames[i];
      
      #ifdef DEBUG_MATLABWS_INIT
      MessageInterface::ShowMessage
         ("   Sending \"%s\" to MATLAB workspace\n", matlabStr.c_str());
      #endif
      
      MatlabInterface::RunMatlabString(matlabStr);
      
      // if it has dot, clear matlab struct
      std::string type, owner, dep;
      GmatStringUtil::ParseParameter(mParamNames[i], type, owner, dep);
      if (owner != "")
      {
         matlabStr = "clear " + owner;
         
         #ifdef DEBUG_MATLABWS_INIT
         MessageInterface::ShowMessage
            ("   Sending \"%s\" to MATLAB workspace\n", matlabStr.c_str());
         #endif
         
         MatlabInterface::RunMatlabString(matlabStr);
      }
   }
   #endif
   //-----------------------------------------------------------------
   
   mDataCount = 0;
   mSendCount = 0;
   
   #ifdef DEBUG_MATLABWS_INIT
   MessageInterface::ShowMessage("MatlabWs::Initialize() exiting\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MatlabWs.
 *
 * @return clone of the MatlabWs.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MatlabWs::Clone(void) const
{
   return (new MatlabWs(*this));
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool MatlabWs::TakeAction(const std::string &action,
                          const std::string &actionData)
{
   if (action == "Clear")
   {
      ClearParameters();
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string MatlabWs::GetParameterText(const Integer id) const
{
    if (id >= SubscriberParamCount && id < MatlabWsParamCount)
        return PARAMETER_TEXT[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer MatlabWs::GetParameterID(const std::string &str) const
{
    for (Integer i = SubscriberParamCount; i < MatlabWsParamCount; i++)
    {
        if (str == PARAMETER_TEXT[i - SubscriberParamCount])
            return i;
    }

    return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType MatlabWs::GetParameterType(const Integer id) const
{
    if (id >= SubscriberParamCount && id < MatlabWsParamCount)
        return PARAMETER_TYPE[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string MatlabWs::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < MatlabWsParamCount)
      return Subscriber::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer MatlabWs::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case UPDATE_FREQUENCY:
      return mUpdateFrequency;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer MatlabWs::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer MatlabWs::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case UPDATE_FREQUENCY:
      mUpdateFrequency = value;
      if (mUpdateFrequency <= 0)
         mUpdateFrequency = 1;
      return mUpdateFrequency;
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label,
//                                     const Integer value)
//------------------------------------------------------------------------------
Integer MatlabWs::SetIntegerParameter(const std::string &label,
                                        const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool MatlabWs::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case ADD:
      return AddParameter(value, mNumParams);
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value)
//------------------------------------------------------------------------------
bool MatlabWs::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool MatlabWs::SetStringParameter(const Integer id, const std::string &value,
                                  const Integer index)
{
   switch (id)
   {
   case ADD:
      return AddParameter(value, index);
   default:
      return Subscriber::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool MatlabWs::SetStringParameter(const std::string &label,
                                  const std::string &value,
                                  const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& MatlabWs::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD:
      return mParamNames;
   default:
      return Subscriber::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& MatlabWs::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* MatlabWs::GetRefObject(const Gmat::ObjectType type,
                                 const std::string &name)
{
   for (int i=0; i<mNumParams; i++)
   {
      if (mParamNames[i] == name)
         return mParams[i];
   }

   throw GmatBaseException("MatlabWs::GetRefObject() the object name: " + name +
                           " not found\n");
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool MatlabWs::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name)
{
   #ifdef DEBUG_MATLABWS_SET
   MessageInterface::ShowMessage
      ("MatlabWs::SetRefObject() obj=%p, name=%s, objtype=%s, objname=%s\n",
       obj, name.c_str(), obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
   
   for (int i=0; i<mNumParams; i++)
   {
      if (mParamNames[i] == name)
      {
         mParams[i] = (Parameter*)obj;
         return true;
      }
   }
   
   #ifdef DEBUG_MATLABWS_SET
   MessageInterface::ShowMessage
      ("MatlabWs::SetRefObject() %s not found, so returning false\n", name.c_str());
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& MatlabWs::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   mAllRefObjectNames.clear();
   
   switch (type)
   {
   case Gmat::PARAMETER:
   case Gmat::UNKNOWN_OBJECT:
      mAllRefObjectNames = mParamNames;
   default:
      break;
   }
   
   return mAllRefObjectNames;
}


//------------------------------------------------------------------------------
// Integer GetNumParameters()
//------------------------------------------------------------------------------
Integer MatlabWs::GetNumParameters()
{
   return mNumParams;
}


//------------------------------------------------------------------------------
// bool AddParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool MatlabWs::AddParameter(const std::string &paramName, Integer index)
{
   #ifdef DEBUG_MATLABWS_PARAM
   MessageInterface::ShowMessage
      ("MatlabWs::AddParameter() name = %s, index = %d\n", paramName.c_str(), index);
   #endif
   
   if (paramName != "" && index == mNumParams)
   {
      StringArray::iterator paramPos = 
         find(mParamNames.begin(), mParamNames.end(), paramName);
      
      // if name is unique, add
      if (paramPos == mParamNames.end())
      {
         mParamNames.push_back(paramName);
         mNumParams = mParamNames.size();
         mParams.push_back(NULL);
         return true;
      }
   }
   
   return false;
}

//--------------------------------------
// protected methods
//--------------------------------------

//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool MatlabWs::Distribute(int len)
{      
   return false;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool MatlabWs::Distribute(const Real * dat, Integer len)
{
   #if DBGLVL_MATLABWS_RUN
   MessageInterface::ShowMessage
      ("MatlabWs::Distribute() entered. mUpdateFrequency=%d, mDataCount=%d, "
       "mSendCount=%d\n", mUpdateFrequency, mDataCount, mSendCount);
   #endif
   
   Rvector varvals = Rvector(mNumParams);
   
   if (len == 0)
   {
      return false;
   }
   else
   {
      mDataCount++;
      
      if ((mDataCount % mUpdateFrequency) == 0)
      {
         mDataCount = 0;
         mSendCount++;
         
         for (int i=0; i < mNumParams; i++)
         {
            if (mParams[i] == NULL)
               throw SubscriberException("MatlabWs::Distribute: Cannot find "
                                         "parameter \"" + mParamNames[i] + "\"\n");
            
            mParams[i]->Evaluate();
            std::string paramVal = mParams[i]->ToString();
            
            #if DBGLVL_MATLABWS_RUN > 1
            MessageInterface::ShowMessage
               ("   %s = %s\n",  mParamNames[i].c_str(), paramVal.c_str());
            #endif
            
            //--------------------------------------------------------
            // if using MATLAB, send it to workspace
            //--------------------------------------------------------
            #ifdef __USE_MATLAB__
            std::string countStr = GmatStringUtil::ToString(mSendCount, 1);
            
            if (mParams[i]->GetTypeName() == "Array")
            {
               countStr = "(" + countStr + ",:) = ";
               paramVal = "[ " + paramVal + "]";
            }
            else
            {
               countStr = "(" + countStr + ") = ";
            }
            
            std::string matlabStr = mParamNames[i] + countStr + paramVal;
            
            #if DBGLVL_MATLABWS_RUN > 1
            MessageInterface::ShowMessage
               ("   Sending \"%s\" to MATLAB workspace\n", matlabStr.c_str());
            #endif
            
            MatlabInterface::RunMatlabString(matlabStr);
            #endif
            //--------------------------------------------------------
         }
      }
   }
   
   return true;
}

//--------------------------------------
// private methods
//--------------------------------------

//------------------------------------------------------------------------------
// void ClearParameters()
//------------------------------------------------------------------------------
void MatlabWs::ClearParameters()
{
   #ifdef DEBUG_MATLABWS_PARAM
   MessageInterface::ShowMessage("\nMatlabWs::ClearParameters() entered\n");
   #endif
   
   mParams.clear();
   mParamNames.clear();
   mNumParams = 0;
}

