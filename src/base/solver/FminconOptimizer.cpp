//$Id$
//------------------------------------------------------------------------------
//                         FminconOptimizer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.07.149
//
/**
 * Implementation for the external optimizer base class. 
 *
 */
//------------------------------------------------------------------------------


#include <sstream>       // for stringstream, istringstream
#include <stdlib.h>      // for atoi atof, etc.
#include "FminconOptimizer.hpp"
#include "MessageInterface.hpp"
#include "SolverException.hpp"
#include "StringUtil.hpp"

#if defined __USE_MATLAB__
   #include "MatlabInterface.hpp"  // currently all static
   #include "GmatInterface.hpp"
   #include "GmatMainFrame.hpp"
   #include "GmatAppData.hpp"
#endif

//#define DEBUG_STATE_MACHINE
//#define DEBUG_ML_CONNECTIONS
//#define DEBUG_FMINCON_OPTIONS
//#define DEBUG_OPTIMIZER_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
FminconOptimizer::PARAMETER_TEXT[
       FminconOptimizerParamCount -ExternalOptimizerParamCount] =
{
   "Options",
   "OptionValues",
};

const Gmat::ParameterType
FminconOptimizer::PARAMETER_TYPE[
       FminconOptimizerParamCount - ExternalOptimizerParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};

// list of allowed fmincon options
// NOTE - if changes are made to this list, changes MUST also be
// made to the Optimize command (where it is adding single quotes to
// string options) and to the IsAllowedValue method; the 
// NUM_MATLAB_OPTIONS parameter may also need to be changed
const std::string FminconOptimizer::ALLOWED_OPTIONS[12] = 
{
   "DiffMaxChange",
   "DiffMinChange",
   "MaxFunEvals",
   "MaxIter",
   "TolX",
   "TolFun",
   "TolCon",
   "DerivativeCheck",
   "Diagnostics",
   "Display",
   "GradObj",
   "GradConstr",
};

const std::string FminconOptimizer::DEFAULT_OPTION_VALUES[12] = 
{
   "0.1000",
   "1.0000e-08",
   "1000",
   "400",
   "1.0000e-04",
   "1.0000e-04",
   "1.0000e-04",
   "off",
   "off",
   "iter",
   "off",
   "off",
};

const Integer FminconOptimizer::NUM_MATLAB_OPTIONS    = 12;
const Integer FminconOptimizer::MATLAB_OPTIONS_OFFSET = 1000;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

FminconOptimizer::FminconOptimizer(std::string name) :
   ExternalOptimizer       ("FminconOptimizer", name),
   fminconExitFlag         (-999)
 {
   objectTypeNames.push_back("FminconOptimizer");
   parameterCount = FminconOptimizerParamCount;
   // set up options list, based on allowed options
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "In fmincon constructor, about to set up option and option value lists.\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
   {
      #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "Fmincon construct: now adding option %s to list\n",
         (ALLOWED_OPTIONS[i]).c_str());
          MessageInterface::ShowMessage(
         "Fmincon construct: now adding option value %s to list\n",
         (DEFAULT_OPTION_VALUES[i]).c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      options.push_back(ALLOWED_OPTIONS[i]);
      optionValues.push_back(DEFAULT_OPTION_VALUES[i]);
   }
   // ********* BEGIN temporary prototype, testing, etc. *****************************//
   //functionPath = "/Users/wcshoan/dev/Ec_GMAT/bin/files/matlab_functions";
   //OpenConnection();  
   // ********* END temporary prototype, testing, etc. *****************************//
   
   AllowStepsizeLimit = false;
   AllowIndependentPerts = false;
}


FminconOptimizer::~FminconOptimizer()
{
   FreeArrays();
   
   // loj: Let's leave it open (4/11/07)
   // So that it will be automatically connected to the current running Matlab session.
   // User has to manually close the Matlab session if we want to create a new session.
   // This works on Windows, will this cause any problems on other platforms?
   //CloseConnection();
}


FminconOptimizer::FminconOptimizer(const FminconOptimizer &opt) :
   ExternalOptimizer       (opt),
   fminconExitFlag         (-999)
{
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "In fmincon copy constructor, about to copy option and option value lists.\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   options          = opt.options;
   optionValues     = opt.optionValues;
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "In fmincon copy constructor, number of options copied = %d\n",
      (Integer) options.size());
      MessageInterface::ShowMessage(
      "In fmincon copy constructor, number of option values copied = %d\n",
      (Integer) optionValues.size());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   parameterCount   = opt.parameterCount;
}


FminconOptimizer& 
    FminconOptimizer::operator=(const FminconOptimizer& opt)
{
    if (&opt == this)
        return *this;

   ExternalOptimizer::operator=(opt);
   options.clear();
   optionValues.clear();
   options          = opt.options;
   optionValues     = opt.optionValues;
   fminconExitFlag  = opt.fminconExitFlag; // right?
   
  
   return *this;
}

bool FminconOptimizer::Initialize()
{
   ExternalOptimizer::Initialize();
   
   // open connection(s) to the external source
   if (!OpenConnection())
      throw SolverException(
            "Fmincon - Unable to connect to external interface");
   
   return true;
}

Solver::SolverState FminconOptimizer::AdvanceState()
{
   switch (currentState)
   {
      case INITIALIZING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage(
            "Entered FminconOptimizer state machine; INITIALIZING\n");
         #endif
         WriteToTextFile();
         ReportProgress();
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "FminconOptimizer::AdvanceState about to complete initialization\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         CompleteInitialization();
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "FminconOptimizer::AdvanceState initialization complete\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         currentState = RUNEXTERNAL;
         break;

      case RUNEXTERNAL:
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
               "Entered FminconOptimizer state machine; RUNEXTERNAL\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         ReportProgress();
         RunExternal();
         ReportProgress();
         break;
        
      case FINISHED:
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
               "Entered FminconOptimizer state machine; FINISHED\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         RunComplete();
         ReportProgress();
         break;
      default:
         break;
   }
     
   return currentState;
}


StringArray FminconOptimizer::AdvanceNestedState(std::vector<Real> vars)
{
   #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "Entering FminconOptimizer::AdvanceNestedState\n");
      for (Integer ii=0; ii<(Integer) vars.size();ii++)
         MessageInterface::ShowMessage(" vars(%d) = %.12f\n",
         ii, vars.at(ii));
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   StringArray results;
   if (nestedState == INITIALIZING)
   {
      nestedState = NOMINAL;
   }
   else if (nestedState == NOMINAL)
   {
      if ((Integer) vars.size() != variableCount)
      {
         std::string errorStr = "FminconOptimizer: incorrect number of ";
         errorStr += "variable values returned from MATLAB";
         throw SolverException(errorStr);
      }
      for (Integer i=0;i<variableCount;i++)
         variable.at(i) = vars.at(i);
      RunNominal();
      nestedState = CALCULATING;
   }
   else if (nestedState == CALCULATING)
   {
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "about to call FminconOptimizer::CalculateParameters\n");
         MessageInterface::ShowMessage(
         "now building return string array\n");
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      CalculateParameters(); 
      std::stringstream outS;
      outS.precision(18);
      std::string       oneResult;
      
      // F
      outS.str();
      outS << cost;
      oneResult = "F = " + outS.str() + ";";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      
      // GradF
      outS.str("");
      for (Integer i=0; i<(Integer) gradient.size(); i++)
         outS << gradient.at(i) << ";";
      oneResult = "GradF = [" + outS.str() + "];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      
      // equality constraints
      outS.str("");
      for (Integer i=0; i<(Integer) eqConstraintValues.size(); i++)
         outS << eqConstraintValues.at(i) << ";";
      oneResult = "NonLinearEqCon = [" + outS.str() + "];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      
      // inequality constraints
      outS.str("");
      for (Integer i=0; i<(Integer) ineqConstraintValues.size(); i++)
         outS << ineqConstraintValues.at(i) << ";";
      oneResult = "NonLinearIneqCon = [" + outS.str() + "];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      
      // Jacobian in the future ---------------------- <future> ----------------
      oneResult = "JacNonLinearEqCon = [];"; 
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      oneResult = "JacNonLinearIneqCon = [];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      // Jacobian in the future ---------------------- <future> ----------------
          
      WriteToTextFile(nestedState);
      nestedState = NOMINAL;
   }
   return results;
}

bool FminconOptimizer::Optimize()
{   
   #ifdef DEBUG_ML_CONNECTIONS
      MessageInterface::ShowMessage("Entering Optimize method ....\n");
   #endif
   #if defined __USE_MATLAB__
      // set format long so that we don't lose precision between string transmission
      MatlabInterface::EvalString("format long");
      // clear last errormsg
      MatlabInterface::EvalString("clear errormsg");
      
      // set up options/values list for OPTIMSET call
      bool allEmpty = true;
      std::string optionsStr     = "GMAToptions = optimset(";
      std::string defaultOptions = optionsStr +"\'fmincon\');";
      std::ostringstream optS;
      
      #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "In Optimize method, the number of options is %d ....\n", 
         (Integer)options.size());
         MessageInterface::ShowMessage(
         "In Optimize method, the number of option values is %d ....\n", 
         (Integer)optionValues.size());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      for (Integer i=0; i < (Integer) options.size(); i++)
      {
         if (optionValues.at(i) != "")
         {
            allEmpty = false;
            if (i != 0) optS << ",";
            optS << "\'" << options.at(i)      << "\',";
            /// @todo do this in a better way, not using 7->11, etc.
            if ((7 <= i) && (i <= 11)) // put single quotes around strings
               optS << "\'" << optionValues.at(i) << "\'";
            else
               optS << optionValues.at(i);         }
      }
      optionsStr += optS.str() + ");";
      #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "In Optimize method, the options are: %s ....\n", optionsStr.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      // call OPTIMSET (using EvalStr) to set up options for fmincon
      if (allEmpty) // if none were set, set options to be default for fmincon
         MatlabInterface::RunMatlabString(defaultOptions);
      else
         MatlabInterface::RunMatlabString(optionsStr);
         
      std::string inParm;
      std::ostringstream mlS;
      // pass to MATLAB the X0 array (needs to be a column vector)
      mlS.str("");
      mlS.precision(18);
      
      for (Integer i=0;i<(Integer)variable.size();i++)
         mlS << variable.at(i) << ";";
      inParm = "X0 = [" + mlS.str() + "];";
      #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "In Optimize method, parameter string is: %s ....\n", inParm.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      MatlabInterface::RunMatlabString(inParm);
      
      // pass to MATLAB the Lower column vector
      mlS.str("");
      for (Integer i=0;i<(Integer)variableMinimum.size();i++)
         mlS << variableMinimum.at(i) << ";";
      inParm = "Lower = [" + mlS.str() + "];";
      #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "In Optimize method, parameter string is: %s ....\n", inParm.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      MatlabInterface::RunMatlabString(inParm);
      
      // pass to MATLAB the Upper column vector
      mlS.str("");
      for (Integer i=0;i<(Integer)variableMaximum.size();i++)
         mlS << variableMaximum.at(i) << ";";
      inParm = "Upper = [" + mlS.str() + "];";
      #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "In Optimize method, parameter string is: %s ....\n", inParm.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      MatlabInterface::RunMatlabString(inParm);
      
      // clear last errormsg
      MatlabInterface::EvalString("clear errormsg");
      //std::string runString = 
      //   "[X] = GmatFminconOptimizationDriver(X0,Lower,Upper);";
      // wcs - made it into an m-file instead of a function
      std::string runString = 
         "GmatFminconOptimizationDriver;";
      MatlabInterface::RunMatlabString(runString);
      // ask MATLAB for the value of exitFlag here and evaluate
      double      outArr[1];
      int         OKint = 0;
      std::string resStr    = "exitFlag";
      OKint                 = MatlabInterface::GetRealArray(resStr, 1, outArr);

      if (!OKint)
         throw SolverException(
               "Error determining exitFlag from fmincon");
      fminconExitFlag = (Integer) outArr[0];
      
      // need to ask for fVal as well?
   #endif
   return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the FminconOptimizer.
 *
 * @return clone of the FminconOptimizer.
 */
//------------------------------------------------------------------------------
GmatBase* FminconOptimizer::Clone() const
{
   return (new FminconOptimizer(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void FminconOptimizer::Copy(const GmatBase* orig)
{
   operator=(*((FminconOptimizer *)(orig)));
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetParameterText(const Integer id) const
{
   if ((id >= ExternalOptimizerParamCount) && (id < FminconOptimizerParamCount))
      return PARAMETER_TEXT[id - ExternalOptimizerParamCount];
   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      return ALLOWED_OPTIONS[id - MATLAB_OPTIONS_OFFSET];
   }
   return ExternalOptimizer::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer FminconOptimizer::GetParameterID(const std::string &str) const
{
   for (Integer i = ExternalOptimizerParamCount; i < FminconOptimizerParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - ExternalOptimizerParamCount])
         return i;
   }
   for (Integer j =0; j < NUM_MATLAB_OPTIONS; j++)
      if (str == ALLOWED_OPTIONS[j])
         return (MATLAB_OPTIONS_OFFSET + j);

   return ExternalOptimizer::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType FminconOptimizer::GetParameterType(
                                              const Integer id) const
{
   if ((id >= ExternalOptimizerParamCount) && (id < FminconOptimizerParamCount))
      return PARAMETER_TYPE[id - ExternalOptimizerParamCount];

   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      return Gmat::STRING_TYPE;
   }
   return ExternalOptimizer::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetParameterTypeString(
                                      const Integer id) const
{
   return ExternalOptimizer::PARAM_TYPE_STRING[GetParameterType(id)];
}

std::string  FminconOptimizer::GetStringParameter(const Integer id) const
{
   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      return optionValues[id - MATLAB_OPTIONS_OFFSET];
   }
   return ExternalOptimizer::GetStringParameter(id);
}

bool FminconOptimizer::SetStringParameter(const Integer id,
                                          const std::string &value)
{
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Fmincon::setting id %d value to %s\n",
      id, value.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      if (!IsAllowedValue(options[id - MATLAB_OPTIONS_OFFSET], value))
      {
          std::string errorStr = "FminconOptimizer error: Value " + 
               value + " not valid for option " 
               + options[id - MATLAB_OPTIONS_OFFSET] + "\n";
          throw SolverException(errorStr);
      }     
      optionValues[id - MATLAB_OPTIONS_OFFSET] = value;
      return true;
   }
   return ExternalOptimizer::SetStringParameter(id, value);
}

std::string  FminconOptimizer::GetStringParameter(const std::string &label) const
{
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
      if (label == options[i])
      {
         return optionValues[i];
      }
   return ExternalOptimizer::GetStringParameter(label);
}

bool FminconOptimizer::SetStringParameter(const std::string &label,
                                          const std::string &value)
{
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Fmincon::setting param %s value to %s\n",
      label.c_str(), value.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   // check options first
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
      if (label == options[i])
      {
         if (!IsAllowedValue(label, value))
         {
            std::string errorStr = "FminconOptimizer error: Value " + 
               value + " not valid for option " + label + "\n";
            throw SolverException(errorStr);
         }
         optionValues[i] = value;
         return true;
      }
   return ExternalOptimizer::SetStringParameter(label, value);
}

std::string FminconOptimizer::GetStringParameter(const Integer id,
                                                 const Integer index) const
{
   if (id == OPTIONS)
   {
     if ((index < 0) || (index >= (Integer) options.size()))
         throw SolverException("FminconOptimizer::GetString index out of range.");
     return options[index];
   }
   if (id == OPTION_VALUES)
   {
      if ((index < 0) || (index >= (Integer) optionValues.size()))
         throw SolverException("FminconOptimizer::GetString index out of range.");
      return optionValues[index];
   }     
    return ExternalOptimizer::GetStringParameter(id, index);
}
                                          
bool FminconOptimizer::SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index)
{
   if (id == OPTIONS)
   {
     if ((index < 0) || (index > (Integer) options.size()))
         throw SolverException("FminconOptimizer::SetString index out of range.");
     if (!IsAllowedOption(value))
         throw SolverException(
            "FminconOptimizer::SetString - invalid option value: " + value);
     if (((Integer) optionValues.size()) > index)
     {
        if (!IsAllowedValue(value, optionValues[index]))
           throw SolverException(
              "FminconOptimizer::SetString - invalid value for option " 
              + value);
     }
     
     if (index == (Integer) options.size())
     {
         options.push_back(value);
     }
     else  // substitute string for already existing one
        options[index] = value;
        
     return true;
   }
   if (id == OPTION_VALUES)
   {
      if ((index < 0) || (index > (Integer) optionValues.size()))
         throw SolverException("FminconOptimizer::SetString index out of range.");
     if (((Integer) options.size()) > index)
        if (!IsAllowedValue(options[index], value))
           throw SolverException(
           "FminconOptimizer::SetString - invalid value for option " 
           + options[index]);
      if (index == (Integer) optionValues.size())
      {
         optionValues.push_back(value);
      }
      else // replace a value currently there
         optionValues[index] = value;
         
      return true;
   }     
    return ExternalOptimizer::SetStringParameter(id, value, index);
}

std::string FminconOptimizer::GetStringParameter(const std::string &label,
                                                 const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool FminconOptimizer::SetStringParameter(const std::string &label, 
                                          const std::string &value,
                                          const Integer index)
{
   return SetStringParameter(GetParameterID(label),value,index);
}

//------------------------------------------------------------------------------
//  std::string  GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 */
//------------------------------------------------------------------------------
const StringArray& FminconOptimizer::GetStringArrayParameter(
                                                        const Integer id) const
{
        
    if (id == OPTIONS)
        return options;
    if (id == OPTION_VALUES)
       return optionValues;
        
    return ExternalOptimizer::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
void FminconOptimizer::CompleteInitialization()
{
   ExternalOptimizer::CompleteInitialization();
   // set callback class pointer for the GmatInterface -> probably don't need to
}

void FminconOptimizer::RunExternal()
{
   Optimize(); 
   currentState = FINISHED;
   nestedState  = INITIALIZING;
}

void FminconOptimizer::RunNominal()
{
   ++iterationsTaken;
   WriteToTextFile(nestedState);
}

void FminconOptimizer::CalculateParameters()
{
   //ExternalOptimizer::CalculateParameters(); 
   // check to make sure we have all of the data we need, from Minimize, etc.?
}

void FminconOptimizer::RunComplete()
{
   ExternalOptimizer::RunComplete();
   WriteToTextFile();
}


void FminconOptimizer::FreeArrays()
{
   ExternalOptimizer::FreeArrays();
   //options.clear();
   //optionValues.clear();
}

//std::string FminconOptimizer::GetProgressString()
//{
// moved to Optimizer class
//}

void FminconOptimizer::WriteToTextFile(SolverState stateToUse)
{
   StringArray::iterator current;
   Integer i;
   
   SolverState trigger = currentState;
   if (stateToUse != UNDEFINED_STATE)
      trigger = stateToUse;
      
   if (initialized)
   {
      std::stringstream message;

      switch (trigger)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Optimize
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size();
               Integer localEqCount = eqConstraintNames.size();
               Integer localIneqCount = ineqConstraintNames.size();
               message << "************************************************"
                        << "********\n"
                        << "*** Optimizer Text File\n"
                        << "*** \n"
                        << "*** Using Fmincon Optimization\n***\n";

               // Write out the setup data
               message << "*** " << localVariableCount << " variables\n*** "
                        << localEqCount << " equality constraints\n***\n*** "
                        << localIneqCount << " inequality constraints\n***\n*** "
                        << "Variables:\n***    ";

               // Iterate through the variables and constraints, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  message << *current << "\n***    ";
               }
                 
               message << "\n*** Equality Constraints:\n***    ";
                 
               for (current = eqConstraintNames.begin(), i = 0;
                    current != eqConstraintNames.end(); ++current)
               {
                  message << *current << "\n***    ";
               }
                 
               message << "\n*** Inequality Constraints:\n***    ";
                 
               for (current = ineqConstraintNames.begin(), i = 0;
                    current != ineqConstraintNames.end(); ++current)
               {
                  message << *current << "\n***    ";
               }
               message << "\n****************************"
                        << "****************************\n"
                        << std::endl;
            }
            break;
            
         case NOMINAL:
            message << "Iteration " << iterationsTaken
                     << "\n   Variable Values: [";
            // Iterate through the variables, writing them to the file
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               message << " " << variable[i++] << " ";
            }
            message << "]" << std::endl;
            break;
            
//         case PERTURBING:
//            if ((textFileMode == "Verbose") || (textFileMode == "Debug"))
//            {
//               if (pertNumber != 0)
//               {
//                  /*
//                  // Iterate through the goals, writing them to the file
//                  textFile << "Goals and achieved values:\n   ";
//                   
//                  for (current = goalNames.begin(), i = 0;
//                       current != goalNames.end(); ++current)
//                  {
//                     textFile << *current << "  Desired: " << goal[i]
//                              << " Achieved: " << achieved[pertNumber-1][i]
//                              << "\n   ";
//                     ++i;
//                  }
//                  */
//                  textFile << std::endl;
//               }
//               textFile << "Perturbing with variable values:\n   ";
//               for (current = variableNames.begin(), i = 0;
//                    current != variableNames.end(); ++current)
//               {
//                  textFile << *current << " = " << variable[i++] << "\n   ";
//               }
//               textFile << std::endl;
//            }
//            
//            if (textFileMode == "Debug")
//            {
//               textFile << "------------------------------------------------\n"
//                        << "Command stream data:\n"
//                        << debugString << "\n"
//                        << "------------------------------------------------\n";
//            }
//                
//            break;
            
         case CALCULATING:
            if (textFileMode == "Verbose")
               message << "In the Calculating state" << std::endl;
            if (ineqConstraintCount > 0)
            {
               message << "   Inequality Constraint Values: [";
               for (std::vector<Real>::iterator i = ineqConstraintValues.begin();
                    i != ineqConstraintValues.end(); ++i)
                  message << " " << (*i) << " ";
               message << "]" << std::endl;
            }
            if (eqConstraintCount > 0)
            {
               message << "   Equality Constraint Values: [";

               for (std::vector<Real>::iterator i = eqConstraintValues.begin();
                    i != eqConstraintValues.end(); ++i)
                  message << " " << (*i) << " ";
               message << "]" << std::endl;
            }
            message << "   Objective function value:  " << cost << std::endl;
            break;
            
//         case CHECKINGRUN:
//            // Iterate through the goals, writing them to the file
//            /*
//            textFile << "Goals and achieved values:\n   ";
//                
//            for (current = goalNames.begin(), i = 0;
//                 current != goalNames.end(); ++current)
//            {
//               textFile << *current << "  Desired: " << goal[i]
//                        << " Achieved: " << nominal[i]
//                        << "\n   Tolerance: " << tolerance[i]
//                        << "\n   ";
//               ++i;
//            }
//            */    
//            textFile << "\n*****************************"
//                     << "***************************\n"
//                     << std::endl;
//            break;
            
         case FINISHED:
            message << "\n****************************"
                     << "****************************\n"
                     << "*** Optimization Completed in " << iterationsTaken
                     << " iterations"
                     << "\n****************************"
                     << "****************************\n"
                     << std::endl;
                         
            break;
            
         case RUNEXTERNAL:
            message << "Iteration " << iterationsTaken+1
                     << "\nExternal Run\nVariables:\n   ";
            // Iterate through the variables, writing them to the file
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               message << *current << " = " << variable[i++] << "\n   ";
            }
            message << std::endl;
            break;
            
         default:
            MessageInterface::ShowMessage(
            "Solver state %d not supported for FminconOptimizer\n", 
            currentState);
            //throw SolverException(
            //   "Solver state not supported for the Fmincon optimizer");
      }
      textFile << message.str();
      MessageInterface::ShowMessage("%s", message.str().c_str());
   }
}
 
//#define __USE_MATLAB__
bool FminconOptimizer::OpenConnection()
{
   
   #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Entering FminconOptimizer::OpenConnection");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

#if defined __USE_MATLAB__
   // open the MatlabInterface (which is currently a static class)
   if (!MatlabInterface::Open())
      throw SolverException("Error attempting to access interface to MATLAB");
      
   // clear the last error message
   MatlabInterface::EvalString("clear errormsg");

   // set the path 
   if (functionPath != "")
   {
      std::string setPath = "path(path ,'" + functionPath + "')";
      MatlabInterface::RunMatlabString(setPath);
   #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("MATLAB path set to %s\n", 
      functionPath.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   }

   // check for availability of Optimization Toolbox (well, really
   // just fmincon, but of course, its existence implies the existence of
   // the entire toolbox) here 
   double      outArr[1], outArr2[1];
   double      out1[1], out2[1], out3[1], out4[1];
   int         OKint = 0;
   std::string evalStr   = "fminconexist = exist(\'fmincon\');";
   std::string resStr    = "fminconexist";
   MatlabInterface::RunMatlabString(evalStr);
   OKint                 = MatlabInterface::GetRealArray(resStr, 1, outArr);

   if (!OKint)
      throw SolverException(
            "Error determining existence of Optimization Toolbox");
   if (outArr[0] > 0.0) // 2 means it is in the MATLAB path
   {
      #ifdef DEBUG_ML_CONNECTIONS
         MessageInterface::ShowMessage(
         "fmincon exists (code = %.4f)\n", outArr[0]);
      #endif
      evalStr   = "startupexist = exist(\'gmat_startup\');";
      resStr    = "startupexist";
      MatlabInterface::RunMatlabString(evalStr);
      OKint                 = MatlabInterface::GetRealArray(resStr, 1, outArr2); 
      if (!OKint)
         throw SolverException(
               "Error determining existence of MATLAB gmat_startup");
      if (outArr2[0] > 0.0) // 2 means it is in the MATLAB path
      {
         #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "gmat_startup exists (code = %.4f), running gmat_startup\n", 
            outArr2[0]);
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         // run the startup file to add to the MATLAB path correctly
         evalStr = "gmat_startup;";
         MatlabInterface::RunMatlabString(evalStr);
         // check for existence on the path of the support MATLAB files 
         evalStr = "driverexist = exist(\'GmatFminconOptimizationDriver\');"; 
         resStr  = "driverexist";  
         MatlabInterface::RunMatlabString(evalStr);
         OKint                 = MatlabInterface::GetRealArray(resStr, 1, out1); 
         evalStr = "objectiveexist = exist(\'EvaluateGMATObjective\');"; 
         resStr  = "objectiveexist";  
         MatlabInterface::RunMatlabString(evalStr);
         OKint                 = MatlabInterface::GetRealArray(resStr, 1, out2); 
         evalStr = "constraintexist = exist(\'EvaluateGMATConstraints\');"; 
         resStr  = "constraintexist";  
         MatlabInterface::RunMatlabString(evalStr);
         OKint                 = MatlabInterface::GetRealArray(resStr, 1, out3); 
         evalStr = "callbackexist = exist(\'CallGMATfminconSolver\');"; 
         resStr  = "callbackexist";  
         MatlabInterface::RunMatlabString(evalStr);
         OKint                 = MatlabInterface::GetRealArray(resStr, 1, out4); 
         #ifdef DEBUG_ML_CONNECTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "existence codes for support files  = %.4f  %.4f  %.4f  %.4f\n", 
            out1[0], out2[0], out3[0], out4[0]);
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         
         if ((out1[0] <= 0.0) || (out2[0] <= 0.0) ||
             (out3[0] <= 0.0) || (out4[0] <= 0.0))
         {
            MessageInterface::ShowMessage("ERROR - MATLAB support files not in MATLAB path");
            sourceReady = false;
            return false;
         }
         /// start the GMATServer here
//         static GmatMainFrame *theMain = GmatAppData::Instance()->GetMainFrame();
//         theMain->StartServer();
         if (inSource == NULL) inSource = GmatInterface::Instance();
         if (inSource == NULL)  
            throw SolverException(
            "Error attempting to access GMAT server (to MATLAB)");
         // ********* BEGIN temporary prototype, testing, etc. *****************************//
         //inSource->RegisterCallbackServer(this);
         //evalStr = "bogusFunction;";
         //MessageInterface::ShowMessage("Now trying to execute bogusFunction ...\n");
         //int OK = MatlabInterface::EvalString(evalStr);
         //if (OK == 0) 
         //   MessageInterface::ShowMessage("ERROR calling EvalString\n");
         //MessageInterface::ShowMessage("Done executing bogusFunction\n");
         // ********* END temporary prototype, testing, etc. *****************************//
         sourceReady = true;
      }
      else
      {
         MessageInterface::ShowMessage("ERROR - gmat_startup.m not in MATLAB path");
         sourceReady = false;
      }
   }
   else
   {
      MessageInterface::ShowMessage("fmincon not in MATLAB path");
      sourceReady = false;
   }
   return sourceReady; 
#else
   throw SolverException("MATLAB required for FminconOptimizer");
#endif
return false;
}

void FminconOptimizer::CloseConnection()
{
#if defined __USE_MATLAB__
   MatlabInterface::Close();   // but wait!  what if someone else is still using it??
#endif
   // no need to close anything when running the server??

}


//------------------------------------------------------------------------------
// void WriteParameters(std::string &prefix, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 * 
 * @param prefix Starting portion of the script string used for the parameter.
 * @param obj The object that is written.
 */
//------------------------------------------------------------------------------
void FminconOptimizer::WriteParameters(Gmat::WriteMode mode, std::string &prefix, 
                                 std::stringstream &stream)
{
   Integer id;
   Gmat::ParameterType parmType;
   std::stringstream value;
   value.precision(GetDataPrecision()); 
   for (id = 0; id < parameterCount; ++id)
   {
      if (IsParameterReadOnly(id) == false) 
      {
         parmType = GetParameterType(id);
         if (parmType != Gmat::STRINGARRAY_TYPE)
         {  
            parmType = GetParameterType(id);
            // Skip unhandled types
            if (
                (parmType != Gmat::UNSIGNED_INTARRAY_TYPE) &&
                (parmType != Gmat::RVECTOR_TYPE) &&
                (parmType != Gmat::RMATRIX_TYPE) &&
                (parmType != Gmat::UNKNOWN_PARAMETER_TYPE)
               )
            {
               // Fill in the l.h.s.
               value.str("");
               WriteParameterValue(id, value);
               if (value.str() != "")
               {
                    std::string attCmtLn = GetAttributeCommentLine(id);
                    
                    if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) || 
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                     stream << attCmtLn;
                     
                  stream << prefix << GetParameterText(id)
                         << " = " << value.str() << ";";
                  
                  // overwrite tmp variable for attribute cmt line
                  attCmtLn = GetInlineAttributeComment(id);
                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) || 
                      (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                     stream << attCmtLn << "\n";
                  else
                     stream << "\n";
               }
            }
         }
         else
         {
            // Handle StringArrays
            if ((id != OPTIONS) && (id != OPTION_VALUES))
            {
               StringArray sar = GetStringArrayParameter(id);
               if (sar.size() > 0)
               {
                  std::string attCmtLn = GetAttributeCommentLine(id);
                       
                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) || 
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                  {
                     stream << attCmtLn.c_str();
                  }
           
                  stream << prefix << GetParameterText(id) << " = {";
                  
                  for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n)
                  {
                     if (n != sar.begin())
                        stream << ", ";
                     if (inMatlabMode)
                        stream << "'";
                     stream << (*n);
                     if (inMatlabMode)
                        stream << "'";
                  }
                  
                  attCmtLn  = GetInlineAttributeComment(id);
                  
                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) || 
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                  {
                     stream << "};" << attCmtLn << "\n";
                  }
                  else
                     stream << "};\n";
               }
            }
            else if (id == OPTIONS)
            {
               // Handle options list
               for (Integer ii = 0; ii < NUM_MATLAB_OPTIONS; ii++)
               {
                  value.str("");
                  WriteParameterValue((ii + MATLAB_OPTIONS_OFFSET), value);
                  if (value.str() != "")
                  {
                       std::string attCmtLn = GetAttributeCommentLine(id);
                       
                       if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) || 
                        (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                        stream << attCmtLn;
                        
                     stream << prefix << GetParameterText(ii + MATLAB_OPTIONS_OFFSET)
                            << " = " << value.str() << ";";
                     
                     // overwrite tmp variable for attribute cmt line
                     attCmtLn = GetInlineAttributeComment(id);
                     if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) || 
                         (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                        stream << attCmtLn << "\n";
                     else
                        stream << "\n";
                  }
               }
            }
            else 
               ; // do nothing for OPTION_VALUES - handled with OPTIONS      
         }
      }
   } // for 
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


bool FminconOptimizer::IsAllowedOption(const std::string &str)
{
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
      if (str == FminconOptimizer::ALLOWED_OPTIONS[i])  return true;
   return false;
}

bool FminconOptimizer::IsAllowedValue(const std::string &opt,
                                      const std::string &val)
{
   if ((opt == ALLOWED_OPTIONS[0]) ||
       (opt == ALLOWED_OPTIONS[1]) || // need to check for this being <= DiffMaxChange
       (opt == ALLOWED_OPTIONS[4]) ||
       (opt == ALLOWED_OPTIONS[5]) ||
       (opt == ALLOWED_OPTIONS[6]))
   {
      //if (atof(val.c_str()) > 0.0)  return true;
      //return false;
      Real tmpVal;
      if (!GmatStringUtil::ToReal(val.c_str(), &tmpVal)) return false;
      return true;
   }
   else if ((opt == ALLOWED_OPTIONS[2]) || 
            (opt == ALLOWED_OPTIONS[3]))
   {
      if (atoi(val.c_str()) > 0) return true;
      return false;
   }
   else if ((opt == ALLOWED_OPTIONS[7]) ||
            (opt == ALLOWED_OPTIONS[8]) ||
            (opt == ALLOWED_OPTIONS[10]) ||
            (opt == ALLOWED_OPTIONS[11]))
   {
      if ((val == "On") || (val == "Off") ||
          (val == "ON") || (val == "OFF") ||
          (val == "on") || (val == "off")) return true;
      return false;
   }
   else if (opt == ALLOWED_OPTIONS[9])
   {
      if ((val == "Iter")   || (val == "Off")   ||
          (val == "Notify") || (val == "Final") ||
          (val == "ITER")   || (val == "OFF")   ||
          (val == "NOTIFY") || (val == "FINAL") ||
          (val == "iter")   || (val == "off")   ||
          (val == "notify") || (val == "final")) return true;
      return false;
   }
   else
      return false;
}
