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
#include "Moderator.hpp"         // for GetParameter()
#include "Publisher.hpp"         // for Instance()

#define DEBUG_MATLABWS 1

//---------------------------------
// static data
//---------------------------------
const std::string
MatlabWs::PARAMETER_TEXT[MatlabWsParamCount - SubscriberParamCount] =
{
   "Precision",
   "VarList",
   "Add",
   "Clear",
};

const Gmat::ParameterType
MatlabWs::PARAMETER_TYPE[MatlabWsParamCount - SubscriberParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::BOOLEAN_TYPE,
};

//------------------------------------------------------------------------------
// MatlabWs(const std::string &name, Parameter *firstVarParam)
//------------------------------------------------------------------------------
MatlabWs::MatlabWs(const std::string &name, Parameter *firstVarParam) :
   Subscriber("MatlabWs", name),
   precision(12)
{
   mNumVarParams = 0;

   if (firstVarParam != NULL)
      AddVarParameter(firstVarParam->GetName());

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
   Subscriber(copy),
   precision(copy.precision)
{
   mVarParams = copy.mVarParams; 
   mVarParamMap = copy.mVarParamMap;
   mNumVarParams = copy.mNumVarParams;
   mVarParamNames = copy.mVarParamNames;
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
   
   precision = right.precision;
   
   mVarParams = right.mVarParams; 
   mVarParamMap = right.mVarParamMap;
   mNumVarParams = right.mNumVarParams;
   mVarParamNames = right.mVarParamNames;
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
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer MatlabWs::GetIntegerParameter(const Integer id) const
{
   if (id == PRECISION)
      return precision;
   return Subscriber::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer MatlabWs::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == PRECISION)
   {
      if (value > 0)
         precision = value;
      return precision;
   }

   return Subscriber::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string MatlabWs::GetStringParameter(const Integer id) const
{
   if (id == ADD)
      return "";
   return Subscriber::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool MatlabWs::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == ADD)
   {
      return AddVarParameter(value);
   }
   
   return Subscriber::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& MatlabWs::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case VAR_LIST:
      return mVarParamNames;
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
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool MatlabWs::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
   case CLEAR:
      return true;
   default:
         return Subscriber::GetBooleanParameter(id);
   }
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
bool MatlabWs::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool MatlabWs::SetBooleanParameter(const Integer id, const bool value)
{
   switch (id)
   {
   case CLEAR:
      ClearVarParameters();
      return true;
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label,
//                          const bool value)
//------------------------------------------------------------------------------
bool MatlabWs::SetBooleanParameter(const std::string &label,
                                 const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// Integer GetNumVarParameters()
//------------------------------------------------------------------------------
Integer MatlabWs::GetNumVarParameters()
{
   return mNumVarParams;
}

//------------------------------------------------------------------------------
// bool AddVarParameter(const std::string &paramName)
//------------------------------------------------------------------------------
bool MatlabWs::AddVarParameter(const std::string &paramName)
{
   bool status = false;
   Moderator *theModerator = Moderator::Instance();
    
   if (paramName != "")
   {
      mVarParamNames.push_back(paramName);
      mVarParamMap[paramName] = NULL;
      mNumVarParams = mVarParamNames.size();

      // get parameter pointer
      Parameter *param = theModerator->GetParameter(paramName);
      if (param != NULL)
      {
         mVarParamMap[paramName] = param;
         mVarParams.push_back(param);
      }
      status = true;
   }

   return status;
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
#if DEBUG_MATLAB
   MessageInterface::ShowMessage("\nMatlabWs::Distribute() entered\n");
#endif
             
   Rvector varvals = Rvector(mNumVarParams);
     
   if (len == 0)
   {
      return false;
   }
   else
   {
      for (int i=0; i < mNumVarParams; i++)
      {
          mVarParams[i]->Evaluate();
      } 
   }

   return true;
}

//--------------------------------------
// private methods
//--------------------------------------

//------------------------------------------------------------------------------
// void ClearYParameters()
//------------------------------------------------------------------------------
void MatlabWs::ClearVarParameters()
{
   mVarParams.clear();
   mVarParamNames.clear();
   mNumVarParams = 0;
}

