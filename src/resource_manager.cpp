#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"
#include <iostream>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager() 
    : resourcePath("resources/"), currentLanguage("spa-spa") {
}

ResourceManager::~ResourceManager() {
    UnloadAll();
}

ResourceManager* ResourceManager::GetInstance() {
    if (instance == nullptr) {
        instance = new ResourceManager();
    }
    return instance;
}

void ResourceManager::Destroy() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void ResourceManager::Initialize(const std::string& resPath) {
    resourcePath = resPath;
    if (resourcePath.back() != '/') {
        resourcePath += '/';
    }
}

void ResourceManager::SetLanguage(const std::string& lang) {
    currentLanguage = lang;
}

Texture2D ResourceManager::LoadCharacterSprite(const std::string& character, 
                                               const std::string& emotion) {
    std::string key = character + "_" + emotion;
    
    // Verificar si ya está cargado
    if (textures.find(key) != textures.end()) {
        return textures[key];
    }
    
    // Cargar sprite
    std::string path = resourcePath + "characters/" + character + "/" + emotion + ".png";
    
    if (FileExists(path.c_str())) {
        Texture2D tex = LoadTexture(path.c_str());
        textures[key] = tex;
        return tex;
    } else {
        std::cerr << "Warning: Character sprite not found: " << path << std::endl;
        // Retornar textura vacía
        Texture2D empty = { 0 };
        return empty;
    }
}

Texture2D ResourceManager::LoadBackground(const std::string& bgName) {
    std::string key = "bg_" + bgName;
    
    if (textures.find(key) != textures.end()) {
        return textures[key];
    }
    
    std::string path = resourcePath + "backgrounds/" + bgName + ".png";
    
    if (FileExists(path.c_str())) {
        Texture2D tex = LoadTexture(path.c_str());
        textures[key] = tex;
        return tex;
    } else {
        std::cerr << "Warning: Background not found: " << path << std::endl;
        Texture2D empty = { 0 };
        return empty;
    }
}

Texture2D ResourceManager::LoadCG(const std::string& cgName) {
    std::string key = "cg_" + cgName;
    
    if (textures.find(key) != textures.end()) {
        return textures[key];
    }
    
    std::string path = resourcePath + "cgs/" + cgName + ".png";
    
    if (FileExists(path.c_str())) {
        Texture2D tex = LoadTexture(path.c_str());
        textures[key] = tex;
        return tex;
    } else {
        std::cerr << "Warning: CG not found: " << path << std::endl;
        Texture2D empty = { 0 };
        return empty;
    }
}

Music ResourceManager::LoadMusic(const std::string& musicName) {
    std::string key = "music_" + musicName;
    
    if (musicTracks.find(key) != musicTracks.end()) {
        return musicTracks[key];
    }
    
    std::string path = resourcePath + "music/" + musicName + ".ogg";
    // Intentar también .mp3
    if (!FileExists(path.c_str())) {
        path = resourcePath + "music/" + musicName + ".mp3";
    }
    
    if (FileExists(path.c_str())) {
        Music music = LoadMusicStream(path.c_str());
        musicTracks[key] = music;
        return music;
    } else {
        std::cerr << "Warning: Music not found: " << musicName << std::endl;
        Music empty = { 0 };
        return empty;
    }
}

Sound ResourceManager::LoadSound(const std::string& soundName) {
    std::string key = "sfx_" + soundName;
    
    if (sounds.find(key) != sounds.end()) {
        return sounds[key];
    }
    
    std::string path = resourcePath + "sfx/" + soundName + ".wav";
    // Intentar también .ogg
    if (!FileExists(path.c_str())) {
        path = resourcePath + "sfx/" + soundName + ".ogg";
    }
    
    if (FileExists(path.c_str())) {
        Sound snd = LoadSound(path.c_str());
        sounds[key] = snd;
        return snd;
    } else {
        std::cerr << "Warning: Sound not found: " << soundName << std::endl;
        Sound empty = { 0 };
        return empty;
    }
}

Font ResourceManager::LoadFont(const std::string& fontName) {
    if (fonts.find(fontName) != fonts.end()) {
        return fonts[fontName];
    }
    
    std::string path = resourcePath + "fonts/" + fontName;
    
    if (FileExists(path.c_str())) {
        Font font = LoadFontEx(path.c_str(), 32, 0, 0);
        fonts[fontName] = font;
        return font;
    } else {
        std::cerr << "Warning: Font not found: " << path << std::endl;
        return GetFontDefault();
    }
}

Texture2D ResourceManager::GetTexture(const std::string& key) {
    if (textures.find(key) != textures.end()) {
        return textures[key];
    }
    Texture2D empty = { 0 };
    return empty;
}

Music ResourceManager::GetMusic(const std::string& key) {
    if (musicTracks.find(key) != musicTracks.end()) {
        return musicTracks[key];
    }
    Music empty = { 0 };
    return empty;
}

Sound ResourceManager::GetSound(const std::string& key) {
    if (sounds.find(key) != sounds.end()) {
        return sounds[key];
    }
    Sound empty = { 0 };
    return empty;
}

Font ResourceManager::GetFont(const std::string& key) {
    if (fonts.find(key) != fonts.end()) {
        return fonts[key];
    }
    return GetFontDefault();
}

void ResourceManager::UnloadTexture(const std::string& key) {
    if (textures.find(key) != textures.end()) {
        UnloadTexture(textures[key]);
        textures.erase(key);
    }
}

void ResourceManager::UnloadMusic(const std::string& key) {
    if (musicTracks.find(key) != musicTracks.end()) {
        UnloadMusicStream(musicTracks[key]);
        musicTracks.erase(key);
    }
}

void ResourceManager::UnloadSound(const std::string& key) {
    if (sounds.find(key) != sounds.end()) {
        UnloadSound(sounds[key]);
        sounds.erase(key);
    }
}

void ResourceManager::UnloadAll() {
    // Unload textures
    for (auto& pair : textures) {
        UnloadTexture(pair.second);
    }
    textures.clear();
    
    // Unload music
    for (auto& pair : musicTracks) {
        UnloadMusicStream(pair.second);
    }
    musicTracks.clear();
    
    // Unload sounds
    for (auto& pair : sounds) {
        UnloadSound(pair.second);
    }
    sounds.clear();
    
    // Unload fonts
    for (auto& pair : fonts) {
        if (pair.second.texture.id != GetFontDefault().texture.id) {
            UnloadFont(pair.second);
        }
    }
    fonts.clear();
}

std::string ResourceManager::GetDialoguePath(const std::string& fileName) {
    return resourcePath + "dialogues/" + currentLanguage + "/" + fileName;
}