#version 330
uniform sampler2D myTexture;
uniform vec3 couleur;

in vec2 vsoTexCoord;

out vec4 fragColor;

void main(void) {

  fragColor = texture(myTexture, vsoTexCoord);
    if(fragColor.a==0)
      discard;  

  
}