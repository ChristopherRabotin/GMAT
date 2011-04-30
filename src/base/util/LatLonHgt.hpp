//$Id$
//------------------------------------------------------------------------------
//                              LatLonHgt
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed by Dr. Matthew P. Wilkins, Schafer Corporation
//
// Author: Matthew P. Wilkins
// Created: 2008/05/28
//
/**
 *
 * Implements LatLonHgt Base class to contain the associated elements
 * Latitude, Longitude, and Geodectic/Geocentric Height
 *
 */
//------------------------------------------------------------------------------

#ifndef LatLonHgt_hpp
#define LatLonHgt_hpp

#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "Linear.hpp"
#include "RealUtilities.hpp"
#include "Rvector3.hpp"

class GMAT_API LatLonHgt 
{
public:

  LatLonHgt();
  LatLonHgt(const Rvector3 &cartPosition, const Real &equatorialRadius, 
	    const Real &flattening, const std::string &typ="Geodetic",
	    const std::string &hgtRef="Ellipsoid");
  LatLonHgt(const Real &lat, const Real &lon, const Real &hgt, 
	    const std::string &typ="Geodetic", 
	    const std::string &hgtRef="Ellipsoid"); 
  LatLonHgt(const LatLonHgt &LatLonHgt);
  LatLonHgt& operator=(const LatLonHgt &LatLonHgt);
  virtual ~LatLonHgt();
  
  // Friend function
  friend std::ostream& operator<<(std::ostream& output, LatLonHgt &llh);
  friend std::istream& operator>>(std::istream& input, LatLonHgt &llh);
  
  // public method 
  Real GetLatitude() const;
  void SetLatitude(const Real &lat, const std::string &typ);
  
  Real GetLongitude() const;
  void SetLongitude(const Real &lon);
    
  Real GetHeight() const;
  void SetHeight(const Real &hgt);

  std::string GetHeightRef() const;
  void SetHeightRef(const std::string &hgtReference);

  std::string GetType() const;
  void SetType(const std::string &typ);
  
  Integer GetNumData() const;
  const std::string* GetDataDescriptions() const;
  const std::string* GetTypeDescriptions() const;
  const std::string* GetHeightDescriptions() const;
  std::string* ToValueStrings();
  
  Rvector3 GetSitePosition(const Real &equatorialRadius, const Real &flattening);
  void GeocentricToGeodeticLat( const Real &flattening );
  void GeodeticToGeocentricLat( const Real &flattening );
  void ReducedToGeodeticLat( const Real &flattening );
  void ReducedToGeocentricLat( const Real &flattening );
  void GeocentricToReducedLat( const Real &flattening );
  void GeodeticToReducedLat( const Real &flattening );

  Real GeodeticToReducedLat( const Real &gdlat, const Real &flattening );
  Real GeocentricToReducedLat( const Real &gclat, const Real &flattening );
  Real ReducedToGeocentricLat( const Real &rdlat, const Real &flattening );
  Real ReducedToGeodeticLat( const Real &rdlat, const Real &flattening );
  Real GeocentricToGeodeticLat( const Real &gclat, const Real &flattening );
  Real GeodeticToGeocentricLat( const Real &gdlat, const Real &flattening );
  
protected:
  
  Real     latitude;    //  angle measured from the Equatorial plane to point of interest
  Real     longitude;       // angle measured positive to the east from the Greenwhich meridian 
  Real     height;          //  height above the Earth's surface
  std::string type;   // Geodetic, geocentric, reduced
  std::string hgtRef; // Ellipsoid, Geoid, MeanSeaLevel

  enum TYPE_REPS {
    GEOCENTRIC_ID = 0,
    GEODETIC_ID,
    REDUCED_ID,
    EndTypeReps
  };

  enum HEIGHT_REPS {
    ELLIPSOID_ID = 0,
    GEOID_ID,
    MEANSEALEVEL_ID,
    EndHeightReps
  };

  // protected methods
  void CartesianToLatLonHgt( const Rvector3 &cartVector, const Real &equatorialRadius, 
			     const Real &flattening, const std::string &typ="Geodetic",
			     const std::string &hgtReference="Ellipsoid");
  Integer GetTypeID(const std::string &label);
  std::string GetTypeText(const Integer &id) const;
  Integer GetHeightID(const std::string &label);
  std::string GetHeightText(const Integer &id) const;
  
private:
  static const Integer NUM_DATA = 4;
  static const std::string DATA_DESCRIPTIONS[NUM_DATA];
  static const std::string TYPE_DESCRIPTIONS[3];
  static const std::string HEIGHT_DESCRIPTIONS[3];
  std::string stringValues[NUM_DATA];
  Real GetDegree(const Real angle, const Real minAngle, 
			    const Real maxAngle);
  
};
#endif // LatLonHgt_hpp
