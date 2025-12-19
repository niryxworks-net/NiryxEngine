#include "engine.h"
#include "splash_screen.h"
#include "dialogue_system.h"
#include "scene_manager.h"
#include "dialogue_parser.h"
#include "resource_manager.h"

enum GameState {
    STATE_SPLASH,
    STATE_MENU,
    STATE_DIALOGUE,
    STATE_EXIT
};

int main() {
    // Inicializar el motor
    Engine engine(1366, 768, "Niryx Engine 1.0", true);
    if (!engine.Initialize()) {
        return -1;
    }
    
    // Inicializar el ResourceManager
    ResourceManager::GetInstance()->Initialize("resources/");
    ResourceManager::GetInstance()->SetLanguage("spa-spa"); // Español español
    
    // Inicializar sistemas
    SplashScreen splash("resources/backgrounds/splash-screen.png");
    splash.SetLoadingTime(3.0f);
    splash.SetShowLoadingBar(true);
    
    SceneManager sceneManager;
    DialogueSystem dialogue;
    DialogueParser parser(&sceneManager);
    
    // Cargar fuente personalizada
    dialogue.LoadFonts("GenJyuuGothicX-Bold.ttf");
    dialogue.SetTextSpeed(40.0f); // Velocidad de texto (caracteres por segundo)
    
    GameState currentState = STATE_SPLASH;
    float deltaTime = 0.0f;
    
    // Main loop
    while (!engine.ShouldClose()) {
        deltaTime = GetFrameTime();
        
        // Update
        switch (currentState) {
            case STATE_SPLASH:
                splash.Update(deltaTime);
                if (splash.IsFinished()) {
                    currentState = STATE_DIALOGUE;
                    
                    // Cargar el capítulo 0 desde archivo
                    if (!parser.LoadDialogueFile("ch0.txt", dialogue)) {
                        // Si no se puede cargar, usar diálogos de ejemplo
                        dialogue.AddLine("Sistema", "No se pudo cargar ch0.txt");
                        dialogue.AddLine("Sistema", "Verifica que el archivo esté en resources/dialogues/spa-spa/");
                    }
                }
                break;
                
            case STATE_DIALOGUE:
                sceneManager.Update(deltaTime);
                dialogue.Update(deltaTime);
                
                // Controles
                if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (dialogue.IsLineFinished()) {
                        dialogue.NextLine();
                    } else {
                        dialogue.SkipToEnd();
                    }
                }
                
                // Retroceder (tecla BACKSPACE o botón derecho del mouse)
                if (IsKeyPressed(KEY_BACKSPACE) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    dialogue.PreviousLine();
                }
                
                // Avance automático rápido (mantener CTRL)
                if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
                    if (dialogue.IsLineFinished()) {
                        dialogue.NextLine();
                    }
                }
                
                if (dialogue.IsFinished()) {
                    currentState = STATE_EXIT;
                }
                break;
                
            case STATE_MENU:
                // Lógica del menú aquí
                break;
                
            case STATE_EXIT:
                engine.SetRunning(false);
                break;
        }
        
        // Render
        BeginDrawing();
        
        if (currentState == STATE_SPLASH) {
            splash.Render(engine.GetScreenWidth(), engine.GetScreenHeight());
        } else {
            // Renderizar escena
            sceneManager.RenderBackground(engine.GetScreenWidth(), engine.GetScreenHeight());
            sceneManager.RenderCharacters(engine.GetScreenWidth(), engine.GetScreenHeight());
            
            // Renderizar diálogo
            dialogue.Render(engine.GetScreenWidth(), engine.GetScreenHeight());
            
            // Mostrar controles (debug)
            DrawText("ESPACIO/CLICK: Continuar | BACKSPACE/CLICK-DER: Atrás | CTRL: Avance rápido | ESC: Salir", 
                    10, 10, 16, WHITE);
            
            // Mostrar info de debug
            if (IsKeyDown(KEY_F1)) {
                DrawText(TextFormat("Línea: %d/%d", 
                        (int)dialogue.GetCurrentLineIndex() + 1, 
                        (int)dialogue.GetTotalLines()), 
                        10, 40, 16, YELLOW);
            }
        }
        
        EndDrawing();
        
        // Salir con ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            engine.SetRunning(false);
        }
    }
    
    // Cleanup
    ResourceManager::Destroy();
    engine.Shutdown();
    return 0;
}