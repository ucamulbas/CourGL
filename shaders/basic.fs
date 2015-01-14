#version 330
uniform sampler2D myTexture;

in vec2 vsoTexCoord;
in vec3 lumVec;
in vec3 normal;
in vec3 camVec;

out vec4 fragColor;

void main(void) {
  int shininess = 20;
  vec3 R = reflect(normalize(-lumVec), normalize(normal));
  vec3 lightColor = vec3(1.0,1.0,1.0) * dot(normalize(normal),normalize(lumVec));
  vec3 specColor = pow(max(dot(R, normalize(camVec)), 0.0), shininess) * vec3(1.0, 1.0, 1.0);
  fragColor =texture(myTexture, vsoTexCoord) * (vec4(lightColor,1.0) + vec4(specColor,1.0));
}
