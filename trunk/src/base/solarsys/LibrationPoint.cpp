//$Id$
//------------------------------------------------------------------------------
//                                  LibrationPoint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2005/04/04
//
/**
 * Implementation of the LibrationPoint class.
 *
 * @note This is an abstract class.
 */
//------------------------------------------------------------------------------

#include <vector>
#include "gmatdefs.hpp"
#include "CalculatedPoint.hpp"
#include "LibrationPoint.hpp"
#include "Barycenter.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "MessageInterface.hpp"

#include <iostream>
using namespace std;

//---------------------------------
// static data
//---------------------------------
const Real LibrationPoint::CONVERGENCE_TOLERANCE = 1.0e-8;
const Real LibrationPoint::MAX_ITERATIONS        = 2000;

const std::string
LibrationPoint::PARAMETER_TEXT[LibrationPointParamCount - CalculatedPointParamCount] =
{
   "Primary",
   "Secondary",
   "Point",
};

const Gmat::ParameterType
LibrationPoint::PARAMETER_TYPE[LibrationPointParamCount - CalculatedPointParamCount] =
{
   Gmat::OBJECT_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::STRING_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  LibrationPoint(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the LibrationPoint class
 * (default constructor).
 *
 * @param <ptType>  string representation of its body type
 * @param <itsName> parameter indicating the name of the LibrationPoint.
 */
//------------------------------------------------------------------------------
LibrationPoint::LibrationPoint(const std::string &itsName) :
CalculatedPoint("LibrationPoint", itsName),
primaryBodyName     (""),
secondaryBodyName   (""),
whichPoint          (""),
primaryBody         (NULL),
secondaryBody       (NULL)
{
   objectTypes.push_back(Gmat::LIBRATION_POINT);
   objectTypeNames.push_back("LibrationPoint");
   parameterCount = LibrationPointParamCount;
}

//------------------------------------------------------------------------------
//  LibrationPoint(const LibrationPoint &lp)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the LibrationPoint class as a copy of the
 * specified LibrationPoint class (copy constructor).
 *
 * @param <lp> LibrationPoint object to copy.
 */
//------------------------------------------------------------------------------
LibrationPoint::LibrationPoint(const LibrationPoint &lp) :
CalculatedPoint          (lp),
primaryBodyName          (lp.primaryBodyName),
secondaryBodyName        (lp.secondaryBodyName),
whichPoint               (lp.whichPoint),
primaryBody              (lp.primaryBody),
secondaryBody            (lp.secondaryBody)
{
}

//------------------------------------------------------------------------------
//  LibrationPoint& operator= (const LibrationPoint& lp)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the LibrationPoint class.
 *
 * @param <lp> the LibrationPoint object whose data to assign to "this"
 *             calculated point.
 *
 * @return "this" LibrationPoint with data of input LibrationPoint lp.
 */
//------------------------------------------------------------------------------
LibrationPoint& LibrationPoint::operator=(const LibrationPoint &lp)
{
   if (&lp == this)
      return *this;

   CalculatedPoint::operator=(lp);
   primaryBodyName     = lp.primaryBodyName;
   secondaryBodyName   = lp.secondaryBodyName;
   whichPoint          = lp.whichPoint;
   primaryBody         = lp.primaryBody;
   secondaryBody       = lp.secondaryBody;
   return *this;
}

//------------------------------------------------------------------------------
//  ~LibrationPoint()
//------------------------------------------------------------------------------
/**
 * Destructor for the LibrationPoint class.
 */
//------------------------------------------------------------------------------
LibrationPoint::~LibrationPoint()
{
}


//---------------------------------------------------------------------------
//  const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000 state of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the state is requested.
 *
 * @return state of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector6 LibrationPoint::GetMJ2000State(const A1Mjd &atTime)
{
   CheckBodies();
   // Compute position and velocity from primary to secondary
   Rvector6 primaryState = primaryBody->GetMJ2000State(atTime);
   Rvector6 pToS = (secondaryBody->GetMJ2000State(atTime)) -
                   primaryState;
   Rvector3 r    = pToS.GetR();
   Rvector3 v    = pToS.GetV();
   Rvector3 a    = (secondaryBody->GetMJ2000Acceleration(atTime)) -
                   (primaryBody->GetMJ2000Acceleration(atTime));
   
   Real     massPrimary, massSecondary;
   if ((primaryBody->GetType()) == Gmat::CELESTIAL_BODY)
      massPrimary = ((CelestialBody*) primaryBody)->GetMass();
   else  // Barycenter
      massPrimary = ((Barycenter*) primaryBody)->GetMass();
   if ((secondaryBody->GetType()) == Gmat::CELESTIAL_BODY)
      massSecondary = ((CelestialBody*) secondaryBody)->GetMass();
   else  // Barycenter
      massSecondary = ((Barycenter*) secondaryBody)->GetMass();
   if ((massPrimary == 0.0) && (massSecondary == 0.0))
      throw SolarSystemException(
            "Primary and secondary bodies for LibrationPoint are massless");
   Real muStar = massSecondary / (massPrimary + massSecondary);
   /*
   cout.setf(ios::fixed);
   cout.precision(30);
   
   cout << "Mass of the primary is "  << massPrimary << endl;
   cout << "Mass of the secondary is " << massSecondary << endl;
   cout << "Mu(star) = " << muStar << endl;
   */
   Real gamma = 0.0;
   Real gamma2 = 0.0, gamma3 = 0.0, gamma4 = 0.0, gamma5 = 0.0, gammaPrev = 0.0;
   Real F = 0.0, Fdot = 0.0;
   if ((whichPoint == "L1") || (whichPoint == "L2") ||
       (whichPoint == "L3"))
   {
      // Determine initial gamma
      if (whichPoint == "L3")  gamma = 1.0;
      else  gamma = GmatMathUtil::Pow((muStar / (3.0 * (1.0 - muStar))),
                                      (1.0 / 3.0));
      
      //cout << ">>>>>> gamma to start is : " << gamma << endl;

      Integer counter = 0;
      Real diff = 999.99;
      while (diff > CONVERGENCE_TOLERANCE)
      {
         if (counter > MAX_ITERATIONS)
            throw SolarSystemException(
                  "Libration point gamma not converging.");
         gamma2 = gamma  * gamma;
         gamma3 = gamma2 * gamma;
         gamma4 = gamma3 * gamma;
         gamma5 = gamma4 * gamma;
         if (whichPoint == "L1")
         {
            F = gamma5 - ((3.0 - muStar) * gamma4) + 
                ((3.0 - 2.0 * muStar) * gamma3) - 
                (muStar * gamma2) + (2.0 * muStar * gamma) - muStar;
            Fdot = (5.0 * gamma4) - (4.0 * (3.0 - muStar) * gamma3) + 
                   (3.0 * (3.0 - 2.0 * muStar) * gamma2) - 
                   (2.0 * muStar * gamma) + (2.0 * muStar);
         }
         else if (whichPoint == "L2")
         {
            F = gamma5 + ((3.0 - muStar) * gamma4) + 
                ((3.0 - 2.0 * muStar) * gamma3) - 
                (muStar * gamma2) - (2.0 * muStar * gamma) - muStar;
            Fdot = (5.0 * gamma4) + (4.0 * (3.0 - muStar) * gamma3) + 
               (3.0 * (3.0 - 2.0 * muStar) * gamma2) - (2.0 * muStar * gamma) - 
               (2.0 * muStar);
         }
         else  // whichPoint == "L3"
         {
            F = gamma5 + ((2.0 + muStar) * gamma4) + 
                ((1.0 + 2.0 * muStar) * gamma3) -
                ((1.0 - muStar) * gamma2) - (2.0 * (1.0 - muStar) * gamma) - 
                (1.0 - muStar);
            Fdot = (5.0 * gamma4) + (4.0 * (2.0 +  muStar) * gamma3) + 
               (3.0 * (1.0 +  2.0 * muStar) * gamma2) - 
               (2.0 * (1.0 - muStar) * gamma) - (2.0 * (1.0 - muStar));
         }
         counter++;
         gammaPrev = gamma;
         gamma     = gammaPrev - (F / Fdot);
         diff      = GmatMathUtil::Abs(gamma - gammaPrev);/*
         if (whichPoint == "L3" && counter <= 4)
         {
            cout << ">>>>>>>>>>> F  = " << F << endl;
            cout << ">>>>>>>>>>> Fdot  = " << Fdot << endl;
            cout << ">>>>>>>>>>> Gamma  = " << gamma << endl;
         }*/
      }
   }
   Real x = 0.0;
   Real y = 0.0;
   if (whichPoint == "L1") 
   {
      x = 1.0 - gamma;
      y = 0.0;
   }
   else if (whichPoint == "L2")
   {
      x = 1.0 + gamma;
      y = 0.0;
   }
   else if (whichPoint == "L3")
   {
      x = - gamma;
      y = 0.0;
   }
   else if (whichPoint == "L4")
   {
      x = 0.5;
      y = GmatMathUtil::Sqrt(3.0) / 2.0;
   }
   else if (whichPoint == "L5")
   {
      x = 0.5;
      y = - GmatMathUtil::Sqrt(3.0) / 2.0;
   }
   else // ERROR
      throw SolarSystemException("Illegal value for libration point.");
   
   // Express position and velocity of the libration point in the rotating
   // system with the origin centered on the primary body
   Rvector3 ri(x, y, 0.0);
   Rvector3 vi(x, y, 0.0);
   Real rMag  = r.GetMagnitude();
   ri         = rMag * ri;
   Real vMult = (v * r) / rMag;
   vi         = vMult * vi;
   /*
   cout << "ri = " << endl << ri << endl;
   cout << "vi = " << endl << vi << endl;
   */
   // Determine the rotation matrix and its derivative
   Rvector3 xHat    = r / r.GetMagnitude();  // unit vector
   Rvector3 zHat    = (Cross(r,v)).GetUnitVector();
   Rvector3 yHat    = Cross(zHat, xHat);
   Rvector3 xDotHat = (v / rMag) - (xHat / rMag) * (xHat * v);
   Rvector3 ra      = Cross(r,a);
   Rvector3 rv      = Cross(r,v);
   Real     rvMag   = rv.GetMagnitude();
   Rvector3 zDotHat = ra / rvMag - (zHat / rvMag) * ((ra * zHat));
   Rvector3 yDotHat = Cross(zDotHat, xHat) + Cross(zHat, xDotHat);
   Rmatrix33 R;
   R(0,0) = xHat(0);
   R(0,1) = yHat(0);
   R(0,2) = zHat(0);
   R(1,0) = xHat(1);
   R(1,1) = yHat(1);
   R(1,2) = zHat(1);
   R(2,0) = xHat(2);
   R(2,1) = yHat(2);
   R(2,2) = zHat(2);
   
   Rmatrix33 RDot;
   RDot(0,0) = xDotHat(0);
   RDot(0,1) = yDotHat(0);
   RDot(0,2) = zDotHat(0);
   RDot(1,0) = xDotHat(1);
   RDot(1,1) = yDotHat(1);
   RDot(1,2) = zDotHat(1);
   RDot(2,0) = xDotHat(2);
   RDot(2,1) = yDotHat(2);
   RDot(2,2) = zDotHat(2);

   Rvector3 rLi = R * ri;
   Rvector3 vLi = RDot * ri + R * vi;

   Rvector6 rvFK5(rLi(0), rLi(1), rLi(2), vLi(0), vLi(1), vLi(2));
   
   // Translate so that the origin is at the j2000Body
   Rvector6 rvResult = rvFK5 + primaryState;
   return rvResult;
}

//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000 position of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the position is requested.
 *
 * @return position of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 LibrationPoint::GetMJ2000Position(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetR());
}

//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000 velocity of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the velocity is requested.
 *
 * @return velocity of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 LibrationPoint::GetMJ2000Velocity(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetV());
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
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetParameterText(const Integer id) const
{
   if (id >= CalculatedPointParamCount && id < LibrationPointParamCount)
      return PARAMETER_TEXT[id - CalculatedPointParamCount];
   return CalculatedPoint::GetParameterText(id);
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
 *
 */
//------------------------------------------------------------------------------
Integer     LibrationPoint::GetParameterID(const std::string &str) const
{
   for (Integer i = CalculatedPointParamCount; i < LibrationPointParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CalculatedPointParamCount])
         return i;
   }
   
   return CalculatedPoint::GetParameterID(str);
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
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType LibrationPoint::GetParameterType(const Integer id) const
{
   if (id >= CalculatedPointParamCount && id < LibrationPointParamCount)
      return PARAMETER_TYPE[id - CalculatedPointParamCount];
      
   return CalculatedPoint::GetParameterType(id);
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
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetParameterTypeString(const Integer id) const
{
   return CalculatedPoint::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetStringParameter(const Integer id) const
{
   if (id == PRIMARY_BODY_NAME)             
   {
      if (primaryBody)  return primaryBody->GetName();
      return primaryBodyName;
   }
   if (id == SECONDARY_BODY_NAME)             
   {
      if (secondaryBody)  return secondaryBody->GetName();
      return secondaryBodyName;
   }
   if (id == WHICH_POINT)             
   {
      return whichPoint;
   }
   
   return CalculatedPoint::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const Integer id, 
                                        const std::string &value)
{     
   if (id == PRIMARY_BODY_NAME)             
   {
      // since we don't know the order of setting, we cannot do the ckecking
      // of primary and secondary bodies are the same
      primaryBodyName = value;
      return true;
   }
   if (id == SECONDARY_BODY_NAME)             
   {
      // since we don't know the order of setting, we cannot do the ckecking
      // of primary and secondary bodies are the same
      secondaryBodyName = value;
      return true;
   }
   if (id == WHICH_POINT)             
   {
      if ((value != "L1") && (value != "L2") && (value != "L3") &&
          (value != "L4") && (value != "L5"))
         throw SolarSystemException(
            "The value of \"" + value + "\" for field \"Libration\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ L1, L2, L3, L4, L5 ]. ");
      whichPoint = value;
      return true;
   }
   
   return CalculatedPoint::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const std::string &label, 
//                                  const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const std::string &label, 
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID, and the index.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 * @note ADDED TO SATISFY DUFUS COMPILER
 *
 */
//------------------------------------------------------------------------------
bool  LibrationPoint::SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index) 
{
   return CalculatedPoint::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string &label, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label, and the index.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 * @note ADDED TO SATISFY DUFUS COMPILER
 *
 */
//------------------------------------------------------------------------------
bool  LibrationPoint::SetStringParameter(const std::string &label,
                                          const std::string &value,
                                          const Integer index) 
{
   return SetStringParameter(GetParameterID(label),value,index);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& LibrationPoint::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}

//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. 
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& LibrationPoint::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      static StringArray refs;
      refs.clear();
      refs.push_back(primaryBodyName);
      refs.push_back(secondaryBodyName);
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObjectNameArray call
   return CalculatedPoint::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object.
 *
 * @param <obj>   reference object pointer.
 * @param <type>  type of the reference object.
 * @param <name>  name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetRefObject(GmatBase *obj, 
                                  const Gmat::ObjectType type,
                                  const std::string &name)
{
   if (obj == NULL)
      return false;

   #ifdef DEBUG_LP_OBJECT
   MessageInterface::ShowMessage
      ("LibrationPoint::SetRefObject() this=%s, obj=<%p><%s> entered\n",
       GetName().c_str(), obj, obj->GetName().c_str());
   #endif
   
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      if (name == primaryBodyName)
         primaryBody = (SpacePoint*)obj;
      else if (name == secondaryBodyName)
         secondaryBody = (SpacePoint*)obj;
   }
   
   // Call parent class to add objects to bodyList
   return CalculatedPoint::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LibrationPoint.
 *
 * @return clone of the LibrationPoint.
 *
 */
//------------------------------------------------------------------------------
GmatBase* LibrationPoint::Clone() const
{
   return (new LibrationPoint(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void LibrationPoint::Copy(const GmatBase* orig)
{
   // We don't want to copy instanceName
   std::string name = instanceName;
   operator=(*((LibrationPoint *)(orig)));
   instanceName = name;
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  void CheckBodies()
//------------------------------------------------------------------------------
/**
 * Method for the Barycenter class that checks to make sure the bodyList has
 * been defined appropriately (i.e. all CelestialBody objects).
 *
 */
//------------------------------------------------------------------------------
void LibrationPoint::CheckBodies()
{
   bool foundPrimary   = false;
   bool foundSecondary = false;
   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      #ifdef DEBUG_CHECK_BODIES
      MessageInterface::ShowMessage
         ("   bodyList[%d] = %s\n", i,
          (bodyList[i] == NULL ? "NULL" : bodyList[i]->GetName().c_str()));
      #endif
      
      if (((bodyList.at(i))->GetType() != Gmat::CELESTIAL_BODY) &&
          ((bodyList.at(i))->GetTypeName() != "Barycenter"))
         throw SolarSystemException(
             "Bodies for LibrationPoint must be CelestialBodys or Barycenters");
      if ((bodyList.at(i))->GetName() == primaryBodyName) 
      {
         foundPrimary = true;
         primaryBody  = bodyList.at(i);
      }
      if ((bodyList.at(i))->GetName() == secondaryBodyName) 
      {
         foundSecondary = true;
         secondaryBody  = bodyList.at(i);
      }
   }
   if (!foundPrimary)
      throw SolarSystemException(
            "Primary body not found for LibrationPoint");
   if (!foundSecondary)
      throw SolarSystemException(
            "Secondary body not found for LibrationPoint");
   if (primaryBody == secondaryBody)
      throw SolarSystemException(
            "Primary and Secondary bodies cannot be the same for LibrationPoint");
}


