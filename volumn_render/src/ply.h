#ifndef PLY_H
#define PLY_H

#include <stdio.h>

//#define INTERLEAVED
typedef float Vector3f[3];
typedef unsigned char Color3u[3];
typedef float Texture2f[6];
typedef int Index3i[3];

#ifdef INTERLEAVED
struct RenderPoint {
  Vector3f pos;		// coordinates    12 bytes
  Vector3f nor;		// normal         12 bytes
  float    size;	// size            4 bytes
  Color3u  c;		// color           3 bytes
  unsigned char padding;
};
#endif


class PLYObject {
public:

  PLYObject(FILE *in);
  ~PLYObject();

  bool checkHeader(FILE *in);
  void readVertices(FILE *in);
  void readFaces(FILE *in);
  void write(FILE *out);
  void resize();
  void invertNormals();
  void setPlanarTexture();
  void draw();
  
  int nproperties;			// number of vertex properties
  int order[11];			// order of (x,y,z), (nx,ny,nz), (red,green,blue), (tu,tv) vertex properties
  bool hasnormal, hascolor, hastexture;
  int nv, nf, ne;			// number of vertices, faces, edges
  Vector3f min, max;

#ifdef INTERLEAVED
  RenderPoint *vertices;	// array of compacted vertex data
#else
  Vector3f *vertices;		// array of point coordinates
  Vector3f *normals;		// array of point normals
  Color3u *colors;			// array of point colors
#endif
  Texture2f *texcoords;		// array of texture coords
  Index3i *faces;			// array of face indices
  Vector3f *fnormals;		// array of face normals
};

#endif
