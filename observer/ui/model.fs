#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;


void main()
{
    vec4 text = texture(texture_diffuse1, TexCoords);

//         FragColor = texture(texture_diffuse1, TexCoords) + texture(texture_diffuse2, TexCoords) + texture(texture_diffuse0, TexCoords);
//     FragColor = text * 100 + 0.5;
        FragColor = vec4(TexCoords,0,0);
}