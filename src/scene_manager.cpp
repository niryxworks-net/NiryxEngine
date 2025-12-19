#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"

SceneManager::SceneManager()
    : musicVolume(0.5f), transitionAlpha(0.0f), isTransitioning(false) {
    currentBackground.id = 0;
    currentMusic.ctxType = 0;
}

SceneManager::~SceneManager() {
    if (currentMusic.ctxType != 0) {
        StopMusicStream(currentMusic);
    }
}

void SceneManager::SetBackground(const std::string& bgName) {
    currentBgName = bgName;
    currentBackground = ResourceManager::GetInstance()->LoadBackground(bgName);
}

void SceneManager::ClearBackground() {
    currentBackground.id = 0;
    currentBgName = "";
}

void SceneManager::PlayMusic(const std::string& musicName, bool loop) {
    // Detener música actual si existe
    if (currentMusic.ctxType != 0 && currentMusicName != musicName) {
        StopMusicStream(currentMusic);
    }
    
    currentMusicName = musicName;
    currentMusic = ResourceManager::GetInstance()->LoadMusic(musicName);
    
    if (currentMusic.ctxType != 0) {
        currentMusic.looping = loop;
        SetMusicVolume(currentMusic, musicVolume);
        PlayMusicStream(currentMusic);
    }
}

void SceneManager::StopMusic() {
    if (currentMusic.ctxType != 0) {
        StopMusicStream(currentMusic);
    }
    currentMusicName = "";
}

void SceneManager::SetMusicVolume(float volume) {
    musicVolume = volume;
    if (currentMusic.ctxType != 0) {
        SetMusicVolume(currentMusic, musicVolume);
    }
}

void SceneManager::PlaySound(const std::string& soundName) {
    Sound sfx = ResourceManager::GetInstance()->LoadSound(soundName);
    if (sfx.frameCount > 0) {
        PlaySound(sfx);
    }
}

Character* SceneManager::GetCharacter(const std::string& name) {
    if (characters.find(name) == characters.end()) {
        characters[name] = std::make_shared<Character>(name);
    }
    return characters[name].get();
}

void SceneManager::ShowCharacter(const std::string& name, const std::string& emotion,
                                CharacterPosition pos) {
    Character* character = GetCharacter(name);
    character->SetEmotion(emotion);
    character->SetPosition(pos);
    character->Show();
}

void SceneManager::HideCharacter(const std::string& name) {
    if (characters.find(name) != characters.end()) {
        characters[name]->Hide();
    }
}

void SceneManager::ClearAllCharacters() {
    for (auto& pair : characters) {
        pair.second->Hide();
    }
}

void SceneManager::Update(float deltaTime) {
    // Actualizar música
    if (currentMusic.ctxType != 0) {
        UpdateMusicStream(currentMusic);
    }
    
    // Actualizar transiciones
    if (isTransitioning) {
        transitionAlpha += deltaTime * 2.0f;
        if (transitionAlpha >= 1.0f) {
            transitionAlpha = 1.0f;
            isTransitioning = false;
        }
    }
}

void SceneManager::RenderBackground(int screenWidth, int screenHeight) {
    if (currentBackground.id > 0) {
        // Escalar el fondo para cubrir toda la pantalla
        float scaleX = (float)screenWidth / currentBackground.width;
        float scaleY = (float)screenHeight / currentBackground.height;
        float scale = (scaleX > scaleY) ? scaleX : scaleY;
        
        Rectangle source = { 0, 0, (float)currentBackground.width, 
                           (float)currentBackground.height };
        Rectangle dest = { 0, 0, currentBackground.width * scale, 
                         currentBackground.height * scale };
        
        // Centrar si es necesario
        if (dest.width > screenWidth) {
            dest.x = -(dest.width - screenWidth) / 2;
        }
        if (dest.height > screenHeight) {
            dest.y = -(dest.height - screenHeight) / 2;
        }
        
        Vector2 origin = { 0, 0 };
        DrawTexturePro(currentBackground, source, dest, origin, 0.0f, WHITE);
    } else {
        // Fondo negro si no hay imagen
        ClearBackground(BLACK);
    }
}

void SceneManager::RenderCharacters(int screenWidth, int screenHeight) {
    // Renderizar personajes en orden: left, center, right
    std::vector<CharacterPosition> renderOrder = {
        CharacterPosition::LEFT,
        CharacterPosition::CENTER,
        CharacterPosition::RIGHT
    };
    
    for (CharacterPosition pos : renderOrder) {
        for (auto& pair : characters) {
            if (pair.second->GetPosition() == pos) {
                pair.second->Render(screenWidth, screenHeight);
            }
        }
    }
}

void SceneManager::StartTransition() {
    isTransitioning = true;
    transitionAlpha = 0.0f;
}