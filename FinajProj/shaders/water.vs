varying vec3 v_view_direction;
varying vec3 v_normal;
varying vec2 v_texture_coordinate;
varying vec3 pos;
uniform vec3 eye;

void main(void)
{
   pos = gl_Vertex.xyz;
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   //v_texture_coordinate = gl_MultiTexCoord0.xy;
   //v_view_direction =-gl_ModelViewMatrix[3].xyz;
   v_normal = gl_NormalMatrix * gl_Normal;
   v_view_direction =pos.xyz - eye.xyz;
   //if(v_view_direction.x < 0) gl_Position = vec4(0);
   gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_TexCoord[1] = gl_MultiTexCoord1;
   gl_TexCoord[2] = gl_MultiTexCoord2;
   gl_TexCoord[3] = gl_MultiTexCoord3;
   gl_TexCoord[4] = gl_MultiTexCoord4;
   gl_TexCoord[5] = gl_MultiTexCoord5;
}