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
// Created: 2003/12/03
/**
 * Implements TextTrajectoryFile class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "TextTrajectoryFile.hpp"

//------------------------------------------------------------------------------
// TextTrajectoryFile(const std::string &fileName)
//------------------------------------------------------------------------------
TextTrajectoryFile::TextTrajectoryFile(const std::string &fileName)
{
    mFileName = fileName;
    mDataArray.reserve(GmatTraj::INITIAL_NUM_POINTS);
}

//------------------------------------------------------------------------------
// ~TextTrajectoryFile()
//------------------------------------------------------------------------------
TextTrajectoryFile::~TextTrajectoryFile()
{
    if (mInStream.is_open())
        mInStream.close();
}

//------------------------------------------------------------------------------
// bool Open(const std::string &fileName)
//------------------------------------------------------------------------------
bool TextTrajectoryFile::Open(const std::string &fileName)
{
    if (mFileName == "")
        mFileName = fileName;

    if (mFileName != "")
    {
        if (mInStream.is_open())
            mInStream.close();
        
        mInStream.open(mFileName.c_str());
        if (!mInStream.is_open())
            return false; //or throw exception
    }
    else
    {
        return false; //or throw exception
    }
    
    return true;
}

//------------------------------------------------------------------------------
// void Close()
//------------------------------------------------------------------------------
void TextTrajectoryFile::Close()
{
    if (mInStream.is_open())
        mInStream.close();
}

//------------------------------------------------------------------------------
// TrajectoryArray& GetData()
//------------------------------------------------------------------------------
TrajectoryArray& TextTrajectoryFile::GetData()
{
    if (mInStream.is_open())
    {
        while(!mInStream.eof())
        {
            ReadLine();
        }
    }

    return mDataArray;
}

//------------------------------------------------------------------------------
// bool ReadLine()
//------------------------------------------------------------------------------
bool TextTrajectoryFile::ReadLine()
{
    //loj: how can I read first 4 items and skip the rest of the line
    for (int i=0; i<GmatTraj::NUM_ITEM_IN_LINE; i++)
        mInStream >> mTempData[i];

    mTrajData.time = mTempData[0];
    mTrajData.x = (float)mTempData[1];
    mTrajData.y = (float)mTempData[2];
    mTrajData.z = (float)mTempData[3];
    
    // add to mData
    mDataArray.push_back(mTrajData);
    return true;
}
