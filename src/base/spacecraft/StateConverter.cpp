//$Header$ 
//------------------------------------------------------------------------------
//                              StateConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/03/22
//
/**
 * Implements the StateConverter class
 */
//------------------------------------------------------------------------------

#include "StateConverter.hpp"
#include "MessageInterface.hpp"

// #define DEBUG_STATE_CONVERTER 1

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  StateConverter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
StateConverter::StateConverter()
// :   Converter("Cartesian") 
{
   type = "Cartesian";
   mu = DEFAULT_MU;
}

//---------------------------------------------------------------------------
//  StateConverter(const std::string &newTtype)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <newType> Element type of coordinate system.
 *
 */
StateConverter::StateConverter(const std::string &newType) 
// :   Converter(type) 
{
   type = newType;
   mu = DEFAULT_MU;
}

//---------------------------------------------------------------------------
//  StateConverter(const std::string &newTtype, const Real newMu)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <newType> Element type of coordinate system.
 *
 */
StateConverter::StateConverter(const std::string &newType, const Real newMu) 
{
   type = newType;
   mu = newMu;
}

//---------------------------------------------------------------------------
//  StateConverter(const StateConverter &stateConverter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base StateConverter structures.
 *
 * @param <stateConverter> The original that is being copied.
 */
StateConverter::StateConverter(const StateConverter &stateConverter) 
//    : Converter (stateConverter.type)
{
   type = stateConverter.type;
   mu   = stateConverter.mu; 
}

//---------------------------------------------------------------------------
//  ~StateConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
StateConverter::~StateConverter()
{
}

//---------------------------------------------------------------------------
//  StateConverter& operator=(const StateConverter &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
StateConverter& StateConverter::operator=(const StateConverter &converter)
{
    // Don't do anything if copying self
    if (&converter == this)
        return *this;

    // Will fix it later
    return *this;
}

//---------------------------------------------------------------------------
//  Real StateConverter::GetMu() const
//---------------------------------------------------------------------------
/**
 * Get the mu 
 *
 * @return mu's value
 */
Real StateConverter::GetMu() const
{
   return mu;
}

//---------------------------------------------------------------------------
//  bool StateConverter::SetMu(const SolarSystem *solarSystem, 
//                             const std::string &body)
//---------------------------------------------------------------------------
/**
 * Set the mu from the Celestial body's gravitational constant
 *
 * @return true if successful; otherwise, return false
 *
 */
bool StateConverter::SetMu(SolarSystem *solarSystem, 
                           const std::string &body)
{
   if (solarSystem == NULL) return false;

   CelestialBody *centralBody = solarSystem->GetBody(body);
   if (centralBody == NULL) return false;

   // Get the gravitational constant and set new value for mu
   mu = centralBody->GetGravitationalConstant();

   return true;
}


//loj: 10/21/04 added
//---------------------------------------------------------------------------
//  Rvector6 StateConverter::Convert(const Rvector6 &state,   
//                                   const std::string &fromElementType,
//                                   const std::string &toElementType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state> Element states 
 * @param <fromElementType>  Element Type 
 * @param <toElementType> Element Type
 *
 * @return Converted states from the specific element type 
 */
//---------------------------------------------------------------------------
Rvector6 StateConverter::Convert(const Rvector6 &state,   
                                 const std::string &fromElementType,
                                 const std::string &toElementType)
{
   Real tempState[6];
   for (int i=0; i<6; i++)
      tempState[i] = state.Get(i);

   return Convert(tempState, fromElementType, toElementType);
}

//---------------------------------------------------------------------------
//  Rvector6 StateConverter::Convert(const Real *state,   
//                                   const std::string &toElementType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state> Element states 
 * @param <toElementType> Element Type
 *
 * @return Converted states from the specific element type 
 */
Rvector6 StateConverter::Convert(const Real *state,
                                 const std::string &toElementType)
{
   return Convert(state, type, toElementType);
}

//---------------------------------------------------------------------------
//  Rvector6 StateConverter::Convert(Real *state,   
//                                   const std::string &fromElementType,
//                                   const std::string &toElementType)
//---------------------------------------------------------------------------
/**
 * Assignment operator for StateConverter structures.
 *
 * @param <state> Element states 
 * @param <fromElementType>  Element Type 
 * @param <toElementType> Element Type
 *
 * @return Converted states from the specific element type 
 */
Rvector6 StateConverter::Convert(const Real *state,   
                                 const std::string &fromElementType,
                                 const std::string &toElementType)
{
   Rvector6 newState;
   newState.Set(state[0],state[1],state[2],state[3],state[4],state[5]); 

#if DEBUG_STATE_CONVERTER
   MessageInterface::ShowMessage
       ("StateConverter::Convert() fromElementType=%s, toElementType=%s,"
        " state=\n %f %f %f %f %f %f\n", fromElementType.c_str(),
        toElementType.c_str(), state[0], state[1], state[2], state[3],
        state[4], state[5]);
#endif

   // Check if both are the same then return the state with no conversion 
   if (fromElementType == toElementType)
      return newState;    // @todo - should throw exception??
    
   try
   {
      // Determine the input of coordinate representation 
      if (fromElementType == "Cartesian") 
      {
         if (toElementType == "Keplerian" || 
             toElementType == "ModifiedKeplerian") 
         {
            Real meanAnomaly;   // why use MeanAnomaly?
            Rvector6 kepl = CartesianToKeplerian(newState,mu,&meanAnomaly);
                      
            if (toElementType == "ModifiedKeplerian")
                return KeplerianToModKeplerian(kepl);
            else
               return kepl; 
         } 

         if (toElementType == "SphericalAZFPA")
            return CartesianToSphericalAZFPA(newState);

         if (toElementType == "SphericalRADEC")
            return CartesianToSphericalRADEC(newState);

      }
      else if (fromElementType == "Keplerian")
      {       
         if (toElementType == "Cartesian")
            return(KeplerianToCartesian(newState,mu,CoordUtil::TA));

         else if (toElementType == "ModifiedKeplerian")
            return KeplerianToModKeplerian(newState); 

         else if (toElementType == "SphericalAZFPA")
            return KeplerianToSphericalAZFPA(newState,mu);

         else if (toElementType == "SphericalRADEC")
            return KeplerianToSphericalRADEC(newState,mu);
      }
      else if (fromElementType == "ModifiedKeplerian")
      {       
         if (toElementType == "Cartesian")
         {
            Rvector6 keplerian = ModKeplerianToKeplerian(newState);
            return KeplerianToCartesian(keplerian,mu,CoordUtil::TA);
         }
         else if (toElementType == "Keplerian")
            return ModKeplerianToKeplerian(newState); 

         else if (toElementType == "SphericalAZFPA")
         {
            Rvector6 keplerian = ModKeplerianToKeplerian(newState);
            return KeplerianToSphericalAZFPA(keplerian,mu);
         }

         else if (toElementType == "SphericalRADEC")
         {
            Rvector6 keplerian = ModKeplerianToKeplerian(newState);
            return KeplerianToSphericalRADEC(keplerian,mu);
         }
      }
      else if (fromElementType == "SphericalAZFPA")
      {       
         if (toElementType == "Cartesian")
            return SphericalAZFPAToCartesian(newState);

         else if (toElementType == "Keplerian")
            return SphericalAZFPAToKeplerian(newState,mu);

         else if (toElementType == "ModifiedKeplerian")
         {
            Rvector6 keplerian = SphericalAZFPAToKeplerian(newState,mu);
            return KeplerianToModKeplerian(keplerian);
         }

         else if (toElementType == "SphericalRADEC")
            return AZFPA_To_RADECV(newState);
      }
      else if (fromElementType == "SphericalRADEC")
      {       
         if (toElementType == "Cartesian")
            return SphericalRADECToCartesian(newState);

         else if (toElementType == "Keplerian")
            return SphericalRADECToKeplerian(newState,mu);

         else if (toElementType == "ModifiedKeplerian")
         {
            Rvector6 keplerian = SphericalRADECToKeplerian(newState,mu);
            return KeplerianToModKeplerian(keplerian);
         }

         else if (toElementType == "SphericalAZFPA")
            return RADECV_To_AZFPA(newState);
      }
    }
    catch(UtilityException &ue)
    {
       throw ue;
    } 

    return newState;   //  Nothing change
}
