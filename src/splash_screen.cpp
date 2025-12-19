#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"

SplashScreen::SplashScreen(const std::string& imagePath)
    : alpha(255.0f), loadingProgress(0.0f), elapsedTime(0.0f), 
      totalLoadTime(3.0f), isFinished(false), showLoadingBar(true),
      texturePath(imagePath) {
    
    // Cargar la imagen si existe
    if (FileExists(imagePath.c_str())) {
        splashTexture = LoadTexture(imagePath.c_str());
    }
}

void SplashScreen::Update(float deltaTime) {
    if (isFinished) return;
    
    elapsedTime += deltaTime;
    loadingProgress = elapsedTime / totalLoadTime;
    
    // Suavizar la barra con easing (Smooth Step)
    if (loadingProgress > 1.0f) {
        loadingProgress = 1.0f;
        isFinished = true;
    }
}

void SplashScreen::Render(int screenWidth, int screenHeight) {
    if (isFinished) return;
    
    BeginDrawing();
    ClearBackground(WHITE);
    
    // Use render size to avoid issues when raylib downscales the final output
    float rw = (float)GetRenderWidth();
    float rh = (float)GetRenderHeight();

    // Renderizar imagen centrada manteniendo proporción usando el tamaño de render real
    if (splashTexture.id > 0) {
        float texW = (float)splashTexture.width;
        float texH = (float)splashTexture.height;

        float maxW = rw * 0.85f;
        float maxH = rh * 0.7f;
        float scale = 1.0f;
        if (texW > maxW || texH > maxH) {
            float sx = maxW / texW;
            float sy = maxH / texH;
            scale = (sx < sy) ? sx : sy;
        }

        Rectangle src = { 0.0f, 0.0f, texW, texH };
        Rectangle dst = { (rw - texW * scale) / 2.0f, (rh - texH * scale) / 2.0f - 40.0f,
                          texW * scale, texH * scale };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(splashTexture, src, dst, origin, 0.0f, WHITE);
    }

    if (showLoadingBar) {
        // Make the bar bigger and centered relative to render size
        int barWidth = (int)(rw * 0.6f);
        int barHeight = (int)(rh * 0.04f); // ~4% of render height
        int barX = (int)((rw - barWidth) / 2.0f);
        int barY = (int)(rh - (rh * 0.12f));

        // Background and border
        DrawRectangle(barX - 4, barY - 4, barWidth + 8, barHeight + 8, (Color){60, 60, 60, 255});
        DrawRectangleLines(barX - 4, barY - 4, barWidth + 8, barHeight + 8, (Color){200, 200, 200, 255});

        // Bar background (dark)
        DrawRectangle(barX, barY, barWidth, barHeight, (Color){200, 200, 200, 255});

        // Fill (solid red)
        int fillWidth = (int)(barWidth * loadingProgress);
        if (fillWidth > 0) {
            DrawRectangle(barX, barY, fillWidth, barHeight, (Color){220, 20, 20, 255});
        }

        // No percentage text; only optional static caption
        const char* loadingText = ""; // empty
        if (loadingText[0] != '\0') {
            int textW = MeasureText(loadingText, 20);
            DrawText(loadingText, barX + (barWidth - textW) / 2, barY + barHeight + 8, 20, (Color){80, 80, 80, 255});
        }
    }
    
    EndDrawing();
}

void SplashScreen::Reset() {
    elapsedTime = 0.0f;
    loadingProgress = 0.0f;
    isFinished = false;
    alpha = 255.0f;
}

void SplashScreen::SetLoadingTime(float totalTime) {
    totalLoadTime = totalTime;
}

SplashScreen::~SplashScreen() {
    if (splashTexture.id > 0) {
        UnloadTexture(splashTexture);
    }
}
