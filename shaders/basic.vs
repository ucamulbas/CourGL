#version 330

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform vec3 lumPos;
uniform vec3 camPos;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec2 vsoTexCoord;
out vec3 normal;
out vec3 lumVec;
out vec3 camVec;

void main(void) {
  normal = vsiNormal * mat3(transpose(inverse(modelMatrix))); //modelMatrix peut tout subir sauf projection et lookat !
  gl_Position = vec4(vsiPosition, 1.0) * modelViewProjectionMatrix ; //celle-ci peut TOUT subir
  lumVec = lumPos - vec3(vec4(vsiPosition, 1.0) * modelMatrix);
  camVec = camPos - vec3(vec4(vsiPosition, 1.0) * modelMatrix);
  vsoTexCoord = vec2(vsiTexCoord.s,vsiTexCoord.t);
}
