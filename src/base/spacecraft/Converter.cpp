//$Header$ 
//------------------------------------------------------------------------------
//                                 Converter
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
 * Implements the Converter base class
 */
//------------------------------------------------------------------------------

#include "Converter.hpp"

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  Converter()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 */
Converter::Converter()  
{
    type = "unknown";
}

//---------------------------------------------------------------------------
//  Converter(std::string &type)
//---------------------------------------------------------------------------
/**
 * Creates constructors with parameters.
 *
 * @param <typeStr> GMAT script string associated with this type of object.
 *
 */
Converter::Converter(const std::string &type) :
    type  (type)
{
}

//---------------------------------------------------------------------------
//  Converter(const Converter &converter)
//---------------------------------------------------------------------------
/**
 * Copy Constructor for base Converter structures.
 *
 * @param <converter> The original that is being copied.
 */
Converter::Converter(const Converter &converter) :
    type (converter.type)
{
}

//---------------------------------------------------------------------------
//  ~Converter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
Converter::~Converter()
{
}

//---------------------------------------------------------------------------
//  Converter& operator=(const Converter &a)
//---------------------------------------------------------------------------
/**
 * Assignment operator for Converter structures.
 *
 * @param <a> The original that is being copied.
 *
 * @return Reference to this object
 */
Converter& Converter::operator=(const Converter &converter)
{
    // Don't do anything if copying self
    if (&converter == this)
        return *this;

    // Duplicate the member data        
    type    = converter.type;

    return *this;
}

//---------------------------------------------------------------------------
//  std::string GetType()
//---------------------------------------------------------------------------
/**
 * Get the type name.
 * 
 * @return the type name 
 *
 */
std::string Converter::GetType()
{
    return type;
}

//---------------------------------------------------------------------------
//  void SetType(const std::string &type)
//---------------------------------------------------------------------------
/**
 * Set the type name.
 * 
 * @param <type>  Type name 
 *
 */
void Converter::SetType(const std::string &type)
{
    this->type = type;
}
