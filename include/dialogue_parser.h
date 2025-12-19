#ifndef DIALOGUE_PARSER_H
#define DIALOGUE_PARSER_H

#include "dialogue_system.h"
#include "scene_manager.h"
#include <string>
#include <vector>

enum class CommandType {
    NONE,
    BACKGROUND,    // @bg
    MUSIC,        // @music
    SFX,          // @sfx
    CHARACTER,    // Nombre emocion posicion
    DIALOGUE,     // Nombre: "texto"
    NARRATION,    // "texto" sin nombre
    COMMENT       // # comentario
};

struct ParsedCommand {
    CommandType type;
    std::string value1;  // nombre personaje, comando, etc
    std::string value2;  // emocion, texto, etc
    std::string value3;  // posicion, etc
};

class DialogueParser {
private:
    SceneManager* sceneManager;
    
    std::string Trim(const std::string& str);
    std::vector<std::string> Split(const std::string& str, char delimiter);
    ParsedCommand ParseLine(const std::string& line);
    CharacterPosition ParsePosition(const std::string& pos);

public:
    DialogueParser(SceneManager* scene);
    ~DialogueParser();
    
    // Cargar archivo de diálogo
    bool LoadDialogueFile(const std::string& fileName, DialogueSystem& dialogue);
    
    // Ejecutar comando inmediatamente (para comandos @)
    void ExecuteCommand(const ParsedCommand& cmd);
};

#endif // DIALOGUE_PARSER_H