varying vec3 N;
varying vec3 v;
varying float vertexHeight;

uniform float minHeight;
uniform float maxHeight;

void main()
{
	v = vec3(gl_ModelViewMatrix * gl_Vertex);
   vertexHeight = gl_Vertex.y;
   
   N = normalize(gl_NormalMatrix * gl_Normal);

   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
   
   //set water level:
   if(vertexHeight < minHeight + 0.3*(maxHeight - minHeight))
   {
      gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.x, minHeight + 0.3*(maxHeight - minHeight), gl_Vertex.z, gl_Vertex.w);
	  N = gl_NormalMatrix * vec3(0,1,0);
   }
}