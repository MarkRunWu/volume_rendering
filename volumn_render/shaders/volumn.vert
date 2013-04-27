// simple vertex shader
varying vec3 textureCoords;
varying vec3 normal;
void main()
{
    textureCoords = gl_Vertex.xyz;
	normal =   gl_Normal;
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
}
