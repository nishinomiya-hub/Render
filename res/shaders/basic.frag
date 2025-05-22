uniform bool material.hasDiffuseMap;
uniform sampler2D material.diffuseMap;

uniform bool material.hasSpecularMap;
uniform sampler2D material.specularMap;

uniform bool material.hasOpacityMap;
uniform sampler2D material.opacityMap;

uniform bool material.hasEmissiveMap;
uniform sampler2D material.emissiveMap;

uniform vec3 material.diffuseColor;
uniform vec3 material.specularColor;
uniform float material.shininess;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    vec4 baseColor = vec4(material.diffuseColor, 1.0);
    if (material.hasDiffuseMap)
        baseColor = texture(material.diffuseMap, TexCoords);

    if (material.hasOpacityMap) {
        float alpha = texture(material.opacityMap, TexCoords).r;
        if (alpha < 0.1)
            discard;
        baseColor.a *= alpha;
    }

    vec3 emissive = vec3(0.0);
    if (material.hasEmissiveMap)
        emissive = texture(material.emissiveMap, TexCoords).rgb;

    // 最基本的光照示例，可按需替换为 Blinn-Phong 或 PBR
    vec3 lighting = baseColor.rgb + emissive;

    FragColor = vec4(lighting, baseColor.a);
}
