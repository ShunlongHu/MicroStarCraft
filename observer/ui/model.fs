#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emission1;
    sampler2D texture_normal1;
    sampler2D texture_opacity1;
    bool is_specular;
    bool is_emission;
    bool is_normal;
    bool is_opacity;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // opacity discard
    float opacity = 1.0;
    if (material.is_opacity) {
        opacity = texture(material.texture_diffuse1, TexCoords).a;
        if (opacity < 0.03) {
            discard;
        }
    }

    // ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    // specular
    vec3 specular = vec3(0,0,0);
    if (material.is_specular) {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;
    }

    // emission
    vec3 emission = vec3(0,0,0);
    if (material.is_emission) {
            float offset = 1.0 / 300.0;
            vec2 offsets[9] = vec2[](
                    vec2(-offset,  offset), // 左上
                    vec2( 0.0f,    offset), // 正上
                    vec2( offset,  offset), // 右上
                    vec2(-offset,  0.0f),   // 左
                    vec2( 0.0f,    0.0f),   // 中
                    vec2( offset,  0.0f),   // 右
                    vec2(-offset, -offset), // 左下
                    vec2( 0.0f,   -offset), // 正下
                    vec2( offset, -offset)  // 右下
                );

            float kernel[9] = float[](
                    1.0 / 16, 2.0 / 16, 1.0 / 16,
                    2.0 / 16, 4.0 / 16, 2.0 / 16,
                    1.0 / 16, 2.0 / 16, 1.0 / 16
                );
            vec3 sampleTex[9];
            for(int i = 0; i < 9; i++)
            {
                sampleTex[i] = vec3(texture(material.texture_emission1, TexCoords.st + offsets[i]));
            }
            vec3 col = vec3(0.0);
            for(int i = 0; i < 9; i++) {
                col += sampleTex[i] * kernel[i];
            }
            vec3 emission = col;
    }

    vec3 result = ambient + diffuse + specular + emission;

    // gamma correction
    float gamma = 2.2;
    result = pow(result, vec3(1.0/gamma));
    FragColor = vec4(result, 1.0);
} 