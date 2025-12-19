#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"
#include <algorithm>

DialogueSystem::DialogueSystem()
    : currentLineIndex(0), isDisplaying(false), textRevealSpeed(50.0f), 
      displayTimer(0.0f), customFontsLoaded(false) {
}

DialogueSystem::~DialogueSystem() {
    Clear();
}

void DialogueSystem::LoadFonts(const std::string& fontName) {
    dialogueFont = ResourceManager::GetInstance()->LoadFont(fontName);
    nameFont = dialogueFont; // Usar la misma fuente, pero se puede cambiar
    customFontsLoaded = true;
}

void DialogueSystem::AddLine(const std::string& character, const std::string& text,
                            const std::string& emotion, Color color) {
    DialogueLine line;
    line.character = character;
    line.text = text;
    line.emotion = emotion;
    line.textColor = color;
    dialogueLines.push_back(line);
}

void DialogueSystem::AddLines(const std::vector<DialogueLine>& lines) {
    for (const auto& line : lines) {
        dialogueLines.push_back(line);
    }
}

void DialogueSystem::Update(float deltaTime) {
    if (currentLineIndex >= dialogueLines.size()) {
        return;
    }
    
    if (!isDisplaying) {
        isDisplaying = true;
        displayTimer = 0.0f;
        displayedText = "";
    }
    
    // Revelar texto gradualmente
    if (displayedText.length() < dialogueLines[currentLineIndex].text.length()) {
        displayTimer += deltaTime;
        
        size_t charsToReveal = (size_t)(displayTimer * textRevealSpeed);
        displayedText = dialogueLines[currentLineIndex].text.substr(0, charsToReveal);
    }
}

void DialogueSystem::Render(int screenWidth, int screenHeight) {
    if (currentLineIndex >= dialogueLines.size()) {
        return;
    }
    
    const DialogueLine& currentLine = dialogueLines[currentLineIndex];
    
    // Fondo del diálogo
    int dialogueHeight = 200;
    int dialogueY = screenHeight - dialogueHeight;

    DrawRectangle(0, dialogueY, screenWidth, dialogueHeight, (Color){0, 0, 0, 220});
    DrawRectangleLines(0, dialogueY, screenWidth, dialogueHeight, (Color){100, 100, 100, 255});

    // Nombre del personaje (si existe)
    int textX = 30;
    int textY = dialogueY + 15;
    
    if (!currentLine.character.empty()) {
        // Fondo del nombre
        int nameWidth = MeasureText(currentLine.character.c_str(), 28) + 30;
        DrawRectangle(textX - 10, textY - 5, nameWidth, 40, (Color){50, 50, 50, 255});
        DrawRectangleLines(textX - 10, textY - 5, nameWidth, 40, YELLOW);
        
        if (customFontsLoaded) {
            DrawTextEx(nameFont, currentLine.character.c_str(), 
                      (Vector2){(float)textX, (float)textY}, 28, 2, YELLOW);
        } else {
            DrawText(currentLine.character.c_str(), textX, textY, 28, YELLOW);
        }
    }

    // Texto del diálogo con word wrap
    int textStartY = textY + 55;
    int maxWidth = screenWidth - (textX + 50);
    int fontSize = 24;
    float spacing = 2.0f;
    
    Font font = customFontsLoaded ? dialogueFont : GetFontDefault();
    
    // Word wrap mejorado
    auto DrawTextWrapped = [&](const Font& f, const std::string& str, int x, int y, 
                               int maxW, int fs, float sp, Color tint) {
        std::string word;
        std::string line;
        float currentY = (float)y;
        
        for (size_t i = 0; i <= str.size(); ++i) {
            char c = (i < str.size()) ? str[i] : ' ';
            
            if (c == ' ' || c == '\n' || i == str.size()) {
                if (!word.empty()) {
                    std::string testLine = line.empty() ? word : line + " " + word;
                    Vector2 size = MeasureTextEx(f, testLine.c_str(), (float)fs, sp);
                    
                    if (size.x > maxW && !line.empty()) {
                        // Dibujar línea actual y empezar nueva
                        DrawTextEx(f, line.c_str(), (Vector2){(float)x, currentY}, 
                                 (float)fs, sp, tint);
                        currentY += fs + sp + 4;
                        line = word;
                    } else {
                        line = testLine;
                    }
                    word.clear();
                }
                
                if (c == '\n' && !line.empty()) {
                    DrawTextEx(f, line.c_str(), (Vector2){(float)x, currentY}, 
                             (float)fs, sp, tint);
                    currentY += fs + sp + 4;
                    line.clear();
                }
            } else {
                word.push_back(c);
            }
        }
        
        if (!line.empty()) {
            DrawTextEx(font, line.c_str(), (Vector2){(float)x, currentY}, 
                     (float)fs, sp, tint);
        }
    };
    
    DrawTextWrapped(font, displayedText, textX + 5, textStartY, 
                   maxWidth, fontSize, spacing, currentLine.textColor);

    // Indicador de continuar
    if (IsLineFinished()) {
        int indicatorX = screenWidth - 40;
        int indicatorY = screenHeight - 35;
        
        // Animación simple del indicador
        float pulse = (sin(GetTime() * 5.0f) + 1.0f) / 2.0f;
        Color indicatorColor = (Color){255, 255, 100, (unsigned char)(150 + pulse * 105)};
        
        DrawText("▼", indicatorX, indicatorY, 24, indicatorColor);
    }
}

void DialogueSystem::NextLine() {
    if (currentLineIndex < dialogueLines.size() - 1) {
        currentLineIndex++;
        isDisplaying = false;
        displayedText = "";
        displayTimer = 0.0f;
    }
}

void DialogueSystem::PreviousLine() {
    if (currentLineIndex > 0) {
        currentLineIndex--;
        isDisplaying = false;
        displayedText = "";
        displayTimer = 0.0f;
    }
}

void DialogueSystem::SkipToEnd() {
    if (currentLineIndex < dialogueLines.size()) {
        displayedText = dialogueLines[currentLineIndex].text;
    }
}

bool DialogueSystem::IsFinished() const {
    return currentLineIndex >= dialogueLines.size();
}

bool DialogueSystem::IsLineFinished() const {
    if (currentLineIndex >= dialogueLines.size()) {
        return false;
    }
    return displayedText.length() >= dialogueLines[currentLineIndex].text.length();
}

void DialogueSystem::Clear() {
    dialogueLines.clear();
    currentLineIndex = 0;
    isDisplaying = false;
    displayedText = "";
    displayTimer = 0.0f;
}