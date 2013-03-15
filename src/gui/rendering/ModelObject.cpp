// $Id$
//------------------------------------------------------------------------------
//                              ModelObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC, Thinking Systems, Inc., and Schafer Corp.,
// under AFRL NOVA Contract #FA945104D03990003
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/24
// Modified: Dunning Idle 5th
// Date:     Summer 2010
// Changes:  Took out all minus signs from spacecraft position to correct 
//           attitude.
//
/**
 * Object for storing model data loaded from a file. Also handles self-rendering
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "GmatConstants.hpp"
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
#include "ModelObject.hpp"
//#include "Load3ds.hpp"
//#include "LoadPOV.hpp"
#include "ThreeDSLoader.hpp"
#include <wx/image.h>
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"


//#define DRAW_NORMALS
//#define WIREFRAME_MODE
//#define WARN_ON_COUNTS		// Post a warning if multiple vertex or face chunks

//#define FILL_WITH_DEFAULT

//#define DEBUG_LOAD
//#define DEBUG_LOAD_TEXTURE
//#define DEBUG_DRAW
//#define DEBUG_DRAW_MORE
//#define DEBUG_INDICES
//#define DEBUG_MODEL_DATA

//int control_object;

//------------------------------------------------------------------------------
// ModelObject()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ModelObject::ModelObject() :
   num_vertices      (0),
   num_polygons      (0),
   num_materials     (0),
   isLoaded          (false),
   mass              (0.0),
   bsphere_radius    (10.0),
   id_texture        (-1)
{
}


//------------------------------------------------------------------------------
// ~ModelObject()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ModelObject::~ModelObject()
{
}


//------------------------------------------------------------------------------
// ModelObject(const ModelObject& mo)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * This method is not used, but is provided for completeness and to match GMAT
 * style.
 */
//------------------------------------------------------------------------------
ModelObject::ModelObject(const ModelObject& mo) :
   num_vertices      (0),
   num_polygons      (0),
   num_materials     (0),
   isLoaded          (false),
   mass              (0.0),
   bsphere_radius    (10.0),
   id_texture        (-1)
{
   MessageInterface::ShowMessage("ModelObject::ModelObject(const ModelObject& )"
         "called but not implemented\n");
}

//------------------------------------------------------------------------------
// ModelObject& operator=(const ModelObject& mo)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * This method is not used, but is provided for completeness and to match GMAT
 * style.
 */
//------------------------------------------------------------------------------
ModelObject& ModelObject::operator=(const ModelObject& mo)
{
   MessageInterface::ShowMessage("ModelObject::operator=(const ModelObject& )"
         "called but not implemented\n");

   return *this;
}


//------------------------------------------------------------------------------
// char Load(const std::string &objectName, const std::string &path, ...)
//------------------------------------------------------------------------------
// Loads a model from a 3ds file and sets some parameters
//
// objectName: the filename of the model to be loaded
// path: the directory path containing the model and its textures. Assumed
//       to be within MODEL_PATH as defined by the default or startup file. 
// posX: starting position x coordinate
// posY: starting position y coordinate
// posZ: starting position z coordinate
// rotX: starting orientation x axis
// rotY: starting orientation y axis
// rotZ: starting orientation z axis
//
// Return value: 1 if the object loaded correctly, 0 otherwise (char)
//------------------------------------------------------------------------------
char ModelObject::Load(const std::string &objectName, const std::string &path, 
                       float posX, float posY, float posZ,
                       int rotX, int rotY, int rotZ)
{
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage
      ("ModelObject::Load() entered, objectName='%s', path='%s'\n",
       objectName.c_str(), path.c_str());
   #endif
   
   filename = objectName;
   //   WIREFRAME_MODE = 0;
   std::string thepath, thestring;
   FileManager *fm = FileManager::Instance();
   
   thepath = std::string(fm->GetPathname("MODEL_PATH").c_str());
   thepath += path;
   thestring = thepath;
   thestring += objectName;

   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage("   Loading '%s'\n", thestring.c_str());
   #endif

   char retval = Load(thestring, posX, posY, posZ, rotX, rotY, rotZ);
   
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage
      ("ModelObject::Load() '%s' returning '%c' from Load()\n",
       objectName.c_str(), retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// char Load(const std::string &full_path, ...)
//------------------------------------------------------------------------------
// Loads a model from a 3ds file and sets some parameters
//
// full_path: The full path description of the model file to be loaded
// posX: starting position x coordinate
// posY: starting position y coordinate
// posZ: starting position z coordinate
// rotX: starting orientation x axis
// rotY: starting orientation y axis
// rotZ: starting orientation z axis
//
// Return value: 1 if the object loaded correctly, 0 otherwise (char)
//------------------------------------------------------------------------------
char ModelObject::Load(const std::string &full_path,
                       float posX, float posY, float posZ,
                       int rotX, int rotY, int rotZ)
{
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage
      ("ModelObject::Load() entered, full_path='%s'\n", full_path.c_str());
   #endif
   
   std::string path, string, extension;
   UnsignedInt loc = full_path.find_last_of('/');
   if (loc != std::string::npos)
      path = full_path.substr(0,loc);

   // Might have Windows path delimiters
   if (path.length() <= 0)
   {
      loc = full_path.find_last_of('\\');
      if (loc != std::string::npos)
         path = full_path.substr(0,loc);
      path += '\\';
   }
   else
      path += '/';
   
   loc = full_path.find_last_of('.');
   extension = full_path.substr(loc+1);
   #ifdef DEBUG_LOAD
      MessageInterface::ShowMessage("   Extension '%s'\n", extension.c_str());
   #endif

   if (extension == wxT("3ds") || extension == wxT("3DS"))
   {
      #ifdef DEBUG_LOAD
         MessageInterface::ShowMessage("   Loading 3ds '%s'\n", full_path.c_str());
      #endif

      ThreeDSLoader loader;
      if (!loader.LoadFileIntoModel(this, full_path))
         return 0;
      
      isLoaded = true;

      // Calculate the normals for the object
      // We only do this for 3ds files, since pov files
      // come with their norms
      CalcNormals();
   }
   else if (extension == wxT("pov") || extension == wxT("POV"))
   {
      #ifdef DEBUG_LOAD
         MessageInterface::ShowMessage("   Loading pov '%s'\n",
               full_path.c_str());
      #endif

      MessageInterface::ShowMessage("POV model files are not currently "
            "supported in GMAT\n");
      return 0;

//      LoadPOV(this, full_path);
      //CalcNormals();
      //for (int i = 0; i < num_vertices; i++){
      //VectorNormalize(&normal[i]);
      //}
      //if (LoadPOV(this, full_path) == 0)
      //return 0;
   }
   filePath = path;

   // Then we go through each texture from the materials and load those. Any that failed
   // we bind to the default texture passed in
   LoadTextures();
   // Create the bounding sphere and AABB of the model
   CreateBSphere();
   // Determine all of the faces neighbors
   // Initialize the object's matrix
   //MatrixIdentity(&matrix);
   SetBaseScale(1, 1, 1);
   // Initialize the object's position
   Reposition(posX, posY, posZ);
   // Initialize the object's orientation
   Rotate(1, rotX, rotY, rotZ);
   // Success! So return 1
   isLoaded = true;

   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage("ModelObject::Load() returning 1\n");
   #endif
   
   return 1;
}

//-------------------------------------------------------------------------------
// void LoadTextures()
//-------------------------------------------------------------------------------
void ModelObject::LoadTextures()
{
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage
      ("ModelObject::LoadTextures() entered, num_materials = %d\n",
       num_materials);
   #endif
   
   int i;
   GLuint result;
   std::string string;
   for (i = 0; i < num_materials; i++)
   {
      string = filePath;
      string += material[i].texture_name;
      result = LoadTexture(string);
      material[i].id_texture = result;
   }
}

//-------------------------------------------------------------------------------
// int LoadTexture(const std::string &filename)
//-------------------------------------------------------------------------------
/**
 * Loads a texture, binds it, and returns the generated texture id
 */
//-------------------------------------------------------------------------------
int ModelObject::LoadTexture(const std::string &filename)
{
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage
      ("ModelObject::LoadTexture() entered, filename='%s'\n", filename.c_str());
   #endif
   
   GLuint id;
   GLenum error;
   wxImage image; 
   bool result = false;
   std::string ext;
   
   if (!wxFileExists(filename.c_str()))
   {
      #ifdef DEBUG_LOAD_TEXTURE
      MessageInterface::ShowMessage
         ("ModelObject::LoadTexture() returning -1, file does not exist\n");
      #endif
      return -1;
   }
   
   // Load the file
   UnsignedInt loc = filename.find_last_of('.');
   if (loc != std::string::npos)
      ext = filename.substr(loc+1);
   if (strcmp(ext.c_str(), "bmp") == 0 || strcmp(ext.c_str(), "BMP") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_BMP, -1);
   else if (strcmp(ext.c_str(), "jpg") == 0 || strcmp(ext.c_str(), "JPG") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_JPEG, -1);
   else if (strcmp(ext.c_str(), "png") == 0 || strcmp(ext.c_str(), "PNG") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_PNG, -1);
   else if (strcmp(ext.c_str(), "tif") == 0 || strcmp(ext.c_str(), "TIF") == 0)
      result = image.LoadFile(filename.c_str(), wxBITMAP_TYPE_TIF, -1);
   else
      return -1;
   
   if (!result)
   {
      #ifdef DEBUG_LOAD_TEXTURE
      MessageInterface::ShowMessage
         ("ModelObject::LoadTexture() returning -1, Could not load image\n");
      #endif
      return -1;
   }
   
   int width = image.GetWidth();
   int height = image.GetHeight();
   GLubyte *data = image.GetData();
   
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage("   Calling glGenTextures() to generate texture id\n");
   #endif
   // Generate the texture id
   glGenTextures(1, &id);
   error = glGetError();

   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage("   Calling glBindTexture() with texture id: %d\n", id);
   #endif
   // Bind the texture id
   glBindTexture(GL_TEXTURE_2D, id);
   error = glGetError();
   
   // Set the texture parameters
   // Repeat the image if the u,v coordinates exceed the 0,1 range
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   
   //=================================================================
   // Create the 2D mipmaps for minification non liniux platform
   // gluBuild2DMipmaps() crashes on Linux, so it is excluded here.
   #ifndef __WXGTK__
   //=================================================================
   
   // Linearly minify the texture (is that even a real word?)
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
   // Linearly magnify the texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   // This call crashes GMAT on Linux, so it is excluded here. 
   gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
   
   //=================================================================
   #else
   //=================================================================
   
   // Linearly minify the texture (is that even a real word?)
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   // Linearly magnify the texture
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // Define the 2D texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   
   //=================================================================
   #endif
   //=================================================================
   
   #ifdef DEBUG_LOAD_TEXTURE
   MessageInterface::ShowMessage
      ("ModelObject::LoadTexture() returning texture id: %d\n", id);
   #endif
   
   return id;
}


//------------------------------------------------------------------------------
// void SetName(const std::string &nameFromFile)
//------------------------------------------------------------------------------
/**
 * Sets the model name, usually from the data in the model file
 *
 * @param nameFromFile The model name read from the file
 */
//------------------------------------------------------------------------------
void ModelObject::SetName(const std::string &nameFromFile)
{
   name = nameFromFile;
}

//------------------------------------------------------------------------------
// Integer GetNumVertices()
//------------------------------------------------------------------------------
/**
 * Gets the current vertex count
 *
 * @return The number of defined vertices
 */
//------------------------------------------------------------------------------
Integer ModelObject::GetNumVertices()
{
   return num_vertices;
}

//------------------------------------------------------------------------------
// void SetNumVertices(Integer vCount)
//------------------------------------------------------------------------------
/**
 * Sets the vertex count
 *
 * @param vCount The total number of vertices
 */
//------------------------------------------------------------------------------
void ModelObject::SetNumVertices(Integer vCount)
{
   #ifdef WARN_ON_COUNTS
      if ((num_vertices > 0) && (num_vertices != vCount))
         MessageInterface::ShowMessage("*** WARNING *** The model file %s has "
               "multiple vertex blocks and may not display correctly.\n",
               filename.c_str());
   #endif
   num_vertices = vCount;
}

//------------------------------------------------------------------------------
// Integer GetNumPolygons()
//------------------------------------------------------------------------------
/**
 * Retrieves the polygon count
 *
 * @return The number of polygons
 */
//------------------------------------------------------------------------------
Integer ModelObject::GetNumPolygons()
{
   return num_polygons;
}

//------------------------------------------------------------------------------
// void SetNumPolygons(Integer pCount)
//------------------------------------------------------------------------------
/**
 * Sets the polygon count
 *
 * @param pCount The new polygon count
 */
//------------------------------------------------------------------------------
void ModelObject::SetNumPolygons(Integer pCount)
{
   #ifdef WARN_ON_COUNTS
      if ((num_polygons > 0) && (num_polygons != pCount))
         MessageInterface::ShowMessage("*** WARNING *** The model file %s has "
               "multiple polygon blocks and may not display correctly.\n",
               filename.c_str());
   #endif
   num_polygons = pCount;
}

//------------------------------------------------------------------------------
// Integer GetNumMaterials()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of materials in the model
 *
 * @return The number of materials
 */
//------------------------------------------------------------------------------
Integer ModelObject::GetNumMaterials()
{
   return num_materials;
}

//------------------------------------------------------------------------------
// void SetNumMaterials(Integer mCount)
//------------------------------------------------------------------------------
/**
 * Sets teh number of materials in the model
 *
 * @param mCount The new material count
 */
//------------------------------------------------------------------------------
void ModelObject::SetNumMaterials(Integer mCount)
{
   num_materials = mCount;
}

//------------------------------------------------------------------------------
// bool IsLoaded()
//------------------------------------------------------------------------------
/**
 * Retrieves the flag indicating if the model has been loaded
 *
 * @return The isLoaded flag
 */
//------------------------------------------------------------------------------
bool ModelObject::IsLoaded()
{
   return isLoaded;
}

//------------------------------------------------------------------------------
// vector_type *GetVertexArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the vertex array
 *
 * @return The vertex array
 */
//------------------------------------------------------------------------------
vector_type *ModelObject::GetVertexArray()
{
   // If we move to dynamic memory allocation, here's a start that assumes a
   // single vertex block in the model file
   //if (vertex == NULL)
   //   if (num_vertices > 0)
   //      vertex = new vector_type[num_vertices];
   return vertex;
}

//------------------------------------------------------------------------------
// polygon_type *GetPolygonArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the polygon array
 *
 * @return The polygon array
 */
//------------------------------------------------------------------------------
polygon_type *ModelObject::GetPolygonArray()
{
   return polygon;
}

//------------------------------------------------------------------------------
// texmap_coord_type *GetTextureMap()
//------------------------------------------------------------------------------
/**
 * Retrieves the texture map array
 *
 * @return The mapcoord array
 */
//------------------------------------------------------------------------------
texmap_coord_type *ModelObject::GetTextureMap()
{
   return mapcoord;
}

//------------------------------------------------------------------------------
// material_type *GetMaterials()
//------------------------------------------------------------------------------
/**
 * Retrieves the material array
 *
 * @return The material array
 */
//------------------------------------------------------------------------------
material_type *ModelObject::GetMaterials()
{
   return material;
}

//------------------------------------------------------------------------------
// vector_type& GetBSphereCenter()
//------------------------------------------------------------------------------
/**
 * Retrieves the center of the bounding sphere
 *
 * @return The location of the center
 */
//------------------------------------------------------------------------------
vector_type& ModelObject::GetBSphereCenter()
{
   return bsphere_center;
}

//------------------------------------------------------------------------------
// float GetBSphereRadius()
//------------------------------------------------------------------------------
/**
 * Retireves the radius of the bounding sphere
 *
 * @return The radius
 */
//------------------------------------------------------------------------------
float ModelObject::GetBSphereRadius()
{
   return bsphere_radius;
}


//------------------------------------------------------------------------------
// void CreateBSphere()
//------------------------------------------------------------------------------
/**
 * Calculates the object's bounding sphere using the object's Axis-Aligned Bounding Box (AABB)
 */
//------------------------------------------------------------------------------
void ModelObject::CreateBSphere()
{
   //MessageInterface::ShowMessage("==> ModelObject::CreateBSphere() entered\n");
   
   vector_type vector;
   // Create the AABB
   CreateAABB();

   // Calculate the object's center using the extreme points of the AABB
   bsphere_center.x = (aabb[6].x + aabb[0].x)/2;
   bsphere_center.y = (aabb[6].y + aabb[0].y)/2;
   bsphere_center.z = (aabb[6].z + aabb[0].z)/2;

   // Calculate the object's radius using the extreme points of the AABB
   VectorCreate(&aabb[0], &aabb[6], &vector);
   bsphere_radius = VectorLength(&vector)/2;
}

//------------------------------------------------------------------------------
// void CreateAABB()
//------------------------------------------------------------------------------
/**
 * Creates the object's Axis-Aligned Bounding Box (AABB)
 */
//------------------------------------------------------------------------------
void ModelObject::CreateAABB()
{
   //MessageInterface::ShowMessage("==> ModelObject::CreateAABB() entered\n");
   
   int i;

   #ifdef DEBUG_MODEL_DATA
      MessageInterface::ShowMessage("Vertices:\n");
      for (i = 1; i < num_vertices; i++)
         MessageInterface::ShowMessage("   [%4d] -> [%lf %lf %lf]\n", i,
               vertex[i].x, vertex[i].y, vertex[i].z);
      MessageInterface::ShowMessage("Polygons:\n");
      for (i = 1; i < num_polygons; i++)
         MessageInterface::ShowMessage("   [%4d] -> [%d %d %d]\n", i,
               polygon[i].a, polygon[i].b, polygon[i].c);
      MessageInterface::ShowMessage("Mappings [u,v]:\n");
      for (i = 1; i < num_polygons; i++)
         MessageInterface::ShowMessage("   [%4d] -> [%lf %lf]\n", i,
               mapcoord[i].u, mapcoord[i].v);
   #endif
   // We first store the first vertex into the first AABB vertex
   for (i = 0; i < 8; i++)
   {
      aabb[i].x = vertex[0].x;
      aabb[i].y = vertex[0].y;
      aabb[i].z = vertex[0].z;
   }
   // Get the 8 AABB vertex by making some comparisons to get the 8 extreme vertices
   for (i = 1; i < num_vertices; i++)
   {
      if (vertex[i].x < aabb[0].x)
         aabb[0].x = vertex[i].x;
      if (vertex[i].y < aabb[0].y)
         aabb[0].y = vertex[i].y;
      if (vertex[i].z < aabb[0].z)
         aabb[0].z = vertex[i].z;

      if (vertex[i].x > aabb[1].x)
         aabb[1].x = vertex[i].x;
      if (vertex[i].y < aabb[1].y)
         aabb[1].y = vertex[i].y;
      if (vertex[i].z < aabb[1].z)
         aabb[1].z = vertex[i].z;

      if (vertex[i].x > aabb[2].x)
         aabb[2].x = vertex[i].x;
      if (vertex[i].y > aabb[2].y)
         aabb[2].y = vertex[i].y;
      if (vertex[i].z < aabb[2].z)
         aabb[2].z = vertex[i].z;

      if (vertex[i].x < aabb[3].x)
         aabb[3].x = vertex[i].x;
      if (vertex[i].y > aabb[3].y)
         aabb[3].y = vertex[i].y;
      if (vertex[i].z < aabb[3].z)
         aabb[3].z = vertex[i].z;

      if (vertex[i].x < aabb[4].x)
         aabb[4].x = vertex[i].x;
      if (vertex[i].y < aabb[4].y)
         aabb[4].y = vertex[i].y;
      if (vertex[i].z > aabb[4].z)
         aabb[4].z = vertex[i].z;

      if (vertex[i].x > aabb[5].x)
         aabb[5].x = vertex[i].x;
      if (vertex[i].y < aabb[5].y)
         aabb[5].y = vertex[i].y;
      if (vertex[i].z > aabb[5].z)
         aabb[5].z = vertex[i].z;

      if (vertex[i].x > aabb[6].x)
         aabb[6].x = vertex[i].x;
      if (vertex[i].y > aabb[6].y)
         aabb[6].y = vertex[i].y;
      if (vertex[i].z > aabb[6].z)
         aabb[6].z = vertex[i].z;

      if (vertex[i].x < aabb[7].x)
         aabb[7].x = vertex[i].x;
      if (vertex[i].y > aabb[7].y)
         aabb[7].y = vertex[i].y;
      if (vertex[i].z > aabb[7].z)
         aabb[7].z = vertex[i].z;
   }
}

//------------------------------------------------------------------------------
// void CalcNormals()
//------------------------------------------------------------------------------
/**
 * Calculate all of the normals for both the polygons and vertices in the object
 */
//------------------------------------------------------------------------------
void ModelObject::CalcNormals()
{
   #ifdef DEBUG_CALC_NORMALS
   MessageInterface::ShowMessage("ModelObject::CalcNormals() entered\n");
   #endif
   
   int i;
   // Some local vectors for calculation purposes
   vector_type vect1, vect2, vect3, vect_b1, vect_b2, norm;
   // Number of polygons around each vertex
   int num_connections[MAX_VERTICES];
   
   #ifdef DEBUG_CALC_NORMALS
   MessageInterface::ShowMessage
      ("num_vertices = %d, num_polygons = %d\n", num_vertices, num_polygons);
   #endif
   
   // Reset the vertices' normals
   for (i = 0; i < num_vertices; i++)
   {
      normal[i].x = 0.0;
      normal[i].y = 0.0;
      normal[i].z = 0.0;
      num_connections[i] = 0;
   }
   
   // Go through all of the polygons
   for (i = 0; i < num_polygons; i++)
   {
      // Set the local vertices to the polygons' component vertices
      vect1.x = vertex[polygon[i].a].x;
      vect1.y = vertex[polygon[i].a].y;
      vect1.z = vertex[polygon[i].a].z;
      vect2.x = vertex[polygon[i].b].x;
      vect2.y = vertex[polygon[i].b].y;
      vect2.z = vertex[polygon[i].b].z;
      vect3.x = vertex[polygon[i].c].x;
      vect3.y = vertex[polygon[i].c].y;
      vect3.z = vertex[polygon[i].c].z;

      // Calculate the polygons' normals
      // b1 is from vertex 1 to vertex 2
      VectorCreate(&vect1, &vect2, &vect_b1);
      // b2 is from vertex 1 to vertex 3
      VectorCreate(&vect1, &vect3, &vect_b2);
      VectorNormalize(&vect_b1);
      VectorNormalize(&vect_b2);
      // Find the cross product of the two
      VectorCross(&vect_b1, &vect_b2, &norm);
      VectorNormalize(&norm);
      // For each vertex shared by this polygon we increase the number of connections
      num_connections[polygon[i].a] += 1;
      num_connections[polygon[i].b] += 1;
      num_connections[polygon[i].c] += 1;
      // For each vertex shared by this polygon we add the polygon normal
      VectorAdd(&normal[polygon[i].a], &norm, &normal[polygon[i].a]);
      VectorAdd(&normal[polygon[i].b], &norm, &normal[polygon[i].b]);
      VectorAdd(&normal[polygon[i].c], &norm, &normal[polygon[i].c]);
   }
   // Calculate all of the vertex normals by averaging the polygon normals of the
   // polygons that the vertex belongs to
   for (i = 0; i < num_vertices; i++)
   {
      #ifdef DEBUG_CALC_NORMAL
      MessageInterface::ShowMessage
         ("   num_connections[%d] = %d\n", i, num_connections[i]);
      #endif
      
      if (num_connections[i] > 0)
         VectorNormalize(&normal[i]);
      
      #ifdef DEBUG_CALC_NORMAL
      MessageInterface::ShowMessage
         ("   normal[%d] = [%f, %f, %f]\n", i, normal[i].x, normal[i].y, normal[i].z);
      #endif
   }
}

//------------------------------------------------------------------------------
// void FindNeighbors()
//------------------------------------------------------------------------------
void ModelObject::FindNeighbors()
{
   int i,j,k,l;
   int edge1[2], edge2[2];
   for (i = 0; i < num_polygons; i++)
      for (j = 0; j < 3; j++)
         polygon[i].neighbors[j] = -1;
   for (i = 0; i < num_polygons; i++){
      for (k = 0; k < 3; k++){
         if (polygon[i].neighbors[k] == -1){
            switch (k){
               case 0:
                  edge1[0] = polygon[i].a;
                  edge1[1] = polygon[i].b;
                  break;
               case 1:
                  edge1[0] = polygon[i].b;
                  edge1[1] = polygon[i].c;
                  break;
               case 2:
                  edge1[0] = polygon[i].c;
                  edge1[1] = polygon[i].a;
                  break;
            }
            for (j = 0; j < num_polygons; j++){
               if (i == j )
                  continue;
               for (l = 0; l < 3; l++)
               {
                  if (polygon[j].neighbors[l] == -1)
                  {
                     switch (l)
                     {
                        case 0:
                           edge2[0] = polygon[j].a;
                           edge2[1] = polygon[j].b;
                           break;
                        case 1:
                           edge2[0] = polygon[j].b;
                           edge2[1] = polygon[j].c;
                           break;
                        case 2:
                           edge2[0] = polygon[j].c;
                           edge2[1] = polygon[j].a;
                           break;
                     }
                     if ((edge1[0] == edge2[0] && edge1[1] == edge2[1]) |
                         (edge1[0] == edge2[1] && edge1[1] == edge2[0]))
                     {
                        polygon[i].neighbors[k] = j;
                        polygon[j].neighbors[l] = i;
                        break;
                     }
                  }
               }   
            }
         }
      }
   }
}

//-------------------------------------------------------------------------------
// void Reposition(float x, float y, float z)
//-------------------------------------------------------------------------------
// Sets the object's matrix translation values
//
// x: x coordinate
// y: y coordinate
// z: z coordinate
//-------------------------------------------------------------------------------
void ModelObject::Reposition(float x, float y, float z)
{
   // Change out the position fields in the matrix with the given values
   /*MatrixSetElement(&matrix, 3, 0, x);//0, 3, x);
   MatrixSetElement(&matrix, 3, 1, y);//1, 3, y);
   MatrixSetElement(&matrix, 3, 2, z);//2, 3, z);*/
        translation.Set(x, y, z);
   Translate(baseOffset[0], baseOffset[1], baseOffset[2]);
}

//-------------------------------------------------------------------------------
// void Translate(float x, float y, float z)
//-------------------------------------------------------------------------------
// Translate the object using the local axis 
//
// x: x coordinate
// y: y coordinate
// z: z coordinate
//-------------------------------------------------------------------------------
void ModelObject::Translate(float x, float y, float z)
{
   /*matrix_type mat;
   MatrixIdentity(&mat);
   MatrixSetElement(&mat, 3, 0, x);//0, 3, x);
   MatrixSetElement(&mat, 3, 1, y);//1, 3, y);
   MatrixSetElement(&mat, 3, 2, z);//2, 3, z);*/

   translation.Set(translation[0] + x, translation[1] + y, translation[2] + z);

   // The object's matrix is multiplied by the translation matrix
   //MatrixMult(&mat, &matrix, &matrix);
}

//-------------------------------------------------------------------------------
// void TranslateW(float x, float y, float z)
//-------------------------------------------------------------------------------
// Translate the object according to the world axis system
//
// x: x coordinate
// y: y coordinate
// z: z coordinate
//-------------------------------------------------------------------------------
void ModelObject::TranslateW(float x, float y, float z)
{
   // Just add the x,y,z to the the translation elements of the matrix
   /*MatrixSetElement(&matrix, 3, 0, MatrixGetElement(&matrix, 3, 0) + x);
   MatrixSetElement(&matrix, 3, 1, MatrixGetElement(&matrix, 3, 1) + y);
   MatrixSetElement(&matrix, 3, 2, MatrixGetElement(&matrix, 3, 2) + z);*/

   translation.Set(translation[0] + x, translation[1] + y, translation[2] + z);
}

//-------------------------------------------------------------------------------
// void Rotate(bool useDegrees, float x, float y, float z)
//-------------------------------------------------------------------------------
// Rotates the object around its local axi by the given angles
//
// use_degrees: use radians or degrees
// x: x angle
// y: y angle
// z: z angle
//-------------------------------------------------------------------------------
void ModelObject::Rotate(bool useDegrees, float x, float y, float z)
{
   //matrix_type mat;
   //float rotcos, rotsin;
   float factor = 1.0;
   // If we're using degrees, we'll need to add the conversion factor
   if (useDegrees)
      factor = (float)(GmatMathConstants::RAD_PER_DEG);
   
   x *= factor;
   y *= factor;
   z *= factor;
   
   rotation.Set(rotation[0] + x, rotation[1] + y, rotation[2] + z);
   
   // X rotation
   /*if (x != 0.0){
      // Create the rotation matrix
      MatrixIdentity(&mat);
      rotcos = (float)cos(x);
      rotsin = (float)sin(x);
      MatrixSetElement(&mat, 1, 1, rotcos);
      MatrixSetElement(&mat, 1, 2, rotsin);
      MatrixSetElement(&mat, 2, 1, -rotsin);
      MatrixSetElement(&mat, 2, 2, rotcos);
      // Multiply the rotation matrix by the current matrix
      MatrixMult(&mat, &matrix, &matrix);
   }
   // Y rotation
   if (y != 0.0){
      // Create the rotation matrix
      MatrixIdentity(&mat);
      rotcos = (float)cos(y);
      rotsin = (float)sin(y);
      MatrixSetElement(&mat, 0, 0, rotcos);
      MatrixSetElement(&mat, 0, 2, -rotsin);
      MatrixSetElement(&mat, 2, 0, rotsin);
      MatrixSetElement(&mat, 2, 2, rotcos);
      //matrix = res;
      MatrixMult(&mat, &matrix, &matrix);
   }
   // Z rotation
   if (z != 0.0){
      // Create the rotation matrix
      MatrixIdentity(&mat);
      //mat.SetIdentity();
      rotcos = (float)cos(z);
      rotsin = (float)sin(z);
      MatrixSetElement(&mat, 0, 0, rotcos);
      MatrixSetElement(&mat, 0, 1, rotsin);
      MatrixSetElement(&mat, 1, 0, -rotsin);
      MatrixSetElement(&mat, 1, 1, rotcos);
      //matrix = res;
      MatrixMult(&mat, &matrix, &matrix);
   }*/
   //MatrixMult(&mat, &matrix, &matrix);
}

//-------------------------------------------------------------------------------
// void Scale(float x, float y, float z)
//-------------------------------------------------------------------------------
// Scales the object
//
// x: The amount to scale by in the x direction
// y: The amount to scale by in the y direction
//-------------------------------------------------------------------------------
void ModelObject::Scale(float x, float y, float z)
{
   /*matrix_type mat;
   MatrixIdentity(&mat);
   MatrixSetElement(&mat, 0, 0, x);
   MatrixSetElement(&mat, 1, 1, y);
   MatrixSetElement(&mat, 2, 2, z);*/
   
   scale.Set(scale[0] + x, scale[1] + y, scale[2] + z);
   
   //MatrixMult(&mat, &matrix, &matrix);
}

//-------------------------------------------------------------------------------
// void Reset()
//-------------------------------------------------------------------------------
// Resets the model's matrix to the identity matrix
void ModelObject::Reset()
{
   //MatrixIdentity(&matrix);
   for (int i = 0; i < 3; i++)
   {
      rotation[i] = 0;
      translation[i] = 0;
      scale[i] = 0;
   }
   Scale(baseScale[0], baseScale[1], baseScale[2]);
   Translate(baseOffset[0], baseOffset[1], baseOffset[2]);
   Rotate(false, baseRotation[0], baseRotation[1], baseRotation[2]);
}

//-------------------------------------------------------------------------------
// void SetBaseOffset(float x, float y, float z)
//-------------------------------------------------------------------------------
// Sets the base offset of the model to the given values. The base
// offset is used to correct the centering of the model. The base
// offset is not cleared when the model's matrix is reset. Use
// Translate, TranslateW, and Reposition to adjust the models's
// position in most cases. 
//
// x: The base x offset
// y: The base y offset
// z: The base z offset
//-------------------------------------------------------------------------------
void ModelObject::SetBaseOffset(float x, float y, float z)
{
   baseOffset[0] = x;
   baseOffset[1] = y;
   baseOffset[2] = z;
   Reset();
}

//-------------------------------------------------------------------------------
// void SetBaseRotation(bool useDegrees, float x, float y, float z)
//-------------------------------------------------------------------------------
// Sets the base rotation of the model to the given values. The
// base rotation is used to determine the starting angle of the
// model. The base rotation is not cleared when the model's matrix
// is reset. Use Rotate to rotate the model in most cases. 
//
// x: The base x offset
// y: The base y rotation
// z: The base z rotation
//-------------------------------------------------------------------------------
void ModelObject::SetBaseRotation(bool useDegrees, float x, float y, float z)
{
   float factor = 1.0f;
   if (useDegrees)
      factor = (float)(GmatMathConstants::RAD_PER_DEG);
   baseRotation[0] = x * factor;
   baseRotation[1] = y * factor;
   baseRotation[2] = z * factor;
   Reset();
}

//-------------------------------------------------------------------------------
// void SetBaseScale(float x, float y, float z)
//-------------------------------------------------------------------------------
// Sets the base scale of the model to the given values. The
// base scale is used to determine the starting size of the
// mode. The base scale is not cleared when the model's matrix
// is reset. Use Scale to scale the model in most cases. 
//
// x: The base x scale
// y: The base y scale
// z: The base z scale
//-------------------------------------------------------------------------------
void ModelObject::SetBaseScale(float x, float y, float z)
{
   baseScale[0] = x;
   baseScale[1] = y;
   baseScale[2] = z;
   Reset();
}

//-------------------------------------------------------------------------------
// void Accelerate(char axis_system, float ax, float ay, float az)
//-------------------------------------------------------------------------------
// Accelerates the object (units are m/s)
//
// axis_system: either world (0) or local (1) coordinates
// ax: x acceleration
// ay: y acceleration
// az: z acceleration
// NOTE: CURRENTLY BROKEN
//-------------------------------------------------------------------------------
void ModelObject::Accelerate(char axis_system, float ax, float ay, float az)
{
   /*Rmatrix direction(1,4), result(1,4);

   // World version
   if (axis_system == 0){
      lin_speed.x += ax;
      lin_speed.y += ay;
      lin_speed.z += az;
   }
   // Object-local version
   else
   {
      direction.SetElement(0, 0, ax);
      direction.SetElement(0, 1, ay);
      direction.SetElement(0, 2, az);
      direction.SetElement(0, 3, 0);

      result = direction * matrix;
      lin_speed.x += result.GetElement(0,0);
      lin_speed.y += result.GetElement(0,1);
      lin_speed.z += result.GetElement(0,2);
   }*/
}

//-------------------------------------------------------------------------------
// void AccelerateRotation(float vrx, float vry, float vrz)
//-------------------------------------------------------------------------------
// Accelerate the angular speed of the object (units are rad/s)
//
// vrx: x acceleration
// vry: y acceleration
// vrz: z acceleration
//-------------------------------------------------------------------------------
void ModelObject::AccelerateRotation(float vrx, float vry, float vrz)
{
   rot_speed.x += vrx;
   rot_speed.y += vry;
   rot_speed.z += vrz;
}

//-------------------------------------------------------------------------------
// void Force(char axis_system, float ix, float iy, float iz)
//-------------------------------------------------------------------------------
// Applies a force to the object (units are Newtons)
// For reference, 1 N = 1 kr -> 1 m/s
//                Acceleration = Force / Mass
//
// axis_system: either world (0) or object-local (1) coordinates
// ix: x force
// iy: y force
// iz: z force
//-------------------------------------------------------------------------------
void ModelObject::Force(char axis_system, float ix, float iy, float iz)
{
   float m = mass;
   if (m != 0){
      Accelerate(axis_system, ix/m, iy/m, iz/m);
   }
}

//-------------------------------------------------------------------------------
// void Torque(float mx, float my, float mz)
//-------------------------------------------------------------------------------
// Applies a torque (angular force) to the object
// For reference: Angular Acceleration = Torque / moment of inertia
//                Torque = F dot r
//                Angular momentum = P dot r
//                                 = mv dot r
//                                 = mWr dot r
//                                 = mr^2W
//                Moment of inertia = I = mr^2
//                Moment of inertia is approximated for an axis through the center of mass to be
//                                  = I = 1/12*ML^2, where M is the mass and L the radius of the 
//                                     object from center of mass
//                Angular momentum = IW
//                Torque = dI/dT x dW/dt = I x Angular acceleration
//                Angular acceleration = Torque / I
//
// axis_system: either world (0) or object-local (1) coordinates
// mx: x torque
// my: y torque
// mz: z torque
//-------------------------------------------------------------------------------
void ModelObject::Torque(float mx, float my, float mz)
{
   float moment_of_inertia = mass * (bsphere_radius*2) * (bsphere_radius*2)/12;
   AccelerateRotation(mx/moment_of_inertia, my/moment_of_inertia, mz/moment_of_inertia);
}

//-------------------------------------------------------------------------------
// void Drag()
//-------------------------------------------------------------------------------
/**
 * Applies a Drag force to the object, which will decrease the speed until it stops
 */
//-------------------------------------------------------------------------------
void ModelObject::Drag()
{
   // Linear Drag
   Force(0, -lin_speed.x*100, 
         -lin_speed.y*100, 
         -lin_speed.z*100);
   // Angular Drag
   Torque(-rot_speed.x*5000, 
          -rot_speed.y*5000, 
          -rot_speed.z*5000);
}

//-------------------------------------------------------------------------------
// void Dynamics(float time_factor)
//-------------------------------------------------------------------------------
/**
 * Applies dynamics laws to the object (linear and angular moment conservation)
 *
 * time_factor: the time factor which depends on the physics fps, should be 1/fps
 */
//-------------------------------------------------------------------------------
void ModelObject::Dynamics(float time_factor)
{
   // Linear momentum conservation
   TranslateW(lin_speed.x * time_factor,
              lin_speed.y * time_factor,
              lin_speed.z * time_factor);
   // Angular momentum conservation
   Rotate(0, rot_speed.x * time_factor,
          rot_speed.y * time_factor,
          rot_speed.z * time_factor);
}

//-------------------------------------------------------------------------------
// void ModelObject::Draw(bool isLit)
//-------------------------------------------------------------------------------
/**
 * Draw the object on the screen
 */
//-------------------------------------------------------------------------------
void ModelObject::Draw(bool isLit)
{
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage("ModelObject::Draw() entered, isLit = %d\n", isLit);
   #endif
   
   int i,j,k;
   float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
   float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

   glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
   glMatrixMode(GL_MODELVIEW);
   
   SetMatrix();
   
   // Save the current matrix
   glPushMatrix();
   // Multiply the object matrix by the object's matrix
   glMultMatrixf(matrix.element);
   
#ifndef WIREFRAME_MODE
//   if (WIREFRAME_MODE == 0){
   // Go through all of the materials in the object
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage("   num_materials = %d\n", num_materials);
   #endif
//   for (i = 0; i < num_materials; i++)
   for (i = num_materials-1; i >= 0; --i)
   {
      GLenum error;
      // As long as have a texture id, we bind it and enable textures
      if (material[i].id_texture != -1)
      {
         #ifdef DEBUG_DRAW_MORE
         MessageInterface::ShowMessage
            ("   %d, Enabling  texture 2d with %d\n", i, material[i].id_texture);
         #endif
         
         GLint params = -13;
         error = glGetError();
         glBindTexture(GL_TEXTURE_2D, material[i].id_texture);
         error = glGetError();
         glGetIntegerv(GL_TEXTURE_BINDING_2D, &params);
         error = glGetError();
         glEnable(GL_TEXTURE_2D);
         error = glGetError();
      }
      // If we have no texture id, we disable textures
      else
      {
         #ifdef DEBUG_DRAW_MORE
         MessageInterface::ShowMessage("   %d, Disabling texture 2d\n", i);
         #endif
         
         glDisable(GL_TEXTURE_2D);
      }
      
      // Apply the diffuse color of the material
      //glMaterialfv(GL_FRONT, GL_AMBIENT, &material[i].mat_ambient.r);
      //glMaterialfv(GL_FRONT, GL_DIFFUSE, &material[i].mat_diffuse.r);
      //glColor3f(1.0, 1.0, 1.0);
      
      glColor3fv(&material[i].mat_diffuse.r);//&material[i].mat_ambient.r);
      
      if (isLit)
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &material[i].mat_specular.r);
      else
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
      glMaterialf(GL_FRONT, GL_SHININESS, material[i].mat_shininess);

      
      // Begin to draw the triangles
      glBegin(GL_TRIANGLES);
      
      #ifdef DEBUG_DRAW_MORE
      MessageInterface::ShowMessage
         ("   material[%d].num_faces = %d\n", i,  material[i].num_faces);
      #endif
      
      // Fill in with default texture
      #ifdef FILL_WITH_DEFAULT
         if (i == 0)
         {
            // Go through all of the faces stored in the material
            for (j = 0; j < num_polygons; j++)
            {
               glEnable(GL_NORMALIZE); // added (LOJ: 2012.07.11)

   //            // Store the face number, for easy reference
   //            k = material[i].faces[j];
   //
               // Apply the normals for the first vertex
               glNormal3fv(&normal[polygon[j].a].x);
               // Apply the texture coordinates for the first vertex
               glTexCoord2fv(&mapcoord[polygon[j].a].u);
               // Draw the first vertex
               glVertex3fv(&vertex[polygon[j].a].x);

               // Apply the normals for the second vertex
               glNormal3fv(&normal[polygon[j].b].x);
               // Apply the texture coordinates for the second vertex
               glTexCoord2fv(&mapcoord[polygon[j].b].u);
               // Draw the second vertex
               glVertex3fv(&vertex[polygon[j].b].x);

               // Apply the normals for the third vertex
               glNormal3fv(&normal[polygon[j].c].x);
               // Apply the texture coordinates for the third vertex
               glTexCoord2fv(&mapcoord[polygon[j].c].u);
               // Draw the third vertex
               glVertex3fv(&vertex[polygon[j].c].x);
            }
         }
         else
      #endif
      {
         // Go through all of the faces stored in the material
         for (j = 0; j < material[i].num_faces; j++)
         {
            glEnable(GL_NORMALIZE); // added (LOJ: 2012.07.11)

            // Store the face number, for easy reference
            k = material[i].faces[j];

            // Apply the normals for the first vertex
            glNormal3fv(&normal[polygon[k].a].x);
            // Apply the texture coordinates for the first vertex
            glTexCoord2fv(&mapcoord[polygon[k].a].u);
            // Draw the first vertex
            glVertex3fv(&vertex[polygon[k].a].x);

            // Apply the normals for the second vertex
            glNormal3fv(&normal[polygon[k].b].x);
            // Apply the texture coordinates for the second vertex
            glTexCoord2fv(&mapcoord[polygon[k].b].u);
            // Draw the second vertex
            glVertex3fv(&vertex[polygon[k].b].x);

            // Apply the normals for the third vertex
            glNormal3fv(&normal[polygon[k].c].x);
            // Apply the texture coordinates for the third vertex
            glTexCoord2fv(&mapcoord[polygon[k].c].u);
            // Draw the third vertex
            glVertex3fv(&vertex[polygon[k].c].x);
         }
      }
      // Finish drawing for this material
      glEnd();
   }
   
//   }
#endif

#ifdef DRAW_NORMALS
   float x,y,z;
   float normalLength = 3;
   glDisable(GL_LIGHTING);
   glPushMatrix();
   glBegin(GL_LINES);
   glColor3f(0.1f, 1.0f, 0.1f);
   glColor3f(1.0f, 0.1f, 0.1f);
   for (j = 0; j < num_materials; j++){
      for (i = 0; i < material[j].num_faces; i++){
         glVertex3f(vertex[polygon[i].a].x, vertex[polygon[i].a].y, vertex[polygon[i].a].z);
         glVertex3f(vertex[polygon[i].a].x + normal[polygon[i].a].x * normalLength, 
            vertex[polygon[i].a].y + normal[polygon[i].a].y * normalLength,
            vertex[polygon[i].a].z + normal[polygon[i].a].z * normalLength);
      }
   }
   glEnd();
   glPopMatrix();
   glEnable(GL_LIGHTING);
#endif
   
#ifdef WIREFRAME_MODE
//   if (WIREFRAME_MODE == 1){
   glDisable(GL_LIGHTING);
   glPushMatrix();
   glBegin(GL_LINES);

   for (i = 0; i < num_polygons; i++)
   {
      glColor3f(0.1f, 0.1f, 1.0f);
      glVertex3fv(&vertex[polygon[i].a].x);
      glVertex3fv(&vertex[polygon[i].b].x);

      glVertex3fv(&vertex[polygon[i].b].x);
      glVertex3fv(&vertex[polygon[i].c].x);

      glVertex3fv(&vertex[polygon[i].c].x);
      glVertex3fv(&vertex[polygon[i].a].x);
   }
   glEnd();

   glPointSize(3.0);
   glBegin(GL_POINTS);   
   glColor3f(1.0, 1.0, 1.0);
   for (i = 0; i < num_vertices; i++){
      glVertex3fv(&vertex[i].x);
   }
   glEnd();

   glPopMatrix();
   glEnable(GL_LIGHTING);
//   }
#endif

   // Put the matrix back on the stack
   glPopMatrix();
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
   // Disable textures
   glDisable(GL_TEXTURE_2D);
   // Flush openGL
   glFlush();
   
   #ifdef DEBUG_DRAW
   MessageInterface::ShowMessage("ModelObject::Draw() leaving\n");
   #endif
}

//-------------------------------------------------------------------------------
// void SetMatrix()
//-------------------------------------------------------------------------------
// Sets the matrix to use the base and currents for translation, rotation, etc.
//-------------------------------------------------------------------------------
void ModelObject::SetMatrix()
{
   matrix_type mat;
   MatrixIdentity(&matrix);
   float rotcos, rotsin;
   
   MatrixIdentity(&mat);
   MatrixSetElement(&mat, 0, 0, scale[0]);
   MatrixSetElement(&mat, 1, 1, scale[1]);
   MatrixSetElement(&mat, 2, 2, scale[2]);
   MatrixMult(&mat, &matrix, &matrix);
   
   MatrixIdentity(&mat);
   MatrixSetElement(&mat, 3, 0, translation[0]);
   MatrixSetElement(&mat, 3, 1, translation[1]);
   MatrixSetElement(&mat, 3, 2, translation[2]);
   MatrixMult(&mat, &matrix, &matrix);
   
   MatrixIdentity(&mat);
   rotcos = (float)cos(rotation[0]);
   rotsin = (float)sin(rotation[0]);
   MatrixSetElement(&mat, 1, 1, rotcos);
   MatrixSetElement(&mat, 1, 2, rotsin);
   MatrixSetElement(&mat, 2, 1, -rotsin);
   MatrixSetElement(&mat, 2, 2, rotcos);
   MatrixMult(&mat, &matrix, &matrix);
   
   MatrixIdentity(&mat);
   rotcos = (float)cos(rotation[1]);
   rotsin = (float)sin(rotation[1]);
   MatrixSetElement(&mat, 0, 0, rotcos);
   MatrixSetElement(&mat, 0, 2, -rotsin);
   MatrixSetElement(&mat, 2, 0, rotsin);
   MatrixSetElement(&mat, 2, 2, rotcos);
   MatrixMult(&mat, &matrix, &matrix);
   
   MatrixIdentity(&mat);
   rotcos = (float)cos(rotation[2]);
   rotsin = (float)sin(rotation[2]);
   MatrixSetElement(&mat, 0, 0, rotcos);
   MatrixSetElement(&mat, 0, 1, rotsin);
   MatrixSetElement(&mat, 1, 0, -rotsin);
   MatrixSetElement(&mat, 1, 1, rotcos);
   MatrixMult(&mat, &matrix, &matrix);
}

//-------------------------------------------------------------------------------
// void VectorAdd(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr result)
//-------------------------------------------------------------------------------
// Adds two vectors together, storing the result in result
// 
// vector1: The first vector to be added
// vector2: The second vector to be added
// result: The sum vector
//-------------------------------------------------------------------------------
void ModelObject::VectorAdd(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr result)
{
   result->x = vector1->x + vector2->x;
   result->y = vector1->y + vector2->y;
   result->z = vector1->z + vector2->z;
}

//-------------------------------------------------------------------------------
// void VectorCreate(vector_type_ptr start, vector_type_ptr end, vector_type_ptr result)
//-------------------------------------------------------------------------------
// Creates a vector that points from the start to the end
//
// start: The starting vector
// end: The ending vector
// result: The resulting vector which points from start to end
//-------------------------------------------------------------------------------
void ModelObject::VectorCreate(vector_type_ptr start, vector_type_ptr end, vector_type_ptr result)
{
   result->x = end->x - start->x;
   result->y = end->y - start->y;
   result->z = end->z - start->z;
}

//-------------------------------------------------------------------------------
// void VectorCross(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr normal)
//-------------------------------------------------------------------------------
// Generates the cross product of the two vectors. 
// Calculates vector1 x vector2 and stores the result in normal
//
// vector1: The first vector
// vector2: The second vector
// normal: The resultant vector, normal to vector1 and vector2
//-------------------------------------------------------------------------------
void ModelObject::VectorCross(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr normal)
{
   normal->x = vector1->y*vector2->z - vector1->z*vector2->y;
   normal->y = vector1->z*vector2->x - vector1->x*vector2->z;
   normal->z = vector1->x*vector2->y - vector1->y*vector2->x;
}

//-------------------------------------------------------------------------------
// float VectorDot(vector_type_ptr vector1, vector_type_ptr vector2)
//-------------------------------------------------------------------------------
// Returns the scalar dot product of two vectors.
// Calculates vector1 dot vector2
//
// vector1: The first vector
// vector2: The second vector
// 
// Return value: Returns the dot product (float)
//-------------------------------------------------------------------------------
float ModelObject::VectorDot(vector_type_ptr vector1, vector_type_ptr vector2)
{
   return (vector1->x*vector2->x + vector1->y*vector2->y + vector1->z*vector2->z);
}

//-------------------------------------------------------------------------------
// float VectorLength(vector_type_ptr vector)
//-------------------------------------------------------------------------------
// Returns the length of the given vector
//
// vector: The vector whose length we want to find
//
// Return value: The length of vector (float)
//-------------------------------------------------------------------------------
float ModelObject::VectorLength(vector_type_ptr vector)
{
   return (float)(sqrt(vector->x*vector->x + vector->y*vector->y + vector->z*vector->z));
}

//-------------------------------------------------------------------------------
// void VectorNormalize(vector_type_ptr vector)
//-------------------------------------------------------------------------------
// Normalizes the length of a vector
//
// vector: The vector to be normalized
//-------------------------------------------------------------------------------
void ModelObject::VectorNormalize(vector_type_ptr vector)
{
   float len = VectorLength(vector);
   if (len == 0)
      len = 1;

   vector->x /= len;
   vector->y /= len;
   vector->z /= len; 
}

//-------------------------------------------------------------------------------
// void VectorSub(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr result)
//-------------------------------------------------------------------------------
// Calculates one vector minus the other. Functionally equivalent to VectorCreate
// Calculates vector1 - vector2 and stores the result in result
//
// vector1: The first vector
// vector2: The second vector
// result: The resulting vector
//-------------------------------------------------------------------------------
void ModelObject::VectorSub(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr result)
{
   result->x = vector1->x - vector2->x;
   result->y = vector1->y - vector2->y;
   result->z = vector1->z - vector2->z;
}

//-------------------------------------------------------------------------------
// void MatrixSetElement(matrix_type_ptr matrix, int r, int c, float value)
//-------------------------------------------------------------------------------
// Sets the the element of matrix.
//
// matrix: The matrix whose element we are setting
// r: The row location of the element
// c: The column location of the element
// value: The value to set the element to
//-------------------------------------------------------------------------------
void ModelObject::MatrixSetElement(matrix_type_ptr matrix, int r, int c, float value)
{
   matrix->element[4*r+c] = value;
}

//-------------------------------------------------------------------------------
// float MatrixGetElement(matrix_type_ptr matrix, int r, int c)
//-------------------------------------------------------------------------------
// Retrieve an element of a matrix
//
// matrix: The matrix whose element we are getting
// r: The row location of the element
// c: The column location of the element
// 
// Return value: The value of the element at the given location (float)
//-------------------------------------------------------------------------------
float ModelObject::MatrixGetElement(matrix_type_ptr matrix, int r, int c)
{
   return matrix->element[4*r+c];
}

//-------------------------------------------------------------------------------
// void MatrixIdentity(matrix_type_ptr matrix)
//-------------------------------------------------------------------------------
// Sets the matrix to an identity matrix. All data previously stored will be lost
//
// matrix: The matrix we are setting to an identity matrix
//-------------------------------------------------------------------------------
void ModelObject::MatrixIdentity(matrix_type_ptr matrix)
{
   for (int i = 0; i < 16; i++)
   {
      if (i%5 == 0)
         matrix->element[i] = 1;
      else
         matrix->element[i] = 0;
   }
}

//-------------------------------------------------------------------------------
// void MatrixZero(matrix_type_ptr matrix)
//-------------------------------------------------------------------------------
// Zeros out all elements of a matrix (sets the elements to zero).
//
// maitrx: The matrix we will zero out
//-------------------------------------------------------------------------------
void ModelObject::MatrixZero(matrix_type_ptr matrix)
{
   for (int i = 0; i < 16; i++)
      matrix->element[i] = 0;
}

//-------------------------------------------------------------------------------
// void MatrixCopy(matrix_type_ptr source, matrix_type_ptr destination)
//-------------------------------------------------------------------------------
// Copy all of the elements from one matrix to another
//
// source: The matrix we are copying from
// destination: The matrix we are copying to
//-------------------------------------------------------------------------------
void ModelObject::MatrixCopy(matrix_type_ptr source, matrix_type_ptr destination)
{
   for (int i = 0; i < 16; i++)
      destination->element[i] = source->element[i];
}

//-------------------------------------------------------------------------------
// void MatrixMult(matrix_type_ptr matrix1, matrix_type_ptr matrix2, matrix_type_ptr result)
//-------------------------------------------------------------------------------
// Multiply one matrix by another.
// As order matters with matrices, the order is thus: matrix1 * matrix2
//
// matrix1: The first matrix in the multiplication
// matrix2: The second matrix in the multiplication
// result: Where the resulting matrix is stored
//-------------------------------------------------------------------------------
void ModelObject::MatrixMult(matrix_type_ptr matrix1, matrix_type_ptr matrix2, matrix_type_ptr result)
{
   matrix_type res;
   MatrixZero(&res);
   for (int i = 0; i < 16; i++)
      for (int j = 0; j < 4; j++)
         res.element[i] += matrix1->element[j+(i/4*4)] * matrix2->element[j*4+i%4];
   MatrixCopy(&res, result);
}
