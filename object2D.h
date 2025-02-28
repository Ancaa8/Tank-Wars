#pragma once

#include <string>
#include <vector>
#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"

namespace object2D
{
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

    Mesh* CreateTerrain(const std::string& name, float width, float height);

	Mesh* CreateSemiCircle(const std::string& name, float radius, glm::vec3 color);
}