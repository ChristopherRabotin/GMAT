//$Id$
//------------------------------------------------------------------------------
//                                  CalculatedPoint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2005.04.01 (no foolin')
//
/**
 * This is the base class for calculated points.
 *
 * @note This is an abstract class.
 */
//------------------------------------------------------------------------------

#ifndef CalculatedPoint_hpp
#define CalculatedPoint_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "Rmatrix.hpp"
#include "Rvector6.hpp"
#include "TimeTypes.hpp"

/**
 * CalculatedPoint base class, from which all types of calculated points 
 * will derive.
 *
 * The CalculatedPoint class is primarily an intermediate base class, from which
 * all types of calculated points will derive.  CalculatedPoint itself derives 
 * from SpacePoint.
 *
 * @note Bodies are sent to an (sub)object of this class via the SetRefObject
 *       method.
 */
class GMAT_API CalculatedPoint : public SpacePoint
{
public:
   // default constructor, specifying calculated point type and name
   CalculatedPoint(const std::string &ptType = "LibrationPoint", 
                   const std::string &itsName = "");
   // copy constructor
   CalculatedPoint(const CalculatedPoint &cp);
   // operator=
   CalculatedPoint& operator=(const CalculatedPoint &cp);
   // destructor
   virtual ~CalculatedPoint();
   
   // methods inherited from SpacePoint, that must be implemented here (and/or
   // in the derived classes (--> in derived classes)
   //virtual const Rvector6           GetMJ2000State(const A1Mjd &atTime);
   //virtual const Rvector3           GetMJ2000Position(const A1Mjd &atTime);
   //virtual const Rvector3           GetMJ2000Velocity(const A1Mjd &atTime);   
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType 
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index);
   
   virtual const StringArray&  
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&  
                        GetStringArrayParameter(const std::string &label) const;
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, 
                                     const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray& 
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
protected:
   
   enum
   {
      NUMBER_OF_BODIES = SpacePointParamCount, 
      BODY_NAMES,
      CalculatedPointParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[CalculatedPointParamCount - SpacePointParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[CalculatedPointParamCount - SpacePointParamCount];
   
   /// number of bodies
   Integer                  numberOfBodies;
   /// list of bodies
   std::vector<SpacePoint*> bodyList;
   /// list of body names
   StringArray              bodyNames;
    
private:
      
   //------------------------------------------------------------------------------
   //  void CheckBodies()
   //------------------------------------------------------------------------------
   /**
    * Abstract method for derived classes that checks to make sure the bodyList has
    * been defined appropriately (i.e. all are of a tyep that makes sense).
    *
    */
    //------------------------------------------------------------------------------
   virtual void CheckBodies() = 0;      
};
#endif // CalculatedPoint_hpp

