#include "lab_m1/Tema1/Tema1.h"
#include <vector>
#include <iostream>
#include <cmath>
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

namespace m1 {
    Tema1::Tema1()
        : skyColor(180.0f / 255.0f, 235.0f / 255.0f, 1.0f),
          tankRadius(30.0f),
          snowmanRadius(30.0f),
          projectileRadius(5.0f), 
          tank1Health(100),
          tank2Health(100),
          armRotation1(0.0f),
          armRotation2(0.0f)
    {
    }

    Tema1::~Tema1() {
    }

    void Tema1::Init() {
        glm::ivec2 resolution = window->GetResolution();
        auto camera = GetSceneCamera();

        camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
        camera->SetPosition(glm::vec3(0, 0, 50));
        camera->SetRotation(glm::vec3(0, 0, 0));
        camera->Update();
        GetCameraInput()->SetActive(false);

        cx = resolution.x / 2;
        cy = resolution.y / 2;
		barrelrotation = 0.0f;

        cx2 = resolution.x / 2 + 300.0f;
        cy2 = resolution.y / 2;
        barrelrotation2 = 0.0f;

        elapsedTime = 0.0f;

        float terrainWidth = resolution.x;
        float terrainHeight = 100;
        int numSegments = 1000;  
        terrainHeights.resize(numSegments);

        float amplitude = 60.0f;
        float baseline = 200.0f;
        float frequency = 0.005f;
        float segmentWidth = terrainWidth / numSegments;


        for (int i = 0; i < numSegments; ++i) {
            float x = i * segmentWidth;
            terrainHeights[i] = amplitude * (sin(x * frequency) + 2 * sin(0.5f * x * frequency) + 0.5f * sin(3.0f * x * frequency)) + baseline;
        }

        terrainHeightsOcean = terrainHeights;

        Mesh* terrainDay = CreateTerrain("terrainDay", terrainWidth, terrainHeight);
        AddMeshToList(terrainDay);

		Mesh* terrainNight = CreateTerrain2("terrainNight", terrainWidth, terrainHeight, glm::vec3(33.0f / 255.0f, 117.0f / 255.0f, 24.0f / 255.0f));
		AddMeshToList(terrainNight);

        Mesh* ocean = CreateTerrain2("ocean", terrainWidth, terrainHeight, glm::vec3(62.0f / 255.0f, 141.0f / 255.0f, 178.0f / 255.0f));
        AddMeshToList(ocean);

        Mesh* cloud = CreateCloud("cloud", 200.0f, 50.0f); 
        AddMeshToList(cloud);

        Mesh* tankbase = CreateTankBase("tankbase");
        AddMeshToList(tankbase);

		Mesh* tankbase2 = CreateTankBaseWithColor("tankbase2", glm::vec3(80.0f / 255.0f, 40.0f / 255.0f, 112.0f / 255.0f));
		AddMeshToList(tankbase2);

        Mesh* tankmiddle = CreateTankBase2("tankmiddle");
        AddMeshToList(tankmiddle);

		Mesh* tankmiddle2 = CreateTankBaseWithColor("tankmiddle2", glm::vec3(192.0f / 255.0f, 160.0f / 255.0f, 222.0f / 255.0f));
		AddMeshToList(tankmiddle2);

        Mesh* semicircle = CreateSemiCircle("semicircle", 2.0f, glm::vec3(247.0f / 255.0f, 215.0f / 255.0f, 246.0f / 255.0f));
        AddMeshToList(semicircle);

        Mesh* semicircle2 = CreateSemiCircle("semicircle2", 2.0f, glm::vec3(226.0f / 255.0f, 200.0f / 255.0f, 247.0f / 255.0f));
        AddMeshToList(semicircle2);

        Mesh* rectangle = CreateRectangle("rectangle", 10.0f, 10.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        AddMeshToList(rectangle);

		Mesh* projectile = CreateCircle("projectile", 5.0f, glm::vec3(0.0f, 0.0f, 0.0f));
		AddMeshToList(projectile);

        Mesh* snowball = CreateCircle("snowball", 5.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        AddMeshToList(snowball);

        Mesh* healthBarFrame = CreateRectangle("healthBarFrame", 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        AddMeshToList(healthBarFrame);

        Mesh* healthBarFill = CreateRectangle("healthBarFill", 1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)); 
        AddMeshToList(healthBarFill);

		Mesh* point = CreateCircle("point", 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		AddMeshToList(point);

		Mesh* sun = CreateCircle("sun", 60.0f, glm::vec3(1.0f, 1.0f, 0.0f));
		AddMeshToList(sun);

		Mesh* sunray = CreateLine("sunray");
		AddMeshToList(sunray);

		Mesh* moon = CreateCircle("moon", 60.0f, glm::vec3(238.0f / 255.0f, 238.0f / 255.0f, 165.0f / 255.0f));
		AddMeshToList(moon);

		Mesh* mooncircle = CreateCircle("mooncircle", 50.0f, glm::vec3(9.0f / 255.0f, 33.0f / 255.0f, 100.0f / 255.0f));
		AddMeshToList(mooncircle);

		Mesh* triangle = CreateRightTriangle("triangle", 3.0f, 5.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		AddMeshToList(triangle);


        lastFrameTime = glfwGetTime();
    }

    Mesh* Tema1::CreateTerrain(const std::string& name, float width, float height) {
        int numSegments = terrainHeights.size();
        float rectWidth = width / numSegments;

        Mesh* terrain = new Mesh(name);
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        glm::vec3 terrainColor = glm::vec3(114.0f / 255.0f, 186.0f / 255.0f, 50.0f / 255.0f);

        for (int i = 0; i < numSegments; i++) {
            float x1 = i * rectWidth;
            float y1 = terrainHeights[i];

            float x2 = (i + 1) * rectWidth;
            float y2 = terrainHeights[std::min(i + 1, numSegments - 1)];

            glm::vec3 p1(x1, 0, 0);
            glm::vec3 p2(x2, 0, 0);
            glm::vec3 p3(x2, y2, 0);
            glm::vec3 p4(x1, y1, 0);

            vertices.push_back(VertexFormat(p1, terrainColor));
            vertices.push_back(VertexFormat(p2, terrainColor));
            vertices.push_back(VertexFormat(p3, terrainColor));
            vertices.push_back(VertexFormat(p4, terrainColor));

            int startIndex = i * 4;
            indices.push_back(startIndex);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
        }

        terrain->InitFromData(vertices, indices);
        return terrain;
    }

    Mesh* Tema1::CreateTerrain2(const std::string& name, float width, float height, glm::vec3 color) {
        int numSegments = terrainHeights.size();  
        float rectWidth = width / numSegments; 

        Mesh* terrain = new Mesh(name);
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        glm::vec3 terrainColor = color;

        for (int i = 0; i < numSegments; i++) {
            float x1 = i * rectWidth;
            float y1 = terrainHeights[i];

            float x2 = (i + 1) * rectWidth;
            float y2 = terrainHeights[std::min(i + 1, numSegments - 1)];

            glm::vec3 p1(x1, 0, 0);       
            glm::vec3 p2(x2, 0, 0);      
            glm::vec3 p3(x2, y2, 0);      
            glm::vec3 p4(x1, y1, 0); 

            vertices.push_back(VertexFormat(p1, terrainColor));
            vertices.push_back(VertexFormat(p2, terrainColor));
            vertices.push_back(VertexFormat(p3, terrainColor));
            vertices.push_back(VertexFormat(p4, terrainColor));

            int startIndex = i * 4;
            indices.push_back(startIndex);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
        }

        terrain->InitFromData(vertices, indices);
        return terrain;
    }

    Mesh* Tema1::CreateCloud(const std::string& name, float width, float height) {
        float numSegments = 200.0f;          
        float amplitude = 30.0f;             
        float rectWidth = width / numSegments;
        float baseline = 300.0f;             
        float frequency = 0.02f;             

        Mesh* cloud = new Mesh(name);
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f); 

        for (int i = 0; i < numSegments; i++) {
            float x1 = i * rectWidth;
            float y1 = amplitude * (sin(x1 * frequency) + 0.5f * sin(2.0f * x1 * frequency)) + baseline;

            float x2 = (i + 1) * rectWidth;
            float y2 = amplitude * (sin(x2 * frequency) + 0.5f * sin(2.0f * x2 * frequency)) + baseline;

            glm::vec3 p1(x1, 290.0f, 0);
            glm::vec3 p2(x2, 290.0f, 0);
            glm::vec3 p3(x2, y2, 0);
            glm::vec3 p4(x1, y1, 0);

            vertices.push_back(VertexFormat(p1, cloudColor));
            vertices.push_back(VertexFormat(p2, cloudColor));
            vertices.push_back(VertexFormat(p3, cloudColor));
            vertices.push_back(VertexFormat(p4, cloudColor));

            int startIndex = i * 4;
            indices.push_back(startIndex);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
        }

        cloud->InitFromData(vertices, indices);
        return cloud;
    }

    float Tema1::GetTerrainHeightAt(float x) {
        int numSegments = terrainHeights.size();
        float terrainWidth = window->GetResolution().x;
        float segmentWidth = terrainWidth / numSegments;

        int index1 = static_cast<int>(x / segmentWidth);
        int index2 = index1 + 1;                              
        if (index1 >= numSegments - 1) 
            return terrainHeights.back();  
        if (index1 < 0) 
            return terrainHeights.front();

        float t = (x - index1 * segmentWidth) / segmentWidth;  
        return terrainHeights[index1] * (1 - t) + terrainHeights[index2] * t;
    }

    Mesh* Tema1::CreateTankBase(const std::string& name) {
        std::vector<VertexFormat> vertices
        {

           VertexFormat(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
           VertexFormat(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
           VertexFormat(glm::vec3(0.8f, -1.0f, 1.0f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
           VertexFormat(glm::vec3(-0.8f, -1.0f, 1.0f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),


           VertexFormat(glm::vec3(-0.5f, 1.0f, -0.5f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
           VertexFormat(glm::vec3(0.5f, 1.0f, -0.5f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
           VertexFormat(glm::vec3(0.4f, 1.0f, 0.5f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
           VertexFormat(glm::vec3(-0.4f, 1.0f, 0.5f), glm::vec3(112.0f / 255.0f, 5.0f / 255.0f, 78.0f / 255.0f)),
        };

        std::vector<unsigned int> indices =
        {

            0, 1, 2,
            0, 2, 3,


            4, 5, 6,
            4, 6, 7,


            0, 1, 5,
            0, 5, 4,

            1, 2, 6,
            1, 6, 5,

            2, 3, 7,
            2, 7, 6,

            3, 0, 4,
            3, 4, 7
        };

        Mesh* tankbase = new Mesh(name);
        tankbase->InitFromData(vertices, indices);
        return tankbase;
    }

    Mesh* Tema1::CreateTankBase2(const std::string& name) {
        std::vector<VertexFormat> vertices
        {
           VertexFormat(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
           VertexFormat(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
           VertexFormat(glm::vec3(0.8f, -1.0f, 1.0f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
           VertexFormat(glm::vec3(-0.8f, -1.0f, 1.0f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),

          VertexFormat(glm::vec3(-0.5f, 1.0f, -0.5f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
          VertexFormat(glm::vec3(0.5f, 1.0f, -0.5f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
          VertexFormat(glm::vec3(0.4f, 1.0f, 0.5f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
          VertexFormat(glm::vec3(-0.4f, 1.0f, 0.5f), glm::vec3(250.0f / 255.0f, 164.0f / 255.0f, 223.0f / 255.0f)),
        };

        std::vector<unsigned int> indices =
        {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            0, 1, 5,
            0, 5, 4,

            1, 2, 6,
            1, 6, 5,

            2, 3, 7,
            2, 7, 6,

            3, 0, 4,
            3, 4, 7
        };

        Mesh* tankmiddle = new Mesh(name);
        tankmiddle->InitFromData(vertices, indices);
        return tankmiddle;
    }

    Mesh* Tema1::CreateTankBaseWithColor(const std::string& name, glm::vec3 color) {
        std::vector<VertexFormat> vertices{
            VertexFormat(glm::vec3(-1.0f, -1.0f, -1.0f), color),
            VertexFormat(glm::vec3(1.0f, -1.0f, -1.0f), color),
            VertexFormat(glm::vec3(0.8f, -1.0f, 1.0f), color),
            VertexFormat(glm::vec3(-0.8f, -1.0f, 1.0f), color),
            VertexFormat(glm::vec3(-0.5f, 1.0f, -0.5f), color),
            VertexFormat(glm::vec3(0.5f, 1.0f, -0.5f), color),
            VertexFormat(glm::vec3(0.4f, 1.0f, 0.5f), color),
            VertexFormat(glm::vec3(-0.4f, 1.0f, 0.5f), color),
        };

        std::vector<unsigned int> indices{
            0, 1, 2,
            0, 2, 3,
            4, 5, 6,
            4, 6, 7,
            0, 1, 5,
            0, 5, 4,
            1, 2, 6,
            1, 6, 5,
            2, 3, 7,
            2, 7, 6,
            3, 0, 4,
            3, 4, 7
        };

        Mesh* tankBase = new Mesh(name);
        tankBase->InitFromData(vertices, indices);
        return tankBase;
    }

    Mesh* Tema1::CreateSemiCircle(const std::string& name, float radius, glm::vec3 color) {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;
        int segments = 30;

        vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color));

        for (int i = 0; i <= segments; ++i) {
            float angle = 3.14159f * i / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            vertices.push_back(VertexFormat(glm::vec3(x, y, 0.0f), color));
        }

        for (int i = 1; i <= segments; ++i) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        Mesh* semicircle = new Mesh(name);
        semicircle->InitFromData(vertices, indices);
        return semicircle;
    }

    Mesh* Tema1::CreateRectangle(const std::string& name, float width, float height, glm::vec3 color) {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color));
        vertices.push_back(VertexFormat(glm::vec3(width, 0.0f, 0.0f), color));
        vertices.push_back(VertexFormat(glm::vec3(width, height, 0.0f), color));
        vertices.push_back(VertexFormat(glm::vec3(0.0f, height, 0.0f), color));

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);

        Mesh* rectangle = new Mesh(name);
        rectangle->InitFromData(vertices, indices);
        return rectangle;
    }

    Mesh* Tema1::CreateCircle(const std::string& name, float radius, glm::vec3 color) {
		int segments = 30;
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color));

        for (int i = 0; i <= segments; ++i) {
            float angle = 2.0f * glm::pi<float>() * i / segments;  
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            vertices.push_back(VertexFormat(glm::vec3(x, y, 0.0f), color));
        }

        for (int i = 1; i <= segments; ++i) {
            indices.push_back(0);       
            indices.push_back(i);      
            indices.push_back(i + 1);  
        }

        Mesh* circle = new Mesh(name);
        circle->InitFromData(vertices, indices);
        return circle;
    }

    Mesh* Tema1::CreateLine(const std::string& name) {
        std::vector<VertexFormat> vertices{
            VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 1, 0)), 
            VertexFormat(glm::vec3(20, 0, 0), glm::vec3(1, 1, 0)) 
        };
        std::vector<unsigned int> indices{ 0, 1 };

        Mesh* line = new Mesh(name);
        line->SetDrawMode(GL_LINES); 
        line->InitFromData(vertices, indices);
        return line;
    }

    Mesh* Tema1::CreateRightTriangle(const std::string& name, float width, float height, glm::vec3 color) {
        std::vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color), 
            VertexFormat(glm::vec3(width, 0.0f, 0.0f), color),            
            VertexFormat(glm::vec3(0.0f, height, 0.0f), color)         
        };

        std::vector<unsigned int> indices = { 0, 1, 2 };

        Mesh* triangle = new Mesh(name);
        triangle->InitFromData(vertices, indices);
        return triangle;
    }

    Mesh* Tema1::CreateSnowyTerrain(const std::string& name, float width, float height) {
        int numSegments = terrainHeights.size();
        float rectWidth = width / numSegments;

        Mesh* terrain = new Mesh(name);
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        glm::vec3 snowColor = glm::vec3(1.0f, 1.0f, 1.0f); 

        for (int i = 0; i < numSegments; i++) {
            float x1 = i * rectWidth;
            float y1 = terrainHeights[i];

            float x2 = (i + 1) * rectWidth;
            float y2 = terrainHeights[std::min(i + 1, numSegments - 1)];

            glm::vec3 p1(x1, 0, 0);
            glm::vec3 p2(x2, 0, 0);
            glm::vec3 p3(x2, y2, 0);
            glm::vec3 p4(x1, y1, 0);

            vertices.push_back(VertexFormat(p1, snowColor));
            vertices.push_back(VertexFormat(p2, snowColor));
            vertices.push_back(VertexFormat(p3, snowColor));
            vertices.push_back(VertexFormat(p4, snowColor));

            int startIndex = i * 4;
            indices.push_back(startIndex);
            indices.push_back(startIndex + 1);
            indices.push_back(startIndex + 2);

            indices.push_back(startIndex);
            indices.push_back(startIndex + 2);
            indices.push_back(startIndex + 3);
        }

        terrain->InitFromData(vertices, indices);
        return terrain;
    }

    Mesh* Tema1::CreateSnowman(const std::string& name, glm::vec3 baseColor, glm::vec3 detailColor) {
        Mesh* snowman = new Mesh(name);

        Mesh* base = CreateCircle("snowman_base", 30.0f, baseColor);
        AddMeshToList(base);

        Mesh* middle = CreateCircle("snowman_middle", 20.0f, baseColor);
        AddMeshToList(middle);

        Mesh* head = CreateCircle("snowman_head", 15.0f, baseColor);
        AddMeshToList(head);

        Mesh* leftEye = CreateCircle("snowman_left_eye", 2.0f, detailColor);
        AddMeshToList(leftEye);

        Mesh* rightEye = CreateCircle("snowman_right_eye", 2.0f, detailColor);
        AddMeshToList(rightEye);

        Mesh* nose = CreateRightTriangle("snowman_nose", 5.0f, 3.0f, glm::vec3(1.0f, 0.5f, 0.0f)); // Orange
        AddMeshToList(nose);

        Mesh* hand = CreateRectangle("snowman_hand", 15.0f, 2.0f, glm::vec3(0.55f, 0.27f, 0.07f)); // Brown
        AddMeshToList(hand);

        return snowman;
    }

    bool Tema1::CheckCollision(const glm::vec2& position1, float radius1, const glm::vec2& position2, float radius2) {
        float distance = glm::distance(position1, position2);
        return distance <= (radius1 + radius2);
    }

    bool Tema1::CheckProjectileTerrainCollision(const glm::vec2& projectilePosition) {
        float terrainHeight = GetTerrainHeightAt(projectilePosition.x);
        float collisionThreshold = 5.0f; 

        return (projectilePosition.y - terrainHeight) <= collisionThreshold;
    }

    void Tema1::DeformTerrain(float impactX, float radius, float deformationAmount) {
        int numSegments = terrainHeights.size();
        float terrainWidth = window->GetResolution().x;
        float rectWidth = terrainWidth / numSegments;

        int start = std::max(0, static_cast<int>((impactX - radius) / rectWidth));
        int end = std::min(numSegments - 1, static_cast<int>((impactX + radius) / rectWidth));

        for (int i = start; i <= end; i++) {
            float x = i * rectWidth;
            float distance = std::abs(impactX - x);

            if (distance <= radius) {
                float t = distance / radius;
                float deformationFactor = 1.0f - smoothstep(0.0f, 1.0f, t);

                terrainHeights[i] -= deformationAmount * deformationFactor;

                if (terrainHeights[i] < 0)
                    terrainHeights[i] = 0;
            }
        }

        lastDeformedStart = start;
        lastDeformedEnd = end;

        isSmoothingActive = true;
        gaussianSmoothingTimeElapsed = 0.0f;

        shouldUpdateTerrainDayMesh = true;
        shouldUpdateTerrainNightMesh = true;
    }

    float Tema1::smoothstep(float edge0, float edge1, float x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  
        return x * x * (3.0f - 2.0f * x);
    }

    float Tema1::clamp(float x, float minVal, float maxVal) {
        return std::max(minVal, std::min(x, maxVal));
    }

    void Tema1::LaunchProjectile() {
        float dx = 5.0f; 
        float y1 = GetTerrainHeightAt(cx - dx);
        float y2 = GetTerrainHeightAt(cx + dx);
        float angle = atan2(y2 - y1, 2 * dx); 
        float offsetX = -20.0f * cos(barrelrotation); 
        float offsetY = 21.0f + 2.5f * sin(barrelrotation);
        glm::vec2 startPosition = glm::vec2(cx + offsetX, cy + offsetY);


        float launchAngle = barrelrotation + angle + glm::pi<float>() / 2;

        float arcProjectileSpeed = projectileSpeed * 1.0f;

        glm::vec2 startVelocity = glm::vec2(arcProjectileSpeed * cos(launchAngle), arcProjectileSpeed * sin(launchAngle));

        projectiles.push_back({ startPosition, startVelocity, true, false });
    }

    void Tema1::LaunchProjectile2() {
        float dx2 = 5.0f;
        float y12 = GetTerrainHeightAt(cx2 - dx2);
        float y22 = GetTerrainHeightAt(cx2 + dx2);
        float angle2 = atan2(y22 - y12, 2 * dx2);
        float offsetX2 = -20.0f * cos(barrelrotation2);
        float offsetY2 = 21.0f + 2.5f * sin(barrelrotation2);
        glm::vec2 startPosition2 = glm::vec2(cx2 + offsetX2, cy2 + offsetY2);

        float launchAngle2 = barrelrotation2 + angle2 + glm::pi<float>() / 2;
        glm::vec2 startVelocity2 = glm::vec2(projectileSpeed * cos(launchAngle2), projectileSpeed * sin(launchAngle2));

        projectiles2.push_back({ startPosition2, startVelocity2, true, false });
    }

    void Tema1::FrameStart() {
        if (isNightMode) {
            skyColor = glm::vec3(9.0f / 255.0f, 33.0f / 255.0f, 100.0f / 255.0f); 
        }
        else {
            skyColor = glm::vec3(180.0f / 255.0f, 235.0f / 255.0f, 1.0f); 
        }

        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::ivec2 resolution = window->GetResolution();
        glViewport(0, 0, resolution.x, resolution.y);
    }

    void Tema1::Update(float deltaTimeSeconds) {
        float currentFrameTime = glfwGetTime();
        deltaTimeSeconds = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        applyTerrainSlide(deltaTimeSeconds);        
        applyGaussianSmoothing(deltaTimeSeconds);

        glm::vec3 dayColor(180.0f / 255.0f, 235.0f / 255.0f, 1.0f);
        glm::vec3 sunsetColor(255.0f / 255.0f, 153.0f / 255.0f, 51.0f);
        glm::vec3 nightColor(236.0f / 255.0f, 52.0f / 255.0f, 177.0f / 255.0f);

        if (!isWinterMode && !isOceanMode && !isNightMode) {
            timeOfDay += deltaTimeSeconds;
            float cycleProgress = fmod(timeOfDay, dayNightCycleDuration) / dayNightCycleDuration;

            if (cycleProgress < 0.5f) {
                float t = cycleProgress * 2.0f;
                skyColor = glm::mix(dayColor, sunsetColor, t);
            }
            else { 
                float t = (cycleProgress - 0.5f) * 2.0f;
                skyColor = glm::mix(sunsetColor, nightColor, t);
            }
        }

        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::ivec2 resolution = window->GetResolution();
        glViewport(0, 0, resolution.x, resolution.y);

        
        OnInputUpdate(deltaTimeSeconds);

        cloudoffset += 20.0f * deltaTimeSeconds; 

        if (cloudoffset > 1200.0f) {
            cloudoffset = -200.0f; 
        }

        
        if (!isOceanMode && shouldUpdateTerrainDayMesh) {
            Mesh* updatedTerrainDay = CreateTerrain("terrainDay", window->GetResolution().x, 100);
            meshes["terrainDay"] = updatedTerrainDay;
            shouldUpdateTerrainDayMesh = false;
        }

        if (!isOceanMode && shouldUpdateTerrainNightMesh) {
            Mesh* updatedTerrainNight = CreateTerrain2(
                "terrainNight",
                window->GetResolution().x,
                100,
                glm::vec3(33.0f / 255.0f, 117.0f / 255.0f, 24.0f / 255.0f) 
            );
            meshes["terrainNight"] = updatedTerrainNight;
            shouldUpdateTerrainNightMesh = false;
        }



        modelMatrix = glm::mat3(1);

        if (isWinterMode) {
            RenderMesh2D(meshes["snowyTerrain"], shaders["VertexColor"], modelMatrix);
            glm::mat3 snowmanTransform = glm::mat3(1);

 
            snowmanTransform = glm::mat3(1);
            snowmanTransform *= transform2D::Translate(cx, cy);
            RenderMesh2D(meshes["snowman_base"], shaders["VertexColor"], snowmanTransform);

            snowmanTransform *= transform2D::Translate(0, 40); 
            RenderMesh2D(meshes["snowman_middle"], shaders["VertexColor"], snowmanTransform);

            snowmanTransform *= transform2D::Translate(0, 30);
            RenderMesh2D(meshes["snowman_head"], shaders["VertexColor"], snowmanTransform);

            glm::mat3 eyeTransform = snowmanTransform;
            eyeTransform *= transform2D::Translate(-5.0f, 5.0f); 
            RenderMesh2D(meshes["snowman_left_eye"], shaders["VertexColor"], eyeTransform);

            eyeTransform = snowmanTransform;
            eyeTransform *= transform2D::Translate(5.0f, 5.0f); 
            RenderMesh2D(meshes["snowman_right_eye"], shaders["VertexColor"], eyeTransform);

            glm::mat3 noseTransform = snowmanTransform;
            noseTransform *= transform2D::Translate(0, 0); 
            RenderMesh2D(meshes["snowman_nose"], shaders["VertexColor"], noseTransform);

            glm::mat3 armMatrix = glm::mat3(1);
            armMatrix *= transform2D::Translate(cx + 10, cy + 60.0f); 
            armMatrix *= transform2D::Rotate(armRotation1);      
            armMatrix *= transform2D::Translate(0, -10.0f); 
            RenderMesh2D(meshes["snowman_hand"], shaders["VertexColor"], armMatrix);

            snowmanTransform = glm::mat3(1);

            snowmanTransform *= transform2D::Translate(cx2, cy2);
            RenderMesh2D(meshes["snowman_base"], shaders["VertexColor"], snowmanTransform);

            snowmanTransform *= transform2D::Translate(0, 40);
            RenderMesh2D(meshes["snowman_middle"], shaders["VertexColor"], snowmanTransform);

            snowmanTransform *= transform2D::Translate(0, 30); 
            RenderMesh2D(meshes["snowman_head"], shaders["VertexColor"], snowmanTransform);

            eyeTransform = snowmanTransform;
            eyeTransform *= transform2D::Translate(-5.0f, 5.0f); 
            RenderMesh2D(meshes["snowman_left_eye"], shaders["VertexColor"], eyeTransform);

            eyeTransform = snowmanTransform;
            eyeTransform *= transform2D::Translate(5.0f, 5.0f);
            RenderMesh2D(meshes["snowman_right_eye"], shaders["VertexColor"], eyeTransform);

            noseTransform = snowmanTransform;
            noseTransform *= transform2D::Translate(0, 0); 
            noseTransform *=transform2D::Rotate(glm::radians(-180.0f));
            RenderMesh2D(meshes["snowman_nose"], shaders["VertexColor"], noseTransform);

            armMatrix = glm::mat3(1);
            armMatrix *= transform2D::Translate(cx2-10 , cy2 + 60.0f);
            armMatrix *= transform2D::Rotate(armRotation2);
            armMatrix *= transform2D::Translate(0, -10.0f);
            armMatrix *= transform2D::Rotate(-135.0f);
            RenderMesh2D(meshes["snowman_hand"], shaders["VertexColor"], armMatrix);

            return;
        }

        if (isNightMode) {
            RenderMesh2D(meshes["terrainNight"], shaders["VertexColor"], modelMatrix);
        }
        else if (isOceanMode) {
            RenderMesh2D(meshes["ocean"], shaders["VertexColor"], modelMatrix);
        }
        else {
            RenderMesh2D(meshes["terrainDay"], shaders["VertexColor"], modelMatrix);
        }

        if (isNightMode) {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(150.0f, 650.0f);
            RenderMesh2D(meshes["moon"], shaders["VertexColor"], modelMatrix);
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(180.0f, 650.0f);
            RenderMesh2D(meshes["mooncircle"], shaders["VertexColor"], modelMatrix);
        }
        else {
           
           
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(150.0f, 650.0f);
            RenderMesh2D(meshes["sun"], shaders["VertexColor"], modelMatrix);

            int numRays = 80;
            float sunRadius = 60.0f;

            for (int i = 0; i < numRays; i++) {
                float littleangle = i * glm::two_pi<float>() / numRays;
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(150.0f, 650.0f);
                modelMatrix *= transform2D::Rotate(littleangle);
                modelMatrix *= transform2D::Translate(sunRadius, 0.0f);

                RenderMesh2D(meshes["sunray"], shaders["VertexColor"], modelMatrix);
            }
        }


        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(-1300.0f + cloudoffset, 300.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(-800.0f + cloudoffset, 300.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(-300.0f + cloudoffset, 300.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(200.0f + cloudoffset, 300.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(700.0f + cloudoffset, 300.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(1200.0f + cloudoffset, 300.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(-1500.0f + cloudoffset, 160.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(-1000.0f + cloudoffset, 160.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(-500.0f + cloudoffset, 160.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(0.0f + cloudoffset, 160.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(500.0f + cloudoffset, 160.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(1000.0f + cloudoffset, 160.0f);
        RenderMesh2D(meshes["cloud"], shaders["VertexColor"], modelMatrix);


        
        float dx = 5.0f; 
        float y1 = GetTerrainHeightAt(cx - dx);
        float y2 = GetTerrainHeightAt(cx + dx);

        float angle = atan2(y2 - y1, 2 * dx);

		cy = GetTerrainHeightAt(cx);
        float angleTank1 = atan2(y2 - y1, 2 * dx) + barrelrotation + glm::pi<float>()/2;
        float offsetX = -20.0f * cos(barrelrotation);
        float offsetY = 21.0f + 2.5f * sin(barrelrotation);


        glm::vec2 startPosTank1(cx, cy);
        std::vector<glm::vec2> trajectoryPointsTank1;
        CalculateTrajectory(startPosTank1, angleTank1, trajectoryPointsTank1, glm::vec2(offsetX, offsetY));

        if (tank1Health > 0) {
            for (size_t i = 0; i < trajectoryPointsTank1.size() - 1; ++i) {
                glm::vec2 p1 = trajectoryPointsTank1[i];
                glm::vec2 p2 = trajectoryPointsTank1[i + 1];

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(p1.x, p1.y);
                RenderMesh2D(meshes["point"], shaders["VertexColor"], modelMatrix);
            }
        }



        float dx2 = 5.0f;
        float y12 = GetTerrainHeightAt(cx2 - dx2);
        float y22 = GetTerrainHeightAt(cx2 + dx2);

        float angle2 = atan2(y22 - y12, 2 * dx2);

        cy2 = GetTerrainHeightAt(cx2);

        float angleTank2 = atan2(y22 - y12, 2 * dx) + barrelrotation2 + glm::pi<float>()/2;
        float offsetX2 = -20.0f * cos(barrelrotation2);
        float offsetY2 = 21.0f + 2.5f * sin(barrelrotation2);

        glm::vec2 startPosTank2(cx2, cy2);
        std::vector<glm::vec2> trajectoryPointsTank2;
        CalculateTrajectory(startPosTank2, angleTank2, trajectoryPointsTank2, glm::vec2(offsetX2, offsetY2));

        if (tank2Health > 0) {
            for (size_t i = 0; i < trajectoryPointsTank2.size() - 1; ++i) {
                glm::vec2 p1 = trajectoryPointsTank2[i];
                glm::vec2 p2 = trajectoryPointsTank2[i + 1];

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(p1.x, p1.y);
                RenderMesh2D(meshes["point"], shaders["VertexColor"], modelMatrix);
            }
        }

        

        glDisable(GL_DEPTH_TEST);

        if (tank1Health > 0) {
            glm::mat3 healthBarFrameMatrix1 = glm::mat3(1);
            healthBarFrameMatrix1 *= transform2D::Translate(cx - healthBarWidth / 2, cy + healthBarOffsetY);
            healthBarFrameMatrix1 *= transform2D::Scale(healthBarWidth + 2.0f, healthBarHeight + 2.0f); 
            RenderMesh2D(meshes["healthBarFrame"], shaders["VertexColor"], healthBarFrameMatrix1);


            int activeSegments = tank1Health / (100 / maxHealthSegments); 

            for (int i = 0; i < activeSegments; ++i) {
                glm::mat3 segmentMatrix1 = glm::mat3(1);
                segmentMatrix1 *= transform2D::Translate(cx - healthBarWidth / 2 + i * segmentWidth +1.0f, cy + healthBarOffsetY+ 1.0f);
                segmentMatrix1 *= transform2D::Scale(segmentWidth, healthBarHeight);
                RenderMesh2D(meshes["healthBarFill"], shaders["VertexColor"], segmentMatrix1);
            }
        }


        if (tank2Health > 0) {

            glm::mat3 healthBarFrameMatrix2 = glm::mat3(1);
            healthBarFrameMatrix2 *= transform2D::Translate(cx2 - healthBarWidth / 2, cy2 + healthBarOffsetY);
            healthBarFrameMatrix2 *= transform2D::Scale(healthBarWidth + 2.0f, healthBarHeight + 2.0f); 
            RenderMesh2D(meshes["healthBarFrame"], shaders["VertexColor"], healthBarFrameMatrix2);


            int activeSegments = tank2Health / (100 / maxHealthSegments);

            for (int i = 0; i < activeSegments; ++i) {
                glm::mat3 segmentMatrix2 = glm::mat3(1);
                segmentMatrix2 *= transform2D::Translate(cx2 - healthBarWidth / 2 + i * segmentWidth + 1.0f, cy2 + healthBarOffsetY + 1.0f);
                segmentMatrix2 *= transform2D::Scale(segmentWidth, healthBarHeight);
                RenderMesh2D(meshes["healthBarFill"], shaders["VertexColor"], segmentMatrix2);
            }
        }

       
        if (isWinterMode) {
            for (auto& snowball : snowballs) {
                if (snowball.active) {
                    snowball.velocity.y += gravity * deltaTimeSeconds * 2;
                    snowball.position += snowball.velocity * deltaTimeSeconds;

                    modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(snowball.position.x, snowball.position.y);
                    RenderMesh2D(meshes["snowball"], shaders["VertexColor"], modelMatrix);
                    if (CheckProjectileTerrainCollision(snowball.position) && !isOceanMode) {
                        DeformTerrain(snowball.position.x, 100.0f, 80.0f);
                        snowball.active = false;
                    }

                    if (snowball.position.y < 0 || snowball.position.x < 0 || snowball.position.x > window->GetResolution().x) {
                        snowball.active = false;
                    }

                }
            }
        }

        for (auto& projectile : projectiles) {
            if (projectile.active) {
                projectile.velocity.y += gravity * deltaTimeSeconds * 2;
                projectile.position += projectile.velocity * deltaTimeSeconds;

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
                RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);

                if (!projectile.hitRegistered && CheckCollision(projectile.position, projectileRadius, glm::vec2(cx2, cy2), tankRadius)) {
                    projectile.hitRegistered = true;
                    tank2Health -= 20;
                    std::cout << "Tank2 was hit! Remaining health: " << tank2Health << std::endl;

                    if (tank2Health <= 0) {
                        isTank2Active = false;
                        std::cout << "Tank2 has been destroyed!" << std::endl;
                    }
                    projectile.active = false;
                }

                if (CheckProjectileTerrainCollision(projectile.position) && !isOceanMode) {
                    DeformTerrain(projectile.position.x, 100.0f, 80.0f);
                    projectile.active = false;
                }

                if (projectile.position.y < 0 || projectile.position.x < 0 || projectile.position.x > window->GetResolution().x) {
                    projectile.active = false;
                }
            }
        }

        for (auto& projectile : projectiles2) {
            if (projectile.active) {
                projectile.velocity.y += gravity * deltaTimeSeconds * 2;
                projectile.position += projectile.velocity * deltaTimeSeconds;

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(projectile.position.x, projectile.position.y);
                RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);

                if (!projectile.hitRegistered && CheckCollision(projectile.position, projectileRadius, glm::vec2(cx, cy), tankRadius)) {
                    projectile.hitRegistered = true;
                    tank1Health -= 20;
                    std::cout << "Tank1 was hit! Remaining health: " << tank1Health << std::endl;

                    if (tank1Health <= 0) {
                        isTank1Active = false;
                        std::cout << "Tank1 has been destroyed!" << std::endl;
                    }
                    projectile.active = false;
                }

                if (CheckProjectileTerrainCollision(projectile.position) && !isOceanMode) {
                    DeformTerrain(projectile.position.x, 100.0f, 80.0f);
                    projectile.active = false;
                }

                if (projectile.position.y < 0 || projectile.position.x < 0 || projectile.position.x > window->GetResolution().x) {
                    projectile.active = false;
                }
            }
        }




        if (!isWinterMode) {
            if (isTank1Active && tank1Health > 0) {
                l1 = 40.0f;
                h1 = 5.0f;
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(cx, cy);
                modelMatrix *= transform2D::Rotate(angle);
                modelMatrix *= transform2D::Rotate(glm::pi<float>());
                modelMatrix *= transform2D::Translate(l1 / 2, -h1 / 2);
                modelMatrix *= transform2D::Scale(l1, h1);
                RenderMesh2D(meshes["tankbase"], shaders["VertexColor"], modelMatrix);

                l2 = 50.0f;
                h2 = 8.0f;
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(cx, cy);
                modelMatrix *= transform2D::Rotate(angle);
                modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + 2.5f);
                modelMatrix *= transform2D::Scale(l2, h2);
                RenderMesh2D(meshes["tankmiddle"], shaders["VertexColor"], modelMatrix);

                l3 = 8.0f;
                h3 = 8.0f;

                l4 = 0.2f;
                h4 = 4.0f;

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(cx, cy);
                modelMatrix *= transform2D::Rotate(angle);
                modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + h3 + 2.5f);
                modelMatrix *= transform2D::Rotate(barrelrotation);
                modelMatrix *= transform2D::Translate(-1.0f, h4 / 2);
                modelMatrix *= transform2D::Scale(l4, h4);
                RenderMesh2D(meshes["rectangle"], shaders["VertexColor"], modelMatrix);
                if (!isOceanMode) {
                    modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(cx, cy);
                    modelMatrix *= transform2D::Rotate(angle);
                    modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + h3 + 2.5f);
                    modelMatrix *= transform2D::Scale(l3, h3);
                    RenderMesh2D(meshes["semicircle"], shaders["VertexColor"], modelMatrix);
                }
                else {
                    modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(cx, cy);
                    modelMatrix *= transform2D::Rotate(angle);
                    modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + h3 + 5.5f);
                    modelMatrix *= transform2D::Scale(l3, h3);
                    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);

                }


            }

            if (isTank2Active && tank2Health > 0) {
                l1 = 40.0f;
                h1 = 5.0f;
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(cx2, cy2);
                modelMatrix *= transform2D::Rotate(angle2);
                modelMatrix *= transform2D::Rotate(glm::pi<float>());
                modelMatrix *= transform2D::Translate(l1 / 2, -h1 / 2);
                modelMatrix *= transform2D::Scale(l1, h1);
                RenderMesh2D(meshes["tankbase2"], shaders["VertexColor"], modelMatrix);

                l2 = 50.0f;
                h2 = 8.0f;
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(cx2, cy2);
                modelMatrix *= transform2D::Rotate(angle2);
                modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + 2.5f);
                modelMatrix *= transform2D::Scale(l2, h2);
                RenderMesh2D(meshes["tankmiddle2"], shaders["VertexColor"], modelMatrix);

                l3 = 8.0f;
                h3 = 8.0f;
                l4 = 0.2f;
                h4 = 4.0f;
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(cx2, cy2);
                modelMatrix *= transform2D::Rotate(angle2);
                modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + h3 + 2.5f);
                modelMatrix *= transform2D::Rotate(barrelrotation2);
                modelMatrix *= transform2D::Translate(-1.0f, h4 / 2);
                modelMatrix *= transform2D::Scale(l4, h4);
                RenderMesh2D(meshes["rectangle"], shaders["VertexColor"], modelMatrix);
                if (!isOceanMode) {
                    modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(cx2, cy2);
                    modelMatrix *= transform2D::Rotate(angle2);
                    modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + h3 + 2.5f);
                    modelMatrix *= transform2D::Scale(l3, h3);
                    RenderMesh2D(meshes["semicircle2"], shaders["VertexColor"], modelMatrix);
                }

                else {
                    modelMatrix = glm::mat3(1);
                    modelMatrix *= transform2D::Translate(cx2, cy2);
                    modelMatrix *= transform2D::Rotate(angle2);
                    modelMatrix *= transform2D::Translate(-l1 / 2, h1 + h2 + h3 + 5.5f);
                    modelMatrix *= transform2D::Scale(l3, h3);
                    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);

                }

            }
        }     

        
    }

    void Tema1::FrameEnd() {
    }

    void Tema1::RenderScene() {

    }

    void Tema1::CalculateTrajectory(glm::vec2 startPos, float angle, std::vector<glm::vec2>& points, glm::vec2 offset) {
        points.clear();

        float timeStep = 0.001f; 
        float maxTime = 3.0f; 

        glm::vec2 velocity = glm::vec2(projectileSpeed * cos(angle), projectileSpeed * sin(angle));

        for (float t = 0.0f; t < maxTime; t += timeStep) {
            float x = startPos.x + offset.x + velocity.x * t;
            float y = startPos.y + offset.y + velocity.y * t + 0.5f * gravity * t * t * 2;

            points.push_back(glm::vec2(x, y));

            if (y < GetTerrainHeightAt(x)) {
                break;
            }
        }
    }

    void Tema1::OnInputUpdate(float deltaTimeSeconds)
    {
        const float movespeed = 200.0f;
		rotationspeed = glm::pi<float>() / 4;
	    maxRotation = glm::pi<float>() / 2;
        minRotation = glm::pi<float>() / -2;
        const float rotationSpeed = glm::pi<float>() / 4;
        const float maxRotation = glm::pi<float>() / 2;  
        const float minRotation = -glm::pi<float>() / 2;

        if (window->KeyHold(GLFW_KEY_A))
        {
            cx -= movespeed * deltaTimeSeconds;
            cy = GetTerrainHeightAt(cx);

        }
        if (window->KeyHold(GLFW_KEY_D))
        {
            cx += movespeed * deltaTimeSeconds;
            cy = GetTerrainHeightAt(cx);

        }

        if (window->KeyHold(GLFW_KEY_W)) {
            if (barrelrotation < maxRotation) {
                barrelrotation += rotationspeed * deltaTimeSeconds;
                if (barrelrotation > maxRotation) {
                    barrelrotation = maxRotation;
                }
            }
        }
        if (window->KeyHold(GLFW_KEY_S)) {
            if (barrelrotation > minRotation) {
                barrelrotation -= rotationspeed * deltaTimeSeconds;
                if (barrelrotation < minRotation) {
                    barrelrotation = minRotation;
                }
            }
        }

        if (window->KeyHold(GLFW_KEY_LEFT))
        {
            cx2 -= movespeed * deltaTimeSeconds;
            cy2 = GetTerrainHeightAt(cx2);

        }
        if (window->KeyHold(GLFW_KEY_RIGHT))
        {
            cx2 += movespeed * deltaTimeSeconds;
            cy2 = GetTerrainHeightAt(cx2);

        }

        if (window->KeyHold(GLFW_KEY_UP)) {
            if (barrelrotation2 < maxRotation) {
                barrelrotation2 += rotationspeed * deltaTimeSeconds;
                if (barrelrotation2 > maxRotation) {
                    barrelrotation2 = maxRotation;
                }
            }
        }
        if (window->KeyHold(GLFW_KEY_DOWN)) {
            if (barrelrotation2 > minRotation) {
                barrelrotation2 -= rotationspeed * deltaTimeSeconds;
                if (barrelrotation2 < minRotation) {
                    barrelrotation2 = minRotation;
                }
            }
        }
        if (window->KeyHold(GLFW_KEY_W) && armRotation1 < maxRotation) {
            armRotation1 += rotationSpeed * deltaTimeSeconds;
        }
        if (window->KeyHold(GLFW_KEY_S) && armRotation1 > minRotation) {
            armRotation1 -= rotationSpeed * deltaTimeSeconds;
        }

        if (window->KeyHold(GLFW_KEY_UP) && armRotation2 < maxRotation) {
            armRotation2 += rotationSpeed * deltaTimeSeconds;
        }
        if (window->KeyHold(GLFW_KEY_DOWN) && armRotation2 > minRotation) {
            armRotation2 -= rotationSpeed * deltaTimeSeconds;
        }

    }

    void Tema1::OnKeyPress(int key, int mods) {
        if (key == GLFW_KEY_SPACE) {
            if (isWinterMode) {
                LaunchSnowball();
            }
            else {
                LaunchProjectile(); 
            }
        }
        if (key == GLFW_KEY_N) {
            isNightMode = !isNightMode; 
        }

        if (key == GLFW_KEY_O) {
			isOceanMode = !isOceanMode;
        }
        if (key == GLFW_KEY_ENTER) {
            if (isWinterMode) {
                LaunchSnowball2(); 
            }
            else {
                LaunchProjectile2(); 
            }
        }
        if (key == GLFW_KEY_C) {
            isWinterMode = !isWinterMode;
            if (isWinterMode) {
                Mesh* snowyTerrain = CreateSnowyTerrain("snowyTerrain", window->GetResolution().x, 100);
                AddMeshToList(snowyTerrain);

                Mesh* snowman = CreateSnowman("snowman", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
                AddMeshToList(snowman);

                
            }
        }
        if (key == GLFW_KEY_SPACE && isWinterMode) {
            LaunchSnowball(); 
        }
        if (key == GLFW_KEY_ENTER && isWinterMode) {
            LaunchSnowball2();
        }

    }

    void Tema1::applyTerrainSlide(float deltaTime) {
        if (!isSliding) {
            return; 
        }

        slideTimeElapsed += deltaTime;
        if (slideTimeElapsed >= maxSlideTime) {
            isSliding = false; 
            return;
        }

        float threshold = 10.0f;
        int neighborhood = 5;
        float transferAmount = 1.0f;

        std::vector<float> newHeights = terrainHeights;
        bool modified = false;  

        for (int i = neighborhood; i < terrainHeights.size() - neighborhood; ++i) {
            float d_left = terrainHeights[i] - terrainHeights[i - neighborhood];
            float d_right = terrainHeights[i] - terrainHeights[i + neighborhood];

            if (std::abs(d_left) > threshold) {
                float direction = (d_left > 0) ? -1.0f : 1.0f;
                newHeights[i] += transferAmount * direction;
                newHeights[i - neighborhood] -= transferAmount * direction;
                modified = true; 
            }

            if (std::abs(d_right) > threshold) {
                float direction = (d_right > 0) ? -1.0f : 1.0f;
                newHeights[i] += transferAmount * direction;
                newHeights[i + neighborhood] -= transferAmount * direction;
                modified = true; 
            }
        }

        if (modified) {
            terrainHeights = newHeights;
            shouldUpdateTerrainDayMesh = true;
            shouldUpdateTerrainNightMesh = true;
        }
        else {
            isSliding = false; 
        }
    }

    void Tema1::applyGaussianSmoothing(float deltaTime) {
        if (!isSmoothingActive) {
            return;
        }

        gaussianSmoothingTimeElapsed += deltaTime;

        if (gaussianSmoothingTimeElapsed < maxGaussianSmoothingTime) {
            int radius = 2;
            float sigma = 2.0f;
            std::vector<float> newHeights = terrainHeights;

            std::vector<float> gaussianWeights(2 * radius + 1);
            float sum = 0.0f;
            for (int i = -radius; i <= radius; ++i) {
                float weight = std::exp(-(i * i) / (2 * sigma * sigma));
                gaussianWeights[i + radius] = weight;
                sum += weight;
            }

            for (int i = 0; i < gaussianWeights.size(); ++i) {
                gaussianWeights[i] /= sum;
            }

            int start = std::max(radius, lastDeformedStart);
            int end = std::min(static_cast<int>(terrainHeights.size()) - radius, lastDeformedEnd);

            for (int i = start; i <= end; ++i) {
                float smoothedHeight = 0.0f;
                for (int j = -radius; j <= radius; ++j) {
                    smoothedHeight += terrainHeights[i + j] * gaussianWeights[j + radius];
                }
                newHeights[i] = smoothedHeight;
            }

            terrainHeights = newHeights;
            shouldUpdateTerrainDayMesh = true;
            shouldUpdateTerrainNightMesh = true;
        }
        else {
            isSmoothingActive = false;
        }
    }

    void Tema1::LaunchSnowball() {
        float offsetX = -20.0f * cos(armRotation1);
        float offsetY = 70.0f + 20.0f * sin(armRotation1);
        glm::vec2 startPosition = glm::vec2(cx, cy) + glm::vec2(offsetX, offsetY);

        float launchAngle = armRotation1 + glm::pi<float>() / 2;
        glm::vec2 startVelocity = glm::vec2(projectileSpeed * cos(launchAngle), projectileSpeed * sin(launchAngle));

        snowballs.push_back({ startPosition, startVelocity, true, true, false });
    }

    void Tema1::LaunchSnowball2() {
        float offsetX = -20.0f * cos(armRotation2);
        float offsetY = 70.0f + 20.0f * sin(armRotation2);
        glm::vec2 startPosition = glm::vec2(cx2, cy2) + glm::vec2(offsetX, offsetY);

        float launchAngle = armRotation2 + glm::pi<float>() / 2;
        glm::vec2 startVelocity = glm::vec2(projectileSpeed * cos(launchAngle), projectileSpeed * sin(launchAngle));

        snowballs.push_back({ startPosition, startVelocity, true, true, false });
    }



}
