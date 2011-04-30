//$Id$
//------------------------------------------------------------------------------
//                              TextTrajectoryFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/11/25
/**
 * Declares TextTrajectoryFile class.
 * This class reads 7 elements in the line: Time, X, Y, Z, Vx, Vy, Vz and
 * stores in the internal array.
 */
//------------------------------------------------------------------------------
#ifndef TextTrajectoryFile_hpp
#define TextTrajectoryFile_hpp

#include "gmatdefs.hpp"
#include <fstream>

namespace GmatTraj
{
    class DataType
    {
    public:
        Real time;
        float x;
        float y;
        float z;
    };
    
    const Integer NUM_ITEM_IN_LINE = 7;
    const Integer INITIAL_NUM_POINTS = 100;

}

typedef std::vector<GmatTraj::DataType> TrajectoryArray;

#ifdef EXPORT_TEMPLATES

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // Fix vector of GmatTraj::DataType:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<GmatTraj::DataType>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<GmatTraj::DataType>;

#endif

class GMAT_API TextTrajectoryFile
{
public:
    TextTrajectoryFile(const std::string &fileName = "");
    ~TextTrajectoryFile();
    bool Open(const std::string &fileName = "");
    void Close();
    TrajectoryArray& GetData();
    
protected:
    bool ReadLine();
    
    std::string mFileName;
    std::ifstream mInStream;  // input data stream
    Real mTempData[GmatTraj::NUM_ITEM_IN_LINE];
    GmatTraj::DataType mTrajData;
    TrajectoryArray mDataArray;
    
};

#endif
