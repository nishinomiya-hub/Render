#version 330 core
struct BaseLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct DirectionalLight {
    BaseLight base;
    vec3 direction;
};
struct PointLight {
    BaseLight base;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
};
// ... SpotLight 同理
struct SpotLight {
    BaseLight base;
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;
};

#define MAX_DIR_LIGHTS   4
#define MAX_POINT_LIGHTS 8

uniform int numDirLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

out vec4 FragColor;

// Phong 计算
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    // 合成
    vec3 ambient  = light.base.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.base.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.base.specular * spec;
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance
                               + light.quadratic * (distance * distance));
    vec3 ambient  = light.base.ambient  * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = light.base.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.base.specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

void main() {
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result  = vec3(0.0);

    // 累加所有方向光
    for (int i = 0; i < numDirLights; ++i)
        result += CalcDirLight(dirLights[i], norm, viewDir);
    // 累加所有点光源
    for (int i = 0; i < numPointLights; ++i)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
