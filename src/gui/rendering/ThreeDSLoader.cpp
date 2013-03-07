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

#include "ThreeDSLoader.hpp"

#include <stdio.h>
#include <stdlib.h>
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"

//#define DEBUG_LOADING
//#define DUMP_DATA

//------------------------------------------------------------------------------
// ThreeDSLoader()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * This is the only constructor visible for the .3ds file loader.  The Copy
 * constructor and assignment operators are hidden (made private) to prevent
 * inadvertent copies
 */
//------------------------------------------------------------------------------
ThreeDSLoader::ThreeDSLoader() :
   theFile           (NULL),
   fileSize          (0),
   theModel          (NULL)
{
}

//------------------------------------------------------------------------------
// ~ThreeDSLoader()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThreeDSLoader::~ThreeDSLoader()
{
}

//------------------------------------------------------------------------------
// bool LoadFileIntoModel(ModelObject *model, const std::string &filename)
//------------------------------------------------------------------------------
/**
 * Loads the model with data from a .3ds file
 *
 * @param model The model we are loading
 * @param filename The name of the file containing the data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::LoadFileIntoModel(ModelObject *model,
      const std::string &filename)
{
   bool retval = false;

   if (filename != "")
   {
      #ifdef DEBUG_LOADING
         MessageInterface::ShowMessage("Loading 3ds object: %s\n",
               filename.c_str());
      #endif

      // Load the file, print an error message if we couldn't find the file or it failed for any reason
      theFile = fopen(filename.c_str(), "rb");
      if (theFile == NULL)
      {
         throw SubscriberException("The .3ds model file " + filename +
               "was not found");
      }
      else
      {
         theModel = model;

         fseek(theFile, 0, SEEK_END); // seek to end of file
         fileSize = ftell(theFile); // get current file pointer
         fseek(theFile, 0, SEEK_SET); // seek back to beginning of file

         #ifdef DEBUG_LOADING
            MessageInterface::ShowMessage("   The file is %d bytes long\n",
                  fileSize);
         #endif

         // Refresh the model in case it was previously populated
         model->SetNumVertices(0);
         model->SetNumPolygons(0);
         model->SetNumMaterials(0);

         // Flag used to detect if the case statement failed
         bool noError = true;

         // Data structures used during the read
         unsigned short chunkId;
         unsigned int   chunkLength;
         material_type *currentMaterial = NULL;
         int subtype = 0;

         // Read the file until an error occurs or we reach the end, processing
         // the contents as we go
         while(ftell(theFile) < fileSize)
         {
            // Read in chunk header
            fread(&chunkId, sizeof(unsigned short), 1, theFile);
            fread(&chunkLength, sizeof(unsigned int), 1, theFile);
            #ifdef DEBUG_LOADING
               MessageInterface::ShowMessage("   Current chunk ID: %6d, "
                     "hex %4x, length %10d   ", chunkId, chunkId, chunkLength);
            #endif

            // Process the chunk
            switch (chunkId)
            {
            case CHUNK_MAIN:
               // The main chunk just holds the other pieces
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The Main chunk\n");
               #endif
               break;

            case CHUNK_OBJMESH:
               // This chunk contains 3D editer settings that GMAT ignores
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The 3D editor chunk\n");
               #endif
               break;

            case CHUNK_OBJBLOCK:
               // The object description block, containing the block name, NULL
               // terminated
               {
                  std::string theName;
                  char theChar;
                  do
                  {
                     fread(&theChar, sizeof(char), 1, theFile);
                     theName += theChar;
                  } while (theChar != '\0');
                  #ifdef DEBUG_LOADING
                     MessageInterface::ShowMessage("The descriptor: \"%s\"\n",
                           theName.c_str());
                  #endif
               }
               break;

            case CHUNK_TRIMESH:
               // This is another container entry, for the triangle mesh
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The mesh container\n");
               #endif
               break;

            case CHUNK_VERTLIST:
               // Here we go: These are the vertices!
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The Vertex data:  ");
               #endif
               if (!LoadVertexData())
               {
                  // Clear the pointer so it's not inadvertently stomped
                  theModel = NULL;
                  throw SubscriberException("An error was encountered loading "
                        "the vertex data from " + filename);
               }
               break;

            case CHUNK_FACELIST:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The list of faces  ");
               #endif
               if (!LoadFaceList())
               {
                  // Clear the pointer so it's not inadvertently stomped
                  theModel = NULL;
                  throw SubscriberException("An error was encountered loading "
                        "the polygon map " + filename);
               }
               break;

            case CHUNK_FACEMAT:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The face materials  ");
               #endif
               if (!LoadFaceMaterialMap())
               {
                  // Clear the pointer so it's not inadvertently stomped
                  theModel = NULL;
                  throw SubscriberException("An error was encountered loading "
                        "the polygon map " + filename);
               }
               break;

            case CHUNK_MAPLIST:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The texture mapping  ");
               #endif
               if (!ReadTextureMapping())
               {
                  MessageInterface::ShowMessage("The texture mapping was not "
                        "read correctly\n");
                  noError = false;
               }
               break;

            case CHUNK_SMOOLIST:
               // Not handled -- should it be?
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The face smoothing list\n");
               #endif
               break;

            case CHUNK_MATERIAL:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("Start of the material "
                        "block %d\n", theModel->GetNumMaterials());
               #endif
               {
                  Integer matCount = theModel->GetNumMaterials();
                  currentMaterial = &(theModel->GetMaterials()[matCount]);
                  theModel->SetNumMaterials(matCount + 1);
               }
               break;

            case CHUNK_MATNAME:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The current material name: ");
               #endif
               if (currentMaterial != NULL)
               {
                  if (!GetMaterialName(currentMaterial))
                  {
                     MessageInterface::ShowMessage("The material name was not "
                           "loaded correctly\n");
                     noError = false;
                  }
               }
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("\"%s\"\n",
                        currentMaterial->name);
               #endif
               break;

            case CHUNK_MATACOL:
               // For now ignoring this one; Phil had this line:
               subtype = CHUNK_MATACOL;
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The material ambient color\n");
               #endif

               break;

            case CHUNK_MATDIFF:
               subtype = CHUNK_MATDIFF;
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The material diffuse color\n");
               #endif
               break;

            case CHUNK_MATSPEC:
               subtype = CHUNK_MATSPEC;
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The material specular color\n");
               #endif
               break;

            case CHUNK_SUBCOLOR:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The material subcolors\n");
               #endif
               if (!ReadMaterialSubcolors(currentMaterial, subtype))
               {
                  MessageInterface::ShowMessage("The material subcolor was not "
                        "loaded correctly\n");
                  noError = false;
               }
               break;

            case CHUNK_MATSHINE:
               // Nothing to do here
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The material shininess\n");
               #endif
               break;

            case CHUNK_SUBSHINE:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The material subshininess\n");
               #endif
               if (currentMaterial)
               {
                  unsigned short s;
                  fread(&s, sizeof(unsigned short), 1, theFile);
                  currentMaterial->mat_shininess = (float)s / 255.0f;
               }
               break;

            case CHUNK_TEXMAP:
               // Nothing to do here
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The texture map block\n");
               #endif
               break;

            case CHUNK_MATTEXT:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("The texture file name:  ");
               #endif
               if (!GetTextureFileName(currentMaterial))
               {
                  MessageInterface::ShowMessage("The material filename was not "
                        "read correctly\n");
                  noError = false;
               }
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("\"%s\"\n",
                        currentMaterial->texture_name);
               #endif
               break;

            default:
               #ifdef DEBUG_LOADING
                  MessageInterface::ShowMessage("An unhandled chunk\n");
               #endif
               // Skip unknown chunks
               fseek(theFile, chunkLength-6, SEEK_CUR);
               break;
            };
         }
         fclose(theFile);
         theFile = NULL;      // Prevent mistaken reaccesses

         if (noError)
         {
            retval = true;
         }
         else
         {

         }

         theModel = NULL;
      }
   }

   return retval;
}


////////////////////////////////////////////////////////////////////////////////
// Private methods
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// bool LoadVertexData()
//------------------------------------------------------------------------------
/**
 * Loads the vertex data into the model
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::LoadVertexData()
{
   bool retval = false;
   float v;
   unsigned short vertexCount;

   // Sanity check -- if the file has multiple vertex lists, the code needs
   // updating to handle it
   if (theModel->GetNumVertices() > 0)
      MessageInterface::ShowMessage("Multiple vertex data entries were "
            "detected in the file\n");
   else
   {
      // Retrieve the number of vertices
      fread(&vertexCount, sizeof(unsigned short), 1, theFile);
      theModel->SetNumVertices(vertexCount);

      #ifdef DEBUG_LOADING
         MessageInterface::ShowMessage("Number of vertices: %d\n",
               vertexCount);
      #endif

      if (vertexCount <= MAX_VERTICES)
      {
         vector_type *vertex = theModel->GetVertexArray();

         // Then we loop through all vertices and store them
         for (unsigned short i = 0; i < vertexCount; ++i)
         {
            fread(&v, sizeof(float), 1, theFile);
            vertex[i].x = v;
            fread(&v, sizeof(float), 1, theFile);
            vertex[i].y = v;
            fread(&v, sizeof(float), 1, theFile);
            vertex[i].z = v;

            #ifdef DUMP_DATA
               MessageInterface::ShowMessage("Vertex %d: [%f %f %f]\n", i,
                     vertex[i].x, vertex[i].y, vertex[i].z);
            #endif
         }

         retval = true;
      }
      else
         MessageInterface::ShowMessage("The number of vertices found is too "
               "high!\n");
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool ThreeDSLoader::GetMaterialName(material_type *forMaterial)
//------------------------------------------------------------------------------
/**
 * Retrieved the name of a material used in texture mapping
 *
 * @param forMaterial The material being named
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::GetMaterialName(material_type *forMaterial)
{
   bool retval = false;

   if (forMaterial)
   {
      std::string theName;
      char theChar;
      int i = 0;
      do
      {
         fread(&theChar, sizeof(char), 1, theFile);
         theName += theChar;
         ++i;
      } while ((theChar != '\0') && (i < 255));

      if (theName.length() < 255)
      {
         strcpy(forMaterial->name, theName.c_str());
         retval = true;
      }

      forMaterial->num_faces = 0;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool RealMaterialSubcolors(material_type *forMaterial, int subtype)
//------------------------------------------------------------------------------
/**
 * Loads the substrate color dor a given material and lighting
 *
 * @param forMaterial The material being loaded
 * @param subtype The lighting type that uses this subcolor
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::ReadMaterialSubcolors(material_type *forMaterial,
      int subtype)
{
   bool retval = false;
   unsigned char r, g, b;

   if (forMaterial)
   {
      fread(&r, sizeof(unsigned char), 1, theFile);
      fread(&g, sizeof(unsigned char), 1, theFile);
      fread(&b, sizeof(unsigned char), 1, theFile);

      if (subtype == CHUNK_MATACOL)
      {
         forMaterial->mat_ambient.r = (float)r/255;
         forMaterial->mat_ambient.g = (float)g/255;
         forMaterial->mat_ambient.b = (float)b/255;
         forMaterial->mat_ambient.a = (float)(r+g+b)/3/255;
      }
      else if (subtype == CHUNK_MATDIFF)
      {
         forMaterial->mat_diffuse.r = (float)r/255;
         forMaterial->mat_diffuse.g = (float)g/255;
         forMaterial->mat_diffuse.b = (float)b/255;
         forMaterial->mat_diffuse.a = (float)(r+g+b)/3/255;
      }
      else if (subtype == CHUNK_MATSPEC)
      {
         forMaterial->mat_specular.r = (float)r/255;
         forMaterial->mat_specular.g = (float)g/255;
         forMaterial->mat_specular.b = (float)b/255;
         forMaterial->mat_specular.a = (float)(r+g+b)/3/255;
      }
      retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool GetTextureFileName(material_type *forMaterial)
//------------------------------------------------------------------------------
/**
 * Reads the name of the texture file
 *
 * @param forMaterial The material that uses this texture file
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::GetTextureFileName(material_type *forMaterial)
{
   bool retval = false;

   if (forMaterial)
   {
      std::string theName;
      char theChar;
      int i = 0;
      do
      {
         fread(&theChar, sizeof(char), 1, theFile);
         theName += theChar;
         ++i;
      } while ((theChar != '\0') && (i < 255));

      if (theName.length() < 255)
      {
         strcpy(forMaterial->texture_name, theName.c_str());
         retval = true;
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool ReadTextureMapping()
//------------------------------------------------------------------------------
/**
 * Loads the texture mapping
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::ReadTextureMapping()
{
   bool retval = false;
   unsigned short mapCount, vertexCount;

   fread(&mapCount, sizeof(unsigned short), 1, theFile);

   #ifdef DEBUG_LOADING
      MessageInterface::ShowMessage("Number of textures: %d  Filled from ",
            mapCount);
   #endif

   vertexCount = theModel->GetNumVertices();
   texmap_coord_type *mapcoord = theModel->GetTextureMap();

   if (mapcoord != NULL)
   {
      #ifdef DEBUG_LOADING
         MessageInterface::ShowMessage("%d to %d\n", vertexCount - mapCount,
               vertexCount-1);
      #endif

      // Go through all of the mappings and store them
      for (unsigned short i = vertexCount-mapCount; i < vertexCount; i++)
      {
         fread(&(mapcoord[i]), sizeof(float), 2, theFile);
         mapcoord[i].v = -mapcoord[i].v;

         #ifdef DUMP_DATA
            MessageInterface::ShowMessage("Mapping list u,v for %d: %f,%f\n",
                  i, p_object->mapcoord[i].u, p_object->mapcoord[i].v);
         #endif
      }

      retval = true;
   }
   else
      MessageInterface::ShowMessage("mapcoord is NULL\n");

   return retval;
}

//------------------------------------------------------------------------------
// bool LoadFaceList()
//------------------------------------------------------------------------------
/**
 * Loads the polygon vertices that put skin on the model
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::LoadFaceList()
{
   bool retval = false;
   unsigned short faceCount, faceFlag;

   // Retrieve the number of faces
   fread(&faceCount, sizeof(unsigned short), 1, theFile);

   if (theModel->GetNumPolygons() == 0)
   {
      theModel->SetNumPolygons(faceCount);

      #ifdef DEBUG_LOADING
         MessageInterface::ShowMessage("Number of polygons: %d\n", faceCount);
      #endif

      // Ensure we don't have more polygons than we can handle
      if (faceCount <= MAX_POLYGONS)
      {
         polygon_type *polygon = theModel->GetPolygonArray();

         // Loop through file and extract all of the face information
         for (unsigned short i = 0; i < faceCount; i++)
         {
            fread(&polygon[i].a, sizeof(unsigned short), 1, theFile);
//            polygon[i].a += vert_index[poly_list];
            fread(&polygon[i].b, sizeof(unsigned short), 1, theFile);
//            polygon[i].b += vert_index[poly_list];
            fread(&polygon[i].c, sizeof(unsigned short), 1, theFile);
//            polygon[i].c += vert_index[poly_list];
            fread(&faceFlag, sizeof(unsigned short), 1, theFile);

            #ifdef DUMP_DATA
               MessageInterface::ShowMessage("Polygon point %4d: "
                     "[%4d %4d %4d]  ", i, polygon[i].a,
                     polygon[i].b, polygon[i].c);
               MessageInterface::ShowMessage("Face Flags: %x\n",
                     l_face_flags);
            #endif
         }

         retval = true;
      }
      else
         MessageInterface::ShowMessage("Number of polygons is too high!\n");
   }
   else
      MessageInterface::ShowMessage("Polygons were already loaded; refactoring "
            "is needed\n");

   return retval;
}

//------------------------------------------------------------------------------
// bool ThreeDSLoader::LoadFaceMaterialMap()
//------------------------------------------------------------------------------
/**
 * Loads the material map for each of the materials in the model.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool ThreeDSLoader::LoadFaceMaterialMap()
{
   bool retval = false, materialFound = false;

   unsigned short value;
   char str[255], theChar;
   Integer i = 0, index;
   do
   {
      fread(&theChar, 1, 1, theFile);
      str[i] = theChar;
      i++;
   } while (theChar != '\0' && i < 255);

   material_type *material = theModel->GetMaterials();
   for (i = 0; i < theModel->GetNumMaterials(); i++)
   {
      if (strcmp(material[i].name, str) == 0)
      {
         index = i;
         materialFound = true;
         #ifdef DEBUG_LOADING
            MessageInterface::ShowMessage("for material %s\n", str);
         #endif
         break;
      }
      else if (i == theModel->GetNumMaterials()-1)
      {
         MessageInterface::ShowMessage("Material %s not found!\n", str);
         index = -1;
         break;
      }
   }

   if (materialFound)
   {
      fread(&value, sizeof(unsigned short), 1, theFile);
      if (index != -1)
         material[index].num_faces = value;

      for (i = 0; i < material[index].num_faces; i++)
      {
         fread(&value, sizeof(unsigned short), 1, theFile);
         if (index != -1)
            material[index].faces[i] = value;
      }

      retval = true;
   }

   return retval;
}




////////////////////////////////////////////////////////////////////////////////
// Hidden methods
////////////////////////////////////////////////////////////////////////////////

// Copies not allowed, so don't implement the copy c'tor or assignment operator


//------------------------------------------------------------------------------
// ThreeDSLoader(const ThreeDSLoader& tds) :
//------------------------------------------------------------------------------
/**
 * Copy contructor (Hidden)
 *
 * @param tds The instance being copied
 */
//------------------------------------------------------------------------------
ThreeDSLoader::ThreeDSLoader(const ThreeDSLoader& tds) :
   theFile              (NULL),
   fileSize             (0),
   theModel             (NULL)
{
}

//------------------------------------------------------------------------------
// ThreeDSLoader& operator=(const ThreeDSLoader& tds)
//------------------------------------------------------------------------------
/**
 * Assignment operator (hidden)
 *
 * @param tds The instance being copied
 *
 * @return This instance
 */
//------------------------------------------------------------------------------
ThreeDSLoader& ThreeDSLoader::operator=(const ThreeDSLoader& tds)
{
   return *this;
}
