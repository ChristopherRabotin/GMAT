//$Id$
//------------------------------------------------------------------------------
//                                   Burn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the Burn base class used for impulsive and finite maneuvers. 
 */
//------------------------------------------------------------------------------


#include "Burn.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURN_PARAM 1

//---------------------------------
// static data
//---------------------------------

/// Labels used for the thruster element parameters.
const std::string
Burn::PARAMETER_TEXT[BurnParamCount - GmatBaseParamCount] =
{
   "Origin",
   "Axes",
   "CoordinateSystem",
   "VectorFormat",
   "Element1",
   "Element2",
   "Element3",
   "Element1Label",
   "Element2Label",
   "Element3Label",
   "SpacecraftName",
};

/// Types of the parameters used by thrusters.
const Gmat::ParameterType
Burn::PARAMETER_TYPE[BurnParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE, 
   Gmat::REAL_TYPE, 
   Gmat::REAL_TYPE, 
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Burn(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the Burn object (default constructor).
 * 
 * @param <type>    Gmat::ObjectTypes enumeration for the object.
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
Burn::Burn(Gmat::ObjectType type, const std::string &typeStr,
           const std::string &nomme) :
   GmatBase        (type, typeStr, nomme),
   coordAxes       ("Inertial"),
   vectorFormat    ("Cartesian"),
   coordinateSystem(""),
   satName         (""),
   sc              (NULL),
   solarSystem     (NULL),
   burnOriginName  ("Earth"),
   burnOrigin      (NULL),
   j2000BodyName   ("Earth"),
   j2000Body       (NULL)

{
   objectTypes.push_back(Gmat::BURN);
   objectTypeNames.push_back("Burn");
   
   parameterCount = BurnParamCount;
   
   deltaV[0] = deltaV[1] = deltaV[2] = 0.0;
   frameman = new ManeuverFrameManager;

   /// Load the default maneuver frame
   frame = frameman->GetFrameInstance(coordAxes);

   dvLabels[0] = frame->GetFrameLabel(1);
   dvLabels[1] = frame->GetFrameLabel(2);
   dvLabels[2] = frame->GetFrameLabel(3);
    
   frameBasis[0][0] = frameBasis[1][1] = frameBasis[2][2] = 1.0;
   frameBasis[0][1] = frameBasis[1][0] = frameBasis[2][0] =
   frameBasis[0][2] = frameBasis[1][2] = frameBasis[2][1] = 0.0;
}


//------------------------------------------------------------------------------
//  ~Burn()
//------------------------------------------------------------------------------
/**
 * Destroys the Burn object (destructor).
 */
//------------------------------------------------------------------------------
Burn::~Burn()
{
   delete frameman;
}


//------------------------------------------------------------------------------
//  Burn(const Burn &b)
//------------------------------------------------------------------------------
/**
 * Constructs the Burn object (copy constructor).
 * 
 * @param <b> Object that is copied
 */
//------------------------------------------------------------------------------
Burn::Burn(const Burn &b) :
   GmatBase        (b),
   coordAxes       (b.coordAxes),
   vectorFormat    (b.vectorFormat),
   coordinateSystem(b.coordinateSystem),
   frame           (NULL),
   satName         (b.satName),
   sc              (NULL),
   solarSystem     (b.solarSystem),
   burnOriginName  (b.burnOriginName),
   burnOrigin      (NULL),
   j2000BodyName   (b.j2000BodyName),
   j2000Body       (NULL)
{
   deltaV[0] = b.deltaV[0];
   deltaV[1] = b.deltaV[1];
   deltaV[2] = b.deltaV[2];
   dvLabels[0] = b.dvLabels[0];
   dvLabels[1] = b.dvLabels[1];
   dvLabels[2] = b.dvLabels[2];
    
   for (Integer i = 0; i < 3; i++)
      for (Integer j = 0; j < 3; j++)
         frameBasis[i][j]  = b.frameBasis[i][j];
   
   parameterCount = b.parameterCount;
   
   frameman = new ManeuverFrameManager;
}


//------------------------------------------------------------------------------
//  Burn& operator=(const Burn &b)
//------------------------------------------------------------------------------
/**
 * Sets one burn object to match another (assignment operator).
 * 
 * @param <b> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Burn& Burn::operator=(const Burn &b)
{
   if (this != &b)
   {
      GmatBase::operator=(b);
   
      coordAxes         = b.coordAxes;
      vectorFormat      = b.vectorFormat;
      coordinateSystem  = b.coordinateSystem;
      frame             = NULL;
      satName           = b.satName;
      sc                = NULL;
      solarSystem       = b.solarSystem;
      burnOriginName    = b.burnOriginName;
      burnOrigin        = NULL;
      j2000BodyName     = b.j2000BodyName;
      j2000Body         = NULL;
   
      deltaV[0]         = b.deltaV[0];
      deltaV[1]         = b.deltaV[1];
      deltaV[2]         = b.deltaV[2];
      dvLabels[0]       = b.dvLabels[0];
      dvLabels[1]       = b.dvLabels[1];
      dvLabels[2]       = b.dvLabels[2];
      
      for (Integer i = 0; i < 3; i++)
         for (Integer j = 0; j < 3; j++)
            frameBasis[i][j]  = b.frameBasis[i][j];
      
      frameman = new ManeuverFrameManager;
   }

   return *this;
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string Burn::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BurnParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer Burn::GetParameterID(const std::string &str) const
{
   #if DEBUG_BURN_PARAM
   MessageInterface::ShowMessage
      ("Burn::GetParameterID() str=%s, dvLabels=%s, %s, %s\n", str.c_str(),
       dvLabels[0].c_str(), dvLabels[0].c_str(), dvLabels[0].c_str());
   #endif
   
   // Let users ask for components (e.g. "V", "N", or "B" in VNB coordinates)
   // directly
   if (str == dvLabels[0])
      return DELTAV1;
        
   if (str == dvLabels[1])
      return DELTAV2;
        
   if (str == dvLabels[2])
      return DELTAV3;
   
   for (Integer i = GmatBaseParamCount; i < BurnParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Burn::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < BurnParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
      
   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Burn::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Burn::IsParameterReadOnly(const Integer id) const
{
   if ((id == DELTAV1LABEL) || (id == DELTAV2LABEL) ||
       (id == DELTAV3LABEL) || (id == SATNAME) ||
       (id == COORDINATESYSTEM)) //loj:2007.11.15 until COORDINATESYSTEM works
      return true;
   
   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a Real parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
Real Burn::GetRealParameter(const Integer id) const
{
   if (id == DELTAV1)
      return deltaV[0];
        
   if (id == DELTAV2)
      return deltaV[1];
        
   if (id == DELTAV3)
      return deltaV[2];
        
   return GmatBase::GetRealParameter(id);
}

      
//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Real parameter.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
Real Burn::SetRealParameter(const Integer id, const Real value)
{
   if (id == DELTAV1)
   {
      deltaV[0] = value;
      return deltaV[0];
   }
        
   if (id == DELTAV2)
   {
      deltaV[1] = value;
      return deltaV[1];
   }
   
   if (id == DELTAV3)
   {
      deltaV[2] = value;
      return deltaV[2];
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string Burn::GetStringParameter(const Integer id) const
{
   if (id == BURNORIGIN)
      return burnOriginName;
        
   if (id == BURNAXES)
      return coordAxes;
        
   if (id == COORDINATESYSTEM)
      return coordinateSystem;
        
   if (id == VECTORFORMAT)
      return vectorFormat;
        
   if (id == DELTAV1LABEL)
      return dvLabels[0];
        
   if (id == DELTAV2LABEL)
      return dvLabels[1];
        
   if (id == DELTAV3LABEL)
      return dvLabels[2];
        
   if (id == SATNAME)
      return satName;
      
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
bool Burn::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == BURNORIGIN)
   {
      burnOriginName = value;
      return true;
   }
           
   if (id == BURNAXES)
   {
      /// @todo validate the input value when the CS code is incorporated.
      // if (!IsValidFrame(value))
      //    return false;
      
      // Burns know the frame options, so use that to detect if there is 
      // an issue with the input
      StringArray frames = GetStringArrayParameter(BURNAXES);
      if (find(frames.begin(), frames.end(), value) == frames.end())
      { 
         // For now, keep "Inertial" as a deprecated option
         if (value != "Inertial")
         {
            std::string framelist = frames[0];
            for (UnsignedInt n = 1; n < frames.size(); ++n)
               framelist += ", " + frames[n];
            throw BurnException(
               "The value of \"" + value + "\" for field \"Axes\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ " + framelist + " ]. ");
         }
         else
         {
            MessageInterface::ShowMessage("\"Inertial\" maneuver frames are "
               "deprecated and will be removed from a future build; please use "
               "\"MJ2000Eq\" instead.\n");
               coordAxes = "MJ2000Eq";
         }
      }
      else
         coordAxes = value;
      frame = frameman->GetFrameInstance(coordAxes);

      dvLabels[0] = frame->GetFrameLabel(1);
      dvLabels[1] = frame->GetFrameLabel(2);
      dvLabels[2] = frame->GetFrameLabel(3);
      return true;
   }
        
   if (id == COORDINATESYSTEM)
   {
      throw BurnException("Burns cannot use GMAT coordinate systems yet.");
   }


   if (id == VECTORFORMAT)
   {
      /// @todo added Spherical
      if (value != "Cartesian")
         throw BurnException(
            "The value of \"" + value + "\" for field \"Vector Format\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ Cartesian ]. ");
      vectorFormat = value;
      return true;
   }
        
   // Cannot change the labels -- they are set internally
   if (id == DELTAV1LABEL)
      return false;
        
   if (id == DELTAV2LABEL)
      return false;
        
   if (id == DELTAV3LABEL)
      return false;
        
   if (id == SATNAME)
   {
      satName = value;
      return true;
   }
   
   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value,
//                          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a specific std::string element in an array.
 *
 * @param <id>    Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * @param <index> Index for the element
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool Burn::SetStringParameter(const Integer id, const std::string &value,
                              const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 * 
 * For the Burn classes, calls to this method get passed to the maneuver frame
 * manager when the user requests the frames that are available for the system.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Burn::GetStringArrayParameter(const Integer id) const
{
   if (id == BURNAXES)
      return frameman->GetSupportedFrames();

   return GmatBase::GetStringArrayParameter(id);
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
const ObjectTypeArray& Burn::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   //Spacecraft is not used
   //refObjectTypes.push_back(Gmat::SPACECRAFT);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& Burn::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
      refObjectNames.push_back(satName);
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
      refObjectNames.push_back(burnOriginName);

   return refObjectNames;
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
* This method sets a reference object for the CoordinateSystem class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object 
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool Burn::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                        const std::string &name)
{
   #if DEBUG_BURN_SET
   MessageInterface::ShowMessage
      ("===> Burn::SetRefObject() objType=%d, objTypeName=%s, objName=%s, type=%d, "
       "name=%s\n", obj->GetType(), obj->GetTypeName().c_str(), obj->GetName().c_str(),
       type, name.c_str());
   #endif
   
   switch (type)
   {
   case Gmat::SPACE_POINT:
   case Gmat::CELESTIAL_BODY:
      {
         burnOriginName = obj->GetName();
         burnOrigin = (SpacePoint*)obj;
         return true;
      }
   case Gmat::SPACECRAFT:
      {
         satName = obj->GetName();
         sc = (Spacecraft*)obj;
         return true;
      }
   default:
      return GmatBase::SetRefObject(obj, type, name);
   }
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference object name used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool Burn::RenameRefObject(const Gmat::ObjectType type,
                           const std::string &oldName,
                           const std::string &newName)
{
   if (type == Gmat::SPACECRAFT)
   {
      if (satName == oldName)
         satName = newName;
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  void SetSpacecraftToManeuver(Spacecraft *sat)
//------------------------------------------------------------------------------
/**
 * Accessor method used by Maneuver to pass in the spacecraft pointer
 * 
 * @param <sat> the Spacecraft
 */
//------------------------------------------------------------------------------
void Burn::SetSpacecraftToManeuver(Spacecraft *sat)
{
   sc = sat;
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the internal solar system pointer for objects that have one.
 *
 * @param ss   The solar system.
 */
//------------------------------------------------------------------------------
void Burn::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
//  bool Burn::Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the bodies used in teh burn calculations.
 */
//------------------------------------------------------------------------------
bool Burn::Initialize()
{
   bool retval = GmatBase::Initialize();
   
   if (retval)
   {
      j2000Body = solarSystem->GetBody(j2000BodyName);
      if (!burnOrigin)
         burnOrigin = solarSystem->GetBody(burnOriginName);
   
      if ((!burnOrigin) || (!j2000Body))
         throw BurnException("Unable to initialize the burn object " + 
            instanceName + "; either " + j2000BodyName + " or " + 
            burnOriginName + " was not set for the burn.");
   }
         
   return retval;
}


//------------------------------------------------------------------------------
//  void TransformJ2kToBurnOrigin(const Real *scState, Real *state, Real epoch)
//------------------------------------------------------------------------------
/**
 * Resets the state to use the origin specified for the Burn.
 *
 * @param scState The input spacecraft state.
 * @param state   The transformed state.
 * @param epoch   The epoch of the input (and output) state.
 */
//------------------------------------------------------------------------------
void Burn::TransformJ2kToBurnOrigin(const Real *scState, Real *state, 
                                    Real epoch)
{
   #ifdef DEBUG_BURN_ORIGIN
      MessageInterface::ShowMessage(
         "State transformation for Burn\n"
         "   Input state =  [ %lf %lf %lf %.9lf %.9lf %.9lf ]\n", scState[0], 
         scState[1], scState[2], scState[3], scState[4], scState[5]);
   #endif

   if ((j2000Body == NULL) || (burnOrigin == NULL))
      Initialize();
      
   memcpy(state, scState, 6*sizeof(Real));
   if (j2000Body != burnOrigin)
   {
      Rvector6 j2kState = j2000Body->GetMJ2000State(epoch);
      Rvector6 originState = burnOrigin->GetMJ2000State(epoch);
      Rvector6 delta = j2kState - originState;

      #ifdef DEBUG_BURN_ORIGIN
         MessageInterface::ShowMessage(
            "   j2000       =  [ %lf %lf %lf %.9lf %.9lf %.9lf ]\n",
            j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4], 
            j2kState[5]);
         MessageInterface::ShowMessage(
            "   Origin      =  [ %lf %lf %lf %.9lf %.9lf %.9lf ]\n",
            originState[0], originState[1], originState[2], originState[3], 
            originState[4], originState[5]);
         MessageInterface::ShowMessage(
            "   Delta       =  [ %lf %lf %lf %.9lf %.9lf %.9lf ]\n",
            delta[0], delta[1], delta[2], delta[3], delta[4], delta[5]);
      #endif
      
      state[0] += delta[0];
      state[1] += delta[1];
      state[2] += delta[2];
      state[3] += delta[3];
      state[4] += delta[4];
      state[5] += delta[5];
   }
   
   #ifdef DEBUG_BURN_ORIGIN
      MessageInterface::ShowMessage(
         "   Output state = [ %lf %lf %lf %.9lf %.9lf %.9lf ]\n",
         state[0], state[1], state[2], state[3], state[4], state[5]);
   #endif
}
