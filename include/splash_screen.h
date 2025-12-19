#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "raylib.h"
#include <string>

class SplashScreen {
private:
    float alpha;
    float loadingProgress;      // 0.0f a 1.0f
    float elapsedTime;
    float totalLoadTime;        // Tiempo total de carga simulada
    bool isFinished;
    bool showLoadingBar;        // Mostrar barra de carga
    Texture2D splashTexture;
    std::string texturePath;

public:
    SplashScreen(const std::string& imagePath = "resources/backgrounds/splash-screen.png");
    ~SplashScreen();
    
    void Update(float deltaTime);
    void Render(int screenWidth, int screenHeight);
    
    bool IsFinished() const { return isFinished; }
    void Reset();
    
    void SetLoadingTime(float totalTime = 3.0f);
    void SetShowLoadingBar(bool show) { showLoadingBar = show; }
};

#endif // SPLASH_SCREEN_H
