#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"
#include <string>

class Engine {
private:
    int screenWidth;
    int screenHeight;
    std::string gameTitle;
    bool isRunning;
    bool isFullscreen;

public:
    Engine(int width = 1280, int height = 720, const std::string& title = "PotatoCake - DDLC Engine", bool fullscreen = false);
    ~Engine();
    
    bool Initialize();
    void Update();
    void Render();
    void Shutdown();
    
    bool ShouldClose() const;
    void SetRunning(bool running);
    void SetFullscreen(bool fullscreen);
    
    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    bool IsFullscreen() const { return isFullscreen; }
};

#endif // ENGINE_H
