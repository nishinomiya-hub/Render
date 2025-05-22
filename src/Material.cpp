#include "Material.h"

void Material::Apply(Shader &shader) const
{
    shader.Bind();
    // 颜色属性
    shader.SetUniformVec3("material.diffuseColor", diffuseColor);
    shader.SetUniformVec3("material.specularColor", specularColor);
    shader.SetUniform1f("material.shininess", shininess);

    int texUnit = 0;

    if (diffuseMap) {

        diffuseMap-> Bind(texUnit);
        shader.SetUniform1i("material.diffuseMap", texUnit++);
        shader.setUniformBool("material.hasDiffuseMap", true);
    } else {
        shader.setUniformBool("material.hasDiffuseMap", false);
    }

    if (specularMap) {

        specularMap-> Bind(texUnit);
        shader.SetUniform1i("material.specularMap", texUnit++);
        shader.setUniformBool("material.hasSpecularMap", true);
    } else {
        shader.setUniformBool("material.hasSpecularMap", false);
    }

    if (normalMap) {

        normalMap-> Bind(texUnit);
        shader.SetUniform1i("material.normalMap", texUnit++);
        shader.setUniformBool("material.hasNormalMap", true);
    } else {
        shader.setUniformBool("material.hasNormalMap", false);
    }

    if (opacityMap) {

        opacityMap-> Bind(texUnit);
        shader.SetUniform1i("material.opacityMap", texUnit++);
        shader.setUniformBool("material.hasOpacityMap", true);
    } else {
        shader.setUniformBool("material.hasOpacityMap", false);
    }

    if (emissiveMap) {

        emissiveMap-> Bind(texUnit);
        shader.SetUniform1i("material.emissiveMap", texUnit++);
        shader.setUniformBool("material.hasEmissiveMap", true);
    } else {
        shader.setUniformBool("material.hasEmissiveMap", false);
    }
}
