#version 460 core

in vec3 Normal;
in vec3 Position;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    vec3 I = normalize(Position - cameraPos);
    // vec3 R = reflect(I, normalize(Normal)); // 反射效果
    float ratio = 1.00 / 1.52;// 折射效果
    vec3 R = refract(I, normalize(Normal), ratio);
    R.y = -R.y;
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}