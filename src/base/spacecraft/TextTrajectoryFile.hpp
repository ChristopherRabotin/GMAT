//$Header$
//------------------------------------------------------------------------------
//                              TextTrajectoryFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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

class TextTrajectoryFile
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
