//$Header$
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


#include "FminconOptimizer.hpp"
#include "MessageInterface.hpp"
#include "MatlabInterface.hpp"  // currently all static

//#define DEBUG_STATE_MACHINE

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

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

FminconOptimizer::FminconOptimizer(std::string name) :
   ExternalOptimizer       ("FminconOptimizer", name)
 {
   objectTypeNames.push_back("FminconOptimizer");
   parameterCount = FminconOptimizerParamCount;
}


FminconOptimizer::~FminconOptimizer()
{
   FreeArrays();
}

FminconOptimizer::FminconOptimizer(const FminconOptimizer &opt) :
   ExternalOptimizer       (opt)
{
   options          = opt.options;
   optionValues     = opt.optionValues;
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
  
   return *this;
}

bool FminconOptimizer::Initialize()
{
   ExternalOptimizer::Initialize();
   
   // what else goes in here?
   
   return true;
}

Solver::SolverState FminconOptimizer::AdvanceState()
{
    switch (currentState)
   {
      case INITIALIZING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; INITIALIZING\n");
         #endif
         WriteToTextFile();
         ReportProgress();
         CompleteInitialization();
         break;
            
      case NOMINAL:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; NOMINAL\n");
         #endif
         ReportProgress();
         RunNominal();
         ReportProgress();
         break;
        
      case PERTURBING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; PERTURBING\n");
         #endif
         ReportProgress();
         //RunPerturbation();
         break;
        
      case CALCULATING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; CALCULATING\n");
         #endif
         ReportProgress();
         CalculateParameters();
         break;
        
      case CHECKINGRUN:
          #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; CHECKINGRUN\n");
         #endif
        //CheckCompletion();
         ++iterationsTaken;
         if (iterationsTaken > maxIterations)
         {
            MessageInterface::ShowMessage("FminconOptimizer %s %s\n",
               instanceName.c_str(),
               "has exceeded to maximum number of allowed iterations.");
            currentState = FINISHED;
         }
         break;
        
      case FINISHED:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; FINISHED\n");
         #endif
         RunComplete();
         ReportProgress();
         break;
        
      case ITERATING:             // Intentional drop-through
      default:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage("Entered FminconOptimizer state machine; "
               "Bad state for an FminconOptimizer.\n");
         #endif
         throw SolverException("Solver state not supported for the optimizer");
   }
     
   return currentState;
}

bool FminconOptimizer::Optimize()
{
   return true;   // ********** TBD **********
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
void FminconOptimizer::RunNominal()
{
   // On success, set the state to the next machine state
   WriteToTextFile();
   //currentState = CHECKINGRUN;  // what to do here?
}

void FminconOptimizer::CalculateParameters()
{
   ExternalOptimizer::CalculateParameters(); // **** TBD <<<<<<<<<< **** 
   // call fmincon here, passsing in the options and their values ???
   // use EvalString or a MatlabFunction?
}

void FminconOptimizer::RunComplete()
{
   ExternalOptimizer::RunComplete();
   WriteToTextFile();
}


void FminconOptimizer::FreeArrays()
{
   ExternalOptimizer::FreeArrays();
   options.clear();
   optionValues.clear();
}

std::string FminconOptimizer::GetProgressString()
{
   StringArray::iterator current;
   Integer i;
   std::stringstream progress;
   progress.str("");
   progress.precision(12);

   if (initialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Optimize
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size();
               Integer localEqCount       = eqConstraintNames.size();
               Integer localIneqCount     = ineqConstraintNames.size();
               progress << "************************************************"
                        << "********\n"
                        << "*** Performing Fmincon Optimization "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "*** " << localVariableCount << " variables; "
                        << localEqCount << " equality constraints; "
                        << localIneqCount << " inequality constraints\n   Variables:  ";

               // Iterate through the variables and goals, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  if (current != variableNames.begin())
                     progress << ", ";
                  progress << *current;
               }

               progress << "\n   Equality Constraints:  ";

               for (current = eqConstraintNames.begin(), i = 0;
                    current != eqConstraintNames.end(); ++current)
               {
                  if (current != eqConstraintNames.begin())
                     progress << ", ";
                  progress << *current;
               }

               progress << "\n   Inequality Constraints:  ";

               for (current = ineqConstraintNames.begin(), i = 0;
                    current != ineqConstraintNames.end(); ++current)
               {
                  if (current != ineqConstraintNames.begin())
                     progress << ", ";
                  progress << *current;
               }

               progress << "\n****************************"
                        << "****************************";
            }
            break;

         case NOMINAL:
            progress << instanceName << " Iteration " << iterationsTaken+1
                     << "; Nominal Pass\n   Variables:  ";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               if (current != variableNames.begin())
                  progress << ", ";
               progress << *current << " = " << variable[i++];
            }
            break;

         case PERTURBING:  // does this apply to optimization??
            progress << "   Completed iteration " << iterationsTaken
                     << ", pert " << pertNumber+1 << " ("
                     << variableNames[pertNumber] << " = "
                     << variable[pertNumber] << ")\n";
            break;

         case CALCULATING:
            // Just forces a blank line
            break;

         case CHECKINGRUN:
            // Iterate through the constraints, writing them to the file
            progress << "   Equality Constraints and achieved values:\n      ";

            for (current = eqConstraintNames.begin(), i = 0;
                 current != eqConstraintNames.end(); ++current)
            {
               if (current != eqConstraintNames.begin())
                  progress << ",  ";
                  // does this make sense???
               //progress << *current << "  Desired: " << eqConstaint[i]
               //         << "  Achieved: " << nominal[i];
               ++i;
            }

           progress << "   Inequality Constraints and achieved values:\n      ";

            for (current = ineqConstraintNames.begin(), i = 0;
                 current != ineqConstraintNames.end(); ++current)
            {
               if (current != ineqConstraintNames.begin())
                  progress << ",  ";
                  // does this make sense???
               //progress << *current << "  Desired: " << eqConstaint[i]
               //         << "  Achieved: " << nominal[i];
               ++i;
            }

            break;

         case FINISHED:
            progress << "\n*** Optimization Completed in " << iterationsTaken
                     << " iterations";
                     
            if (iterationsTaken > maxIterations)
               progress << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                     << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                     << "!!! WARNING: Optimizer did NOT converge!"
                     << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                     << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

            progress << "\nFinal Variable values:\n";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
               progress << "   " << *current << " = " << variable[i++] << "\n";
            break;

         case ITERATING:     // Intentional fall through
         default:
            throw SolverException(
               "Solver state not supported for the targeter");
      }
   }
   else
      return Solver::GetProgressString();
      
   return progress.str();

}

void FminconOptimizer::WriteToTextFile()
{
   StringArray::iterator current;
   Integer i;
   //Integer j;
   if (initialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Optimize
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size();
               Integer localEqCount = eqConstraintNames.size();
               Integer localIneqCount = ineqConstraintNames.size();
               textFile << "************************************************"
                        << "********\n"
                        << "*** Optimizer Text File\n"
                        << "*** \n"
                        << "*** Using Fmincon Optimization\n***\n";

               // Write out the setup data
               textFile << "*** " << localVariableCount << " variables\n*** "
                        << localEqCount << " equality constraints\n***\n*** "
                        << localIneqCount << " inequality constraints\n***\n*** "
                        << "Variables:\n***    ";

               // Iterate through the variables and constraints, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  textFile << *current << "\n***    ";
               }
                 
               textFile << "\n*** Equality Constraints:\n***    ";
                 
               for (current = eqConstraintNames.begin(), i = 0;
                    current != eqConstraintNames.end(); ++current)
               {
                  textFile << *current << "\n***    ";
               }
                 
               textFile << "\n*** Inequality Constraints:\n***    ";
                 
               for (current = ineqConstraintNames.begin(), i = 0;
                    current != ineqConstraintNames.end(); ++current)
               {
                  textFile << *current << "\n***    ";
               }
               textFile << "\n****************************"
                        << "****************************\n"
                        << std::endl;
            }
            break;
            
         case NOMINAL:
            textFile << "Iteration " << iterationsTaken+1
                     << "\nRunning Nominal Pass\nVariables:\n   ";
            // Iterate through the variables, writing them to the file
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               textFile << *current << " = " << variable[i++] << "\n   ";
            }
            textFile << std::endl;
            break;
            
         case PERTURBING:
            if ((textFileMode == "Verbose") || (textFileMode == "Debug"))
            {
               if (pertNumber != 0)
               {
                  /*
                  // Iterate through the goals, writing them to the file
                  textFile << "Goals and achieved values:\n   ";
                   
                  for (current = goalNames.begin(), i = 0;
                       current != goalNames.end(); ++current)
                  {
                     textFile << *current << "  Desired: " << goal[i]
                              << " Achieved: " << achieved[pertNumber-1][i]
                              << "\n   ";
                     ++i;
                  }
                  */
                  textFile << std::endl;
               }
               textFile << "Perturbing with variable values:\n   ";
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  textFile << *current << " = " << variable[i++] << "\n   ";
               }
               textFile << std::endl;
            }
            
            if (textFileMode == "Debug")
            {
               textFile << "------------------------------------------------\n"
                        << "Command stream data:\n"
                        << debugString << "\n"
                        << "------------------------------------------------\n";
            }
                
            break;
            
         case CALCULATING:
            if (textFileMode == "Verbose")
            {
               textFile << "Calculating" << std::endl;
               /*    
               // Iterate through the goals, writing them to the file
               textFile << "Goals and achieved values:\n   ";
                   
               for (current = goalNames.begin(), i = 0;
                    current != goalNames.end(); ++current)
               {
                   textFile << *current << "  Desired: " << goal[i]
                            << " Achieved: " << achieved[variableCount-1][i]
                            << "\n    ";
                   ++i;
               }
               */
               textFile << std::endl;
            }
                
            // does this make sense for Optimization?            
            textFile << "\n\nNew variable estimates:\n   ";
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               textFile << *current << " = " << variable[i++] << "\n   ";
            }
            textFile << std::endl;
            break;
            
         case CHECKINGRUN:
            // Iterate through the goals, writing them to the file
            /*
            textFile << "Goals and achieved values:\n   ";
                
            for (current = goalNames.begin(), i = 0;
                 current != goalNames.end(); ++current)
            {
               textFile << *current << "  Desired: " << goal[i]
                        << " Achieved: " << nominal[i]
                        << "\n   Tolerance: " << tolerance[i]
                        << "\n   ";
               ++i;
            }
            */    
            textFile << "\n*****************************"
                     << "***************************\n"
                     << std::endl;
            break;
            
         case FINISHED:
            textFile << "\n****************************"
                     << "****************************\n"
                     << "*** Optimization Completed in " << iterationsTaken
                     << " iterations"
                     << "\n****************************"
                     << "****************************\n"
                     << std::endl;
                         
            break;
            
         case ITERATING:     // Intentional fall through
         default:
            throw SolverException(
               "Solver state not supported for the Fmincon optimizer");
      }
   }
}
   
bool FminconOptimizer::OpenConnection()
{
   if (gmatServer == NULL) gmatServer = GmatInterface::Instance();
   if (gmatServer == NULL) 
      throw SolverException(
      "Error attempting to access GMAT interface (to MATLAB)");

   if (!MatlabInterface::Open())
      throw SolverException("Error attempting to access interface to MATLAB");
   
   // check for availability of Optimization Toolbox here .......
   sourceReady = true;
   return true; 
}

void FminconOptimizer::CloseConnection()
{
   MatlabInterface::Close();   // but wait!  what if someone else is still using it??

}


