//$Id$
//------------------------------------------------------------------------------
//                                  StateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/12/15
//
/**
 * Definition of the StateManager base class.  This is the class for state 
 * managers used in GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#ifndef StateManager_hpp
#define StateManager_hpp

#include "GmatState.hpp"
#include "gmatdefs.hpp"
#include <map>
#include <vector>

struct GMAT_API ListItem
{
   std::string objectName;
   std::string elementName;
   std::string associateName;
   GmatBase*   object;
   // Gmat::StateElementId
   Integer     elementID;
   Integer     subelement;
   Integer     parameterID;
   Gmat::ParameterType
               parameterType;
   Integer     rowIndex;      // Used for vectors and arrays
   Integer     rowLength;     // Used for vectors and arrays
   Integer     colIndex;      // Used for arrays
   Integer     length;
   bool        dynamicObjectProperty;  // Set if property forces object updates
   bool        nonzeroInit;			   // Indicates initial value != 0.0
   Real        initialValue;           // Non-zero initial value
   bool        postDerivativeUpdate;   // true = Post-superposition step needed
};


#ifdef EXPORT_TEMPLATES

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // Fix the vector of ListItem pointers:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<ListItem*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<ListItem*>;

   // Fix the map of GmatBase* -> StringArray* pointers:
   EXPIMP_TEMPLATE template struct DECLSPECIFIER std::less<GmatBase*>;
   EXPIMP_TEMPLATE template struct DECLSPECIFIER std::pair<GmatBase*, StringArray>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::pair<GmatBase*, StringArray const>>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::_Tmap_traits<GmatBase*, StringArray, std::less<GmatBase*>, std::allocator<std::pair<GmatBase*, StringArray const>>, false>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::_Tree_nod<std::_Tmap_traits<GmatBase*, StringArray, std::less<GmatBase*>, std::allocator<std::pair<GmatBase*, StringArray const>>, false>>;
//   EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::_Tree_nod<std::_Tmap_traits<GmatBase *,StringArray,std::less<GmatBase *>,std::allocator<std::pair<GmatBase *,const StringArray>>,false>>::_Node >;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::map<GmatBase*, StringArray*>;
   
#endif



/**
 * The state manager base class.
 */
class GMAT_API StateManager
{
public:
	StateManager(Integer size = 0);
	virtual ~StateManager();
   StateManager(const StateManager& sm);
   StateManager& operator=(const StateManager& sm);
   
   // Abstract methods
   virtual bool SetObject(GmatBase* theObject) = 0;
   virtual bool SetProperty(std::string propName) = 0;
   virtual bool SetProperty(std::string propName, Integer index) = 0;
   virtual bool SetProperty(std::string propName, GmatBase *forObject) = 0;
   virtual bool BuildState() = 0;
   virtual bool MapObjectsToVector() = 0;
   virtual bool MapVectorToObjects() = 0;
   
   virtual Integer GetCount(Gmat::StateElementId elementType = 
                               Gmat::UNKNOWN_STATE);

   virtual bool UpdateState();
   virtual GmatState* GetState();
   virtual Integer GetStateSize();
   
   virtual bool GetStateObjects(ObjectArray& pObjects, 
         Gmat::ObjectType type = Gmat::UNKNOWN_OBJECT);
   
   virtual const StringArray& GetObjectList(std::string ofType = "");
   virtual const std::vector<ListItem*>* GetStateMap();

protected:
   /// Size of the managed state vector
   Integer                    stateSize;
   GmatState                  state;
   
   ObjectArray                objects;
   StringArray                objectNames;

   std::vector<Integer>       epochIDs;
   std::map<GmatBase*, StringArray*>  elements;
   GmatBase*                  current;

   std::vector<ListItem*>     stateMap;
};

#endif /*StateManager_hpp*/
