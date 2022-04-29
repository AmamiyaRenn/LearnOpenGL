#version 460 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main() {
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    // ambient
    vec3 ambient = texture(material.diffuse, TexCoords).rgb * light.ambient;

    // diffuse
    // vec3 lightDir = normalize(-light.direction);
    vec3 normal = normalize(Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * texture(material.diffuse, TexCoords).rgb * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * texture(material.specular, TexCoords).rgb * light.specular;

    // Phong Shading
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 result = ambient + (diffuse + specular) * attenuation * intensity;
    // 否则，使用环境光，让场景在聚光之外时不至于完全黑暗

    FragColor = vec4(result, 1.0);
}