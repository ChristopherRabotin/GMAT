//$Header$
//------------------------------------------------------------------------------
//                                  ReportFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/10/23
//
/**
 * Definition for the ReportFile class.
 */
//------------------------------------------------------------------------------


#ifndef ReportFile_hpp
#define ReportFile_hpp


#include "Subscriber.hpp"
#include <fstream>


class ReportFile :
    public Subscriber
{
public:
    ReportFile(char * filename = NULL);
    virtual ~ReportFile(void);

protected:
    std::ofstream       dstream;

    virtual bool        Distribute(Integer len);
};


#endif
