#pragma once

//为了SIMD,对齐内存

#define GLM_FORCE_ALIGNED
#define GLM_FORCE_INLINE
#define GLM_FORCE_AVX2
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_aligned.hpp>

using RGBA = glm::u8vec4;