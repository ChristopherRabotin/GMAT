//$Header$
//------------------------------------------------------------------------------
//                              RefFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/07
//
/**
 * Defines base class of Reference Frame
 */
//------------------------------------------------------------------------------
#include "RefFrame.hpp"
#include "CelestialBody.hpp"
#include "A1Mjd.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// CelestialBody GetCentralBody() const
//------------------------------------------------------------------------------
/*
 * Retrieves the central body object
 *
 * @return the central body object
 */
//------------------------------------------------------------------------------
CelestialBody RefFrame::GetCentralBody() const
{
   return mCentralBody;
}

//------------------------------------------------------------------------------
// std::string GetCentralBodyName() const
//------------------------------------------------------------------------------
/*
 * Retrieves the name of central body.
 *
 * @return the name of central body
 */
//------------------------------------------------------------------------------
std::string RefFrame::GetCentralBodyName() const
{
   return mCentralBody.GetName();
}

//------------------------------------------------------------------------------
// A1Mjd GetRefDate() const
//------------------------------------------------------------------------------
/*
 * Retrieves the reference date.
 *
 * @return the reference date
 */
//------------------------------------------------------------------------------
A1Mjd RefFrame::GetRefDate() const
{
   return mRefDate;
}

//------------------------------------------------------------------------------
// std::string GetFrameName() const
//------------------------------------------------------------------------------
/*
 * Retrieves the frame name.
 *
 * @return the frame name.
 */
//------------------------------------------------------------------------------
std::string RefFrame::GetFrameName() const
{
   return mFrameName;
}

//------------------------------------------------------------------------------
// bool operator== (const RefFrame &right) const
//------------------------------------------------------------------------------
/*
 * Equal operator.
 *
 * @param <right> the object to compare for equality
 *
 * @return true if central body and ref. date are equal to right;
 *  false otherwise.
 *
 * @note There will be more data to check for == in the future build.
 */
//------------------------------------------------------------------------------
bool RefFrame::operator== (const RefFrame &right) const
{
   if (mCentralBody.GetName() != right.mCentralBody.GetName())
      return false;

   if (mRefDate != right.mRefDate)
      return false;
   
   return true;
}

//------------------------------------------------------------------------------
// bool operator!= (const RefFrame &right) const
//------------------------------------------------------------------------------
/*
 * Equal operator.
 *
 * @param <right> the object to compare for inequality
 *
 * @return true if central body and ref. date are not equal to right;
 *  false otherwise.
 *
 * @note There will be more data to check for != in the future build.
 */
//------------------------------------------------------------------------------
bool RefFrame::operator!= (const RefFrame &right) const
{
   return !(operator==(right));
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// RefFrame(const CelestialBody &centralBody, const A1Mjd &refDate,
//          const std::string &frameName)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <centralBody> reference to central body object
 * @param <refData> reference to reference date object.
 * @param <frameName> reference frame name
 */
//------------------------------------------------------------------------------
RefFrame::RefFrame(const CelestialBody &centralBody, const A1Mjd &refDate,
                   const std::string &frameName)
{
   mCentralBody = centralBody;
   mRefDate = refDate;
   mFrameName = frameName;
}

//------------------------------------------------------------------------------
// ~RefFrame()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RefFrame::~RefFrame()
{
}

//------------------------------------------------------------------------------
// RefFrame()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
RefFrame::RefFrame()
{
}

