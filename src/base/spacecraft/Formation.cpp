//$Header$
//------------------------------------------------------------------------------
//                              Formation
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Implements the class used for formations. 
 */
//------------------------------------------------------------------------------


#include "Formation.hpp"
#include <algorithm>          // for find()

//---------------------------------
// static data
//---------------------------------
const std::string
Formation::PARAMETER_TEXT[FormationParamCount - SpaceObjectParamCount] =
{
   "Add",
   "Remove"
};

const Gmat::ParameterType
Formation::PARAMETER_TYPE[FormationParamCount - SpaceObjectParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::UNKNOWN_PARAMETER_TYPE     // Don't write the "remove" parms
};


Formation::Formation(Gmat::ObjectType typeId, const std::string &typeStr, 
               const std::string &nomme) :
   SpaceObject    (typeId, typeStr, nomme),
   dimension      (0)
{
}


Formation::~Formation()
{
}


Formation::Formation(const Formation& orig) :
   SpaceObject    (orig),
   componentNames (orig.componentNames),
   dimension      (orig.dimension)
{
}


Formation& Formation::operator=(const Formation& orig)
{
   if (&orig == this)
      return *this;
   
   SpaceObject::operator=(orig);
   dimension = orig.dimension;
   componentNames = orig.componentNames;
   components.clear();
   
   return *this;
}


GmatBase* Formation::Clone() const
{
   return new Formation(*this);
}


Integer Formation::GetParameterCount(void) const
{
   return FormationParamCount;
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
std::string Formation::GetParameterText(const Integer id) const
{
   if (id >= SpaceObjectParamCount && id < FormationParamCount)
      return PARAMETER_TEXT[id - SpaceObjectParamCount];
   return SpaceObject::GetParameterText(id);
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
Integer Formation::GetParameterID(const std::string &str) const
{
   for (Integer i = SpaceObjectParamCount; i < FormationParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpaceObjectParamCount])
         return i;
   }
   
   return SpaceObject::GetParameterID(str);
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
Gmat::ParameterType Formation::GetParameterType(const Integer id) const
{
   if (id >= SpaceObjectParamCount && id < FormationParamCount)
      return PARAMETER_TYPE[id - SpaceObjectParamCount];
      
   return GmatBase::GetParameterType(id);
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
std::string Formation::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


bool Formation::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == ADDED_SPACECRAFT) {
      if (find(componentNames.begin(), componentNames.end(), value) != 
              componentNames.end())
         return false;
      componentNames.push_back(value);
      return true;
   }
   return SpaceObject::SetStringParameter(id, value);
}


Real Formation::SetRealParameter(const Integer id, const Real value)
{
   Real retval = SpaceObject::SetRealParameter(id, value);
      
   if (id == EPOCH_PARAM) {
      if (retval != value)
         throw SpaceObjectException("Formation update returned incorrect epoch");
      // Update the epoch on the constituent pieces
      for (std::vector<SpaceObject*>::iterator i = components.begin();
           i != components.end(); ++i) {
          retval = (*i)->SetRealParameter(id, value);
          if (retval != value)
             throw SpaceObjectException("Formation constituent returned incorrect epoch");
      }
   }
   
   return retval;
}


Real Formation::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


bool Formation::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   if (id == ADDED_SPACECRAFT) {
      return false;
   }
   return SpaceObject::SetStringParameter(id, value, index);
}


const StringArray& Formation::GetStringArrayParameter(const Integer id) const
{
   if (id == ADDED_SPACECRAFT)
      return componentNames;
   return SpaceObject::GetStringArrayParameter(id);
}




//bool Formation::SetStringParameter(const std::string &label, 
//                                   const std::string &value)
//{
//}
//
//
//bool Formation::SetStringParameter(const std::string &label, 
//                                   const std::string &value,
//                                   const Integer index)
//{
//}
//
//
//const StringArray& Formation::GetStringArrayParameter(const std::string &label)const
//{
//}


GmatBase* Formation::GetRefObject(const Gmat::ObjectType type, 
                                  const std::string &name,
                                  const Integer index)
{
   return SpaceObject::GetRefObject(type, name, index);
}


bool Formation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name)
{
   SpaceObject *so;
   
   if (type == Gmat::SPACECRAFT) {
      so = ((SpaceObject*)(obj));
      if (find(components.begin(), components.end(), so) == components.end()) {
         PropState *ps = &(so->GetState());
         Integer size = ps->GetDimension();
         dimension += size;
         Real newepoch = so->GetEpoch();
         if (components.size() == 0)
            state.SetEpoch(newepoch);
         else
            if (state.GetEpoch() != newepoch)
               throw SpaceObjectException("Epochs are not synchronized in the formation " + instanceName);
         components.push_back(so);
      }
   }
   
   return SpaceObject::SetRefObject(obj, type, name);
}


bool Formation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name,
                                    const Integer index)
{
   return SpaceObject::SetRefObject(obj, type, name, index);
}


ObjectArray& Formation::GetRefObjectArray(const Gmat::ObjectType type)
{
   static ObjectArray oa;
   oa.clear();
   if ((type == Gmat::SPACECRAFT) || (type == Gmat::FORMATION)) {
      for (std::vector<SpaceObject *>::iterator i = components.begin();
           i != components.end(); ++i)
         if ((*i)->GetType() == type)
            oa.push_back(*i);
      return oa;
   }

   if (type == Gmat::SPACEOBJECT) {
      for (std::vector<SpaceObject *>::iterator i = components.begin();
           i != components.end(); ++i)
         oa.push_back(*i);
      return oa;
   }
      
   return SpaceObject::GetRefObjectArray(type);
}


ObjectArray& Formation::GetRefObjectArray(const std::string& typeString)
{
   Gmat::ObjectType id = Gmat::UNKNOWN_OBJECT;
   if (typeString == "Spacecraft") 
      id = Gmat::SPACECRAFT;
   if (typeString == "Formation") 
      id = Gmat::FORMATION;
   if (typeString == "SpaceObject") 
      id = Gmat::SPACEOBJECT;
      
   if (id != Gmat::UNKNOWN_OBJECT)
      return GetRefObjectArray(id);
      
   return SpaceObject::GetRefObjectArray(typeString);
}


void Formation::BuildState()
{
   // Setup the PropState
   Real *data = new Real[dimension], *st;
   Integer j = 0, k;
   PropState *ps;
   
   if (state.GetDimension() < dimension)
      state.Grow(dimension);
   
   for (std::vector<SpaceObject*>::iterator i = components.begin();
        i != components.end(); ++i) {
       ps = &((*i)->GetState());
       st = ps->GetState();
       for (k = 0; k < ps->GetDimension(); ++k) {
          data[j + k] = st[k];
       }
       j += ps->GetDimension();
   }
   
   if (!state.SetState(data, dimension))
      throw SpaceObjectException("Error building Formation state");
}


void Formation::UpdateElements()
{
   Integer size, index = 0;
   PropState *ps;
   for (std::vector<SpaceObject*>::iterator i = components.begin();
        i != components.end(); ++i) {
       ps = &((*i)->GetState());
       size = ps->GetDimension();
       memcpy(ps->GetState(), &((state.GetState())[index]), size*sizeof(Real));
       index += size;
       if ((*i)->GetType() == Gmat::FORMATION)
          ((Formation*)(*i))->UpdateElements();
   }
}
