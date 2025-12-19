#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"

Character::Character(const std::string& charName)
    : name(charName), currentEmotion("neutral"), position(CharacterPosition::CENTER),
      xPos(0), yPos(0), alpha(1.0f), isVisible(false) {
}

Character::~Character() {
    // Los sprites se liberan en ResourceManager
}

void Character::LoadSprite(const std::string& emotion) {
    if (sprites.find(emotion) == sprites.end()) {
        Texture2D tex = ResourceManager::GetInstance()->LoadCharacterSprite(name, emotion);
        if (tex.id > 0) {
            sprites[emotion] = tex;
        }
    }
}

void Character::SetEmotion(const std::string& emotion) {
    LoadSprite(emotion);
    if (sprites.find(emotion) != sprites.end()) {
        currentEmotion = emotion;
    }
}

void Character::SetPosition(CharacterPosition pos) {
    position = pos;
}

void Character::SetPosition(float x, float y) {
    xPos = x;
    yPos = y;
    position = CharacterPosition::OFFSCREEN; // Custom position
}

void Character::SetAlpha(float a) {
    alpha = a;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
}

void Character::Show() {
    isVisible = true;
}

void Character::Hide() {
    isVisible = false;
}

void Character::Render(int screenWidth, int screenHeight) {
    if (!isVisible || sprites.find(currentEmotion) == sprites.end()) {
        return;
    }
    
    Texture2D sprite = sprites[currentEmotion];
    if (sprite.id == 0) return;
    
    float spriteWidth = (float)sprite.width;
    float spriteHeight = (float)sprite.height;
    
    // Calcular posición según CharacterPosition
    float renderX = xPos;
    float renderY = yPos;
    
    if (position != CharacterPosition::OFFSCREEN) {
        // Ajustar escala si el sprite es muy grande
        float scale = 1.0f;
        float maxHeight = screenHeight * 0.85f; // 85% de la pantalla
        if (spriteHeight > maxHeight) {
            scale = maxHeight / spriteHeight;
        }
        
        spriteWidth *= scale;
        spriteHeight *= scale;
        
        // Posicionar en la parte inferior de la pantalla
        renderY = screenHeight - spriteHeight;
        
        switch (position) {
            case CharacterPosition::LEFT:
                renderX = screenWidth * 0.15f - spriteWidth / 2;
                break;
            case CharacterPosition::CENTER:
                renderX = screenWidth * 0.5f - spriteWidth / 2;
                break;
            case CharacterPosition::RIGHT:
                renderX = screenWidth * 0.85f - spriteWidth / 2;
                break;
            default:
                break;
        }
        
        // Renderizar con escala y alpha
        Rectangle source = { 0, 0, (float)sprite.width, (float)sprite.height };
        Rectangle dest = { renderX, renderY, spriteWidth, spriteHeight };
        Vector2 origin = { 0, 0 };
        
        DrawTexturePro(sprite, source, dest, origin, 0.0f, 
                      Fade(WHITE, alpha));
    } else {
        // Posición custom
        DrawTexture(sprite, (int)renderX, (int)renderY, Fade(WHITE, alpha));
    }
}