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
#include "Rmatrix.hpp"


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
    indx                    (NULL),
    b                       (NULL),
    ludMatrix               (NULL),
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
    // textFileMode = "Verbose";
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
    indx                    (NULL),
    b                       (NULL),
    ludMatrix               (NULL),
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


/**
 * Derived classes use this method to pass in parameter data specific to
 * the algorithm implemented.
 * 
 * @param <data> An array of data appropriate to the variables used in the 
 *               algorithm.
 * @param <name> A label for the data parameter.  Defaults to the empty 
 *               string.
 * 
 * @return The ID used for the variable.
 */
Integer DifferentialCorrector::SetSolverVariables(Real *data, std::string name)
{
    if (variableNames[variableCount] != name)
        throw SolverException("Mismatch between parsed and configured variable");
    variable[variableCount] = data[0];
    perturbation[variableCount] = data[1];
    variableMinimum[variableCount] = data[2];
    variableMaximum[variableCount] = data[3];
    variableMaximumStep[variableCount] = data[4];
    ++variableCount;
    return variableCount-1;
}


/**
 * Derived classes use this method to pass in parameter data specific to
 * the algorithm implemented.
 * 
 * @param <id> The ID used for the variable.
 * 
 * @return The value used for this variable
 */
Real DifferentialCorrector::GetSolverVariable(Integer id)
{
    return variable[id];
}


/**
 * Sets up the data fields used for the results of an iteration.
 * 
 * @param <data> An array of data appropriate to the results used in the 
 *               algorithm (for instance, tolerances for targeter goals).
 * @param <name> A label for the data parameter.  Defaults to the empty 
 *               string.
 * 
 * @return The ID used for this variable.
 */
Integer DifferentialCorrector::SetSolverResults(Real *data, std::string name)
{
    if (goalNames[goalCount] != name)
        throw SolverException("Mismatch between parsed and configured goal");
    goal[goalCount] = data[0];
    tolerance[goalCount] = data[1];
    ++goalCount;
    return goalCount-1;
}


/**
 * Passes in the results obtained from a run in the solver loop.
 * 
 * @param <id> The ID used for this result.
 * @param <value> The corresponding result.
 */
void DifferentialCorrector::SetResultValue(Integer id, Real value)
{
    if (currentState == NOMINAL) {
        nominal[id] = value;
    }
        
    if (currentState == PERTURBING) {
        achieved[pertNumber][id] = value;
    }
}


bool DifferentialCorrector::Initialize(void)
{
    // Setup the variable data structures
    Integer localVariableCount = variableNames.size();
    Integer localGoalCount = goalNames.size();
    
    if (localVariableCount > localVariableCount) {
        std::string errorMessage = "Targeter cannot initialize: ";
        errorMessage += "More goals than variables";
        throw SolverException(errorMessage);
    }
    
    FreeArrays();
    
    variable            = new Real[localVariableCount];
    perturbation        = new Real[localVariableCount];
    variableMinimum     = new Real[localVariableCount];
    variableMaximum     = new Real[localVariableCount];
    variableMaximumStep = new Real[localVariableCount];
    
    // Setup the goal data structures
    goal      = new Real[localGoalCount];
    tolerance = new Real[localGoalCount];
    nominal   = new Real[localGoalCount];
    
    // And the sensitivity matrix
    Integer i;
    achieved        = new Real*[localVariableCount];
    jacobian        = new Real*[localVariableCount];
    inverseJacobian = new Real*[localVariableCount];
    ludMatrix       = new Real*[localVariableCount];
    for (i = 0; i < localVariableCount; ++i) {
        jacobian[i] = new Real[localVariableCount];
        inverseJacobian[i] = new Real[localVariableCount];
        achieved[i] = new Real[localGoalCount];
        ludMatrix[i] = new Real[localVariableCount];
        
        // Initialize to the identity matrix
        jacobian[i][i] = 1.0;
        inverseJacobian[i][i] = 1.0;
        
        // Set default values for min and max parameters
        variable[i]            =  0.0;
        variableMinimum[i]     = -9.999e300;
        variableMaximum[i]     =  9.999e300;
        variableMaximumStep[i] =  9.999e300;
    }
    
    // Prepare the text file for output
    if (solverTextFile != "") {
        textFile.open(solverTextFile.c_str());
        textFile.precision(16);
        WriteToTextFile();
    }
    
    // Allocate the LU arrays
    indx = new Integer[variableCount];
    b = new Real[variableCount];
    
    initialized = true;
    iterationsTaken = 0;
    return true;
}


Solver::SolverState DifferentialCorrector::AdvanceState(void)
{
    switch (currentState) {
        case INITIALIZING:
            WriteToTextFile();
            CompleteInitialization();
            break;
            
        case NOMINAL:
            RunNominal();
            break;
        
        case PERTURBING:
            RunPerturbation();
            break;
        
        case CALCULATING:
            CalculateParameters();
            break;
        
        case CHECKINGRUN:
            CheckCompletion();
            ++iterationsTaken;
            if (iterationsTaken > maxIterations)
                currentState = FINISHED;
            break;
        
        case FINISHED:
            RunComplete();
            break;
        
        case ITERATING:             // Intentional drop-through
        default:
            throw SolverException("Solver state not supported for the targeter");
    }
     
    return currentState;
}


/**
 * Run out the nominal data
 */
void DifferentialCorrector::RunNominal(void)
{
    // On success, set the state to the next machine state
    WriteToTextFile();
    currentState = CHECKINGRUN;
}


void DifferentialCorrector::RunPerturbation(void)
{
    // Calculate the perts, one at a time
    if (pertNumber != -1)
        // Back out the last pert applied
        variable[pertNumber] -= perturbation[pertNumber];
    ++pertNumber;
    if (pertNumber == variableCount) {  // Current set of perts have been run
        currentState = CALCULATING;
        pertNumber = -1;
        return;
    }
    variable[pertNumber] += perturbation[pertNumber];
    WriteToTextFile();
}


void DifferentialCorrector::CalculateParameters(void)
{
    // Build and invert the sensitivity matrix
    CalculateJacobian();
    InvertJacobian();
    
    Real delta;
    // Apply the inverse Jacobian to build the next set of variables
    for (Integer i = 0; i < variableCount; ++i) {
        delta = 0.0;
        for (Integer j = 0; j < goalCount; j++)
            delta += inverseJacobian[j][i] * (goal[j] - nominal[j]);
        variable[i] += delta;
    }
    
    WriteToTextFile();
    currentState = NOMINAL;
}


void DifferentialCorrector::CheckCompletion(void)
{
    WriteToTextFile();
    bool converged = true;          // Assume convergence
    
    // check for lack of convergence
    for (Integer i = 0; i < goalCount; ++i) {
        if (fabs(nominal[i] - goal[i]) > tolerance[i])
            converged = false;
    }

    if (!converged) {
        // Set to run perts if not converged
        pertNumber = -1;
        // Build the first perturbation
        currentState = PERTURBING;
        RunPerturbation();
    }
    else
        // If converged, we're done
        currentState = FINISHED;
}


void DifferentialCorrector::RunComplete(void)
{
    WriteToTextFile();
}


/** 
 * Calculates the matrix of derivatives of the goals with respect to the 
 * variables.
 */
void DifferentialCorrector::CalculateJacobian(void)
{
    Integer i, j;
    
    for (i = 0; i < variableCount; ++i) {
    
        for (j = 0; j < goalCount; ++j) {
            jacobian[i][j] = achieved[i][j] - nominal[j];
            jacobian[i][j] /= perturbation[i];
        }
    }
}


/** 
 * Inverts the matrix of derivatives so that the change in the variables can be
 * estimated.
 */
void DifferentialCorrector::InvertJacobian(void)
{
/* Old code
    LUDecompose();

    for (Integer j = 0; j < variableCount; ++j) {
        for (Integer i = 0; i < variableCount; ++i)
            b[i] = 0.0;
        b[j] = 1.0;
        LUBackSubstitute();
        for (Integer i = 0; i < variableCount; ++i)
            inverseJacobian[i][j] = b[i];
    }
*/
   Rmatrix jac(variableCount, goalCount);
   for (Integer i = 0; i < variableCount; ++i)
      for (Integer j = 0; j < variableCount; ++j)
         jac(i,j) = jacobian[i][j];
         
   Rmatrix inv = jac.Inverse();
   for (Integer i = 0; i < variableCount; ++i)
      for (Integer j = 0; j < variableCount; ++j)
         inverseJacobian[i][j] = inv(i,j);
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
    
    if (indx) {
        delete [] indx;
        indx = NULL;
    }
    
    if (b) {
        delete [] b;
        b = NULL;
    }
    
    if (ludMatrix) {
        for (Integer i = 0; i < variableCount; ++i)
            delete [] ludMatrix[i];
        delete [] ludMatrix;
        ludMatrix = NULL;
    }
}


void DifferentialCorrector::WriteToTextFile(void)
{
    StringArray::iterator current;
    Integer i, j;
    if (!initialized) {
        // Variables and goals are not yet fully initialized, so the counts are 
        // still 0.
        Integer localVariableCount = variableNames.size(), 
                localGoalCount = goalNames.size();
        textFile << "********************************************************\n"
                 << "*** Targeter Text File\n"
                 << "*** \n"
                 << "*** Using Differential Correction\n***\n";
                 
        // Write out the setup data
        textFile << "*** " << localVariableCount << " variables\n*** "
                 << localGoalCount << " goals\n***\n*** Variables:\n***    ";
                 
        // Iterate through the variables and goals, writing them to the file
        for (current = variableNames.begin(), i = 0; 
             current != variableNames.end(); ++current) 
        {
             textFile << *current << "\n***    ";
        }
        
        textFile << "\n*** Goals:\n***    ";
        
        for (current = goalNames.begin(), i = 0; 
             current != goalNames.end(); ++current) 
        {
             textFile << *current << "\n***    ";
        }
        
        textFile << "\n****************************" 
                 << "****************************\n"
                 << std::endl;
    }
    else {
        switch (currentState) {
            case INITIALIZING:
                // This state is basically a "paused state" used for the Target
                // command to finalize the initial data for the variables and 
                // goals.  No output is written here.
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
                if (textFileMode == "Verbose") {
                   if (pertNumber != 0) {
                   
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
                
                break;
            
            case CALCULATING:
                if (textFileMode == "Verbose") {
                   textFile << "Calculating" << std::endl;
                   
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
                   textFile << std::endl;
                }
                
                textFile << "\nJacobian (Sensitivity matrix):\n";
                for (i = 0; i < variableCount; ++i) {
                    for (j = 0; j < goalCount; ++j) {
                        textFile << "   " << jacobian[i][j];
                    }
                    textFile << "\n";
                }
            
                textFile << "\n\nInverse Jacobian:\n";
                for (i = 0; i < variableCount; ++i) {
                    for (j = 0; j < goalCount; ++j) {
                        textFile << "   " << inverseJacobian[i][j];
                    }
                    textFile << "\n";
                }
            
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
                textFile << "Goals and achieved values:\n   ";
                
                for (current = goalNames.begin(), i = 0; 
                     current != goalNames.end(); ++current) 
                {
                     textFile << *current << "  Desired: " << goal[i] 
                              << " Achieved: " << nominal[i] << "\n   ";
                     ++i;
                }
                
                textFile << "\n*****************************" 
                         << "***************************\n"
                         << std::endl;
                break;
            
            case FINISHED:
                textFile << "\n****************************" 
                         << "****************************\n"
                         << "*** Targeting Completed in " << iterationsTaken
                         << " iterations"
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


void DifferentialCorrector::LUDecompose(void)
{
    // Implementation as in Numerical Recipes -- needs to be reworked to real C
    Integer i, j, imax = 0, k;
    Real big, dum, sum, temp, d;
    Real *vv = new Real[variableCount];
    
    // Copy the Jacobian into ludMatrix
    for (i = 0; i < variableCount; ++i) 
        for (j = 0; j < goalCount; ++j) 
            ludMatrix[i][j] = jacobian[i][j];
        
    d = 1.0;
    for (i = 0; i < variableCount; ++i) {
        big = 0.0;
        for (j = 0; j < variableCount; ++j)
            if ((temp = fabs(ludMatrix[i][j])) > big)
                big = temp;
        if (big == 0.0)
            throw SolverException("Singular matrix in DifferentialCorrector::LUDecompose");
        vv[i] = 1.0 / big;
    }
    for (j = 0; j < variableCount; ++j) {
        for (i = 0; i < j; ++i) {
            sum = ludMatrix[i][j];
            for (k = 0; k < i; ++k)
                sum -= ludMatrix[i][k]*ludMatrix[k][j];
            // ludMatrix[i][j] = sum;
            ludMatrix[i][j] = sum;
        }
        big = 0.0;
        for (i = j; i < variableCount; ++i) {
            sum = ludMatrix[i][j];
            for (k = 0; k < j; ++k)
                sum -= ludMatrix[i][k] * ludMatrix[k][j];
            ludMatrix[i][j] = sum;
            if ((dum = vv[i]*fabs(sum)) >= big) {
                big = dum;
                imax = i;
            }
        }
        if (j != imax) {
           for (k = 0; k < variableCount; ++k) {
               dum = ludMatrix[imax][k];
               ludMatrix[imax][k] = ludMatrix[j][k];
               ludMatrix[j][k] = dum;
           }
           d = -d;
           vv[imax] = vv[j];
        }
        indx[j] = imax;
        if (ludMatrix[j][j] == 0.0)
            ludMatrix[j][j] = 1.0e-20;           // Handle singular matrix
        if (j != variableCount-1) {
            dum = 1.0 / ludMatrix[j][j];
            for (i = j+1; i < variableCount; ++i)
                ludMatrix[i][j] *= dum;
        }
    }
    delete [] vv;
}

    
void DifferentialCorrector::LUBackSubstitute(void)
{
    Integer i, ii=0, ip, j;
    Real sum;
    
    for (i = 0; i < variableCount; ++i) {
        ip = indx[i];
        sum = b[ip];
        b[ip] = b[i];
        if (ii)
            for (j = ii; j <= i-1; ++j)
                sum -= ludMatrix[i][j] * b[j];
        else if (sum)
            ii = i;
        b[i] = sum;
    }
    for (i = variableCount-1; i >= 0; --i) {    // Back-substitute
        sum = b[i];
        for (j = i+1; j < variableCount; ++j)
            sum -= ludMatrix[i][j] * b[j];
        b[i] = sum / ludMatrix[i][i];
    }
}

