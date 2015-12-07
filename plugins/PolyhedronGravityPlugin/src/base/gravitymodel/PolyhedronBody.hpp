/*
 * PolyhedronBody.hpp
 *
 *  Created on: Aug 27, 2012
 *      Author: tdnguye2
 */

#ifndef PolyhedronBody_hpp
#define PolyhedronBody_hpp

#include "Rmatrix66.hpp"

struct Edge
{
	Integer vertex1, vertex2;
};
typedef std::vector<Integer>         PolygonFace;
typedef std::vector<Rvector3>        PointsList;
typedef PointsList        			    Vectors3List;
typedef std::vector<PolygonFace>           FacesList;
typedef std::vector<Edge>                  EdgesList;
typedef std::vector<Integer>		          FaceIndexList;
typedef std::map<Integer, Edge>            EdgesMap;
typedef std::map<Integer, Integer>		    FaceIndexMap;

class PolyhedronBody
{
public:

   PolyhedronBody(const std::string &filename);
   virtual ~PolyhedronBody();
   PolyhedronBody(const PolyhedronBody& polybody);
   PolyhedronBody& operator= (const PolyhedronBody& polybody);

   virtual bool Initialize();

   // inherited from GmatBase
   virtual PolyhedronBody*   Clone() const;
   virtual void              Copy(const PolyhedronBody* orig);

public:
   bool LoadBodyShape();	// load the data which define shape of the body

   bool FaceNormals();
   bool Incenters();
   bool IsInEdgesList(Edge& edge, bool& isAttachmentB);
   bool Edges();
   bool EdgeAttachments(Integer edgeindex, Integer& faceA_index, Integer& faceB_index);

   bool Calculation(Rvector6 x, Rvector6& xdot, Rmatrix66& A);		// calculate gravity


   std::string bodyShapeFilename;
   PointsList    verticesList;
   FacesList     facesList;

   Vectors3List  fn;
   PointsList 	  ic;
   EdgesList	  E;
   FaceIndexList attachmentA;
   FaceIndexList attachmentB;

   EdgesMap	    edgeMap;
   FaceIndexMap attachmentAMap;
   FaceIndexMap attachmentBMap;

private:
   bool isLoad;
};


#endif /* PolyhedronBody_hpp */
