//$Id$
//------------------------------------------------------------------------------
//                           ThreeDSLoader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 6, 2013
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef THREEDSLOADER_HPP_
#define THREEDSLOADER_HPP_

#include "ModelObject.hpp"

class ThreeDSLoader
{
public:
   ThreeDSLoader();
   virtual ~ThreeDSLoader();

   bool LoadFileIntoModel(ModelObject *model, const std::string &filename);

private:
   /// File pointer for the .3ds file
   FILE *theFile;
   /// The size of the file; this might be moved into teh Load method
   long int fileSize;
   /// The model we are loading; should be NULL outside of the load process
   ModelObject *theModel;
   /// Starting index for the current block in the vertex array
   Integer vertexStart;
   /// Starting index for the current block in the polygon array
   Integer polygonStart;
   /// Starting index for the current block in the current material map
   Integer materialPolygonStart;
   /// Color loading: if spec loaded and ambient not set, set both to specular
   bool ambientColorLoaded;
   /// Flag indicating that the model uses colors rather than textures
   bool usesColors;
   /// Number of polygons read on last read, for default material if needed
   unsigned short lastPolygonCount;

   enum {
      CHUNK_MAIN      = 0x4d4d,
      CHUNK_OBJMESH   = 0x3d3d,
      CHUNK_OBJBLOCK  = 0x4000,
      CHUNK_TRIMESH   = 0x4100,
      CHUNK_VERTLIST  = 0x4110,
      CHUNK_FACELIST  = 0x4120,
      CHUNK_FACEMAT   = 0x4130,
      CHUNK_MAPLIST   = 0x4140,
      CHUNK_SMOOLIST  = 0x4150,
      CHUNK_MATERIAL  = 0xAFFF,
      CHUNK_MATNAME   = 0xA000,
      CHUNK_MATACOL   = 0xA010,
      CHUNK_SUBCOLORF = 0x0010,
      CHUNK_SUBCOLOR  = 0x0011,
      CHUNK_MATDIFF   = 0xA020,
      CHUNK_MATSPEC   = 0xA030,
      CHUNK_MATSHINE  = 0xA040,
      CHUNK_SUBSHINE  = 0x0030,
      CHUNK_TEXMAP    = 0xA200,
      CHUNK_MATTEXT   = 0xA300
   };

   bool LoadVertexData();
   bool GetMaterialName(material_type *forMaterial);
   bool ReadMaterialSubcolorsFloat(material_type *forMaterial, int subtype);
   bool ReadMaterialSubcolors(material_type *forMaterial, int subtype);
   bool GetTextureFileName(material_type *forMaterial);
   bool ReadTextureMapping();
   bool LoadFaceList();
   bool LoadFaceMaterialMap();
   bool LoadDefaultMaterial();

   // Hidden methods
   ThreeDSLoader(const ThreeDSLoader& tds);
   ThreeDSLoader& operator=(const ThreeDSLoader& tds);
};

#endif /* THREEDSLOADER_HPP_ */
