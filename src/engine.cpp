#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"

Engine::Engine(int width, int height, const std::string& title, bool fullscreen)
    : screenWidth(width), screenHeight(height), gameTitle(title), isRunning(true), isFullscreen(fullscreen) {
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize() {
    // Configurar flags antes de inicializar la ventana
    unsigned int flags = FLAG_WINDOW_RESIZABLE;
    if (isFullscreen) {
        flags |= FLAG_FULLSCREEN_MODE;
    }
    
    SetConfigFlags(flags);
    InitWindow(screenWidth, screenHeight, gameTitle.c_str());
    SetTargetFPS(60);
    
    // Si está en fullscreen, obtener las dimensiones reales
    if (isFullscreen) {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
    }
    
    return true;
}

void Engine::Update() {
    // Lógica de actualización general del motor
    if (WindowShouldClose()) {
        isRunning = false;
    }
}

void Engine::Render() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // La lógica de renderizado específica va en el main loop
    
    EndDrawing();
}

void Engine::Shutdown() {
    CloseWindow();
}

bool Engine::ShouldClose() const {
    return !isRunning || WindowShouldClose();
}

void Engine::SetRunning(bool running) {
    isRunning = running;
}

void Engine::SetFullscreen(bool fullscreen) {
    if (isFullscreen != fullscreen) {
        isFullscreen = fullscreen;
        ToggleBorderlessWindowed();
    }
}
