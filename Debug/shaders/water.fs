uniform sampler2D XN; //left
uniform sampler2D XP; //right
uniform sampler2D YN; //bottom
uniform sampler2D YP; //top
uniform sampler2D ZN; //far
uniform sampler2D ZP; //near
uniform float offset;
uniform vec3 eye;

varying vec3 v_view_direction;
varying vec3 v_normal;
varying vec2 v_texture_coordinate;
varying vec3 pos;

void main(void)
{
   vec3 n_normal = normalize(v_normal);
   vec3 n_wiew_direction = normalize(v_view_direction);
   vec3 n_reflection = normalize(reflect(n_wiew_direction, n_normal)); 
   
   //do some pre-computing for deciding which wall we will reflect.
   vec3 multiplier;
   
   if(n_reflection.x > 0){
      multiplier.x = (offset - pos.x)/n_reflection.x;
   }
   else if(n_reflection.x < 0){
      multiplier.x = (-offset - pos.x)/n_reflection.x;
   }
   if(n_reflection.y > 0){
      multiplier.y = (offset - pos.y)/n_reflection.y;
   }
   else if(n_reflection.y < 0){
      multiplier.y = (-offset - pos.y)/n_reflection.y;
   }
   if(n_reflection.z > 0){
      multiplier.z = (offset - pos.z)/n_reflection.z;
   }
   else if(n_reflection.z < 0){
      multiplier.z = (-offset - pos.z)/n_reflection.z;
   }
   
   //decide which face of the skybox to use, then find the pixel.
   if(multiplier.x < multiplier.y && multiplier.x < multiplier.z){
      if(n_reflection.x > 0){ //use the right wall
	     //multiplier.x will be used.
		 int xPix = int(((pos.z + n_reflection.z * multiplier.x) + offset) * 256/(2*offset))-1;
		 int yPix = int(((pos.y + n_reflection.y * multiplier.x) + offset) * 256/(2*offset))-1;
	     gl_FragColor = texture2D(XP, vec2(xPix,yPix)); //debug
	     gl_FragColor.rgb = vec3(1,0,0);
	  }
	  else if(n_reflection.x < 0){ //use the left wall
	     //multiplier.x will be used.
		 int xPix = int(((pos.z + n_reflection.z * multiplier.x) + offset) * 256/(2*offset))-1;
		 int yPix = int(((pos.y + n_reflection.y * multiplier.x) + offset) * 256/(2*offset))-1;
		 int test = 200;
	     gl_FragColor = texture2D(XN, vec2(xPix,yPix)); //debug
		 gl_FragColor.a = 1; //debug
		 
	     //gl_FragColor.rgb = vec3(0,1,0);
	  }
   }
   else if(multiplier.y < multiplier.x && multiplier.y < multiplier.z){
	  if(n_reflection.y > 0){ //use the top wall
	     //multiplier.y will be used.
		 float xPix = pos.z + n_reflection.z * multiplier.y;
		 float yPix = pos.y + n_reflection.y * multiplier.y;
	     gl_FragColor = texture2D(YP, vec2(xPix,yPix)); //debug
	     gl_FragColor.rgb = vec3(1,1,0);
	  }
	  else if(n_reflection.y < 0){ //use the bottom wall
	     float xPix = 0;
		 float yPix = 0;
         gl_FragColor = texture2D(YN, vec2(xPix,yPix)); //debug
		 gl_FragColor.rgb = vec3(1,0,1);
	  }
   }
   else if(multiplier.z < multiplier.x && multiplier.z < multiplier.y){
	  if(n_reflection.z > 0){ //use the near wall
	     float xPix = 0;
		 float yPix = 0;
	     gl_FragColor = texture2D(ZP, vec2(xPix,yPix)); //debug
	     gl_FragColor.rgb = vec3(0,1,1);
	  }
	  else if(n_reflection.z < 0){ //use the far wall
	     float xPix = 0;
		 float yPix = 0;
	     gl_FragColor = texture2D(ZN, vec2(xPix,yPix)); //debug
	     gl_FragColor.rgb = vec3(0,0,1);
	  }
   }
   
   //gl_FragColor.a = 1.0;
}