#shader vertex

#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(position, 0.0,  1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
};

#shader fragment

#version 330

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, TexCoord);
};