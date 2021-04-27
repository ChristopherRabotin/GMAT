//$Id$
//------------------------------------------------------------------------------
//                                  FieldOfViewException
//
// Author: Syeda Kazmi
// Created: 5/23/2019
//
/**
 * Exception class used by the FieldOfView hierarchy.
 */
 //------------------------------------------------------------------------------


#include "FieldOfViewException.hpp"

FieldOfViewException::FieldOfViewException(std::string details) :
	BaseException("FieldOfView Exception Thrown: ", details)
{}


FieldOfViewException::~FieldOfViewException()
{}


FieldOfViewException::FieldOfViewException(const FieldOfViewException &he) :
	BaseException(he)
{}

