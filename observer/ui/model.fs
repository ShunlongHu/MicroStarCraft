#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    float shininess;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height =  texture(material.texture_height1, texCoords).r;
    return texCoords - viewDir.xy * (height * heightScale);
}

void main()
{
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;

//     vec2 a = ParallaxMapping(fs_in.TexCoords,  viewDir);
//     if(a.x > 1.0 || a.y > 1.0 || a.x < 0.0 || a.y < 0.0) {
//         discard;
//     }
    // obtain normal from normal map
    vec3 normal = texture(material.texture_normal1, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // get diffuse color
    vec3 color = texture(material.texture_diffuse1, texCoords).rgb;
    // ambient
    vec3 ambient = light.ambient * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * color;
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = light.specular * spec * texture(material.texture_specular1, texCoords).rgb;
//     vec3 result = abs(texture(material.texture_diffuse1, texCoords).rgb - texture(material.texture_specular1, texCoords).rgb);
    vec3 result = texture(material.texture_specular1, texCoords).rgb;

    // gamma correction
    float gamma = 2.2;
    result = pow(result, vec3(1.0/gamma));
    FragColor = vec4(result, 1.0);
}