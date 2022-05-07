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
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcEnvironmentLight(EnvironmentLight light, vec3 normal, vec3 viewDir);

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform Material material;
uniform EnvironmentLight envLight;

void main() {
    vec3 I = normalize(FragPos - viewPos);
    // vec3 R = reflect(I, normalize(Normal)); // 反射效果
    float ratio = 1.00 / 1.52;// 折射效果
    vec3 R = refract(I, normalize(Normal), ratio);
    R.y = -R.y;
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
    // envLight.ambient=
}

vec3 CalcEnvironmentLight(EnvironmentLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse1, TexCoords).rgb;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse1, TexCoords).rgb;

    // specular
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular1, TexCoords).rgb;

    // DirLight
    return ambient + diffuse + specular;
}