#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

#include "GmatBase.hpp"
#include "PropState.hpp"

class SpaceObject : public GmatBase
{
public:
	SpaceObject(Gmat::ObjectType typeId, const std::string &typeStr, 
               const std::string &nomme);
	virtual ~SpaceObject();
   SpaceObject(const SpaceObject& so);
   SpaceObject&      operator=(const SpaceObject& so);
   
protected:
   /// Enumerated parameter IDs   
   enum
   {
      SpaceObjectParamCount = GmatBaseParamCount
   }; 
   
};

#endif // SPACEOBJECT_H
