/*
** Student ID number: // fill_me
*/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <GL/glut.h>

#include "ply.h"
#include "geometry.h"

extern int box;
extern int wire;
extern int light;
extern int flat;
extern int texture;

extern Vector3f light_pos, viewer_pos;
extern Vector4f light_color, ambient_light;

float diffuse[4] = {0.9, 0.9, 0.8, 1.0};
static float specular[4] = {0.5, 0.5, 0.5, 1.0};
static float shininess[1] = {100};
static float white[4] = {1.0, 1.0, 1.0, 1.0};


PLYObject::PLYObject(FILE *in)
{
  int i;
  
  nproperties = 0;
  hasnormal = hascolor = hastexture = false;

  nv = nf = ne = 0;

  vertices = NULL;
#ifndef INTERLEAVED
  normals = NULL;
  colors = NULL;
#endif
  texcoords = NULL;
  faces = NULL;

  // init bounding box
  for (i = 0; i < 3; i++) {
    min[i] = FLT_MAX;
    max[i] = -FLT_MAX;
  }
  
  // default order
  for (i = 0; i < 11; i++)
    order[i] = -1;

  if (!checkHeader(in)) {
    fprintf(stderr, "Error: could not read PLY file.\n");
    return;
  }

#ifdef INTERLEAVED
  vertices = (RenderPoint*)calloc(nv, sizeof(RenderPoint));
#else
  vertices = (Vector3f*)calloc(nv, sizeof(Vector3f));
  normals = (Vector3f*)calloc(nv, sizeof(Vector3f));
  if (hascolor)
    colors = (Color3u*)calloc(nv, sizeof(Color3u));
#endif
  if (hastexture){
	texcoords = (Texture2f*)calloc(nf, sizeof(Texture2f));
  }

  faces = (Index3i*)calloc(nf, sizeof(Index3i));
  fnormals = (Vector3f*)calloc(nf, sizeof(Vector3f));

  readVertices(in);
  readFaces(in);
}


PLYObject::~PLYObject()
{
  // delete all allocated arrays

  if (vertices)
    free(vertices);
#ifndef INTERLEAVED
  if (normals)
    free(normals);
  if (colors)
    free(colors);
#endif
  if (texcoords)
    free(texcoords);
  if (faces)
    free(faces);  
}


bool PLYObject::checkHeader(FILE *in)
{
  char buf[128], type[128], c[32];
  int i;

  fprintf(stderr, "ply parser ver: 2009/10/05\n\n");
  // read ply file header

  //ply
  fscanf(in, "%s\n", buf);
  if (strcmp(buf, "ply") != 0) {
    fprintf(stderr, "Error: Input file is not of .ply type.\n");
    return false;
  }

  //format ascii 1.0
  fgets(buf, 128, in);
  if (strncmp(buf, "format ascii", 12) != 0) {
    fprintf(stderr, "Error: Input file is not in ASCII format.\n");
    return false;
  }

  //comment VCGLIB generated
  //comment TextureFile xxxx.ppm
  fgets(buf, 128, in);
  while (strncmp(buf, "comment", 7) == 0)
    fgets(buf, 128, in);

  // read number of vertices
  //element vertex xxxx
  if (strncmp(buf, "element vertex", 14) == 0)
    sscanf(buf, "element vertex %d\n", &nv);
  else {
    fprintf(stderr, "Error: number of vertices expected.\n");
    return false;
  }

  // read vertex properties order
  //property float x
  //property float y
  //property float z
  i = 0;
  fgets(buf, 128, in);  
  while (strncmp(buf, "property", 8) == 0) {
    sscanf(buf, "property %s %s\n", type, c);
    if (strncmp(c, "x", 1) == 0)
      order[0] = i;
    else if (strncmp(c, "y", 1) == 0)
      order[1] = i;
    else if (strncmp(c, "z", 1) == 0)
      order[2] = i;

    else if (strncmp(c, "nx", 2) == 0)
      order[3] = i;
    else if (strncmp(c, "ny", 2) == 0)
      order[4] = i;
    else if (strncmp(c, "nz", 2) == 0)
      order[5] = i;

    else if (strncmp(c, "red", 3) == 0)
      order[6] = i;
    else if (strncmp(c, "green", 5) == 0)
      order[7] = i;
    else if (strncmp(c, "blue", 4) == 0)
      order[8] = i;

    else if (strncmp(c, "tu", 2) == 0)
      order[9] = i;
    else if (strncmp(c, "tv", 2) == 0)
      order[10] = i;

    i++;
    fgets(buf, 128, in);
	//fprintf(stderr, "...buf = %s\n", buf);
  }
  nproperties = i;

  for (i = 0; i < 3; i++) {
    if (order[i] < 0) {
      fprintf(stderr, "Error: not enough vertex coordinate fields (nx, ny, nz).\n");
      return false;
   }
  }
  hasnormal = true;
  for (i = 3; i < 6; i++)
    if (order[i] < 0)
      hasnormal = false;
  hascolor = true;
  for (i = 6; i < 9; i++)
    if (order[i] < 0)
      hascolor = false;
  hastexture = true;
  for (i = 9; i < 11; i++)
    if (order[i] < 0)
      hastexture = false;

  if (!hasnormal)
    fprintf(stderr, "Warning: no normal coordinates used from file.\n");
  if (!hascolor)
    fprintf(stderr, "Warning: no color used from file.\n");
  if (!hastexture)
    fprintf(stderr, "Warning: no texture coordinates used from file.\n");


  // number of faces and face properties
  //element face xxxx
  if (strncmp(buf, "element face", 12) == 0)
    sscanf(buf, "element face %d\n", &nf);
  else {
    fprintf(stderr, "Error: number of faces expected.\n");
    return false;
  }
  ne = 3 * nf;

  //property list uchar int vertex_indices
  //property list uchar float texcoord
  fgets(buf, 128, in);
  while (strncmp(buf, "property list", 13) == 0) { // fish
	  if (strncmp(buf, "property list uchar float texcoord", 34) == 0){
		  hastexture = true;
	  }
	  fgets(buf, 128, in);
  }
  
  //fprintf(stderr, "hastexture = %d.\n", hastexture);

  //end_header
  //fgets(buf, 128, in);
  while (strncmp(buf, "end_header", 10) != 0)
    fgets(buf, 128, in);

  return true;
}


void PLYObject::readVertices(FILE *in)
{
  char buf[128];
  int i, j;
  float values[32];
  
  // read in vertex attributes
  for (i = 0; i < nv; i++) {
    fgets(buf, 128, in);
    sscanf(buf,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &values[0], &values[1], &values[2], &values[3],
            &values[4], &values[5], &values[6], &values[7], &values[8], &values[9], &values[10], &values[11],
           &values[12], &values[13], &values[14], &values[15]);

#ifdef INTERLEAVED
    for (j = 0; j < 3; j++)
      vertices[i].pos[j] = values[order[j]];
    if (hasnormal)
      for (j = 0; j < 3; j++)
        vertices[i].nor[j] = values[order[3+j]];
    if (hascolor)
      for (j = 0; j < 3; j++)
        vertices[i].c[j] = (unsigned char)values[order[6+j]];

    for (j = 0; j < 3; j++) {
      if (vertices[i].pos[j] < min[j])
        min[j] = vertices[i].pos[j];
      if (vertices[i].pos[j] > max[j])
        max[j] = vertices[i].pos[j];
    }
#else
    for (j = 0; j < 3; j++)
      vertices[i][j] = values[order[j]];
    if (hasnormal)
      for (j = 0; j < 3; j++)
        normals[i][j] = values[order[3+j]];
    if (hascolor)
      for (j = 0; j < 3; j++)
        colors[i][j] = (unsigned char)values[order[6+j]];

    for (j = 0; j < 3; j++) {
      if (vertices[i][j] < min[j])
        min[j] = vertices[i][j];
      if (vertices[i][j] > max[j])
        max[j] = vertices[i][j];
    }
#endif
/*
    if (hastexture)
      for (j = 0; j < 2; j++)
        texcoords[i][j] = values[order[9+j]];
*/
  }
}


void PLYObject::readFaces(FILE *in)
{
  char buf[128];
  int i, j, k;

  float st[6]; // fish
  int num6;  

  // read in face connectivity
  for (i = 0; i < nf; i++) {
    fgets(buf, 128, in);

	if (!hastexture){
      sscanf(buf, "%d %d %d %d", &k, &faces[i][0], &faces[i][1], &faces[i][2]);
	}
	else { // fish
	    sscanf(	buf, "%d %d %d %d %d %f %f %f %f %f %f", 
				&k, &faces[i][0], &faces[i][1], &faces[i][2],
				&num6, &st[0], &st[1], &st[2], &st[3], &st[4], &st[5]);
		texcoords[i][0] = st[0];
		texcoords[i][1] = st[1];
		texcoords[i][2] = st[2];
		texcoords[i][3] = st[3];
		texcoords[i][4] = st[4];
		texcoords[i][5] = st[5];
	}

    if (k != 3) {
      fprintf(stderr, "Error: not a triangular face.\n");
      exit(1);
    }
#ifdef INTERLEAVED
    // set up face normal
    normal(fnormals[i], vertices[faces[i][0]].pos, vertices[faces[i][1]].pos, vertices[faces[i][2]].pos);

    // accumulate normal information of each vertex
    if (!hasnormal)
      for (j = 0; j < 3; j++)
        for (k = 0; k < 3; k++)
          vertices[faces[i][j]].nor[k] += fnormals[i][k];
#else
    // set up face normal
    normal(fnormals[i], vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]]);

    // accumulate normal information of each vertex
    if (!hasnormal)
      for (j = 0; j < 3; j++)
        for (k = 0; k < 3; k++)
          normals[faces[i][j]][k] += fnormals[i][k];
#endif
  }
  
#ifdef INTERLEAVED
  if (!hasnormal)
    for (i = 0; i < nv; i++)
      normalize(vertices[i].nor);
#else
  if (!hasnormal)
    for (i = 0; i < nv; i++)
      normalize(normals[i]);
#endif
      
  hasnormal = true;
}


void PLYObject::write(FILE *out)
{
  int i;

  // print header
  fprintf(out, "ply\n");
  fprintf(out, "format ascii 1.0\n");
  fprintf(out, "comment created by Renato's PLYObject\n");
  fprintf(out, "element vertex %d\n", nv);

  fprintf(out, "property float x\n");
  fprintf(out, "property float y\n");
  fprintf(out, "property float z\n");
  if (hasnormal) {
    fprintf(out, "property float nx\n");
    fprintf(out, "property float ny\n");
    fprintf(out, "property float nz\n");
  }
  if (hascolor) {
    fprintf(out, "property uint red\n");
    fprintf(out, "property uint green\n");
    fprintf(out, "property uint blue\n");
  }
  if (hastexture) {
    fprintf(out, "property float ut\n");
    fprintf(out, "property float tv\n");
  }

  fprintf(out, "element face %d\n", nf);
  fprintf(out, "property list uchar int vertex_indices\n");
  fprintf(out, "end_header\n");

  // print vertices
  for (i = 0; i < nv; i++) {
#ifdef INTERLEAVED
    fprintf(out, "%f %f %f", vertices[i].pos[0], vertices[i].pos[1], vertices[i].pos[2]);
    if (hasnormal)
      fprintf(out, " %f %f %f", vertices[i].nor[0], vertices[i].nor[1], vertices[i].nor[2]);
    if (hascolor)
      fprintf(out, " %d %d %d", vertices[i].c[0], vertices[i].c[1], vertices[i].c[2]);
#else
    fprintf(out, "%f %f %f", vertices[i][0], vertices[i][1], vertices[i][2]);
    if (hasnormal)
      fprintf(out, " %f %f %f", normals[i][0], normals[i][1], normals[i][2]);
    if (hascolor)
      fprintf(out, " %d %d %d", colors[i][0], colors[i][1], colors[i][2]);
#endif
    if (hastexture)
      fprintf(out, " %f %f", texcoords[i][0], texcoords[i][1]);
    fprintf(out, "\n");
  }

  // print faces
  for (i = 0; i < nf; i++) {
    fprintf(out, "3 %d %d %d\n", faces[i][0], faces[i][1], faces[i][2]);
  }
}


void PLYObject::resize()
{
  int i;
  float size, scale;
  Vector3f tmin, tmax;

  // rescale vertex coordinates to be in [-1,1]
  size = 0.0;
  if (size < max[0]-min[0])
    size = max[0]-min[0];
  if (size < max[1]-min[1])
    size = max[1]-min[1];
  if (size < max[2]-min[2])
    size = max[2]-min[2];
  scale = 2.0 / size;
#ifdef INTERLEAVED
  for (i = 0; i < nv; i++) {
    vertices[i].pos[0] = scale * (vertices[i].pos[0] - 0.5*(max[0]+min[0]));
    vertices[i].pos[1] = scale * (vertices[i].pos[1] - 0.5*(max[1]+min[1]));
    vertices[i].pos[2] = scale * (vertices[i].pos[2] - 0.5*(max[2]+min[2]));
  }
#else
  for (i = 0; i < nv; i++) {
    vertices[i][0] = scale * (vertices[i][0] - 0.5*(max[0]+min[0]));
    vertices[i][1] = scale * (vertices[i][1] - 0.5*(max[1]+min[1]));
    vertices[i][2] = scale * (vertices[i][2] - 0.5*(max[2]+min[2]));
  }
#endif
  for (i = 0; i < 3; i++) {
    tmin[i] = scale * 0.5 * (min[i]-max[i]);
    tmax[i] = scale * 0.5 * (max[i]-min[i]);
    min[i] = tmin[i];
    max[i] = tmax[i];
  }
}


void PLYObject::invertNormals()
{
  int i, tmp;

#ifdef INTERLEAVED
  for (i = 0; i < nv; i++)
    scale(vertices[i].nor, -1.0);
#else
  for (i = 0; i < nv; i++)
    scale(normals[i], -1.0);
#endif
  for (i = 0; i < nf; i++) {
    scale(fnormals[i], -1.0);
    tmp = faces[i][0];
    faces[i][0] = faces[i][2];
    faces[i][2] = tmp;
  }
}


void PLYObject::setPlanarTexture()
{
  int i;
  
  if (!hastexture) {
    texcoords = (Texture2f*)calloc(nv, sizeof(Texture2f));
    hastexture = 1;
  }

#ifdef INTERLEAVED
  for (i = 0; i < nv; i++) {
    texcoords[i][0] = (vertices[i].pos[0] - min[0]) / (max[0]-min[0]);
    texcoords[i][1] = (vertices[i].pos[1] - min[1]) / (max[1]-min[1]);
  }
#else
  for (i = 0; i < nv; i++) {
    texcoords[i][0] = (vertices[i][0] - min[0]) / (max[0]-min[0]);
    texcoords[i][1] = (vertices[i][1] - min[1]) / (max[1]-min[1]);
  }
#endif
}


void PLYObject::draw()
{
  int i, j;

  if (box) {
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(5.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(min[0], min[1], min[2]);
    glVertex3f(min[0], min[1], max[2]);
    glVertex3f(min[0], max[1], max[2]);
    glVertex3f(min[0], max[1], min[2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(max[0], max[1], max[2]);
    glVertex3f(max[0], max[1], min[2]);
    glVertex3f(max[0], min[1], min[2]);
    glVertex3f(max[0], min[1], max[2]);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(min[0], min[1], min[2]);
    glVertex3f(max[0], min[1], min[2]);
    glVertex3f(min[0], min[1], max[2]);
    glVertex3f(max[0], min[1], max[2]);
    glVertex3f(min[0], max[1], max[2]);
    glVertex3f(max[0], max[1], max[2]);
    glVertex3f(min[0], max[1], min[2]);
    glVertex3f(max[0], max[1], min[2]);
    glEnd();
  }

  // setup default material
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

  // render wireframe model
  if (wire) {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.05, 0.05, 0.05);
    
    glBegin(GL_TRIANGLES);
    for (i = 0; i < nf; i++){
      for (j = 0; j < 3; j++){
#ifdef INTERLEAVED
        glVertex3fv(vertices[faces[i][j]].pos);	// vertex coordinates
#else
        glVertex3fv(vertices[faces[i][j]]);	// vertex coordinates
		//printf("%d-(%f,%f,%f),", j, vertices[faces[i][j]][0], vertices[faces[i][j]][1], vertices[faces[i][j]][2]);
#endif
	  }
	  //printf("\n");
	}
    glEnd();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.5, 1.0);
  }

  // setup per-point color mode
  //glColor3fv(diffuse);
  // fill_me

  if (hascolor) {
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	printf("hascolor...\n");
  }

  // set lighting if enabled
  if (light){
	glEnable(GL_LIGHTING);
  }
  else{
	glDisable(GL_LIGHTING);  
	glColor4fv(diffuse); // fish
  }

  // setup texture mode
  if (texture && hastexture) {
    glEnable(GL_TEXTURE_2D);

    // setup white object color for use with texture modulation
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
  glBegin(GL_TRIANGLES);

  for (i = 0; i < nf; i++) {
    if (flat)
      glNormal3fv(fnormals[i]);			// face normal
    
    for (j = 0; j < 3; j++) {
	  if (texture && hastexture){
		glTexCoord2f(texcoords[i][j*2], texcoords[i][j*2+1]);
	  }
#ifdef INTERLEAVED
      if (!texture && hascolor)
        glColor3ubv((GLubyte*)vertices[faces[i][j]].c);
      if (!flat)
        glNormal3fv(vertices[faces[i][j]].nor);	// vertex normal

      glVertex3fv(vertices[faces[i][j]].pos);	// vertex coordinates
#else
      if (!texture && hascolor)
        glColor3ubv((GLubyte*)colors[faces[i][j]]);
      if (!flat)
        glNormal3fv(normals[faces[i][j]]);	// vertex normal

      glVertex3fv(vertices[faces[i][j]]);	// vertex coordinates
#endif
    }
  }
  glEnd();

  glDisable(GL_POLYGON_OFFSET_FILL);
  if (hascolor)
    glDisable(GL_COLOR_MATERIAL);
  if (texture && hastexture){
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
  }
}
