//$Header$
//------------------------------------------------------------------------------
//                         DifferentialCorrector
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/29
//
/**
 * Implementation for the differential corrector targeter. 
 */
//------------------------------------------------------------------------------


#include "DifferentialCorrector.hpp"


DifferentialCorrector::DifferentialCorrector(std::string name) :
    Solver                  ("DifferentialCorrector", name), 
    variableCount           (0),
    goalCount               (0),
    iterationsTaken         (0),
    maxIterations           (25),
    variable                (NULL),
    perturbation            (NULL),
    variableMinimum         (NULL),
    variableMaximum         (NULL),
    variableMaximumStep     (NULL),
    goal                    (NULL),
    tolerance               (NULL),
    nominal                 (NULL),
    achieved                (NULL),
    jacobian                (NULL),
    inverseJacobian         (NULL),
    useCentralDifferences   (false),
    initialized             (false),
    solverTextFile          ("targeter.data"),
    solverTextFileID        (parameterCount),
    variableNamesID         (parameterCount+1),
    goalNamesID             (parameterCount+2),
    maxIterationsID         (parameterCount+3),
    useCentralDifferencingID(parameterCount+4)
{
    parameterCount += 5;
}


DifferentialCorrector::~DifferentialCorrector()
{
    FreeArrays();
}


DifferentialCorrector::DifferentialCorrector(const DifferentialCorrector &dc) :
    Solver                  (dc), 
    variableCount           (dc.variableCount),
    goalCount               (dc.goalCount),
    iterationsTaken         (0),
    maxIterations           (dc.maxIterations),
    variable                (NULL),
    perturbation            (NULL),
    variableMinimum         (NULL),
    variableMaximum         (NULL),
    variableMaximumStep     (NULL),
    goal                    (NULL),
    tolerance               (NULL),
    nominal                 (NULL),
    achieved                (NULL),
    jacobian                (NULL),
    inverseJacobian         (NULL),
    useCentralDifferences   (dc.useCentralDifferences),
    initialized             (false),
    solverTextFile          (dc.solverTextFile),
    solverTextFileID        (dc.solverTextFileID),
    variableNamesID         (dc.variableNamesID),
    goalNamesID             (dc.goalNamesID),
    maxIterationsID         (dc.maxIterationsID),
    useCentralDifferencingID(dc.useCentralDifferencingID)
{
    parameterCount = dc.parameterCount;
}


DifferentialCorrector& 
    DifferentialCorrector::operator=(const DifferentialCorrector& dc)
{
    if (&dc == this)
        return *this;
        
    return *this;
}


// Access methods overriden from the base class

std::string DifferentialCorrector::GetParameterText(const Integer id) const
{
    if (id == solverTextFileID)
        return "TargeterTextFile";
        
    if (id == variableNamesID)
        return "Variables";
        
    if (id == goalNamesID)
        return "Goals";
        
    if (id == maxIterationsID)
        return "MaximumIterations";
        
    if (id == useCentralDifferencingID)
        return "UseCentralDifferences";
        
    return Solver::GetParameterText(id);
}


Integer DifferentialCorrector::GetParameterID(const std::string &str) const
{
    if (str == "TargeterTextFile")
        return solverTextFileID;
        
    if (str == "Variables")
        return variableNamesID;
        
    if (str == "Goals")
        return goalNamesID;

    if (str == "MaximumIterations")
        return maxIterationsID;
        
    if (str == "UseCentralDifferences")
        return useCentralDifferencingID;
        
    return Solver::GetParameterID(str);
}


Gmat::ParameterType DifferentialCorrector::GetParameterType(const Integer id) const
{
    if (id == solverTextFileID)
        return Gmat::STRING_TYPE;
        
    if (id == variableNamesID)
        return Gmat::STRINGARRAY_TYPE;
        
    if (id == goalNamesID)
        return Gmat::STRINGARRAY_TYPE;
        
    if (id == maxIterationsID)
        return Gmat::INTEGER_TYPE;
        
    if (id == useCentralDifferencingID)
        return Gmat::BOOLEAN_TYPE;

    return Solver::GetParameterType(id);
}


std::string DifferentialCorrector::GetParameterTypeString(const Integer id) const
{
    if (id == solverTextFileID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == variableNamesID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];
        
    if (id == goalNamesID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];
        
    if (id == maxIterationsID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::INTEGER_TYPE];
        
    if (id == useCentralDifferencingID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];

    return Solver::GetParameterTypeString(id);
}



Integer DifferentialCorrector::GetIntegerParameter(const Integer id) const
{
    if (id == maxIterationsID)
        return maxIterations;
        
    return Solver::GetIntegerParameter(id);
}


Integer DifferentialCorrector::SetIntegerParameter(const Integer id, 
                                                   const Integer value)
{
    if (id == maxIterationsID) {
        if (value > 0)
            maxIterations = value;
        return maxIterations;
    }
    
    return Solver::SetIntegerParameter(id, value);
}


bool DifferentialCorrector::GetBooleanParameter(const Integer id) const
{
    if (id == useCentralDifferencingID)
        return useCentralDifferences;
        
    return Solver::GetBooleanParameter(id);
}


bool DifferentialCorrector::SetBooleanParameter(const Integer id, 
                                                const bool value)
{
    if (id == useCentralDifferencingID) {
        useCentralDifferences = value;
        return useCentralDifferences;
    }
        
    return Solver::SetBooleanParameter(id, value);
}


std::string DifferentialCorrector::GetStringParameter(const Integer id) const
{
    if (id == solverTextFileID)
        return solverTextFile;
        
    return Solver::GetStringParameter(id);
}


bool DifferentialCorrector::SetStringParameter(const Integer id, 
                                               const std::string &value)
{
    if (id == solverTextFileID) {
        solverTextFile = value;
        return true;
    }
        
    if (id == variableNamesID) {
        variableNames.push_back(value);
        return true;
    }
    
    if (id == goalNamesID) {
        goalNames.push_back(value);
        return true;
    }
    
    return Solver::SetStringParameter(id, value);
}


const StringArray& DifferentialCorrector::GetStringArrayParameter(
                                                        const Integer id) const
{
    if (id == variableNamesID)
        return variableNames;
        
    if (id == goalNamesID)
        return goalNames;
        
    return Solver::GetStringArrayParameter(id);
}


bool DifferentialCorrector::Initialize(void)
{
    // Setup the variable data structures
    variableCount = variableNames.size();
    goalCount = goalNames.size();
    
    if (goalCount > variableCount) {
        std::string errorMessage = "Targeter cannot initialize: ";
        errorMessage += "More goals than variables";
        throw SolverException(errorMessage);
    }
    
    FreeArrays();
    
    variable            = new Real[variableCount];
    perturbation        = new Real[variableCount];
    variableMinimum     = new Real[variableCount];
    variableMaximum     = new Real[variableCount];
    variableMaximumStep = new Real[variableCount];
    
    // Setup the goal data structures
    goal      = new Real[goalCount];
    tolerance = new Real[goalCount];
    nominal   = new Real[goalCount];
    
    // And the sensitivity matrix
    Integer i;
    achieved        = new Real*[goalCount];
    jacobian        = new Real*[variableCount];
    inverseJacobian = new Real*[variableCount];
    for (i = 0; i < variableCount; ++i) {
        jacobian[i] = new Real[variableCount];
        inverseJacobian[i] = new Real[variableCount];
        achieved[i] = new Real[variableCount];
        
        // Initialize to the identity matrix
        jacobian[i][i] = 1.0;
        inverseJacobian[i][i] = 1.0;
        
        // Set default values for min and max parameters
        variableMinimum[i]     = -9.999e300;
        variableMaximum[i]     =  9.999e300;
        variableMaximumStep[i] =  9.999e300;
    }
    
    // Prepare the text file for output
    if (solverTextFile != "") {
        textFile.open(solverTextFile.c_str());
        WriteToTextFile();
    }
    
    initialized = true;
    return true;
}


bool DifferentialCorrector::AdvanceState(void)
{
    switch (currentState) {
        case NOMINAL:
            RunNominal();
            break;
        
        case PERTURBING:
        case ITERATING:
        case CALCULATING:
        case CHECKINGRUN:
        case FINISHED:
        default:
            throw SolverException("Solver state not supported for the targeter");
    }
    
    return false;
}


void DifferentialCorrector::RunNominal(void)
{
    // On success, set the state to the next machine state
    
}


void DifferentialCorrector::RunPerturbation(void)
{
}


void DifferentialCorrector::CalculateParameters(void)
{
}


void DifferentialCorrector::CheckCompletion(void)
{
}


/** 
 * Calculates the matrix of derivatives of the goals with respect to the 
 * variables.
 */
void DifferentialCorrector::CalculateJacobian(void)
{
}


/** 
 * Inverts the matrix of derivatives so that the change in the variables can be
 * estimated.
 */
void DifferentialCorrector::InvertJacobian(void)
{
}


/**
 * Frees the memory used by the targeter, so it can be reused later in the 
 * sequence.  This method is also called by the destructor when the script is 
 * cleared.
 */
void DifferentialCorrector::FreeArrays(void)
{
    if (textFile.is_open()) {
        textFile.flush();
        textFile.close();
    }
        
    if (variable) {
        delete [] variable;
        variable = NULL;
    }
    
    if (perturbation) {
        delete [] perturbation;
        perturbation = NULL;
    }
            
    if (variableMinimum) {
        delete [] variableMinimum;
        variableMinimum = NULL;
    }

    if (variableMaximum) {
        delete [] variableMaximum;
        variableMaximum = NULL;
    }

    if (variableMaximumStep) {
        delete [] variableMaximumStep;
        variableMaximumStep = NULL;
    }

    if (goal) {
        delete [] goal;
        goal = NULL;
    }

    if (tolerance) {
        delete [] tolerance;
        tolerance = NULL;
    }

    if (nominal) {
        delete [] nominal;
        nominal = NULL;
    }
    
    if (achieved) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] achieved[i];
        delete [] achieved;
        achieved = NULL;
    }

    if (jacobian) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] jacobian[i];
        delete [] jacobian;
        jacobian = NULL;
    }

    if (inverseJacobian) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] inverseJacobian[i];
        delete [] inverseJacobian;
        inverseJacobian = NULL;
    }
}


void DifferentialCorrector::WriteToTextFile(void)
{
    StringArray::iterator current;
    Integer i;
    
    if (!initialized) {
        textFile << "********************************************************\n"
                 << "*** Targeter Text File\n"
                 << "*** \n"
                 << "*** Using Differential Correction\n***\n";
                 
        // Write out the setup data
        textFile << "*** " << variableCount << " variables\n*** "
                 << goalCount << " goals\n***\n*** Variables:\n***    ";
                 
        // Iterate through the variables and goals, writing them to the file
        for (current = variableNames.begin(), i = 0; 
             current != variableNames.end(); ++current) 
        {
             textFile << *current << " = " << variable[i++] << "\n***    ";
        }
        
        textFile << "\n*** Goals:\n***    ";
        
        for (current = goalNames.begin(), i = 0; 
             current != goalNames.end(); ++current) 
        {
             textFile << *current << "   Desired value: " 
                      << goal[i] << "\n***    ";
        }
        
        textFile << "\n****************************" 
                 << "****************************\n"
                 << std::endl;
    }
    else {
        switch (currentState) {
            case NOMINAL:
                textFile << "Running Nominal Pass\nVariables:\n   ";
                // Write out the nominal variable values
                
                // Iterate through the variables, writing them to the file
                for (current = variableNames.begin(), i = 0; 
                     current != variableNames.end(); ++current) 
                {
                     textFile << *current << " = " << variable[i++] << "\n   ";
                }
                break;
            
            case PERTURBING:
                break;
            
            case CALCULATING:
                break;
            
            case CHECKINGRUN:
                // Iterate through the goals, writing them to the file
                textFile << "\n*** Goals and achieved values:\n***    ";
                
                for (current = goalNames.begin(), i = 0; 
                     current != goalNames.end(); ++current) 
                {
                     textFile << *current << "   Desired value: " << goal[i] 
                              << "Current value: " << nominal[i++] << "\n***    ";
                }
                
                textFile << "\n*****************************" 
                         << "***************************\n\n"
                         << std::endl;
                break;
            
            case FINISHED:
                textFile << "\n****************************" 
                         << "****************************\n"
                         << "*** Targeting Complete!"
                         << "\n****************************" 
                         << "****************************\n"
                         << std::endl;
                break;
            
            case ITERATING:     // Intentional fall through
            default:
                throw SolverException("Solver state not supported for the targeter");
        }
    }
}
