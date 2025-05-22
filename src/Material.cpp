#include "Material.h"

void Material::Apply(Shader &shader) const
{
    shader.Bind();
    // 颜色属性
    shader.SetUniformVec3("materialdiffuseColor", diffuseColor);
    shader.SetUniformVec3("materialspecularColor", specularColor);
    shader.SetUniform1f("materialshininess", shininess);

    int texUnit = 0;

    if (diffuseMap) {

        diffuseMap-> Bind(texUnit);
        shader.SetUniform1i("materialdiffuseMap", texUnit++);
        shader.setUniformBool("materialhasDiffuseMap", true);
    } else {
        shader.setUniformBool("materialhasDiffuseMap", false);
    }

    if (specularMap) {

        specularMap-> Bind(texUnit);
        shader.SetUniform1i("materialspecularMap", texUnit++);
        shader.setUniformBool("materialhasSpecularMap", true);
    } else {
        shader.setUniformBool("materialhasSpecularMap", false);
    }

    if (normalMap) {

        normalMap-> Bind(texUnit);
        shader.SetUniform1i("materialnormalMap", texUnit++);
        shader.setUniformBool("materialhasNormalMap", true);
    } else {
        shader.setUniformBool("materialhasNormalMap", false);
    }

    if (opacityMap) {

        opacityMap-> Bind(texUnit);
        shader.SetUniform1i("materialopacityMap", texUnit++);
        shader.setUniformBool("materialhasOpacityMap", true);
    } else {
        shader.setUniformBool("materialhasOpacityMap", false);
    }

    if (emissiveMap) {

        emissiveMap-> Bind(texUnit);
        shader.SetUniform1i("materialemissiveMap", texUnit++);
        shader.setUniformBool("materialhasEmissiveMap", true);
    } else {
        shader.setUniformBool("materialhasEmissiveMap", false);
    }
}
