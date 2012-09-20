//$Id$
//------------------------------------------------------------------------------
//                              GmatFunctionIntro
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, task order 28.
//
// Author: Darrel Conway (Thinking Systems)
// Created: 2012/09/18
//
/**
 * Doxygen introduction page for the GmatFunction plugin.
 */
//------------------------------------------------------------------------------

/**
 * \mainpage GMAT Function Overview
 *
 * This documentation provides a description of the contents of GMAT's
 * GmatFunction plugin module.
 *
 * The interfaces and function class needed to use GMAT functions were moved
 * from the libGmatBase code into a plug-in module for the R2013a release, so
 * that testing of the core system for operations could be performed without the
 * exponential testing burden of all of the permutations possible with
 * GmatFunction objects.
 *
 * \section Setup Setting up GMAT Functions for use
 *
 * In order for GMAT to use the full functionality of the C interface, the
 * interface should be loaded into GMAT as a plug-in at run time.  To do this,
 * add the following line to your startup file:
 *
 * PLUGIN                 = ./libGmatFunction
 *
 * \section Running Using GMAT Functions
 *
 * See the GMAT help system for information about using GmatFunction objects.
 */
