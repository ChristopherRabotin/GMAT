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
 * Writes the specified output to a file.
 */
//------------------------------------------------------------------------------


#include "ReportFile.hpp"

ReportFile::ReportFile(char * filename)
{
    if (filename)
        dstream.open(filename);
    else
        dstream.open("ReportFile.txt");
}


ReportFile::~ReportFile(void)
{
	dstream.flush();
    dstream.close();
}


bool ReportFile::Distribute(int len)
{
    if (!dstream.is_open())
        return false;

    if (len == 0)
        dstream << data;
    else
        for (int i = 0; i < len; ++i)
            dstream << data[i];

    return true;
}
