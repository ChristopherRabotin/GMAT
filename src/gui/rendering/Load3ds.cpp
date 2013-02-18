//------------------------------------------------------------------------------
//                              Load3ds
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
 * Loads a .3ds file and stores the information in a ModelObject
 */
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
//#include <io.h>
#include "ModelObject.hpp"
#include "Load3ds.hpp"
#include "MessageInterface.hpp"

#define CHUNK_MAIN      0x4d4d
#define CHUNK_OBJMESH   0x3d3d
#define CHUNK_OBJBLOCK  0x4000
#define CHUNK_TRIMESH   0x4100
#define CHUNK_VERTLIST  0x4110
#define CHUNK_FACELIST  0x4120
#define CHUNK_FACEMAT   0x4130
#define CHUNK_MAPLIST   0x4140
#define CHUNK_SMOOLIST  0x4150
#define CHUNK_MATERIAL  0xAFFF
#define CHUNK_MATNAME   0xA000
#define CHUNK_MATACOL   0xA010
#define CHUNK_SUBCOLOR  0x0011
#define CHUNK_MATDIFF   0xA020
#define CHUNK_MATSPEC   0xA030
#define CHUNK_MATSHINE  0xA040
#define CHUNK_SUBSHINE  0x0030
#define CHUNK_TEXMAP    0xA200
#define CHUNK_MATTEXT   0xA300


//------------------------------------------------------------------------------
// char Load3DS(ModelObject *p_object, const std::string &p_filename)
//------------------------------------------------------------------------------
/**
 * This function loads a mesh from a 3ds file.
 *
 * It takes in only the vertices, polygons, and mapping lists.  Other
 * information, such as lightings, materials, etc. must be added in
 *
 * @param p_object pointer to a ModelObject
 *
 * @return 1 if the object loaded correctly, 0 otherwise (char)
 */
//------------------------------------------------------------------------------
char Load3DS(ModelObject *p_object, const std::string &p_filename)
{
   int i, chunk = 0; // Index
   int vert_index[MAX_LISTS]; // The starting index of a particular list of vertices
   int poly_index[MAX_LISTS]; // The starting index of a particular list of polygons
   int vert_list = 0, poly_list = 0, map_list = 0;
	int index = 0;
   FILE *l_file; // File pointer
   unsigned short l_chunk_id; // Chunk id
   unsigned int l_chunk_length; // Chunk length
   char l_char; // Char
   unsigned char r,g,b;
   unsigned short l_qty; // Numbers of elements in each chunk
   unsigned short l_face_flags; // Flag storing some face info
	long size; // length of the file

   // Make we have a file name
   if (p_filename[0] == '\0') 
      return 0;
   if (LOAD3DS_DEBUG)
      MessageInterface::ShowMessage("Loading 3ds object: %s\n",
            p_filename.c_str());

   // Load the file, print an error message if we couldn't find the file or it failed for any reason
	if ((l_file = fopen(p_filename.c_str(), "rb")) == NULL)
	{
	   MessageInterface::ShowMessage("File %s not found!", p_filename.c_str());
      return 0;
   }

	fseek(l_file, 0, SEEK_END); // seek to end of file
	size = ftell(l_file); // get current file pointer
	fseek(l_file, 0, SEEK_SET); // seek back to beginning of file

   p_object->num_vertices = 0;
   p_object->num_polygons = 0;
   p_object->num_materials = 0;

   // Loop through the whole file
	while(ftell(l_file) < size){//filelength(fileno(l_file))){
      // Read in chunk header
      fread(&l_chunk_id, 2, 1, l_file); 
      if (LOAD3DS_DEBUG)
         MessageInterface::ShowMessage("ChunkID: %x\n", l_chunk_id);
      // Read in the length of the chunk
      fread(&l_chunk_length, 4, 1, l_file);
      if (LOAD3DS_DEBUG)
         MessageInterface::ShowMessage("Chunk Length: %x\n", l_chunk_length);

      // How we proceed depends on the chunk we are examining
      switch (l_chunk_id)
      {
         // Main Chunk, which contains all other chunks
         // We do not need to do anything here except skip it
         case CHUNK_MAIN:
            break;
         // 3D Editor Chunk, contains editor information and flags
         // We ignore it
         case CHUNK_OBJMESH:
            break;
         // Object Block chunk, contains information on each object in the model
         // We extract the names
         case CHUNK_OBJBLOCK:
            i = 0;
            do
            {
               fread(&l_char, 1, 1, l_file);
               p_object->name += l_char;
               i++;
            } while (l_char != '\0' && i < 20);
            break;
         // Triangular Mesh chunk, contains the chunks detailing mesh info
         // Just a header for the sub-chunks, we skip it
         case CHUNK_TRIMESH:
            break;
         // Vertices List chunk, has all of the vertices for the object
         // We want these
         case CHUNK_VERTLIST:
            float v;
            // Retrieve the number of vertices
            fread(&l_qty, sizeof(unsigned short), 1, l_file);
            vert_index[vert_list] = p_object->num_vertices;
            p_object->num_vertices += l_qty;
            if (LOAD3DS_DEBUG)
               MessageInterface::ShowMessage("Number of vertices: %d\n", l_qty);
            // If the number of vertices is beyond our max, we bug out
            if (p_object->num_vertices > MAX_VERTICES)
            {
               MessageInterface::ShowMessage("Number of vertices too high!\n");
               return 0;
            }
            // Then we loop through all vertices and store them
            for (i = p_object->num_vertices - l_qty; i < p_object->num_vertices; i++)
            {
               fread(&v, sizeof(float), 1, l_file);
               p_object->vertex[i].x = v;
               fread(&v, sizeof(float), 1, l_file);
               p_object->vertex[i].y = v;
               fread(&v, sizeof(float), 1, l_file);
               p_object->vertex[i].z = v;
               if (LOAD3DS_DEBUG)
               {
                  MessageInterface::ShowMessage("Vertices List x: %f\n", p_object->vertex[i].x);
                  MessageInterface::ShowMessage("Vertices List y: %f\n", p_object->vertex[i].y);
                  MessageInterface::ShowMessage("Vertices List z: %f\n", p_object->vertex[i].z);
               }
            }
            vert_list++;
            break;
         // Faces list chunk, stores indices of vertices that make up the faces
         // We extract this information
         case CHUNK_FACELIST:
            // Retrieve the number of faces
            fread(&l_qty, sizeof(unsigned short), 1, l_file);
            poly_index[poly_list] = p_object->num_polygons;
            p_object->num_polygons += l_qty;
            if (LOAD3DS_DEBUG)
               MessageInterface::ShowMessage("Number of polygons: %d\n", l_qty);
            // Ensure we don't have more polygons than we can handle
            if (p_object->num_polygons > MAX_POLYGONS)
            {
               MessageInterface::ShowMessage("Number of polygons is too high!\n");
               return 0;
            }
            // Loop through file and extract all of the face information
            for (i = p_object->num_polygons-l_qty; i < p_object->num_polygons; i++){
               fread(&p_object->polygon[i].a, sizeof(unsigned short), 1, l_file);
               p_object->polygon[i].a += vert_index[poly_list];
               fread(&p_object->polygon[i].b, sizeof(unsigned short), 1, l_file);
               p_object->polygon[i].b += vert_index[poly_list];
               fread(&p_object->polygon[i].c, sizeof(unsigned short), 1, l_file);
               p_object->polygon[i].c += vert_index[poly_list];
               fread(&l_face_flags, sizeof(unsigned short), 1, l_file);
               if (LOAD3DS_DEBUG)
               {
                  MessageInterface::ShowMessage("Polygon point a: %d\n", p_object->polygon[i].a);
                  MessageInterface::ShowMessage("Polygon point b: %d\n", p_object->polygon[i].b);
                  MessageInterface::ShowMessage("Polygon point c: %d\n", p_object->polygon[i].c);
                  MessageInterface::ShowMessage("Face Flags: %x\n", l_face_flags);
               }
            }
            poly_list++;
            break;
         // The chunk defining which materials belong to which face. We
         // use this information to build our mat_map that maps a polygon index to
         // a material index
         case CHUNK_FACEMAT:
            unsigned short value;
            char str[255];
            i = 0;
            do
            {
               fread(&l_char, 1, 1, l_file);
               str[i] = l_char;
               i++;
            } while (l_char != '\0' && i < 255);
            for (i = 0; i < p_object->num_materials; i++)
            {
               if (strcmp(p_object->material[i].name, str) == 0)
               {
                  index = i;
                  break;
               }
               else if (i == p_object->num_materials-1)
               {
                  fprintf(stdout, "Material %s not found!\n", str);
                  index = -1;
                  return 0;
               }
            }
            fread(&l_qty, sizeof(unsigned short), 1, l_file);
            if (index != -1)
               p_object->material[index].num_faces += l_qty;
            for (i = p_object->material[index].num_faces - l_qty; i < p_object->material[index].num_faces; i++)
            {
               fread(&value, sizeof(unsigned short), 1, l_file);
               if (index != -1)
                  p_object->material[index].faces[i] = value + poly_index[map_list];
            }
            map_list++;
            break;
         // Texture mapping chunk, has the u,v coordinates for each vertex
         // We extract this information
         case CHUNK_MAPLIST:
            // Retrieve the number of mappings
            fread(&l_qty, sizeof(unsigned short), 1, l_file);
            // Go through all of the mappings and store them
            for (i = p_object->num_vertices-l_qty; i < p_object->num_vertices; i++)
            {
               fread(&p_object->mapcoord[i], sizeof(float), 2, l_file);
//					p_object->mapcoord[i].u = p_object->mapcoord[i].u;
					p_object->mapcoord[i].v = -p_object->mapcoord[i].v;
               if (LOAD3DS_DEBUG)
               {
                  MessageInterface::ShowMessage("Mapping list u,v: %f,%f\n",
                        p_object->mapcoord[i].u, p_object->mapcoord[i].v);
               }
            }
            break;
         // Start of the Materials chunk. Contains no data, so we move on
         case CHUNK_MATERIAL:
            p_object->num_materials++;
            break;
         // The name of the material. We extract and store
         case CHUNK_MATNAME:
            i = 0;
            do
            {
               fread(&l_char, 1, 1, l_file);
               p_object->material[p_object->num_materials-1].name[i] = l_char;
               i++;
            } while (l_char != '\0' && i < 255);
            p_object->material[p_object->num_materials-1].num_faces = 0;
            break;
         // The ambient color of the material. Extract and store 
         case CHUNK_MATACOL:
            chunk = l_chunk_id;
            break;
         case CHUNK_SUBCOLOR:
            fread(&r, sizeof(unsigned char), 1, l_file);
            fread(&g, sizeof(unsigned char), 1, l_file);
            fread(&b, sizeof(unsigned char), 1, l_file);
            if (chunk == CHUNK_MATACOL)
            {
               p_object->material[p_object->num_materials-1].mat_ambient.r = (float)r/255;
               p_object->material[p_object->num_materials-1].mat_ambient.g = (float)g/255;
               p_object->material[p_object->num_materials-1].mat_ambient.b = (float)b/255;
               p_object->material[p_object->num_materials-1].mat_ambient.a = (float)(r+g+b)/3/255;
            }
            else if (chunk == CHUNK_MATDIFF)
            {
               p_object->material[p_object->num_materials-1].mat_diffuse.r = (float)r/255;
               p_object->material[p_object->num_materials-1].mat_diffuse.g = (float)g/255;
               p_object->material[p_object->num_materials-1].mat_diffuse.b = (float)b/255;
               p_object->material[p_object->num_materials-1].mat_diffuse.a = (float)(r+g+b)/3/255;
            }
            else if (chunk == CHUNK_MATSPEC)
            {
               p_object->material[p_object->num_materials-1].mat_specular.r = (float)r/255;
               p_object->material[p_object->num_materials-1].mat_specular.g = (float)g/255;
               p_object->material[p_object->num_materials-1].mat_specular.b = (float)b/255;
               p_object->material[p_object->num_materials-1].mat_specular.a = (float)(r+g+b)/3/255;
            }
            break;
         // The diffuse color of the material. Extract and store 
         case CHUNK_MATDIFF:
            chunk = l_chunk_id;
            break;
         // The specular color of the material. Extract and store
         case CHUNK_MATSPEC:
            chunk = l_chunk_id;
            break;
         // The shininess of the material. Extract and store
         case CHUNK_MATSHINE:
            break;
         case CHUNK_SUBSHINE:
            unsigned short s;
            fread(&s, sizeof(unsigned short), 1, l_file);
            p_object->material[p_object->num_materials-1].mat_shininess = (float)s/255.0f;
            break;
         // The header chunk containing the material's texture. Skip it. 
         case CHUNK_TEXMAP:
            break;
         // The file name of the material's texture. Extract and store. We will load
         // and bind the textures in the object. 
         case CHUNK_MATTEXT:
            i = 0;
            do
            {
               fread(&l_char, 1, 1, l_file);
               p_object->material[p_object->num_materials-1].texture_name[i] = l_char;
               i++;
            } while (l_char != '\0' && i < 255);
            break;
         // We skip all unknown chunks.
         // Since they all begin with the chunk length, we use that information to skip. 
         // If you want to add other information to the model (materials info, for example),
         // then add their cases here
         default:
            fseek(l_file, l_chunk_length-6, SEEK_CUR);
      }
   }
   fclose(l_file); // Close the file

   if (LOAD3DS_DEBUG)
      MessageInterface::ShowMessage("Vertex count: %d  Face count: %d\n",
            p_object->num_vertices, p_object->num_polygons);

   return 1; // Return successfully
}
