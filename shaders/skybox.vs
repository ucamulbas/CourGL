#version 330

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
layout (location = 0) in vec3 vsiPosition;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec2 vsoTexCoord;
out vec4 vsoPosition;

void main(void) {
  gl_Position = vec4(vsiPosition, 1.0) * modelViewProjectionMatrix ; 
  vsoPosition = vec4(vsiPosition, 1.0) * modelMatrix;
  vsoTexCoord = vec2(vsiTexCoord.s,1-vsiTexCoord.t);
 
}
