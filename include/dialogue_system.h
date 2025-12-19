#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include "raylib.h"
#include <string>
#include <vector>
#include <memory>

struct DialogueLine {
    std::string character;
    std::string text;
    std::string emotion;
    Color textColor;
};

class DialogueSystem {
private:
    std::vector<DialogueLine> dialogueLines;
    size_t currentLineIndex;
    bool isDisplaying;
    float textRevealSpeed;
    std::string displayedText;
    float displayTimer;
    
    Font dialogueFont;
    Font nameFont;
    bool customFontsLoaded;

public:
    DialogueSystem();
    ~DialogueSystem();
    
    void LoadFonts(const std::string& fontName);
    
    void AddLine(const std::string& character, const std::string& text, 
                 const std::string& emotion = "neutral", Color color = WHITE);
    void AddLines(const std::vector<DialogueLine>& lines);
    
    void Update(float deltaTime);
    void Render(int screenWidth, int screenHeight);
    
    void NextLine();
    void PreviousLine();
    void SkipToEnd();
    
    bool IsFinished() const;
    bool IsLineFinished() const;
    
    void Clear();
    void SetTextSpeed(float speed) { textRevealSpeed = speed; }
    
    size_t GetTotalLines() const { return dialogueLines.size(); }
    size_t GetCurrentLineIndex() const { return currentLineIndex; }
};

#endif // DIALOGUE_SYSTEM_H