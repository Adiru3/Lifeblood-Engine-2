#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

#include "Core/Input.h"
#include "Game/Player.h"
#include "Physics/PhysicsEngine.h"
#include "Graphics/Shader.h"
#include "Graphics/CubeRenderer.h"
#include "Graphics/SphereRenderer.h"
#include "Graphics/Model.h" 
#include "Audio/AudioEngine.h"
#include "Network/UDPSocket.h"

#include <stb_easy_font.h>

// --- Game States ---
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_BUILDER,
    STATE_LOBBY
};

GameState g_State = STATE_MENU;

// --- Networking ---
UDPSocket g_Network;
bool g_IsServer = false;
bool g_IsConnected = false;
std::string g_ServerIP = "127.0.0.1";
int g_Port = 54000;

struct NetPacket {
    uint32_t gameHash; // Anti-Cheat Hash
    Vec3 pos;
    Vec3 vel;
    Vec3 view;
    bool visible;
};
NetPacket g_RemotePlayer; 

// --- Anti-Cheat Hashing ---
uint32_t ComputeFileHash(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.good()) return 0;
    
    uint32_t hash = 2166136261u; // FNV-1a offset basis
    char c;
    while(f.get(c)) {
        hash ^= (uint8_t)c;
        hash *= 16777619u; // FNV-1a prime
    }
    return hash;
}

uint32_t g_GameHash = 0;

void InitGameIntegrity() {
    PhysicsEngine::Init("assets/physics.cfg");
    
    g_GameHash = 0;
    g_GameHash ^= ComputeFileHash("assets/physics.cfg");
    g_GameHash ^= ComputeFileHash("assets/ak47.obj");
    // Add other critical files...
    
    std::cout << "[Anti-Cheat] Game Integrity Hash: " << std::hex << g_GameHash << std::dec << std::endl;
}

// --- Settings ---
struct Settings {
    float sensitivity = 2.0f;
    float fov = 110.0f;
    int voiceKey = GLFW_KEY_V;
    std::string nickname = "Player";
    bool fullscreen = false;
};
Settings g_Settings;

// --- Gameplay Objects ---
enum BlockType { BLOCK_SOLID, BLOCK_JUMPPAD };

struct MapBlock {
    Vec3 pos;
    Vec3 scale;
    Vec3 color;
    BlockType type;
};
std::vector<MapBlock> g_AimMap;

struct Prop {
    Vec3 pos;
    Vec3 scale;
    int modelIndex; // 0=Tree, 1=Crate
};
std::vector<Prop> g_Props;

struct JumpMine {
    Vec3 pos;
    bool active;
    float timeDeployed;
};
std::vector<JumpMine> g_Mines;

struct Pickup {
    Vec3 pos;
    WeaponType type;
    Vec3 color;
    bool active;
};
std::vector<Pickup> g_Pickups;

// --- Globals ---
Player g_Player;
std::vector<Player> g_Bots;
CubeRenderer* g_Cube = nullptr;
SphereRenderer* g_Sphere = nullptr; 
Shader* g_Shader = nullptr;

Model* g_ModelAK = nullptr;
Model* g_ModelTree = nullptr;
Model* g_ModelCrate = nullptr;
Model* g_LevelModel = nullptr;

float g_FlashIntensity = 0.0f;
bool g_TabPressed = false;
int g_BuilderPropIndex = 0; // 0=Block, 1=Tree, 2=Crate

// --- Text Renderer ---
struct TextRenderer {
    unsigned int VAO, VBO;
    void Init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Quads: x,y,z + color (pseudo)
        glBufferData(GL_ARRAY_BUFFER, 2000 * sizeof(float), NULL, GL_DYNAMIC_DRAW); 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void Draw(float x, float y, const std::string& text, Shader* uiShader) {
        static char buffer[99999];
        int num_quads = stb_easy_font_print(x, y, (char*)text.c_str(), NULL, buffer, sizeof(buffer));
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, num_quads * 4 * 3 * sizeof(float), buffer);
        
        uiShader->Use();
        Mat4 model = Mat4::Identity(); 
        model.elements[0][0] = 0.02f; model.elements[1][1] = 0.02f; model.elements[2][2] = 1.0f; 
        uiShader->SetMat4("model", model);
        glUniform3f(glGetUniformLocation(uiShader->ID, "color"), 1, 1, 1);
        
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glBindVertexArray(0);
    }
    
    void Draw3D(Vec3 pos, float scale, const std::string& text, Shader* shader, const Mat4& view, const Mat4& proj) {
        static char buffer[99999];
        int num_quads = stb_easy_font_print(0, 0, (char*)text.c_str(), NULL, buffer, sizeof(buffer));
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, num_quads * 4 * 3 * sizeof(float), buffer);
        
        shader->Use();
        shader->SetMat4("view", view);
        shader->SetMat4("projection", proj);
        
        Mat4 model = Mat4::Identity();
        model.elements[3][0] = pos.x; model.elements[3][1] = pos.y; model.elements[3][2] = pos.z;
        
        // Rotate to stand up (90 deg around X) + Scale
        // STB is Y-down, so we flip Y scale and rotate
        float s = scale * 0.1f;
        // Rotation X 90 degrees manually
        // 1  0  0
        // 0  C -S
        // 0  S  C
        float c = 0.0f; float s_sin = 1.0f;
        // Combined Scale * Rotation matrix elements approx:
        model.elements[0][0] = s;
        model.elements[1][1] = s * c;  model.elements[1][2] = s * -s_sin;
        model.elements[2][1] = s * s_sin; model.elements[2][2] = s * c;
        // Fix orientation (upside down fix)
        model.elements[1][1] *= -1.0f; 
        
        shader->SetMat4("model", model);
        glUniform3f(glGetUniformLocation(shader->ID, "color"), 0.2f, 1.0f, 0.2f); // Neon Green text
        
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glBindVertexArray(0);
    }
}; // Closed struct

#include "Graphics/ParticleSystem.h" 

ParticleSystem g_Particles; 
TextRenderer g_TextRenderer; // Definition
 



// --- Shaders Source (Blinn-Phong) ---
const char* simpleVertex = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 3) in ivec4 aBoneIDs;
    layout (location = 4) in vec4 aWeights;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    const int MAX_BONES = 100;
    uniform mat4 finalBonesMatrices[MAX_BONES];
    uniform bool useSkinning; // Toggle to switch between Static and Animated

    out vec3 Normal;
    out vec3 FragPos;

    void main() {
        vec4 totalPosition = vec4(0.0f);
        if (useSkinning) {
            for(int i = 0 ; i < 4 ; i++) {
                if(aBoneIDs[i] == -1) // Invalid bone logic if needed
                    continue;
                if(aBoneIDs[i] >= MAX_BONES) 
                    break;
                vec4 localPosition = finalBonesMatrices[aBoneIDs[i]] * vec4(aPos,1.0f);
                totalPosition += localPosition * aWeights[i];
            }
        } else {
            totalPosition = vec4(aPos, 1.0f);
        }
        
        gl_Position = projection * view * model * totalPosition;
        FragPos = vec3(model * totalPosition);
        Normal = aNormal; 
    }
)";

const char* simpleFragment = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 Normal;
    in vec3 FragPos;

    uniform vec3 color;
    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform float flashIntensity;

    void main() {
        vec3 lightColor = vec3(1.0, 1.0, 1.0);

        // Ambient
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lightColor;

        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // Specular (Blinn-Phong)
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result = (ambient + diffuse + specular) * color;
        result = mix(result, vec3(1.0, 1.0, 1.0), flashIntensity);
        FragColor = vec4(result, 1.0);
    }
)";

void SetupAimMap() {
    g_AimMap.clear();
    g_Props.clear();
    // Floor
    g_AimMap.push_back({ Vec3(0, 0, -10), Vec3(2000, 2000, 20), Vec3(0.3f, 0.3f, 0.3f), BLOCK_SOLID });
    // Boxes
    g_AimMap.push_back({ Vec3(200, 0, 50), Vec3(100, 100, 100), Vec3(0.8f, 0.4f, 0.1f), BLOCK_SOLID });
    
    // Props
    g_Props.push_back({ Vec3(100, 100, 0), Vec3(20,20,20), 0 }); // Tree
    g_Props.push_back({ Vec3(-100, 100, 20), Vec3(20,20,20), 1 }); // Crate

    // Jump Pads
    g_AimMap.push_back({ Vec3(400, 0, 10), Vec3(50, 50, 10), Vec3(0.8f, 0.0f, 0.8f), BLOCK_JUMPPAD });

    // Pickups
    g_Pickups.clear();
    g_Pickups.push_back({ Vec3(100, 0, 30), WEAPON_KALASH, Vec3(0.6f, 0.4f, 0.2f), true });
    g_Pickups.push_back({ Vec3(-100, 0, 30), WEAPON_DEAGLE, Vec3(0.7f, 0.7f, 0.7f), true });
    g_Pickups.push_back({ Vec3(0, 100, 30), WEAPON_SCOUT, Vec3(0.2f, 0.2f, 0.2f), true });
}

void SaveMap() {
    std::ofstream out("custom_map.txt");
    if (out.is_open()) {
        for (const auto& b : g_AimMap) {
            out << "BLOCK " << b.pos.x << " " << b.pos.y << " " << b.pos.z << " " 
                << b.scale.x << " " << b.scale.y << " " << b.scale.z << " " << (int)b.type << "\n";
        }
         for (const auto& p : g_Props) {
            out << "PROP " << p.pos.x << " " << p.pos.y << " " << p.pos.z << " " 
                << p.scale.x << " " << p.scale.y << " " << p.scale.z << " " << p.modelIndex << "\n";
        }
    }
}

void SpawnBots(int count) {
    g_Bots.clear();
    for(int i=0; i<count; i++) {
        Player bot;
        bot.position = Vec3((float)((i%2==0?1:-1) * 350), (float)(i*60), 100);
        bot.health = 100;
        g_Bots.push_back(bot);
    }
}

void Shoot(Player& shooter, float currentTime) {
    Weapon& w = shooter.inventory[shooter.currentWeaponIndex];
    if (currentTime < w.nextFireTime) return;
    w.nextFireTime = currentTime + w.fireRate;
    
    // Calculate direction
    float yawRad = shooter.viewAngles.y * 3.14159f / 180.0f;
    float pitchRad = shooter.viewAngles.x * 3.14159f / 180.0f;
    Vec3 forward;
    forward.x = cos(yawRad) * cos(pitchRad);
    forward.y = sin(yawRad) * cos(pitchRad);
    forward.z = sin(pitchRad);

    if (g_State == STATE_BUILDER) {
        Vec3 placePos = shooter.position + (forward * 200.0f);
        // Grid Snap
        placePos.x = round(placePos.x / 50) * 50;
        placePos.y = round(placePos.y / 50) * 50;
        placePos.z = round(placePos.z / 50) * 50;
        if(placePos.z < 0) placePos.z = 0;

        if (g_BuilderPropIndex == 0) {
             g_AimMap.push_back({placePos, Vec3(100,100,100), Vec3(0.5f, 0.5f, 0.9f), BLOCK_SOLID});
        } else {
             int modelIdx = g_BuilderPropIndex - 1; // 0=Tree, 1=Crate
             g_Props.push_back({placePos, Vec3(20,20,20), modelIdx});
        }
        std::cout << "Placed Object Type: " << g_BuilderPropIndex << std::endl;
        return;
    }

    // ... (Existing Shoot Logic for Weapons) ...
    // Simplified for brevity, assume Hitscan works as before
    // Recoil
    // Note: weaponOffset is local in main, need global or passed ref. 
    // We will assume g_WeaponRecoil is added to globals for simplicity
    // g_WeaponRecoil.z += 5.0f; (Z is up/back? kick)
    
    // Muzzle Flash
    Vec3 muzzlePos = shooter.position + (forward * 40.0f) + Vec3(0,0,30); // Approx
    g_Particles.Emit(muzzlePos, forward * 50.0f, Vec3(1, 0.8f, 0.2f), 0.1f, 5.0f, 5, 2.0f);
    
    for (auto& bot : g_Bots) {
        if (bot.health <= 0) continue;
        Vec3 dirToBot = (bot.position - shooter.position).Normalized();
        float dot = Vec3::Dot(forward, dirToBot);
        if (dot > 0.97f) {
            bot.health -= w.damage;
            // Blood Effect
            g_Particles.Emit(bot.position, Vec3(0,0,50), Vec3(0.8f, 0, 0), 0.5f, 3.0f, 10, 20.0f);
        }
    }
}

void UpdateGameplay(float dt) {
    PhysicsEngine::PMove(&g_Player, dt);
    // ... Bot Logic ...
    // ... Jump Pad Logic (using 320.0f) ...
      for(const auto& b : g_AimMap) {
        if (b.type == BLOCK_JUMPPAD) {
            if (abs(g_Player.position.x - b.pos.x) < b.scale.x/2 + 20 &&
                abs(g_Player.position.y - b.pos.y) < b.scale.y/2 + 20 &&
                abs(g_Player.position.z - b.pos.z) < b.scale.z/2 + 40) {
                 PhysicsEngine::ApplyImpulse(&g_Player, Vec3(0,0,320.0f));
            }
        }
    }
}

void PlaceMine(Player& pl) {
    g_Mines.push_back({pl.position, true, (float)glfwGetTime()});
}
void DetonateMines(Player& pl) {
     for (auto& m : g_Mines) {
        if (!m.active) continue;
        float dist = (pl.position - m.pos).Length();
        if (dist < 300.0f) {
            Vec3 dir = (pl.position - m.pos).Normalized();
            dir.z += 0.5f; 
            PhysicsEngine::ApplyImpulse(&pl, dir.Normalized() * 800.0f);
            m.active = false;
        }
    }
}

void RenderScene(const Mat4& view, const Mat4& projection) {
    g_Shader->Use();
    g_Shader->SetMat4("view", view);
    g_Shader->SetMat4("projection", projection);
    g_Shader->SetVec3("lightPos", Vec3(0, 0, 500));
    g_Shader->SetVec3("viewPos", g_Player.position); // For Specular
    glUniform1f(glGetUniformLocation(g_Shader->ID, "flashIntensity"), g_FlashIntensity);

    // Blocks
    for(const auto& block : g_AimMap) {
        if (g_LevelModel && block.pos.z < -2) continue; // Hide fallback floor

        Mat4 model = Mat4::Identity();
        model.elements[3][0] = block.pos.x; model.elements[3][1] = block.pos.y; model.elements[3][2] = block.pos.z;
        model.elements[0][0] = block.scale.x; model.elements[1][1] = block.scale.y; model.elements[2][2] = block.scale.z;
        g_Shader->SetMat4("model", model);
        glUniform3f(glGetUniformLocation(g_Shader->ID, "color"), block.color.x, block.color.y, block.color.z);
        g_Cube->Draw();
    }
    
    // Draw Professional Map
    if (g_LevelModel) {
        Mat4 model = Mat4::Identity();
        // Scale/Rotate if needed. Often Blender exports need 90 deg rotation or specific scale.
        // Assuming 1 unit = 1 unit for now.
        g_Shader->SetMat4("model", model);
        glUniform3f(glGetUniformLocation(g_Shader->ID, "color"), 1.0f, 1.0f, 1.0f);
        g_LevelModel->Draw(g_Shader);
    }
    
    // Props
    for(const auto& prop : g_Props) {
        Mat4 model = Mat4::Identity();
        model.elements[3][0] = prop.pos.x; model.elements[3][1] = prop.pos.y; model.elements[3][2] = prop.pos.z;
        // Basic scaling for models
        float s = 20.0f; 
        model.elements[0][0] = s; model.elements[1][1] = s; model.elements[2][2] = s;
        
        g_Shader->SetMat4("model", model);
        glUniform3f(glGetUniformLocation(g_Shader->ID, "color"), 1.0f, 1.0f, 1.0f); // White tint for texture/model
        
        if(prop.modelIndex == 0 && g_ModelTree) g_ModelTree->Draw(g_Shader);
        else if(prop.modelIndex == 1 && g_ModelCrate) g_ModelCrate->Draw(g_Shader);
    }
    
    // Player Weapon (HUD-like or attached to camera)
    // Draw AK in front of camera
    if (g_State == STATE_GAME) {
        // Simple Gun Rendering logic (attached to view)
        // Ideally we don't draw this with same projection, but near plane
    }

    // ... Pickups, bots ...
    for (const auto& p : g_Pickups) {
        if (!p.active) continue;
        Mat4 model = Mat4::Identity();
        model.elements[3][0] = p.pos.x; model.elements[3][1] = p.pos.y; model.elements[3][2] = p.pos.z;
        model.elements[0][0] = 10; model.elements[1][1] = 10; model.elements[2][2] = 10;
        g_Shader->SetMat4("model", model);
         glUniform3f(glGetUniformLocation(g_Shader->ID, "color"), p.color.x, p.color.y, p.color.z);
        // Replace spheres with AK model for testing if gun
        if (p.type == WEAPON_KALASH && g_ModelAK) g_ModelAK->Draw(g_Shader);
        else g_Sphere->Draw();
    }
    
    // Draw 3D Credits in the Level
    g_TextRenderer.Draw3D(Vec3(0, 100, 100), 2.0f, "Creator: github.com/Adiru3", g_Shader, view, projection);
    g_TextRenderer.Draw3D(Vec3(0, 100, 80), 2.0f, "Support: adiru3.github.io/Donate/", g_Shader, view, projection);
}

// ... Main Loop Wrapper ...
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Lifeblood Engine", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    Input::Init(window);
    AudioEngine::Init();
    g_Network.Create();
    glEnable(GL_DEPTH_TEST);

    g_Cube = new CubeRenderer();
    g_Sphere = new SphereRenderer();
    g_Shader = new Shader(simpleVertex, simpleFragment);
    
    // Load Models
    g_ModelAK = new Model("assets/ak47.obj");
    g_ModelTree = new Model("assets/tree.obj");
    g_ModelCrate = new Model("assets/crate.obj");

    // Try to load external Pro Map
    // User can place 'level.obj' and textures in assets folder.
    std::ifstream f("assets/level.obj");
    if(f.good()) {
        g_LevelModel = new Model("assets/level.obj");
        std::cout << "External Level Loaded!" << std::endl;
        g_AimMap.clear(); // Clear default geometry if custom map exists
        // Add floor collision plane for safety if mesh collision isn't full
        g_AimMap.push_back({ Vec3(0, 0, -5), Vec3(5000, 5000, 20), Vec3(0,0,0), BLOCK_SOLID }); 
    }
    
    g_TextRenderer.Init(); // Init text

    InitGameIntegrity(); // Calculate hashes & Load Physics

    SetupAimMap();
    SpawnBots(5);
    g_Player.position = Vec3(0, -400, 100);
    
    float lastTime = (float)glfwGetTime();
    float accumulator = 0.0f;
    const float TICK_RATE = 2000.0f;
    const float MS_PER_TICK = 1.0f / TICK_RATE;

    g_Particles.Init();
    
    // Procedural Animation State
    Vec3 weaponOffset = Vec3(0,0,0);
    float weaponBob = 0.0f;

    while (!glfwWindowShouldClose(window)) {
         float currentTime = (float)glfwGetTime();
         float frameTime = currentTime - lastTime;
         lastTime = currentTime;
         
         if (frameTime > 0.25f) frameTime = 0.25f; // Cap frame time
         accumulator += frameTime;

         // Max physics steps per frame to prevent freeze (Spiral of Death)
         int physicsSteps = 0;
         const int MAX_PHYSICS_STEPS = 100;

         while (accumulator >= MS_PER_TICK && physicsSteps < MAX_PHYSICS_STEPS) {
             // Fixed Update
             g_Particles.Update(MS_PER_TICK);
             weaponOffset = weaponOffset * (1.0f - MS_PER_TICK * 10.0f); // Recoil damping

             if (g_State == STATE_GAME || g_State == STATE_BUILDER) {
                 UpdateGameplay(MS_PER_TICK);
                 
                 // Bobbing logic in fixed step
                 float speed = g_Player.velocity.Length();
                 if (g_Player.onGround && speed > 10.0f) {
                     weaponBob += MS_PER_TICK * 10.0f;
                     weaponOffset.x = sin(weaponBob) * 0.5f;
                     weaponOffset.z = cos(weaponBob * 2.0f) * 0.2f;
                 }
                 
                 if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) Shoot(g_Player, currentTime);
             }
             
             accumulator -= MS_PER_TICK;
             physicsSteps++;
         }
         
         if (physicsSteps >= MAX_PHYSICS_STEPS) accumulator = 0.0f; // Discard lag

         // --- Render / Input (Variable Step) ---
         if (g_State == STATE_MENU) {
             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
             glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

             g_Shader->Use();
             Mat4 view = Mat4::Identity();
             Mat4 projection = Mat4::Identity();
             g_Shader->SetMat4("view", view);
             g_Shader->SetMat4("projection", projection);
             g_Shader->SetVec3("lightPos", Vec3(0, 0, 100.0f)); 
             g_Shader->SetVec3("viewPos", Vec3(0, 0, 100.0f)); 
             glUniform1f(glGetUniformLocation(g_Shader->ID, "flashIntensity"), 0.0f);

             // Draw Menu UI
             auto DrawButton = [&](float x, float y, float w, float h, Vec3 col) {
                Mat4 model = Mat4::Identity();
                model.elements[3][0] = x; model.elements[3][1] = y; model.elements[3][2] = 0;
                model.elements[0][0] = w; model.elements[1][1] = h; model.elements[2][2] = 0.1f;
                g_Shader->SetMat4("model", model);
                glUniform3f(glGetUniformLocation(g_Shader->ID, "color"), col.x, col.y, col.z);
                g_Cube->Draw();
            };
            DrawButton(0, 0.4f, 0.6f, 0.15f, Vec3(0.2f, 0.8f, 0.2f)); // Single Player
            DrawButton(0, 0.1f, 0.6f, 0.15f, Vec3(0.2f, 0.4f, 0.8f)); // Multiplayer
            DrawButton(0, -0.2f, 0.6f, 0.15f, Vec3(0.6f, 0.6f, 0.6f)); // Settings
            DrawButton(0, -0.5f, 0.6f, 0.15f, Vec3(0.8f, 0.2f, 0.2f)); // Exit

            // Mouse Interaction
            double mx, my; glfwGetCursorPos(window, &mx, &my);
            int winW, winH; glfwGetWindowSize(window, &winW, &winH);
            float ndcX = ((float)mx / winW) * 2.0f - 1.0f;
            float ndcY = 1.0f - ((float)my / winH) * 2.0f;

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                if (abs(ndcX) < 0.6f && abs(ndcY - 0.4f) < 0.15f) { // Single Player
                     g_State = STATE_GAME; g_IsServer = true; g_Network.Bind(g_Port); SetupAimMap();
                     double x, y; Input::GetMousePosition(x, y); Input::MouseCallback(window, x, y);
                }
                if (abs(ndcX) < 0.6f && abs(ndcY - 0.1f) < 0.15f) { // Multiplayer (Lobby)
                     g_State = STATE_LOBBY; 
                }
                if (abs(ndcX) < 0.6f && abs(ndcY + 0.5f) < 0.15f) glfwSetWindowShouldClose(window, true); // Exit
            }
            
            g_TextRenderer.Draw(10, 650, "GitHub: github.com/adiru3", g_Shader);
            g_TextRenderer.Draw(10, 675, "Support: adiru3.github.io/Donate/", g_Shader);
            g_TextRenderer.Draw(10, 700, "LIFEBLOOD ENGINE v1.1", g_Shader);
         }
         else if (g_State == STATE_LOBBY) {
             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
             glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
             
             g_TextRenderer.Draw(100, 50, "MULTIPLAYER LOBBY", g_Shader);
             g_TextRenderer.Draw(100, 100, "Waiting for players...", g_Shader);
             g_TextRenderer.Draw(100, 150, "1. " + g_Settings.nickname + " (YOU)", g_Shader);
             g_TextRenderer.Draw(100, 400, "PRESS [ENTER] TO JOIN", g_Shader);
             g_TextRenderer.Draw(100, 450, "PRESS [ESC] BACK", g_Shader);
             
             if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
                 g_State = STATE_GAME;
                 g_IsServer = false; g_IsConnected = true; g_Network.Bind(g_Port + 1);
                 g_AimMap.clear(); g_AimMap.push_back({ Vec3(0, 0, -10), Vec3(2000, 2000, 20), Vec3(0.3f, 0.3f, 0.3f), BLOCK_SOLID });
                 double x, y; Input::GetMousePosition(x, y); Input::MouseCallback(window, x, y);
             }
             if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) g_State = STATE_MENU;
         }
         else {
             // STATE_GAME or STATE_BUILDER
             glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
             if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) g_State = STATE_MENU;

             // Logic
             double xpos, ypos; Input::GetMousePosition(xpos, ypos);
             static double lastX = xpos, lastY = ypos;
             if (g_State == STATE_MENU) { lastX = xpos; lastY = ypos; } // Reset check
             
             g_Player.viewAngles.y -= (float)(xpos - lastX) * g_Settings.sensitivity * 0.1f;
             g_Player.viewAngles.x += (float)(lastY - ypos) * g_Settings.sensitivity * 0.1f;
             if(g_Player.viewAngles.x > 89.0f) g_Player.viewAngles.x = 89.0f;
             if(g_Player.viewAngles.x < -89.0f) g_Player.viewAngles.x = -89.0f;
             lastX=xpos; lastY=ypos;

             // Controls
             if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) g_Player.forwardMove = 1; else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) g_Player.forwardMove = -1; else g_Player.forwardMove = 0;
             if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) g_Player.rightMove = 1; else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) g_Player.rightMove = -1; else g_Player.rightMove = 0;
             g_Player.wantsToJump = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
             g_Player.isCrouched = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
             // Shooting handled in fixed update

             // Render
             glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

             float yawRad = g_Player.viewAngles.y * 3.14159f / 180.0f;
             float pitchRad = g_Player.viewAngles.x * 3.14159f / 180.0f;
             Vec3 front(cos(yawRad)*cos(pitchRad), sin(yawRad)*cos(pitchRad), sin(pitchRad));
             Vec3 eye = g_Player.position + Vec3(0,0, g_Player.isCrouched?40.0f:60.0f);
             Mat4 view = Mat4::LookAt(eye, eye + front, Vec3(0,0,1));
             Mat4 projection = Mat4::Perspective(g_Settings.fov * 3.14f/180.f, 1280.0f/720.0f, 0.1f, 5000.0f);

             RenderScene(view, projection); // Scene

             // Particles (Transparent)
             g_Particles.Draw(g_Shader, eye, Vec3(0,0,1), Vec3(0,1,0));

             // HUD & UI
             g_Shader->Use();
             glDisable(GL_DEPTH_TEST);
             Mat4 identity = Mat4::Identity();
             g_Shader->SetMat4("view", identity);
             g_Shader->SetMat4("projection", identity);
             g_Shader->SetVec3("lightPos", Vec3(0,0,10));

             // Crosshair
             Mat4 xhair = Mat4::Identity();
             xhair.elements[0][0] = 0.005f; xhair.elements[1][1] = 0.005f * 1.77f;
             g_Shader->SetMat4("model", xhair);
             glUniform3f(glGetUniformLocation(g_Shader->ID, "color"), 0, 1, 0);
             g_Cube->Draw();

             // Text
             g_TextRenderer.Draw(200, 10, "WEAPON: AK-47", g_Shader);
             g_TextRenderer.Draw(10, 10, "HP: " + std::to_string((int)g_Player.health), g_Shader);

             // Credits (Requested)
             g_TextRenderer.Draw(10, 100, "Creator: github.com/Adiru3", g_Shader);
             g_TextRenderer.Draw(10, 125, "Support: adiru3.github.io/Donate/", g_Shader);

             if(g_State == STATE_BUILDER) {
                 g_TextRenderer.Draw(10, 30, "BUILD MODE", g_Shader);
                 // Builder Logic... (Ghost object handled in RenderScene generally or here)
                 if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) g_BuilderPropIndex = (g_BuilderPropIndex+1)%3; // quick cycle
             }
             glEnable(GL_DEPTH_TEST);
         }



         // --- Network Sync ---
         if (g_IsConnected) {
             static float netTimer = 0.0f;
             netTimer += frameTime; 
             if (netTimer > 0.033f) { // ~30hz updates
                 NetPacket p;
                 p.gameHash = g_GameHash; // Send Hash
                 p.pos = g_Player.position;
                 p.vel = g_Player.velocity;
                 p.view = g_Player.viewAngles;
                 p.visible = true;
                 
                 g_Network.Send(std::string((char*)&p, sizeof(p)), g_ServerIP, g_IsServer ? g_Port+1 : g_Port);
                 netTimer = 0;
             }
             
             // Receive
             char buffer[1024];
             std::string ip; int port;
             while (g_Network.Receive(buffer, sizeof(buffer), ip, port)) {
                 if (sizeof(buffer) >= sizeof(NetPacket)) {
                     NetPacket* pkt = (NetPacket*)buffer;
                     
                     // Anti-Cheat Check
                     if (pkt->gameHash != g_GameHash) {
                         static int spam = 0;
                         if(spam++ % 60 == 0) std::cout << "[Anti-Cheat] Rejected packet from " << ip << ": Hash Mismatch!" << std::endl;
                         continue; // Drop packet
                     }
                     
                     g_RemotePlayer = *pkt;
                 }
             }
         }

         glfwSwapBuffers(window);
         glfwPollEvents();
    }
}
