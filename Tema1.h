#pragma once

#include "components/simple_scene.h"
#include "core/gpu/mesh.h"
#include <glm/glm.hpp>
#include <vector>

namespace m1 {
    class Tema1 : public gfxc::SimpleScene {
    public:
        Tema1();
        ~Tema1();
		float cx, cy, l1, h1, l2, h2, l3, h3, l4, h4, barrelrotation, rotationspeed, maxRotation, minRotation;
        float GetTerrainHeightAt(float x);

        void Init() override;
        struct Projectile {
            glm::vec2 position;
            glm::vec2 velocity;
            bool active;
            bool hitRegistered;
            bool isSnowball;
        };

        struct Snowball {
            glm::vec2 position;
            glm::vec2 velocity;
            bool active;        
            bool isSnowball;   
            bool hitRegistered; 
        };

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void RenderScene();
        void OnInputUpdate(float deltaTime);
		void LaunchProjectile();
        void LaunchProjectile2();
		void OnKeyPress(int key, int mods);
        void Bunny(float posX, float posY);
		//void InitBunny();
		//void RenderTank(float posX, float posY, float angle);
        bool CheckCollision(const glm::vec2& position1, float radius1, const glm::vec2& position2, float radius2);
        void CalculateTrajectory(glm::vec2 startPos, float angle, std::vector<glm::vec2>& points, glm::vec2 offset);
        bool CheckProjectileTerrainCollision(const glm::vec2& projectilePosition);
        void DeformTerrain(float impactX, float radius, float deformationAmount);
        float smoothstep(float edge0, float edge1, float x);
        float clamp(float x, float minVal, float maxVal);
        //void ApplyLandslideEffect(float deltaTime);
        void applyTerrainSlide(float deltaTime);
        void applyGaussianSmoothing(float deltaTime);
        void LaunchSnowball();
		void LaunchSnowball2();

        Mesh* CreateTerrain(const std::string& name, float width, float height);
        Mesh* CreateTerrain2(const std::string& name, float width, float height, glm::vec3 color);
        Mesh* CreateTankBase(const std::string& name);
		Mesh* CreateTankBase2(const std::string& name);
        Mesh* CreateTankBaseWithColor(const std::string& name, glm::vec3 color);
		Mesh* CreateSemiCircle(const std::string& name, float radius, glm::vec3 color);
		Mesh* CreateRectangle(const std::string& name, float width, float height, glm::vec3 color);
        Mesh* CreateCloud(const std::string& name, float width, float height);
		Mesh* CreateCircle(const std::string& name, float radius, glm::vec3 color);
        Mesh* CreateLine(const std::string& name);
        Mesh* CreateRightTriangle(const std::string& name, float width, float height, glm::vec3 color);
        Mesh* CreateSnowyTerrain(const std::string& name, float width, float height);
        Mesh* CreateSnowman(const std::string& name, glm::vec3 baseColor, glm::vec3 detailColor);

        glm::vec3 skyColor;    
        glm::mat3 modelMatrix; 
        float deltaTimeSeconds = 0.0f; 
        float lastFrameTime = 0.0f; 
		float cloudoffset = 0.0f;
        float projectileSpeed = 500.0f;  
        float gravity = -500.0f; 
        bool projectileLaunched = false;
        glm::vec2 projectilePosition;
        glm::vec2 projectileVelocity;
        std::vector<Projectile> projectiles;
		std::vector<Projectile> projectiles2;
		bool isNightMode = false;
		bool isOceanMode = false;
        float cx2, cy2, barrelrotation2;
        float tankRadius;

        float projectileRadius;
        float snowmanRadius;
        float tank1Health;
		float tank2Health;
        float snowman1Health;
        float snowman2Health;
        const float healthBarWidth = 50.0f;     
        const float healthBarHeight = 8.0f;  
        const float healthBarOffsetY = 40.0f;    
        const float healthBarFrameThickness = 2.0f;
        const int maxHealthSegments = 5;        
        float segmentWidth = healthBarWidth / maxHealthSegments;
        bool freeMode = false;

        float timeOfDay = 0.0f; 
        float dayNightCycleDuration = 60.0f;
        std::vector<float> terrainHeights;
        bool shouldUpdateTerrainDayMesh = false;
        bool shouldUpdateTerrainNightMesh = false;
        bool isTank1Active = true; 
        bool isTank2Active = true;
        float elapsedTime;
        const float prag = 50.0f;   
        const float epsilon = 0.005f; 
        const float sigma = 0.5f;
        float slideTimeElapsed = 0.0f;    
        float maxSlideTime = 1.0f;         
        bool isSliding = true;
        float gaussianSmoothingTimeElapsed = 0.0f;  
        float maxGaussianSmoothingTime = 3.0f;     
        bool isSmoothingActive = true;
        int lastDeformedStart;
        int lastDeformedEnd;
        std::vector<float> terrainHeightsOcean;
        bool isWinterMode = false;
        std::vector<Projectile> snowballs;  
        std::vector<Projectile> snowballs2;
        float armRotation1; 
        float armRotation2;
    };
}