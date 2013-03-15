//$Id$
//------------------------------------------------------------------------------
//                              ModelObject
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// ** Legal **
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/24
/**
 * Object for storing model data loaded from a file. Also handles self-rendering
 */
//------------------------------------------------------------------------------

#ifndef MODEL_OBJECT_H
#define MODEL_OBJECT_H

#include <wx/string.h>
#include "gmatwxdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector3.hpp"

#define MAX_VERTICES 200000  // Maximum vertices per object
//#define MAX_VERTICES 80000  // Maximum vertices per object

// Same question here
#define MAX_POLYGONS 100000  // Maximum polygons (triangles) per object

#define MAX_LISTS 300       // Maximum number of vertex/polygon lists per object
#define MAX_MATERIALS 500   // Maximum materials per object
#define MAX_POINTS 20000      // Maximum data points per object
#define M_INFINITY 30000000

// The vertex type
typedef struct
{
   float x,y,z;
} vector_type, *vector_type_ptr;

typedef struct
{
   float element[16];
} matrix_type, *matrix_type_ptr;

// The polygon (triangle), the 3 numbers point at 3 vertices
typedef struct
{
   unsigned int a,b,c;
   int neighbors[3];
   bool visible;
} polygon_type;

// Texture map coordinate, 2 texture coordinates per vertex
typedef struct
{
   float u,v;
} texmap_coord_type;

// Color type, rgba format
typedef struct{
   float r;
   float g;
   float b;
   float a;
} rgba_type;

// The material type
typedef struct
{
   char name[255];                     // The name of the material
   char texture_name[255];             // The file name of the texture associated with the material
   int num_faces;                      // The number of faces that use this material

   // This needs to be an int since a model can have more than 65536 faces; not making it unsigned for now
//   unsigned short faces[MAX_POLYGONS]; // The indices of the faces using this material
   int faces[MAX_POLYGONS];            // The indices of the faces using this material
   rgba_type mat_ambient;              // The ambient color of the material
   rgba_type mat_diffuse;              // The diffuse color of the material
   rgba_type mat_specular;             // The specular color of the material
   // ??? This is a float throughout the code
//   unsigned short mat_shininess;       // The shininess factor of the material (between 0 and 100%, 100 being the highest)
   float mat_shininess;                // The shininess factor of the material (between 0 and 100%, 100 being the highest)
   int id_texture;                     // The id of the bound texture
} material_type, *material_type_ptr;


/**
 * Class used to build the OpenGL model of a Spacecraft
 */
class ModelObject
{
   // Data that was exposed but should not be
private:
   std::string name;      // The name of the object
   std::string filename;  // The name of the file this object came from

   // int WIREFRAME_MODE;

   int num_vertices; // Number of vertices
   int num_polygons; // Number of polygons (triangles)
   int num_materials;

   bool isLoaded;
   
   vector_type bsphere_center;                  // Center of the object, calculated via bounding sphere
   vector_type vertex[MAX_VERTICES];            // Array of vertices making up the object
   vector_type normal[MAX_VERTICES];            // Array of vertex normals for lighting purposes
   vector_type aabb[8];                         // Array of the vertices' axis-aligned bounding box (aabb)
   polygon_type polygon[MAX_POLYGONS];          // Array of polygons (triangles), points to vertices list
   texmap_coord_type mapcoord[MAX_VERTICES];    // Array of U,V coordinates for texture mapping

   vector_type lin_speed;   // Object's linear speed
   vector_type rot_speed;   // Object's rotational speed
   float mass;              // Object's mass
   float bsphere_radius;    // the radius of the object's bounding sphere
   
   int id_texture;          // The id of the object's texture
   material_type material[MAX_MATERIALS]; // The materials loaded from the file
   
   matrix_type matrix;      // Object matrix
   std::string filePath;

   
   // Visible Methods
public:
   ModelObject();
   ~ModelObject();
   ModelObject(const ModelObject& mo);
   ModelObject& operator=(const ModelObject& mo);
   
   ///////////////////////////////////////////////////////////////////////////////////////////
   /// These are for loading and saving the model from and to a file
   ///////////////////////////////////////////////////////////////////////////////////////////

   // Load the model information from a .3ds file
   char Load(const std::string &object_name, const std::string &path, 
      float pos_x = 0.0f, float pos_y = 0.0f, float pos_z = 0.0f, 
      int rot_x = 0, int rot_y = 0, int rot_z = 0);
   char Load(const std::string &full_path, 
      float pos_x = 0.0f, float pos_y = 0.0f, float pos_z = 0.0f, 
      int rot_x = 0, int rot_y = 0, int rot_z = 0);
   void LoadTextures();
   
   // Accessors for the private data
   void SetName(const std::string &nameFromFile);
   Integer GetNumVertices();
   void SetNumVertices(Integer vCount);
   Integer GetNumPolygons();
   void SetNumPolygons(Integer pCount);
   Integer GetNumMaterials();
   void SetNumMaterials(Integer mCount);

   bool IsLoaded();

   vector_type *GetVertexArray();
   polygon_type *GetPolygonArray();
   texmap_coord_type *GetTextureMap();
   material_type *GetMaterials();
   vector_type& GetBSphereCenter();
   float GetBSphereRadius();

   ///////////////////////////////////////////////////////////////////////////////////////////
   /// These modify the model's object matrix
   ///////////////////////////////////////////////////////////////////////////////////////////
   
   // Set the model's matrix position
   void Reposition(float x, float y, float z);
   // Move the model's matrix position
   void Translate(float x, float y, float z);
   // Move the model's matrix position by world coordinates
   void TranslateW(float x, float y, float z);
   // Rotate the model's matrix
   void Rotate(bool use_degrees, float angle_x, float angle_y, float angle_z);
   // Scale the model's matrix
   void Scale(float x, float y, float z);
   // Sets the model's matrix to an identity matrix
   void Reset();
   
   // This is the base rotation and offset of the model. This will not be cleared when the 
   // model's matrix is reset
   void SetBaseRotation(bool useDegrees, float x, float y, float z);
   void SetBaseOffset(float x, float y, float z);
   void SetBaseScale(float x, float y, float z);
   
   ///////////////////////////////////////////////////////////////////////////////////////////
   /// These apply different forces to the object. They are NOT used at this time
   ///////////////////////////////////////////////////////////////////////////////////////////
   
   // Increase the linear speed of the model
   void Accelerate(char axis_system, float ax, float ay, float az);
   // Increase the rotational speed of the model
   void AccelerateRotation(float vrx, float vry, float vrz);
   // Apply a force to the model
   void Force(char axis_system, float ix, float iy, float iz);
   // Apply a torque to the model
   void Torque(float mx, float my, float mz);
   // Apply drag to the model
   void Drag();
   // Apply dynamic physics to the model
   void Dynamics(float time_factor);
   // Perform a collision check between two objects
   void CollisionCheck(ModelObject object1, ModelObject object2);
   // Simulate a collision response between two objects
   void CollisionResponse(ModelObject object1, ModelObject object2);
   
   ///////////////////////////////////////////////////////////////////////////////////////////
   /// These perform the openGL drawing
   ///////////////////////////////////////////////////////////////////////////////////////////
   
   // Draw the model at the given frame position
   //void Draw(int frame, bool isLit);
   // Draw the model
   void Draw(bool isLit);
   void RenderShadowVolume(float *light_pos, int frame);
   
private:
   float baseRotation[3];
   float baseOffset[3];
   float baseScale[3];
   Rvector3 rotation;
   Rvector3 translation;
   Rvector3 scale;
   
   void CreateAABB(); // Creates the axis-aligned bounding box for the object
   void CreateBSphere(); // Creates the bounding sphere for the object
   void CalcNormals(); // Calculates the vertex normals of the object
   void FindNeighbors(); // Finds all neighboring faces for all polygons
   int  LoadTexture(const std::string &file_name);
   
   ///////////////////////////////////////////////////////////////////////////////////////////
   /// Helper functions for the vector structure used within the class
   /// The structures are used instead of Rvector because they allow easier model drawing
   ///////////////////////////////////////////////////////////////////////////////////////////

   // Creates a vector that points from start to end
   void VectorCreate(vector_type_ptr start, vector_type_ptr end, vector_type_ptr result);
   // Retrieves the length of a vector
   float VectorLength(vector_type_ptr vector);
   // Normalizes a vector
   void VectorNormalize(vector_type_ptr vector);
   // Adds two vectors together
   void VectorAdd(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr result);
   // Subtracts one vector from another
   void VectorSub(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr result);
   // Calculates the cross product of two vectors
   void VectorCross(vector_type_ptr vector1, vector_type_ptr vector2, vector_type_ptr normal);
   // Calculates the dot product of two vectors
   float VectorDot(vector_type_ptr vector1, vector_type_ptr vector2);

   ///////////////////////////////////////////////////////////////////////////////////////////
   /// Helper functions for the matrix structure used within the class
   /// The structures are used instead of Rmatrix because Rmatrix is finicky and this was faster
   ///////////////////////////////////////////////////////////////////////////////////////////
   
   void SetMatrix();
   
   void  MatrixSetElement(matrix_type_ptr matrix, int r, int c, float value);
   float MatrixGetElement(matrix_type_ptr matrix, int r, int c);
   void  MatrixIdentity(matrix_type_ptr matrix);
   void  MatrixZero(matrix_type_ptr matrix);
   void  MatrixCopy(matrix_type_ptr source, matrix_type_ptr destination);
   void  MatrixMult(matrix_type_ptr matrix1, matrix_type_ptr matrix2, matrix_type_ptr result);
   
};
#endif
