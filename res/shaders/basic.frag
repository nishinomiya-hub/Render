uniform bool materialhasDiffuseMap;
uniform sampler2D materialdiffuseMap;

uniform bool materialhasSpecularMap;
uniform sampler2D materialspecularMap;

uniform bool materialhasOpacityMap;
uniform sampler2D materialopacityMap;

uniform bool materialhasEmissiveMap;
uniform sampler2D materialemissiveMap;

uniform vec3 materialdiffuseColor;
uniform vec3 materialspecularColor;
uniform float materialshininess;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    vec4 baseColor = vec4(materialdiffuseColor, 1.0);
    if (materialhasDiffuseMap)
        baseColor = texture(materialdiffuseMap, TexCoords);

    if (materialhasOpacityMap) {
        float alpha = texture(materialopacityMap, TexCoords).r;
        if (alpha < 0.1)
            discard;
        baseColor.a *= alpha;
    }

    vec3 emissive = vec3(0.0);
    if (materialhasEmissiveMap)
        emissive = texture(materialemissiveMap, TexCoords).rgb;

    // 最基本的光照示例，可按需替换为 Blinn-Phong 或 PBR
    vec3 lighting = baseColor.rgb + emissive;

    FragColor = vec4(lighting, baseColor.a);
}
