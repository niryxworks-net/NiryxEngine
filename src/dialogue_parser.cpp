#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

DialogueParser::DialogueParser(SceneManager* scene) 
    : sceneManager(scene) {
}

DialogueParser::~DialogueParser() {
}

std::string DialogueParser::Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> DialogueParser::Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(Trim(token));
    }
    return tokens;
}

CharacterPosition DialogueParser::ParsePosition(const std::string& pos) {
    std::string lower = pos;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "left" || lower == "izquierda") {
        return CharacterPosition::LEFT;
    } else if (lower == "center" || lower == "centro") {
        return CharacterPosition::CENTER;
    } else if (lower == "right" || lower == "derecha") {
        return CharacterPosition::RIGHT;
    }
    return CharacterPosition::CENTER;
}

ParsedCommand DialogueParser::ParseLine(const std::string& line) {
    ParsedCommand cmd;
    cmd.type = CommandType::NONE;
    
    std::string trimmed = Trim(line);
    
    // Línea vacía
    if (trimmed.empty()) {
        return cmd;
    }
    
    // Comentario
    if (trimmed[0] == '#') {
        cmd.type = CommandType::COMMENT;
        cmd.value1 = trimmed.substr(1);
        return cmd;
    }
    
    // Comando @ (background, music, sfx)
    if (trimmed[0] == '@') {
        size_t spacePos = trimmed.find(' ');
        if (spacePos != std::string::npos) {
            std::string command = trimmed.substr(1, spacePos - 1);
            std::string value = Trim(trimmed.substr(spacePos + 1));
            
            std::transform(command.begin(), command.end(), command.begin(), ::tolower);
            
            if (command == "bg" || command == "background") {
                cmd.type = CommandType::BACKGROUND;
                cmd.value1 = value;
            } else if (command == "music") {
                cmd.type = CommandType::MUSIC;
                cmd.value1 = value;
            } else if (command == "sfx" || command == "sound") {
                cmd.type = CommandType::SFX;
                cmd.value1 = value;
            }
        }
        return cmd;
    }
    
    // Narración (texto entre comillas sin personaje)
    if (trimmed[0] == '"') {
        cmd.type = CommandType::NARRATION;
        // Extraer texto entre comillas
        size_t end = trimmed.find_last_of('"');
        if (end > 0) {
            cmd.value1 = trimmed.substr(1, end - 1);
        }
        return cmd;
    }
    
    // Diálogo con personaje: Nombre: "texto"
    size_t colonPos = trimmed.find(':');
    if (colonPos != std::string::npos) {
        std::string beforeColon = Trim(trimmed.substr(0, colonPos));
        std::string afterColon = Trim(trimmed.substr(colonPos + 1));
        
        // Si después del : hay comillas, es diálogo
        if (!afterColon.empty() && afterColon[0] == '"') {
            cmd.type = CommandType::DIALOGUE;
            cmd.value1 = beforeColon; // Nombre del personaje
            
            // Extraer texto entre comillas
            size_t endQuote = afterColon.find_last_of('"');
            if (endQuote > 0) {
                cmd.value2 = afterColon.substr(1, endQuote - 1);
            }
            return cmd;
        }
    }
    
    // Comando de personaje: Nombre emocion posicion
    std::vector<std::string> parts = Split(trimmed, ' ');
    if (parts.size() >= 2) {
        cmd.type = CommandType::CHARACTER;
        cmd.value1 = parts[0]; // Nombre
        cmd.value2 = parts[1]; // Emoción
        if (parts.size() >= 3) {
            cmd.value3 = parts[2]; // Posición
        } else {
            cmd.value3 = "center"; // Posición por defecto
        }
        return cmd;
    }
    
    return cmd;
}

void DialogueParser::ExecuteCommand(const ParsedCommand& cmd) {
    if (!sceneManager) return;
    
    switch (cmd.type) {
        case CommandType::BACKGROUND:
            sceneManager->SetBackground(cmd.value1);
            break;
            
        case CommandType::MUSIC:
            sceneManager->PlayMusic(cmd.value1);
            break;
            
        case CommandType::SFX:
            sceneManager->PlaySound(cmd.value1);
            break;
            
        case CommandType::CHARACTER:
            sceneManager->ShowCharacter(cmd.value1, cmd.value2, 
                                      ParsePosition(cmd.value3));
            break;
            
        default:
            break;
    }
}

bool DialogueParser::LoadDialogueFile(const std::string& fileName, 
                                      DialogueSystem& dialogue) {
    std::string path = ResourceManager::GetInstance()->GetDialoguePath(fileName);
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open dialogue file: " << path << std::endl;
        return false;
    }
    
    dialogue.Clear();
    std::string currentCharacter = "";
    
    std::string line;
    while (std::getline(file, line)) {
        ParsedCommand cmd = ParseLine(line);
        
        switch (cmd.type) {
            case CommandType::BACKGROUND:
            case CommandType::MUSIC:
            case CommandType::SFX:
            case CommandType::CHARACTER:
                // Ejecutar comandos inmediatamente
                ExecuteCommand(cmd);
                if (cmd.type == CommandType::CHARACTER) {
                    currentCharacter = cmd.value1;
                }
                break;
                
            case CommandType::DIALOGUE:
                // Agregar línea de diálogo
                dialogue.AddLine(cmd.value1, cmd.value2);
                currentCharacter = cmd.value1;
                break;
                
            case CommandType::NARRATION:
                // Agregar narración (sin personaje)
                dialogue.AddLine("", cmd.value1, "neutral", LIGHTGRAY);
                break;
                
            case CommandType::COMMENT:
                // Los comentarios se ignoran (pero podrían usarse para debug)
                break;
                
            case CommandType::NONE:
                // Línea vacía o no reconocida
                break;
        }
    }
    
    file.close();
    return true;
}