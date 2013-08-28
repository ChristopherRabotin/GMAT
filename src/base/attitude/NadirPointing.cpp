//$Id$
//------------------------------------------------------------------------------
//                            Nadir Pointing
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// From Wendy @ 2013.07.16
// Author: Yeerang Lim/KAIST
// Created: 2013.05.09
//
/**
 * Class implementation for the Nadir class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

//#ifdef DEBUG_NADIRPOINTING
//#define DEBUG_NADIRPOINTING
//#define DEBUG_NADIR_POINTING_COMPUTE
//#define DEBUG_ATTITUDE_SET
//#define DEBUG_NADIR_REF_OBJECT
//#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "MessageInterface.hpp"

#include "SpaceObject.hpp" 
#include "NadirPointing.hpp"


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
//const std::string
//NadirPointing::PARAMETER_TEXT[NadirPointingParamCount - KinematicParamCount] =
//{
//	"AttitudeReferenceBody",
//	"ModeOfConstraint",
//	"ReferenceVectorX",
//	"ReferenceVectorY",
//	"ReferenceVectorZ",
//	"ConstraintVectorX",
//	"ConstraintVectorY",
//	"ConstraintVectorZ",
//	"BodyAlignmentVectorX",
//	"BodyAlignmentVectorY",
//	"BodyAlignmentVectorZ",
//	"BodyConstraintVectorX",
//	"BodyConstraintVectorY",
//	"BodyConstraintVectorZ",
//};
//
//const Gmat::ParameterType
//NadirPointing::PARAMETER_TYPE[NadirPointingParamCount - KinematicParamCount] =
//{
//	Gmat::OBJECT_TYPE,
//	Gmat::STRING_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//	Gmat::REAL_TYPE,
//};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Nadir(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Nadir class (constructor).
 */
//------------------------------------------------------------------------------
NadirPointing::NadirPointing(const std::string &itsName) : 
	Kinematic("NadirPointing",itsName)// ,
//   refBodyName ("Earth"),
//	refBody		(NULL)
{
   parameterCount = NadirPointingParamCount;
   objectTypeNames.push_back("NadirPointing");
   attitudeModelName         = "NadirPointing";
   setInitialAttitudeAllowed = false;
   modifyCoordSysAllowed     = false;
   // Reserve spaces to handle attribute comments for owned object
   // LOJ: 2013.03.01 for GMT-3353 FIX

//   ModeOfConstraint = "OrbitNormal";
//   ReferenceVector.Set(-1,0,0);
//   ConstraintVector.Set(0,1,0);
//   BodyAlignmentVector.Set(0,0,1);
//   BodyConstraintVector.Set(1,0,0);

   FinalizeCreation();
}

//------------------------------------------------------------------------------
//  Nadir(const Nadir &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Nadir class as a copy of the
 * specified Nadir class (copy constructor).
 *
 * @param <att> Nadir object to copy.
 */
//------------------------------------------------------------------------------
NadirPointing::NadirPointing(const NadirPointing& att) :
	Kinematic(att)//,
//   refBodyName ("Earth"),
//	refBody		(NULL)
{
//	ModeOfConstraint = att.ModeOfConstraint;
//	ReferenceVector = att.ReferenceVector;
//	ConstraintVector = att.ConstraintVector;
//	BodyAlignmentVector = att.BodyAlignmentVector;
//	BodyConstraintVector = att.BodyConstraintVector;
}
 
//------------------------------------------------------------------------------
//  Nadir& operator= (const Nadir& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Nadir class.
 *
 * @param att the Nadir object whose data to assign to "this"
 *            Nadir.
 *
 * @return "this" Nadir with data of input Nadir att.
 */
//------------------------------------------------------------------------------
NadirPointing& NadirPointing::operator=(const NadirPointing& att)
{

	Kinematic::operator=(att);

//	refBody		= att.refBody;
//	refBodyName	= att.refBodyName;
//
//	ModeOfConstraint = att.ModeOfConstraint;
//	ReferenceVector = att.ReferenceVector;
//	ConstraintVector = att.ConstraintVector;
//	BodyAlignmentVector = att.BodyAlignmentVector;
//	BodyConstraintVector = att.BodyConstraintVector;

   return *this;
}


//------------------------------------------------------------------------------
//  ~Nadir()
//------------------------------------------------------------------------------
/**
 * Destroys the Nadir class (constructor).
 */
//------------------------------------------------------------------------------
NadirPointing::~NadirPointing()
{
   // nothing really to do here ... la la la la la
}


//---------------------------------------------------------------------------
//  bool Initialize() 
//---------------------------------------------------------------------------
 /**
 * Initializes the attitude.
 * 
 * @return Success flag.
 *
  */
//---------------------------------------------------------------------------
bool NadirPointing::Initialize()
{
   if (!Kinematic::Initialize()) return false;

   // Cannot check this here, as Initialize is called on Attitude when getting
   // any data value, including when the SpacecraftPanel is brought up; this
   // happens before the reference body pointer has been set on the Attitude,
   // so this message is generated each time the panel is opened.
   // We can wait until the attitude is computed to do this check.
//   if (!refBody)
//   {
//      std::string attEx  = "Reference body ";
//      attEx             += refBodyName + " not defined for attitude of type \"";
//      attEx             += "NadirPointing\"";
//      throw AttitudeException(attEx);
//   }

   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Nadir.
 *
 * @return clone of the Nadir.
 *
 */
//------------------------------------------------------------------------------
GmatBase* NadirPointing::Clone() const
{
	return (new NadirPointing(*this)); //???(YRL)
}


//// GetParameterText
//std::string NadirPointing::GetParameterText(const Integer id) const
//{
//   if (id >= KinematicParamCount && id < NadirPointingParamCount)
//      return PARAMETER_TEXT[id - KinematicParamCount];
//   return Kinematic::GetParameterText(id);
//}
//
//
//// GetParameterID
//Integer NadirPointing::GetParameterID(const std::string &str) const
//{
//   for (Integer i = KinematicParamCount; i < NadirPointingParamCount; i++)
//   {
//	   if (str == PARAMETER_TEXT[i - KinematicParamCount])
//		   return i;
//   }
//
//   return Kinematic::GetParameterID(str);
//}
//
//
//// GetParameterType
//Gmat::ParameterType NadirPointing::GetParameterType(const Integer id) const
//{
//   if (id >= KinematicParamCount && id < NadirPointingParamCount)
//      return PARAMETER_TYPE[id - KinematicParamCount];
//
//   return Kinematic::GetParameterType(id);
//}


//// GetRealParameter
//Real NadirPointing::GetRealParameter(const Integer id) const
//{
//   if (id == REFERENCE_VECTOR_X)
//   {
//      return ReferenceVector(0);
//   }
//   if (id == REFERENCE_VECTOR_Y)
//   {
//	   return ReferenceVector(1);
//   }
//   if (id == REFERENCE_VECTOR_Z)
//   {
//	   return ReferenceVector(2);
//   }
//   if (id == CONSTRAINT_VECTOR_X)
//   {
//      return ConstraintVector(0);
//   }
//   if (id == CONSTRAINT_VECTOR_Y)
//   {
//	   return ConstraintVector(1);
//   }
//   if (id == CONSTRAINT_VECTOR_Z)
//   {
//	   return ConstraintVector(2);
//   }
//   if (id == BODY_ALIGNMENT_VECTOR_X)
//   {
//      return BodyAlignmentVector(0);
//   }
//   if (id == BODY_ALIGNMENT_VECTOR_Y)
//   {
//	   return BodyAlignmentVector(1);
//   }
//   if (id == BODY_ALIGNMENT_VECTOR_Z)
//   {
//	   return BodyAlignmentVector(2);
//   }
//   if (id == BODY_CONSTRAINT_VECTOR_X)
//   {
//      return BodyConstraintVector(0);
//   }
//   if (id == BODY_CONSTRAINT_VECTOR_Y)
//   {
//	   return BodyConstraintVector(1);
//   }
//   if (id == BODY_CONSTRAINT_VECTOR_Z)
//   {
//	   return BodyConstraintVector(2);
//   }
//
//   return Kinematic::GetRealParameter(id);
//}
//
//
//// SetRealParameter
//Real NadirPointing::SetRealParameter(const Integer id, const Real value)
//{
//   if (id == REFERENCE_VECTOR_X)
//   {
//      ReferenceVector(0) = value;
//	  return ReferenceVector(0);
//   }
//   if (id == REFERENCE_VECTOR_Y)
//   {
//      ReferenceVector(1) = value;
//	  return ReferenceVector(1);
//   }
//   if (id == REFERENCE_VECTOR_Z)
//   {
//	   ReferenceVector(2) = value;
//	   return ReferenceVector(2);
//   }
//   if (id == CONSTRAINT_VECTOR_X)
//   {
//	   ConstraintVector(0) = value;
//      return ConstraintVector(0);
//   }
//   if (id == CONSTRAINT_VECTOR_Y)
//   {
//	   ConstraintVector(1) = value;
//	   return ConstraintVector(1);
//   }
//   if (id == CONSTRAINT_VECTOR_Z)
//   {
//	   ConstraintVector(2) = value;
//	   return ConstraintVector(2);
//   }
//   if (id == BODY_ALIGNMENT_VECTOR_X)
//   {
//	   BodyAlignmentVector(0) = value;
//	   return BodyAlignmentVector(0);
//   }
//   if (id == BODY_ALIGNMENT_VECTOR_Y)
//   {
//	   BodyAlignmentVector(1) = value;
//	   return BodyAlignmentVector(1);
//   }
//   if (id == BODY_ALIGNMENT_VECTOR_Z)
//   {
//	   BodyAlignmentVector(2) = value;
//	   return BodyAlignmentVector(2);
//   }
//   if (id == BODY_CONSTRAINT_VECTOR_X)
//   {
//	   BodyConstraintVector(0) = value;
//       return BodyConstraintVector(0);
//   }
//   if (id == BODY_CONSTRAINT_VECTOR_Y)
//   {
//	   BodyConstraintVector(1) = value;
//	   return BodyConstraintVector(1);
//   }
//   if (id == BODY_CONSTRAINT_VECTOR_Z)
//   {
//	   BodyConstraintVector(2) = value;
//	   return BodyConstraintVector(2);
//   }
//
//   return Kinematic::SetRealParameter(id,value);
//}


//// GetStringParameter
//std::string NadirPointing::GetStringParameter(const Integer id) const
//{
////	if (id == MODE_OF_CONSTRAINT)
////		return ModeOfConstraint;
////	if (id ==ATTITUDE_REFERENCE_BODY)
////		return refBodyName;
//
//   return Kinematic::GetStringParameter(id);
//}


// SetStringParameter
//bool NadirPointing::SetStringParameter(const Integer id,
//                                           const std::string &value)
//{
////	if (id == MODE_OF_CONSTRAINT)
////	{
////		ModeOfConstraint = value;
////		return true;
////	}
////	if (id == ATTITUDE_REFERENCE_BODY)
////	{
////		refBodyName = value;
////		return true;
////	}
//
//   return Kinematic::SetStringParameter(id, value);
//}


// GetRefObjectName
std::string NadirPointing::GetRefObjectName(const Gmat::ObjectType type) const
{
   #ifdef DEBUG_NADIR_REF_OBJECT
      MessageInterface::ShowMessage("NadiPointing::GetRefObjectName with type = %d\n",
            type);
   #endif
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::CELESTIAL_BODY)
   {
	   return refBodyName;
   }

   return Kinematic::GetRefObjectName(type);
}


// GetRefObjectNameArray
const StringArray& NadirPointing::GetRefObjectNameArray(const Gmat::ObjectType type)
{
//   refObjectNames.clear();
   #ifdef DEBUG_NADIR_REF_OBJECT
      MessageInterface::ShowMessage("NadiPointing::GetRefObjectNameArray with type = %d\n",
            type);
   #endif
   refObjectNames = Kinematic::GetRefObjectNameArray(type);
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::CELESTIAL_BODY)
   {
      refObjectNames.push_back(refBodyName);
   }
      
   #ifdef DEBUG_NADIR_REF_OBJECT
      MessageInterface::ShowMessage("NadiPointing::GetRefObjectNameArray returning:\n");
      for (unsigned int ii = 0; ii < refObjectNames.size(); ii++)
         MessageInterface::ShowMessage("   %d   %s\n", ii, refObjectNames.at(ii).c_str());
   #endif
   return refObjectNames;
}

bool NadirPointing::SetRefObjectName(const Gmat::ObjectType type,
                                     const std::string &name)
{
   if (type == Gmat::CELESTIAL_BODY)
   {
      #ifdef DEBUG_ATTITUDE_SET
         MessageInterface::ShowMessage("NadirPointing: Setting refBodyName to %s\n",
                                       name.c_str());
      #endif
      refBodyName = name;
      return true;
   }

   return Kinematic::SetRefObjectName(type, name);
}

GmatBase* NadirPointing::GetRefObject(const Gmat::ObjectType type,
                                      const std::string &name)
{
   switch (type)
   {
      case Gmat::CELESTIAL_BODY:
         if (name == refBodyName)           return refBody;
         break;
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return Kinematic::GetRefObject(type, name);
}

// SetRefObject
bool NadirPointing::SetRefObject(GmatBase *obj,
                                 const Gmat::ObjectType type,
                                 const std::string &name)
{
   #ifdef DEBUG_NADIR_REF_OBJECT
      MessageInterface::ShowMessage(
            "Entering NadirPointing::SetRefObject with obj <%p>, type %d, and name %s\n",
            obj, (Integer) type, name.c_str());
      MessageInterface::ShowMessage("      refBodyName = %s\n", refBodyName.c_str());
   #endif
   if (obj == NULL)
      return false;
   
   if (obj->IsOfType("CelestialBody"))
   {
      if (name == refBodyName)
      {
         if (refBody != (CelestialBody*) obj)
         {
            refBody = (CelestialBody*) obj;
            needsReinit = true;  // need to reinitialize, since Reference Body has changed
         }
      }
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return Kinematic::SetRefObject(obj, type, name);
}


//// HasRefObjectTypeArray (???)
//bool NadirPointing::HasRefObjectTypeArray()
//{
//	return true;
//}


// GetRefObjectTypeArray
const ObjectTypeArray& NadirPointing::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes = Kinematic::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::CELESTIAL_BODY);

   return refObjectTypes;
}


// RenameRefObject
bool NadirPointing::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   if (type == Gmat::CELESTIAL_BODY)
   {
      if (refBodyName == oldName)
         refBodyName = newName;
   }
   
   return Kinematic::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Rmatrix33 TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2)
//------------------------------------------------------------------------------

Rmatrix33 NadirPointing::TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2)
{
	// V1, V2 : defined in frame A
	// W1, W2 : defined in frame B
	// TRIAD algorithm calculates the rotation matrix from A to B

	Rvector3 r1 = V1.GetUnitVector();
	Rvector3 temp = Cross(V1,V2);
	Rvector3 r2 = temp.GetUnitVector();
	Rvector3 r3 = Cross(V1,temp);
	r3 = r3.GetUnitVector();

	Rvector3 s1 = W1.GetUnitVector();
	temp = Cross(W1,W2);
	Rvector3 s2 = temp.GetUnitVector();
	Rvector3 s3 = Cross(W1,temp);
	s3 = s3.GetUnitVector();

	//MessageInterface::LogMessage("s3 = %f\t%f\t%f\n", s3(0), s3(1), s3(2) );

	// YRL, calculate resultRotMatrix
	Rmatrix33 resultRotMatrix = Outerproduct(s1,r1) + Outerproduct(s2,r2) + Outerproduct(s3,r3) ;	

	return resultRotMatrix;
}


//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 * @note This method will update the CosineMatrix parameter of the class.
 */
//------------------------------------------------------------------------------

void NadirPointing::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   //#ifdef DEBUG_NADIRPOINTING
   //#endif

   #ifdef DEBUG_NADIR_POINTING_COMPUTE
      MessageInterface::ShowMessage("In NadirPointing, computing at time %le\n",
            atTime);
      MessageInterface::ShowMessage("ModeOfConstraint = %s\n", modeOfConstraint.c_str());
      if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
      if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
   #endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
      // Check for unset reference body pointer
      if (!refBody)
      {
         std::string attEx  = "Reference body ";
         attEx             += refBodyName + " not defined for attitude of type \"";
         attEx             += "NadirPointing\"";
         throw AttitudeException(attEx);
      }
   }

   /// using a spacecraft object, *owningSC
   A1Mjd theTime(atTime);

   Rvector6 scState = ((SpaceObject*) owningSC)->GetMJ2000State(theTime);
   Rvector6 refState = refBody->GetMJ2000State(theTime);
    
   Rvector3 pos(scState[0] - refState[0], scState[1] - refState[1],  scState[2] - refState[2]);
   Rvector3 vel(scState[3] - refState[3], scState[4] - refState[4],  scState[5] - refState[5]);

   #ifdef DEBUG_NADIR_POINTING_COMPUTE
      MessageInterface::ShowMessage("   scState  = %s\n", scState.ToString().c_str());
      MessageInterface::ShowMessage("   refState = %s\n", refState.ToString().c_str());
      MessageInterface::ShowMessage("   pos      = %s\n", pos.ToString().c_str());
      MessageInterface::ShowMessage("   vel      = %s\n", vel.ToString().c_str());
   #endif

   // Error message
   if ( bodyAlignmentVector.GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("The size of BodyAlignmentVector is too small.");
   }
   else if ( bodyConstraintVector.GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("The size of BodyConstraintVector is too small.");
   }
   else if ( bodyAlignmentVector.GetUnitVector()*bodyConstraintVector.GetUnitVector() > ( 1.0 - 1.0e-5) )
   {
	   throw AttitudeException("BodyAlignmentVector and BodyConstraintVector are the same.");
   }
   else if ( pos.GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("A position vector is zero.");
   }
   else if ( vel.GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("A velocity vector is zero.");
   }
   else if ( Cross(pos,vel).GetMagnitude() < 1.0e-5 )
   {
	   throw AttitudeException("Cross product of position and velocity is zero: LVLH frame cannot be defined.");
   }
   
   Rvector3 normal = Cross(pos,vel);

   Rvector3 xhat = pos/pos.GetMagnitude();
   Rvector3 yhat = normal/normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat,yhat);

   #ifdef DEBUG_NADIR_POINTING_COMPUTE
      MessageInterface::ShowMessage("   normal  = %s\n", normal.ToString().c_str());
      MessageInterface::ShowMessage("   xhat    = %s\n", xhat.ToString().c_str());
      MessageInterface::ShowMessage("   yhat    = %s\n", yhat.ToString().c_str());
      MessageInterface::ShowMessage("   zhat    = %s\n", zhat.ToString().c_str());
   #endif

   // RiI calculation (from inertial to LVLH)
   Rmatrix33 RIi;
   RIi(0,0) = xhat(0);
   RIi(1,0) = xhat(1);
   RIi(2,0) = xhat(2);
   RIi(0,1) = yhat(0);
   RIi(1,1) = yhat(1);
   RIi(2,1) = yhat(2);
   RIi(0,2) = zhat(0);
   RIi(1,2) = zhat(1);
   RIi(2,2) = zhat(2);
   
   // Set alignment/constraint vector in body frame
   if ( modeOfConstraint == "OrbitNormal" )
   {
	   referenceVector[0] = -1;
	   referenceVector[1] = 0;
	   referenceVector[2] = 0;

	   constraintVector[0] = 0;
	   constraintVector[1] = 1;
	   constraintVector[2] = 0;
   }
   else if ( modeOfConstraint == "VelocityConstraint" )
   {
	   referenceVector[0] = -1;
	   referenceVector[1] = 0;
	   referenceVector[2] = 0;

	   constraintVector[0] = 0;
	   constraintVector[1] = 0;
	   constraintVector[2] = 1;
   }

   // RBi calculation using TRIAD (from LVLH to body frame)
   Rmatrix33 RiB = TRIAD( bodyAlignmentVector, bodyConstraintVector, referenceVector, constraintVector );

   // the rotation matrix (from body to inertial)
   dcm = RIi*RiB;
   // the final rotation matrix (from inertial to body)
   dcm =  dcm.Transpose();

   // We do NOT calculate angular velocity for Nadir.
   // TODO : Throw AttitudeException??
   /*
   Rmatrix33 wxIBB = - RBi*RiIDot*dcm.Transpose();
   Rvector3 wIBB;
   wIBB.Set(wxIBB(2,1),wxIBB(0,2),wxIBB(1,0));
   */
}



//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none 
