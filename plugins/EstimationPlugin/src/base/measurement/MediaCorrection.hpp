//$Id: MediaCorrection.hpp 67 2010-06-17 21:56:16Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         MediaCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen GSFC-NASA
// Created: 2010/06/17
//
/**
 * Media correction model.
 */
//------------------------------------------------------------------------------


#ifndef MediaCorrection_hpp
#define MediaCorrection_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"


class ESTIMATION_API MediaCorrection: public GmatBase
{
public:
	MediaCorrection(const std::string &typeStr,
						const std::string &nomme = "");
	virtual ~MediaCorrection();
   MediaCorrection(const MediaCorrection& mdc);
   MediaCorrection& operator=(const MediaCorrection& mc);
   virtual GmatBase*    Clone() const;

	virtual bool SetModel(Integer mod);
	virtual bool SetModelName(std::string modName);

   virtual void SetSolarSystem(SolarSystem *ss);

	virtual RealArray Correction();

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
	Integer model;
	std::string modelName;

	SolarSystem *solarSystem;
};

#endif /* MediaCorrection_hpp */
