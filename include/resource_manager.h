#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <memory>

class ResourceManager {
private:
    // Cache de recursos
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Music> musicTracks;
    std::unordered_map<std::string, Sound> sounds;
    std::unordered_map<std::string, Font> fonts;
    
    std::string resourcePath;
    std::string currentLanguage;
    
    // Singleton
    static ResourceManager* instance;
    ResourceManager();

public:
    ~ResourceManager();
    
    static ResourceManager* GetInstance();
    static void Destroy();
    
    // Inicialización
    void Initialize(const std::string& resPath = "resources/");
    void SetLanguage(const std::string& lang);
    std::string GetLanguage() const { return currentLanguage; }
    
    // Carga de recursos
    Texture2D LoadCharacterSprite(const std::string& character, const std::string& emotion);
    Texture2D LoadBackground(const std::string& bgName);
    Texture2D LoadCG(const std::string& cgName);
    Music LoadMusic(const std::string& musicName);
    Sound LoadSound(const std::string& soundName);
    Font LoadFont(const std::string& fontName);
    
    // Obtener recursos cargados
    Texture2D GetTexture(const std::string& key);
    Music GetMusic(const std::string& key);
    Sound GetSound(const std::string& key);
    Font GetFont(const std::string& key);
    
    // Liberar recursos
    void UnloadTexture(const std::string& key);
    void UnloadMusic(const std::string& key);
    void UnloadSound(const std::string& key);
    void UnloadAll();
    
    // Rutas de diálogos
    std::string GetDialoguePath(const std::string& fileName);
};

#endif // RESOURCE_MANAGER_H