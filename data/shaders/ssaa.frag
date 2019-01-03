#version 330 core

in vec2 fragUV;

uniform sampler2D frameBufferTexture;
uniform vec2 sampleOffsets[4];

out vec4 color;

void main(){
    color = vec4(texture2D(frameBufferTexture, fragUV), 1);
}
