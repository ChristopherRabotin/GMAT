//$Id$
//------------------------------------------------------------------------------
//                            Nadir Pointing
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Yeerang Lim/KAIST
// Created: 2013.05.09
//
/**
 * Class definition for the Nadir (Nadir Pointing) attitude class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef Nadir_hpp
#define Nadir_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"
//#include "Rmatrix33.hpp"


class GMAT_API NadirPointing : public Kinematic 
{
public:
   /// Constructor
   NadirPointing(const std::string &itsName = "");
   /// copy constructor
   NadirPointing(const NadirPointing& att);
   /// operator =
   NadirPointing& operator=(const NadirPointing& att);
   /// destructor
   virtual ~NadirPointing();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;

//   // Access methods derived classes can override
//   virtual std::string  GetParameterText(const Integer id) const;
//   virtual Integer      GetParameterID(const std::string &str) const;
//   virtual Gmat::ParameterType
//                        GetParameterType(const Integer id) const;
//
//   virtual Real         GetRealParameter(const Integer id) const;
//   virtual Real         SetRealParameter(const Integer id,
//                                         const Real value);
//   virtual std::string  GetStringParameter(const Integer id) const;
//   virtual bool         SetStringParameter(const Integer id,
//                                           const std::string &value);
//   // added
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
//   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
protected:
   enum 
   {
//	   ATTITUDE_REFERENCE_BODY = KinematicParamCount,
//	   MODE_OF_CONSTRAINT,
//	   REFERENCE_VECTOR_X,
//	   REFERENCE_VECTOR_Y,
//	   REFERENCE_VECTOR_Z,
//	   CONSTRAINT_VECTOR_X,
//	   CONSTRAINT_VECTOR_Y,
//	   CONSTRAINT_VECTOR_Z,
//	   BODY_ALIGNMENT_VECTOR_X,
//	   BODY_ALIGNMENT_VECTOR_Y,
//	   BODY_ALIGNMENT_VECTOR_Z,
//	   BODY_CONSTRAINT_VECTOR_X,
//	   BODY_CONSTRAINT_VECTOR_Y,
//	   BODY_CONSTRAINT_VECTOR_Z,
//       NadirPointingParamCount
      NadirPointingParamCount = KinematicParamCount
   };
   
//   // static text and types for parameters
//   static const std::string PARAMETER_TEXT[NadirPointingParamCount - KinematicParamCount];
//
//   static const Gmat::ParameterType PARAMETER_TYPE[NadirPointingParamCount - KinematicParamCount];

   ///
//   std::string refBodyName;
//   CelestialBody *refBody;
//
//   std::string ModeOfConstraint;
//
//   Rvector3 ReferenceVector;
//   Rvector3 ConstraintVector;
//   Rvector3 BodyAlignmentVector;
//   Rvector3 BodyConstraintVector;

   Rmatrix33 TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2);

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);   

private:
   // Default constructor - not implemented
   //NadirPointing(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*NadirPointing_hpp*/
