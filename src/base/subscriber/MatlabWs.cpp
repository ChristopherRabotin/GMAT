//$Header$
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
#include "Publisher.hpp"         // for Instance()

//#define DEBUG_MATLABWS_PARAM 1
//#define DEBUG_MATLABWS_RUN 2

//---------------------------------
// static data
//---------------------------------
const std::string
MatlabWs::PARAMETER_TEXT[MatlabWsParamCount - SubscriberParamCount] =
{
   "Add",
};

const Gmat::ParameterType
MatlabWs::PARAMETER_TYPE[MatlabWsParamCount - SubscriberParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
};


//------------------------------------------------------------------------------
// MatlabWs(const std::string &name, Parameter *firstParam)
//------------------------------------------------------------------------------
MatlabWs::MatlabWs(const std::string &name, Parameter *firstParam) :
   Subscriber("MatlabWS", name)
{
   mNumParams = 0;

   if (firstParam != NULL)
      AddParameter(firstParam->GetName(), 0);

   mEvaluateFrequency = 1;
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
   mEvaluateFrequency = copy.mEvaluateFrequency;
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
   mEvaluateFrequency = right.mEvaluateFrequency;

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
   Subscriber::Initialize();

   //-----------------------------------
   //@todo
   // need to set Parameter pointers
   //-----------------------------------
   // implement this later

   if (active)
   {
      ; // anything to do?
   }
   else
   {
      ; // anything to do?
   }   

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


//loj: 1/10/05 Added
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
   for (int i=0; i<mNumParams; i++)
   {
      if (mParamNames[i] == name)
      {
         mParams[i] = (Parameter*)obj;
         return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& MatlabWs::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   return mParamNames;
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
   #if DEBUG_MATLABWS_PARAM
   MessageInterface::ShowMessage("MatlabWs::AddParameter() name = %s, index = %d\n",
                                 paramName.c_str(), index);
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
   #if DEBUG_MATLABWS_RUN
   MessageInterface::ShowMessage("\nMatlabWs::Distribute() entered\n");
   #endif
             
   Rvector varvals = Rvector(mNumParams);
     
   if (len == 0)
   {
      return false;
   }
   else
   {
      for (int i=0; i < mNumParams; i++)
      {
          mParams[i]->Evaluate();
          #if DEBUG_MATLABWS_RUN > 2
          MessageInterface::ShowMessage
             ("MatlabWs::Distribute() mParams[i]->ToString() = %s\n",
              mParams[i]->ToString());
          #endif
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
   mParams.clear();
   mParamNames.clear();
   mNumParams = 0;
}

