#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
};

struct EnvironmentLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform Material material;
uniform EnvironmentLight envLight;

void main() {
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal)); // 反射效果
    // float ratio = 1.00 / 1.52;// 折射效果
    // vec3 R = refract(I, normalize(Normal), ratio);
    R.y = -R.y;
    // FragColor = vec4(texture(skybox, R).rgb, 1.0);

    // ambient
    vec3 ambient = envLight.ambient * texture(material.diffuse1, TexCoords).rgb;

    // diffuse
    vec3 diffuse = envLight.diffuse * texture(material.diffuse1, TexCoords).rgb;

    // specular
    // vec3 halfwayDir = normalize(viewDir + lightDir);
    // float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);
    // vec3 specular = envLight.specular * spec * texture(material.specular1, TexCoords).rgb;
    // float spec = max(,0.0)
    vec3 skyboxTexture = vec3(texture(skybox, R).r, texture(skybox, R).g, texture(skybox, R).b);
    vec3 specular = envLight.specular * texture(skybox, R).rgb * texture(material.specular1, TexCoords).rgb;

    // FragColor = vec4(ambient + diffuse + specular, 1.0);
    FragColor = vec4(ambient + diffuse, 1.0); 
    // FragColor += vec4(specular, 1.0);
}
