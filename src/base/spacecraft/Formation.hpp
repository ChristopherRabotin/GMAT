#ifndef FORMATION_HPP
#define FORMATION_HPP

#include "SpaceObject.hpp"

class GMAT_API Formation : public SpaceObject{
public:
	Formation(Gmat::ObjectType typeId, const std::string &typeStr, 
             const std::string &nomme);
	virtual ~Formation();
   Formation(const Formation& orig);
   Formation&                       operator=(const Formation& orig);
   
   virtual GmatBase*                Clone() const;
   
   
   
   
   // Access methods derived classes can override
   virtual Integer     GetParameterCount(void) const;
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

//   virtual std::string GetStringParameter(const Integer id) const;
//   virtual std::string GetStringParameter(const Integer id,
//                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 
                       
//   virtual std::string GetStringParameter(const std::string &label) const;
//   virtual std::string GetStringParameter(const std::string &label,
//                                          const Integer index) const;

//   virtual bool        SetStringParameter(const std::string &label, 
//                                          const std::string &value);
//   virtual bool        SetStringParameter(const std::string &label, 
//                                          const std::string &value,
//                                          const Integer index);
//   virtual const StringArray& 
//                       GetStringArrayParameter(const std::string &label)const;

protected:
   /// List of the object names used in the formation
   StringArray                      componentNames;
   /// Pointers to the formation members
   std::vector <SpaceObject *>      components;
   /// Full state in the propagation vector
   PropState                        state;
   /// Current epoch of the propagation vector
   Real                             epoch;

   /// Enumerated parameter IDs   
   enum
   {
      ADDED_SPACECRAFT = SpaceObjectParamCount,
      REMOVED_SPACECRAFT,
      FormationParamCount
   }; 
   /// Array of supported parameters
   static const std::string PARAMETER_TEXT[FormationParamCount - 
                                           SpaceObjectParamCount];
   /// Array of parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[FormationParamCount - 
                                                   SpaceObjectParamCount];
};

#endif // FORMATION_HPP
