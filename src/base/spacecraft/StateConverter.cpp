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
StateConverter::StateConverter() :
   Converter("Cartesian") 
{
}

//---------------------------------------------------------------------------
//  StateConverter(std::string &type)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 *
 */
StateConverter::StateConverter(const std::string &type) :
   Converter(type) 
{
}

//---------------------------------------------------------------------------
//  StateConverter(const StateConverter &stateConverter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base StateConverter structures.
 *
 * @param <stateConverter> The original that is being copied.
 */
StateConverter::StateConverter(const StateConverter &stateConverter) :
    Converter (stateConverter.type)
{
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
//  Rvector6 StateConverter::Convert(const Real *state,   
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

    // Determine the input of coordinate representation 
    if (fromElementType == "Cartesian" && toElementType != "Cartesian")
    {
       Cartesian *cartesian = new Cartesian(newState.GetR(),newState.GetV());

       if (toElementType == "Keplerian")
       {
          Keplerian *keplerian = new Keplerian(ToKeplerian(*cartesian,
                                                  GmatPhysicalConst::mu));
         
          // Set the states values 
          newState.Set(keplerian->GetSemimajorAxis(),
                       keplerian->GetEccentricity(),
                       keplerian->GetInclination(),
                       keplerian->GetRAAscendingNode(),
                       keplerian->GetArgumentOfPeriapsis(),
                       keplerian->GetMeanAnomaly() );
                   
          delete keplerian;           
       }
       delete cartesian;
    }
    else if (fromElementType == "Keplerian" && toElementType != "Keplerian")
    {
       Keplerian *keplerian = new Keplerian(state[0],state[1],state[2],
                                            state[3],state[4],state[5]);
   
       Cartesian *cartesian = new Cartesian(ToCartesian(*keplerian,
                                               GmatPhysicalConst::mu));

       delete keplerian;

       if (toElementType == "Cartesian")
       {
          // Get the position and velocity from Cartesian and then
          // set the states values
          Rvector3 position = cartesian->GetPosition();
          Rvector3 velocity = cartesian->GetVelocity();

          newState.Set(position.Get(0),position.Get(1),position.Get(2),
                       velocity.Get(0),velocity.Get(1),velocity.Get(2));

       }
       delete cartesian;
    }
 
    return newState;
}
