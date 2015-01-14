#version 330
uniform sampler2D myTexture;
uniform vec3 couleur;

in vec2 vsoTexCoord;

out vec4 fragColor;

void main(void) {

  fragColor = texture(myTexture, vsoTexCoord);
  if(fragColor.a==0)
    discard; 
  fragColor = mix(vec4(texture(myTexture, vsoTexCoord).rgb * couleur, 1.0), vec4(couleur, 1.0), 0.5);
  if(fragColor.r==0 && fragColor.g==0)
    discard;  
}
