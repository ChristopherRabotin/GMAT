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

//#define DEBUG_NADIRPOINTING
//#define DEBUG_NADIR_POINTING_COMPUTE
//#define DEBUG_ATTITUDE_SET
//#define DEBUG_NADIR_REF_OBJECT

#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "MessageInterface.hpp"

#include "SpaceObject.hpp" 
#include "NadirPointing.hpp"


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Real NadirPointing::DENOMINATOR_TOLERANCE = 1.0e-15;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  NadirPointing(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Nadir class (constructor).
 */
//------------------------------------------------------------------------------
NadirPointing::NadirPointing(const std::string &itsName) : 
	Kinematic("NadirPointing",itsName)
{
   parameterCount = NadirPointingParamCount;
   objectTypeNames.push_back("NadirPointing");
   attitudeModelName         = "NadirPointing";
   setInitialAttitudeAllowed = false;
   modifyCoordSysAllowed     = false;
   // Reserve spaces to handle attribute comments for owned object
   // LOJ: 2013.03.01 for GMT-3353 FIX

   // No rates are computed for this model
   modelComputesRates = false;

   FinalizeCreation();
}

//------------------------------------------------------------------------------
//  NadirPointing(const NadirPointing &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Nadir class as a copy of the
 * specified Nadir class (copy constructor).
 *
 * @param <att> Nadir object to copy.
 */
//------------------------------------------------------------------------------
NadirPointing::NadirPointing(const NadirPointing& att) :
	Kinematic(att)
{
}
 
//------------------------------------------------------------------------------
//  NadirPointing& operator= (const NadirPointing& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NadirPointing class.
 *
 * @param att the NadirPointing object whose data to assign to "this"
 *            NadirPointing.
 *
 * @return "this" NadirPointing with data of input NadirPointing att.
 */
//------------------------------------------------------------------------------
NadirPointing& NadirPointing::operator=(const NadirPointing& att)
{

	Kinematic::operator=(att);

   return *this;
}


//------------------------------------------------------------------------------
//  ~NadirPointing()
//------------------------------------------------------------------------------
/**
 * Destroys the NadirPointing class (constructor).
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

   // Cannot check refBody value here, as Initialize is called on
   // Attitude when getting any data value, including when the SpacecraftPanel
   // is brought up; this happens before the reference body pointer has been
   // set on the Attitude, so an error message here would be generated
   // each time the panel is opened.
   // We can wait until the attitude is computed to do this check.

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

   Real v1Mag = V1.GetMagnitude();
   if (v1Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 r1 = V1 / v1Mag;
   Rvector3 temp = Cross(V1,V2);
   Real tempMag = temp.GetMagnitude();
   if (tempMag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 r2 = temp / tempMag;
   Rvector3 r3 = Cross(V1,temp);
   Real r3Mag = r3.GetMagnitude();
   if (r3Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   r3 = r3 / r3Mag;

   Real w1Mag = W1.GetMagnitude();
   if (w1Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 s1 = W1 / w1Mag;
   temp = Cross(W1,W2);
   Real temp2Mag = temp.GetMagnitude();
   if (temp2Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   Rvector3 s2 = temp/temp2Mag;
   Rvector3 s3 = Cross(W1,temp);
   Real s3Mag = s3.GetMagnitude();
   if (s3Mag < DENOMINATOR_TOLERANCE)
   {
      std::string errmsg = "Nadir attitude for spacecraft \"";
      errmsg += owningSC->GetName() + "\" is singular.\n";
      throw AttitudeException(errmsg);
   }
   s3 = s3/s3Mag;

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
   #ifdef DEBUG_NADIR_POINTING_COMPUTE
      MessageInterface::ShowMessage("In NadirPointing, computing at time %le\n",
            atTime);
      MessageInterface::ShowMessage("attitudeConstraintType = %s\n", attitudeConstraintType.c_str());
      if (!owningSC) MessageInterface::ShowMessage("--- owningSC is NULL!!!!\n");
      if (!refBody) MessageInterface::ShowMessage("--- refBody is NULL!!!!\n");
   #endif

   if (!isInitialized || needsReinit)
   {
      Initialize();
   }

   // Need to do check for unset reference body here since the needsReinit
   // flag may not have been set
   if (!refBody)
   {
      std::string attEx  = "Reference body ";
      attEx             += refBodyName + " not defined for attitude of type \"";
      attEx             += "NadirPointing\" on spacecraft \"";
      attEx             += owningSC->GetName() + "\".";
      throw AttitudeException(attEx);
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
   std::string attErrorMsg = "Nadir Pointing attitude model is singular and/or ";
   attErrorMsg            += "undefined for Spacecraft \"";
   attErrorMsg            += owningSC->GetName() + "\".";
   if ((bodyAlignmentVector.GetMagnitude() < 1.0e-5 )  ||
       (bodyConstraintVector.GetMagnitude() < 1.0e-5)  ||
       (bodyAlignmentVector.GetUnitVector()*
             bodyConstraintVector.GetUnitVector()
             > ( 1.0 - 1.0e-5))                        ||
       (pos.GetMagnitude() < 1.0e-5)                   ||
       (vel.GetMagnitude() < 1.0e-5)                   ||
       (Cross(pos,vel).GetMagnitude() < 1.0e-5)        )
   {
      throw AttitudeException(attErrorMsg);
   }
   
   Rvector3 normal = Cross(pos,vel);

   Rvector3 xhat = pos/pos.GetMagnitude();
   Rvector3 yhat = normal/normal.GetMagnitude();
   Rvector3 zhat = Cross(xhat,yhat);

   Rvector3 referenceVector;
   Rvector3 constraintVector;


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
   if ( attitudeConstraintType == "OrbitNormal" )
   {
	   referenceVector[0] = -1;
	   referenceVector[1] = 0;
	   referenceVector[2] = 0;

	   constraintVector[0] = 0;
	   constraintVector[1] = 1;
	   constraintVector[2] = 0;
   }
   else if ( attitudeConstraintType == "Velocity" )
   {
	   referenceVector[0] = -1;
	   referenceVector[1] = 0;
	   referenceVector[2] = 0;

	   constraintVector[0] = 0;
	   constraintVector[1] = 0;
	   constraintVector[2] = -1;
   }

   // RBi calculation using TRIAD (from LVLH to body frame)
   Rmatrix33 RiB = TRIAD( bodyAlignmentVector, bodyConstraintVector, referenceVector, constraintVector );

   // the rotation matrix (from body to inertial)
   dcm = RIi*RiB;
   // the final rotation matrix (from inertial to body)
   dcm =  dcm.Transpose();

   // We do NOT calculate angular velocity for Nadir.
   // TODO : Throw AttitudeException?? write warning - but what to return?
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
