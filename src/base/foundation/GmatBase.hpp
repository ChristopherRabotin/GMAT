//$Id$
//------------------------------------------------------------------------------
//                                  GmatBase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/25
//
//
// Modification History:
//
// 11/9/2003 D. Conway
//   Made GetParameterCount virtual so PropSetup can override it, returning the
//   count for the member ForceModel, Forces, and Propagator.
/**
 * Definition for the base class for all GMAT extensible objects
 */
//------------------------------------------------------------------------------


#ifndef GmatBase_hpp
#define GmatBase_hpp

#include "gmatdefs.hpp"
#include "GmatBaseException.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"


// Forward reference for the SolarSystem pointer
class SolarSystem;


//------------------------------------------------------------------------------
/**
 * Definition for the base class for all GMAT extensible objects
 *
 * The following class hierarchy trees use this class as their basis:
 *
 *     SpacePoint (hence Spacecraft and Formation, and all CelestialBody's)
 *     Propagator
 *     PhysicalModel (hence Force and ForceModel)
 *     PropConfig
 *     Parameter
 *     GmatCommand
 *
 * Every class that users can use to derive new classes, or that need to be
 * accessed through generic pointers, should be derived from this class to
 * ensure consistent interfaces accessed by the GMAT control systems (i.e. the
 * Moderator, FactoryManager, Configuration, Interpreter, and Sandbox, along
 * with the GUIInterpreter).
 */
//------------------------------------------------------------------------------
class GMAT_API GmatBase
{
public:
   // The usual suspects
   GmatBase(Gmat::ObjectType typeId, const std::string &typeStr, 
            const std::string &nomme = "");
   virtual ~GmatBase() = 0;
   GmatBase(const GmatBase &a);
   GmatBase&            operator=(const GmatBase &a);
   
   // Access methods called on the base class
   virtual Gmat::ObjectType
                        GetType() const;
   inline std::string   GetTypeName() const;
   inline std::string   GetName() const;
   virtual bool         SetName(const std::string &who,
                                const std::string &oldName = "");
   virtual Integer      GetParameterCount() const;
   
   bool                 IsOfType(Gmat::ObjectType ofType);
   bool                 IsOfType(std::string typeDescription);
   
   void                 SetShowPrefaceComment(bool show = true);
   void                 SetShowInlineComment(bool show = true);
   bool                 GetShowPrefaceComment();
   bool                 GetShowInlineComment();
   
   // Access methods derived classes can override on comments
   virtual const std::string
                        GetCommentLine() const;
   virtual void         SetCommentLine(const std::string &comment);
   
   virtual const std::string
                        GetInlineComment() const;
   virtual void         SetInlineComment(const std::string &comment);
   
   virtual const std::string
                        GetAttributeCommentLine(Integer index);
   virtual void         SetAttributeCommentLine(Integer index, 
                                                const std::string &comment);
   
   virtual const std::string
                        GetInlineAttributeComment(Integer index);
   virtual void         SetInlineAttributeComment(Integer index, 
                                                  const std::string &comment);
   
   // Access methods derived classes can override on reference objects
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual bool         SetIsGlobal(bool globalFlag);
   virtual bool         GetIsGlobal() const;
   virtual bool         ExecuteCallback();
   virtual bool         IsCallbackExecuting();
   virtual bool         PutCallbackData(std::string &data);
   virtual std::string  GetCallbackResults();
   
   // required method for all subclasses
   virtual GmatBase*    Clone() const = 0;
   
   // required method for all subclasses that can be copied in a script
   virtual void         Copy(const GmatBase*);
   
   virtual bool         Initialize();
   virtual void         SetSolarSystem(SolarSystem *ss);
   
   virtual bool         RequiresJ2000Body();

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const Integer id,
                                            const Integer index) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value,
                                            const Integer index);
                                           
   virtual UnsignedInt  GetUnsignedIntParameter(const Integer id) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value);
   virtual UnsignedInt  GetUnsignedIntParameter(const Integer id,
                                                const Integer index) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const Integer id,
                                                const UnsignedInt value,
                                                const Integer index);
   virtual const UnsignedIntArray& 
                        GetUnsignedIntArrayParameter(const Integer id) const;
   
   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);
   
   virtual const Rmatrix&
                        GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix&
                        SetRmatrixParameter(const Integer id,
                                            const Rmatrix &value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& 
                        GetStringArrayParameter(const Integer id, 
                                                const Integer index) const;
   
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                         const std::string &value);
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label, 
                                         const Integer row, 
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label,
                                            const Integer index) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value,
                                            const Integer index);
                                    
   virtual UnsignedInt  GetUnsignedIntParameter(const std::string &label) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                                const UnsignedInt value);
   virtual UnsignedInt  GetUnsignedIntParameter(const std::string &label,
                                                const Integer index) const;
   virtual UnsignedInt  SetUnsignedIntParameter(const std::string &label,
                                                const UnsignedInt value,
                                                const Integer index);
   virtual const UnsignedIntArray& 
                        GetUnsignedIntArrayParameter(const std::string &label) const;
   
   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);
   
   virtual const Rmatrix&
                        GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix&
                        SetRmatrixParameter(const std::string &label,
                                            const Rmatrix &value);
   
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index);
                                          
   virtual const StringArray& 
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray& 
                        GetStringArrayParameter(const std::string &label, 
                                                const Integer index) const; 
   
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);
   
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   virtual StringArray  GetGeneratingStringArray(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");
   virtual std::string  BuildPropertyName(GmatBase *ownedObj);
   virtual void         FinalizeCreation();
   
   virtual std::string  GetErrorMessageFormat();
   virtual void         SetErrorMessageFormat(const std::string &fmt);
   
   /// Return value used if the parameter is not accessible as a Real
   static const Real         REAL_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as an Integer
   static const Integer      INTEGER_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as an UnsignedInt
   static const UnsignedInt  UNSIGNED_INT_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a String
   static const std::string  STRING_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a Rvector
   static const Rvector      RVECTOR_PARAMETER_UNDEFINED;
   /// Return value used if the parameter is not accessible as a Rmatrix
   static const Rmatrix      RMATRIX_PARAMETER_UNDEFINED;
   /// String mappings for the GMAT data types
   static const std::string  PARAM_TYPE_STRING[Gmat::TypeCount];
   /// String mappings for the GMAT object types
   static const std::string  OBJECT_TYPE_STRING[Gmat::UNKNOWN_OBJECT - Gmat::SPACECRAFT+1];
   
   /// Method to return the current number of instantiated objects
   static Integer          GetInstanceCount();
   /// Method for getting GMAT object type
   static Gmat::ObjectType GetObjectType(const std::string &typeString);
   /// Method for getting GMAT object type string
   static std::string      GetObjectTypeString(Gmat::ObjectType type);
   /// Method for getting data precision
   static Integer          GetDataPrecision();
   /// Method for getting time precision
   static Integer          GetTimePrecision();
   
protected:
   /// Parameter IDs
   enum
   {
      GmatBaseParamCount = 0,
   };
   
   /// count of the number of GmatBase objects currently instantiated
   static Integer      instanceCount;
   
   /// Count of the accessible parameters
   Integer             parameterCount;
   /// Script string used or this class
   std::string         typeName;
   /// Name of the object -- empty if it is nameless
   std::string         instanceName;
   /// Enumerated base type of the object
   Gmat::ObjectType    type;
   /// Number of owned objects that belong to this instance
   Integer             ownedObjectCount;
   /// Script string used to build the object
   std::string         generatingString;
   /// The list of generic types that this class extends.
   ObjectTypeArray     objectTypes;
   /// The list types that this class extends, by name
   StringArray         objectTypeNames;
   
   /// The list of object types referenced by this class
   ObjectTypeArray     refObjectTypes;
   /// The list of object names referenced by this class
   StringArray         refObjectNames;
   /// flag indicating whether or not the object is Global
   bool                isGlobal;
   
   /// flag indicating whether or not a Callback method is currently executing
   bool                callbackExecuting;
   
   /// error message formats
   std::string         errorMessageFormat;
   std::string         errorMessageFormatUnnamed;
   
   /// flag used to deterine if the current write is in Matlab mode
   bool                inMatlabMode;
   
   /// String used to hold the comment line
   std::string         commentLine;
   /// String used to hold inline comment
   std::string         inlineComment;
   /// String array used to hold the attribute comments
   StringArray         attributeCommentLines;
   /// String array used to hold the attribute inline comments
   StringArray         attributeInlineComments;
   /// Flag to indicating whether to show preface comment
   bool                showPrefaceComment;
   /// Flag to indicating whether to show inline comment
   bool                showInlineComment;
   
   // Scripting interfaces
   void                CopyParameters(const GmatBase &a);
   virtual void        WriteParameters(Gmat::WriteMode mode,
                                       std::string &prefix, 
                                       std::stringstream &stream);
   void                WriteParameterValue(Integer id, 
                                           std::stringstream &stream);
   
private:
   
   virtual void PrepCommentTables();
};


//------------------------------------------------------------------------------
//  std::string GetTypeName() const
//------------------------------------------------------------------------------
/**
 * Retrieves the type name (i.e. the type used in scripting) for the object.
 * 
 * @return The type name.
 */
std::string GmatBase::GetTypeName() const
{
   return typeName;
}


//------------------------------------------------------------------------------
//  std::string GetName() const
//------------------------------------------------------------------------------
/**
 * Retrieves the object's name.
 * 
 * @return The name.
 */
std::string GmatBase::GetName() const
{
   return instanceName;
}
#endif // GmatBase_hpp

