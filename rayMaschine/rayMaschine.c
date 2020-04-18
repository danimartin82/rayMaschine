/**********************************************************************************************
*
*   raylib 32x32 game/demo competition
*
*   Competition consist in developing a videogame in a 32x32 pixels screen size.
*
*   RULES:
*
*     1) Use only raylib (and included libraries), no external libraries allowed
*     2) The submission should consist of just one source file
*     3) Render your game/demo to a 32x32 pixels render texture, 
*        show what you could do with a 32x32 RGB LED matrix!
*     4) No external resources, you CAN only create them programmatically,
*     5) Game/demo can be 2D or 3D, choose wisely
*     5) Shaders (if used) should be included in the source as string (char *) 
*        and loaded with LoadShaderCode()
*     6) Code must compile and execute in PLATFORM_DESKTOP (Windows, Linux, macOS)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2020 Dani Martin (https://github.com/danimartin82)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/


/*------------------------------------- Includes ---------------------------------------*/
#include "raylib.h"
#include "raymath.h"

/*------------------------------------- Global variables -------------------------------*/

#define max(a, b) ((a)>(b)? (a) : (b))
#define min(a, b) ((a)<(b)? (a) : (b))

const int windowWidth = 512;        // NOTE: Game will be scaled x16
const int windowHeight = 512;
const int gameScreenWidth = 32;
const int gameScreenHeight = 32;

float frequencies[4][16] = 
     {{   41.20,   43.65,   46.25,   49.00,   51.91,   55.00,   58.27,   61.74,   65.41,   69.30,   73.42,   77.78,   82.41,   87.31,   92.50,   98.00},
      {  103.83,  110.00,  116.54,  123.47,  130.81,  138.59,  146.83,  155.56,  164.81,  174.61,  185.00,  196.00,  207.65,  220.00,  233.08,  246.94},
      {  261.63,  277.18,  293.66,	311.13,	 329.63,  349.23,  369.99,	392.00,	 415.30,  440.00,  466.16,  493.88,	 523.25,  554.37,  587.33,  622.25},
      {  659.25,  698.46,  739.99,  783.99,  830.61,  880.00,  932.33,  987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98}};
    
/*
         E1,  F1, F#1,  G1, G#1,  A1, A#1,  B1,  C2, C#2,  D2, D#2,  E2,  F2, F#2,  G2
        G#2,  A2, A#2,  B2,  C3, C#3,  D3, D#3,  E3,  F3, F#3,  G3, G#3,  A3, A#3,  B3
         C4, C#4,  D4, D#4,  E4,  F4, F#4,  G4, G#4,  A4, A#4,  B4,  C5, C#5,  D5, D#5
         E5,  F5, F#5,  G5, G#5,  A5, A#5,  B5,  C6, C#6,  D6, D#6,  E6,  F6, F#6,  G6

*/
#define MAX_SAMPLES               4096
int samplerate = 22050;
AudioStream stream;

int melodiesLevel1[3][3][2] =
{ {{1, 3},{1, 7},{1, 9}},
  {{1,16},{1,13},{1,10}},
  {{1, 2},{1, 6},{1, 5}}
};

int melodiesLevel1Length[] = {3, 3, 3};

int melodiesLevel2[3][9][2] =
{ {{3, 7},{1, 7},{2,10},{2, 7},{2, 5},{4, 3},{2, 2},{0, 0},{0, 0}}, // Seven Nation Army
  {{1,14},{1,13},{1,14},{1,13},{1,14},{1, 9},{1,12},{1,10},{3, 7}}, // Fur Elisa
  {{2, 4},{2, 7},{4, 9},{2, 4},{2, 7},{1, 10},{1,9},{0, 0},{0, 0}}  // Smoke on the water
}; 

int melodiesLevel2Length[] = {7, 9, 7};


int melodiesLevel3[3][28][2] =
{ {{1, 3},{1, 3},{2, 3},{1, 3},{2, 5},{2, 0},{3, 3},{2, 3},{1, 8},{3, 10},{1,12},{1,12},{4,12},{1,10},{2,10},{1, 8},{4, 8},{1, 3},{1,12},{3,12},{2,13},{1,12},{2,12},{4,10},{1,12},{1,10},{2,10},{4, 8}}, // Let it be
  {{1, 3},{1, 3},{2, 3},{1, 3},{2, 5},{2, 0},{3, 3},{2, 3},{1, 8},{3, 10},{1,12},{1,12},{4,12},{1,10},{2,10},{1, 8},{4, 8},{1, 3},{1,12},{3,12},{2,13},{1,12},{2,12},{4,10},{1,12},{1,10},{2,10},{4, 8}},
  {{1, 3},{1, 3},{2, 3},{1, 3},{2, 5},{2, 0},{3, 3},{2, 3},{1, 8},{3, 10},{1,12},{1,12},{4,12},{1,10},{2,10},{1, 8},{4, 8},{1, 3},{1,12},{3,12},{2,13},{1,12},{2,12},{4,10},{1,12},{1,10},{2,10},{4, 8}}
};

int melodiesLevel3Length[] = {28, 28, 28};
/*
#define MELODY_3_LENGTH 70
int MELODY_3[MELODY_3_LENGTH][2] = { {1,14},{1,13},{1,14},{1,13},{1,14},{1,9},{1,12},{1,10},{3,7},{1,2},{1,2},{1,7},{3,9},{1,2},{1,6},{1,9},{3,10},{1,2},{1,14},{1,13},{1,14},{1,13},{1,14},{1,9},{1,12},
{1,10},{3,7},{1,2},{1,2},{1,7},{1,9},{1,2},{1,6},{1,12},{1,10},{1,9},{3,7},{1,9},{1,10},{1,12},{3,14},{1,5},{1,15},{1,14},{3,12},{1,3},{1,14},{1,12},{3,10},{1,2},
{1,12},{1,10},{2,9},{1,7},{1,2},{3,14},{1,14},{3,14},{1,13},{3,14},{1,13},{1,14},{1,13},{1,14},{1,13},{1,14},{1,9},{1,12},{1,10},{4,7} };
*/

#define NUMBER_OF_LEVELS 3


// Game melodies
int gameMelodies[3][9][2] =
{ {{1, 3},{1, 7},{1,10},{1, 3},{1, 7},{1,10},{1, 3},{1, 7},{1,10}},  // GAME START
  {{1, 0},{1, 4},{1, 7},{2,12},{1, 7},{2,12},{0, 0},{0, 0},{0, 0}},  // USER WINS
  {{3, 4},{3 ,3},{3, 2},{3, 1},{0, 0},{0, 0},{0 ,0},{0, 0},{0, 0}}   // USER LOOSE
}; 

int gameMelodiesLength[] = { 9, 6, 4 };
#define MELODY_GAME_START 0
#define MELODY_USER_WINS  1
#define MELODY_USER_LOOSE 2

int currentMelody[70][2] = { 0 };
int currentMelodyLength = 0;
int userMelody[70] = { 0 };

int audioTimer = 0;
int screenTimer = 0;
int gameActualNote = 100;
int gameMaxNote = 0;
int userActualNote = 0;

int userTotalScore = 0;
int gameLevel = 1;

bool userDrawingDone = false;
bool phaseCompleted = false;

enum gameStatus{
    GAME_WELCOME_SCREEN,
    GAME_FREE_MODE,
    GAME_START_SCREEN,
    GAME_PLAYING,
    GAME_LOOSE_SCREEN,
    GAME_WIN_SCREEN,
} gameStatus;

enum phaseStatus {
    PHASE_START,
    PHASE_MELODY_START,
    PHASE_MELODY_PLAYING,
    PHASE_MELODY_STOP,
    PHASE_USER_PLAYING,
    PHASE_DONE,
}; phaseStatus;

enum genericMelodyStatus {
    GENERIC_MELODY_START,
    GENERIC_MELODY_PLAYING,
    GENERIC_MELODY_STOP
}genericMelodyStatus;

#define TIMER_HIGHLIGHT_GAME 20
#define TIMER_HIGHLIGHT_USER 20
#define TIMER_START_SCREEN   120
#define TIMER_LOOSE_SCREEN   120


int selector1 = 2;
int selector2 = 0;
#define zRows 4
#define zCols 4
#define numberZones  zRows* zCols
int Yoffset = 4;
int timersUser[numberZones] = { 0 };
int timersGame[numberZones] = { 0 };

Color zColors[numberZones];
Rectangle zones[numberZones];
Color colorGUI = { 0, 153, 255, 255 };
Color colorGUI2 = { 255, 0, 102, 255 };
Color colorSCORE = { 255, 255, 255, 70 };
Color paletteOranges[16]={
    { 218, 165,  32, 255 },
    { 249, 129,  42, 255 },
    { 139,  64,   0, 255 },
    { 249, 166,   2, 255 },
    { 253, 106,   2, 255 },
    { 216, 128,   0, 255 },
    { 252, 102,   0, 255 },
    { 255, 150,   3, 255 },
    { 121,  56,   2, 255 },
    { 255, 116,  23, 255 },
    { 150,  64,   0, 255 },
    { 239, 114,  21, 255 },
    { 203,  92,  13, 255 },  
    { 204, 119,  34, 255 },
    { 177,  86,  15, 255 },
    { 253, 165,  15, 255 },
};

/*------------------------------------- Function headers -------------------------------*/
void initGame(void);
void gameFSM(void);
void levelFSM(void);
void detectUserInteraction(void);
void playUserInteraction(void);
void loadMelodyForLevel(void);
void playGenericMelody(int melody, int scale);
void playNote(int note);
void generateSin(short audio_samples[], int samples_length, int samplerate, float frequency);
void generateSquare(short audio_samples[], int samples_length, int samplerate, float frequency);
void drawGame(void);
Color highlightUser(Color input, int i);
Color highlightGame(Color input, int i);
void calculateColorZones(void);
void wait(double seconds);


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: main()                                                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
int main(void)
{
    //SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "RayMaschine");

    // Render texture initialization, used to hold the rendering
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, FILTER_POINT);  // Texture scale filter to use!

    // ------------ YOUR CODE here -------------
    initGame();
    // ------------ end of YOUR CODE ------------------

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Compute required framebuffer scaling
        float scale = min((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);

            // Draw everything in the render texture, note this will not be rendered on screen, yet
            BeginTextureMode(target);
                ClearBackground(BLACK);         // Clear render texture background color

                // ------------ YOUR CODE here -------------
                gameFSM();           
                // ------------ end of YOUR CODE ------------------

            EndTextureMode();

            // Draw render texture to window, properly scaled
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5,
                           (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

            // Draw the grid like "stencil" is drawn over the squares to make them look not at all like LEDs!
            for (int x = 0; x < GetScreenWidth(); x += 16) DrawRectangle(x, 0, 4, GetScreenHeight(), BLACK);
            for (int y = 0; y < GetScreenHeight(); y += 16) DrawRectangle(0, y, GetScreenWidth(), 4, BLACK);

        EndDrawing();
        //--------------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);    // Unload render texture
    CloseAudioStream(stream);       // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();             // Close audio device (music streaming is automatically stopped)
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: initGame()                                                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void initGame(void)
{

    InitAudioDevice();
    stream = InitAudioStream(samplerate, 16, 1);

    int zWidth = gameScreenWidth / zCols;
    int zHeight = (gameScreenHeight - Yoffset / zRows) / zRows;

    int n = 0;

    for (int i = 0; i < zRows; i++)
    {
        for (int j = 0; j < zCols; j++)
        {;
            zones[n] = (Rectangle){ j * zWidth, i * zHeight + Yoffset, zWidth, zHeight };
            n++;
        }
    }
    calculateColorZones();
    
    gameLevel = 1;
    gameStatus = GAME_WELCOME_SCREEN;
    phaseStatus = PHASE_START;
    TraceLog(LOG_INFO, "gameStatus = GAME_WELCOME_SCREEN");
    TraceLog(LOG_INFO, "phaseStatus = PHASE_START");

    wait(0.1);
    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: gameFSM()                                                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void gameFSM(void)
{
    switch (gameStatus)
    {
    case GAME_WELCOME_SCREEN:
    {
        screenTimer++;
        DrawTextEx(GetFontDefault(), "Ray", (Vector2) { 0, 0 }, 10, 1, colorGUI2);
        DrawTextEx(GetFontDefault(), "Masc", (Vector2) { 0, 10 }, 10, 1, colorGUI2);
        DrawTextEx(GetFontDefault(), "hine", (Vector2) { 0, 20 }, 10, 1, colorGUI2);
        playGenericMelody(MELODY_USER_WINS, 2);

        if (screenTimer > TIMER_START_SCREEN)
        {    
            screenTimer = 0;
            gameStatus = GAME_FREE_MODE;
            TraceLog(LOG_INFO, "gameStatus = GAME_FREE_MODE");
        }
    }
    break;
        case GAME_FREE_MODE:
        {
            screenTimer = 0;
            drawGame();
            detectUserInteraction();
            playUserInteraction();
        }
        break;

        case GAME_START_SCREEN:
        {
            screenTimer++;
            DrawTextEx(GetFontDefault(), "GO!!", (Vector2) { 0, 0 }, 10, 1, colorGUI2);
            DrawTextEx(GetFontDefault(), FormatText("Level%d",gameLevel), (Vector2) { 0, 10 }, 10, 1, colorGUI2);
            playGenericMelody(MELODY_GAME_START,2);

            if (screenTimer > TIMER_START_SCREEN)
            {
                loadMelodyForLevel();
                calculateColorZones();
                screenTimer = 0;
                gameStatus = GAME_PLAYING;
                phaseStatus = PHASE_START;
                TraceLog(LOG_INFO, "gameStatus = GAME_PLAYING");
                TraceLog(LOG_INFO, "phaseStatus = PHASE_START");
            }
        }
        break;

        case GAME_PLAYING:
        {
            drawGame();
            detectUserInteraction();
            playUserInteraction();
            levelFSM();
        }
        break;

        case GAME_LOOSE_SCREEN:
        {
            screenTimer++;

            DrawTextEx(GetFontDefault(), "LOOSE", (Vector2) { 0, 0 }, 10, 1, colorGUI2);
            DrawTextEx(GetFontDefault(), "SCORE", (Vector2) { 0, 10 }, 10, 1, colorGUI2);
            DrawTextEx(GetFontDefault(), FormatText("%d", userTotalScore), (Vector2) { 0, 20 }, 10, 1, colorGUI2);
            
            playGenericMelody(MELODY_USER_LOOSE, 1);

            if (screenTimer > TIMER_LOOSE_SCREEN)
            {
                userTotalScore = 0;
                screenTimer = 0;
                gameLevel = 1;
                gameStatus = GAME_FREE_MODE;
                phaseStatus = PHASE_START;
                TraceLog(LOG_INFO, "gameStatus = GAME_FREE_MODE");
                TraceLog(LOG_INFO, "phaseStatus = PHASE_START");
            }

        }
        break;

        case GAME_WIN_SCREEN:
        {
            screenTimer++;

            DrawTextEx(GetFontDefault(), "WIN!!", (Vector2) { 0, 0 }, 10, 1, colorGUI);
            DrawTextEx(GetFontDefault(), "SCORE", (Vector2) { 0, 10 }, 10, 1, colorGUI);
            DrawTextEx(GetFontDefault(), FormatText("%d", userTotalScore), (Vector2) { 0, 20 }, 10, 1, colorGUI);
            

            playGenericMelody(MELODY_USER_WINS, 2);

            if (screenTimer > TIMER_LOOSE_SCREEN)
            {
                screenTimer = 0;
                if (gameLevel<NUMBER_OF_LEVELS)
                {
                    gameLevel++;
                }
                else
                {
                    gameLevel = 1;
                }
                gameStatus = GAME_START_SCREEN;
                phaseStatus = PHASE_START;
                genericMelodyStatus = GENERIC_MELODY_START;
                TraceLog(LOG_INFO, "gameStatus = GAME_START_SCREEN");
                TraceLog(LOG_INFO, "phaseStatus = PHASE_START");
                TraceLog(LOG_INFO, "genericMelodyStatus = GENERIC_MELODY_STOP;");
            }

        }
        break;
    }
return;
}
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: levelFSM()                                                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void levelFSM(void)
{
    switch (phaseStatus)
    {
        case PHASE_START:
        {
            gameMaxNote = 0;
            phaseStatus = PHASE_MELODY_START;
            TraceLog(LOG_INFO, "phaseStatus = PHASE_MELODY_START");
        }
        break;
        case PHASE_MELODY_START:
        {
            audioTimer = 0;
            gameActualNote = 0;
            phaseStatus = PHASE_MELODY_PLAYING;
            TraceLog(LOG_INFO, "phaseStatus = PHASE_MELODY_PLAYING");
        }
        break;

        case PHASE_MELODY_PLAYING:
        {

            if (audioTimer == 0)
            {
                timersGame[currentMelody[gameActualNote][1]] = TIMER_HIGHLIGHT_GAME;
                playNote(currentMelody[gameActualNote][1]);
            }
            if ((audioTimer / 10 == currentMelody[gameActualNote][0]))
            {
                audioTimer = 0;
                if (gameActualNote == gameMaxNote)
                {
                    phaseStatus = PHASE_MELODY_STOP;
                    TraceLog(LOG_INFO, "phaseStatus = PHASE_MELODY_STOP");
                }
                else
                {
                    gameActualNote++;
                }
            }
            else
            {
                audioTimer++;
            }

         }
        break;
        case PHASE_MELODY_STOP:
        {
            userActualNote = 0;
            memset(userMelody, 0, sizeof(userMelody));
            phaseStatus = PHASE_USER_PLAYING;
            TraceLog(LOG_INFO, "phaseStatus = PHASE_USER_PLAYING");
        }
        break;
        case PHASE_USER_PLAYING:
        {
            bool mistake = false;
            for (int i = 0; i < userActualNote; i++)
            {
                if (userMelody[i] != currentMelody[i][1])
                {
                    mistake = true; 
                    TraceLog(LOG_INFO, FormatText("mistake userMelody[%d]= %d, gameMelody[%d][1]=%d", i, userMelody[i], i, currentMelody[i][1]));
                    break;
                }
            }

            if (mistake == false) 
            {
                if (userActualNote > gameMaxNote)
                {
                    phaseCompleted = true;
                    phaseStatus = PHASE_DONE;
                    TraceLog(LOG_INFO, "phaseStatus = PHASE_DONE phaseCompleted=true");   
                }
               
            }
            else
            {
                phaseCompleted = false;
                phaseStatus = PHASE_DONE;
                TraceLog(LOG_INFO, "phaseStatus = PHASE_DONE phaseCompleted=false");
            }
        }
        break;

        case PHASE_DONE:
        {
            if (userDrawingDone == true)
            {
                wait(1);
                userDrawingDone = false;
                if (phaseCompleted == true)
                {
                    phaseCompleted = false;
                    if (currentMelodyLength - gameMaxNote > 1)
                    {
                        gameMaxNote++;
                        phaseStatus = PHASE_MELODY_START;
                    }
                    else
                    {
                        genericMelodyStatus = GENERIC_MELODY_START;
                        phaseStatus = PHASE_START;
                        gameStatus = GAME_WIN_SCREEN;
                        userTotalScore = userTotalScore + gameMaxNote+1;
                        TraceLog(LOG_INFO, "phaseStatus = PHASE_START");
                        TraceLog(LOG_INFO, "gameStatus = GAME_WIN_SCREEN");
                        TraceLog(LOG_INFO, "genericMelodyStatus = GENERIC_MELODY_START");
                    }
                }
                else
                {
                    genericMelodyStatus = GENERIC_MELODY_START;
                    phaseStatus = PHASE_START;                  
                    gameStatus = GAME_LOOSE_SCREEN;
                    userTotalScore = userTotalScore + gameMaxNote;
                    TraceLog(LOG_INFO, "phaseStatus = PHASE_START");
                    TraceLog(LOG_INFO, "gameStatus = GAME_LOOSE_SCREEN");
                    TraceLog(LOG_INFO, "genericMelodyStatus = GENERIC_MELODY_START");
                }
            }
        }
        break;

    };
    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: detectUserInteraction()                                                    */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/

void detectUserInteraction(void)
{

    float scale = min((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
    int x = (int)GetMouseX() / scale;
    int y = (int)GetMouseY() / scale;


    // Detect if clicked zones
    for (int i = 0; i < numberZones; i++)
    {
        if ((x > zones[i].x) && (x < (zones[i].x + zones[i].width)) && (y > zones[i].y) && (y < (zones[i].y + zones[i].height)))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                userDrawingDone = false;
                timersUser[i] = TIMER_HIGHLIGHT_USER;

                if (phaseStatus == PHASE_USER_PLAYING)
                {
                    userMelody[userActualNote] = i;
                    userActualNote++;
                }
            }
        }
    }
    // Detect if clicked GUI
    if ((x >= 1) && (x <= 9) && (y >= 1) && (y <= 2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        selector1++;

        if (selector1 == 4)
        {
            selector1 = 0;
        }
    }
    if ((x >= 12) && (x <= 19) && (y >= 1) && (y <= 2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        selector2++;

        if (selector2 == 2)
        {
            selector2 = 0;
        }
    }

    if ((x >= 25) && (x <= 28) && (y >= 1) && (y <= 2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        genericMelodyStatus = GENERIC_MELODY_START;
        gameStatus = GAME_START_SCREEN;
        TraceLog(LOG_INFO, "gameStatus = GAME_START_SCREEN");

    }

    return;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: playUserInteraction()                                                      */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void playUserInteraction(void)
{

    for (int i = 0; i < numberZones; i++)
    {
        if (timersUser[i] > 0)
        {
            if (timersUser[i] == TIMER_HIGHLIGHT_USER)
            {
                playNote(i);
            }
            timersUser[i] -= 1;
            if (timersUser[i] == 0)
            {
                userDrawingDone = true;
            }
        }
        
        if (timersGame[i] > 0)
        {
            timersGame[i] -= 1;
        }

    }
    return;
}
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: loadMelodyForLevel()                                                       */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void loadMelodyForLevel(void)
{
    switch (gameLevel)
    {
    case 1:
    {
        selector1 = 0;
        selector2 = 1;
        int song = GetRandomValue(0, 2);
        currentMelodyLength = melodiesLevel1Length[song];
        memcpy(&currentMelody[0][0],&melodiesLevel1[song][0][0], currentMelodyLength * 2 * sizeof(int));

    }
    break;
    case 2:
    {
        selector1 = 1;
        selector2 = 0;
        int song = GetRandomValue(0, 2);
        currentMelodyLength = melodiesLevel2Length[song];
        memcpy(&currentMelody[0][0], &melodiesLevel2[song][0][0], currentMelodyLength * 2 * sizeof(int));
    }
    break;
    case 3:
    {
        selector1 = 2;
        selector2 = 0;
        int song = GetRandomValue(0, 2);
        currentMelodyLength = melodiesLevel3Length[song];
        memcpy(&currentMelody[0][0], &melodiesLevel3[song][0][0], currentMelodyLength * 2 * sizeof(int));
    }
    break;
    }
return;
}
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: playGenericMelody()                                                        */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void playGenericMelody(int melody, int scale)
{
    switch (genericMelodyStatus)
    {
    case GENERIC_MELODY_START:
    {
        selector1 = scale;
        selector2 = 1;

        audioTimer = 0;
        gameActualNote = 0;
        genericMelodyStatus = GENERIC_MELODY_PLAYING;
        TraceLog(LOG_INFO, "genericMelodyStatus = GENERIC_MELODY_PLAYING");
    }
    break;

    case GENERIC_MELODY_PLAYING:
    {
        if (audioTimer == 0)
        {
            playNote(gameMelodies[melody][gameActualNote][1]);
        }
        if ((audioTimer / 10 == gameMelodies[melody][gameActualNote][0]))
        {
            audioTimer = 0;
            if (gameActualNote < (gameMelodiesLength[melody] - 1))
            {
                gameActualNote++;
            }
            else
            {
                genericMelodyStatus = GENERIC_MELODY_STOP;
                TraceLog(LOG_INFO, "genericMelodyStatus = GENERIC_MELODY_STOP");
            }
        }
        else
        {
            audioTimer++;
        }
    }
    break;
    case GENERIC_MELODY_STOP:
    {

    }
    break;
    }


    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: playNote()                                                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void playNote(int note)
{
    short audio_samples[MAX_SAMPLES] = { 0 };

    if (selector2 == 0)
    {
        generateSin(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][note]);
    }
    else
    {
        generateSquare(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][note]);
    }
    if (IsAudioStreamPlaying(stream))
    {
        StopAudioStream(stream);
    }
    UpdateAudioStream(stream, audio_samples, MAX_SAMPLES);
    PlayAudioStream(stream);

    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: generateSin()                                                              */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void generateSin(short audio_samples[], int samples_length, int samplerate, float frequency)
{
    int waveLength = (int)(samplerate / frequency);
    int number_waves = (int)(samples_length / waveLength);
    for (int i = 0; i < waveLength * number_waves; i++)
    {
        int amplitude = 32000 -8 * i;

        audio_samples[i] = (short)(sinf(((2 * PI * (float)i / waveLength))) * amplitude);

    }
    return;
}
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: generateSquare()                                                           */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void generateSquare(short audio_samples[], int samples_length, int samplerate, float frequency)
{
    int waveLength = (int)(samplerate / frequency);
    int number_waves = (int)(samples_length / waveLength);
    for (int i = 0; i < waveLength * number_waves; i++)
    {
        int amplitude = 32000 - 8 * i;
        int sign = 1;
        if (i % waveLength > waveLength / 2)
        {
            sign = -1;
        }
        audio_samples[i] = amplitude * sign;

    }
    return;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: drawGame()                                                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void drawGame(void)
{
    Color temp;

    // Zones
    for (int i = 0; i < numberZones; i++)
    {
        temp = zColors[i];

        if (i == currentMelody[gameActualNote][1])
        {
            temp = highlightGame(zColors[i], timersGame[i]);
        }

        if (timersUser[i] > 0)
        {       
            temp = highlightUser(zColors[i], timersUser[i]);
        }

        DrawRectangle(zones[i].x, zones[i].y, zones[i].width, zones[i].height, temp);

    }

    if (gameStatus == GAME_PLAYING)
    {
        DrawTextEx(GetFontDefault(), FormatText("%d", gameMaxNote+1), (Vector2) { 6, 4}, 30, 0,colorSCORE);
    }

    // GUI
    DrawPixel(1, 1, colorGUI);
    DrawLine(1, 2, 10, 2, colorGUI);
    DrawLine(2 + 2 * selector1, 1, 4 + 2 * selector1, 1, colorGUI2);

    DrawPixel(12, 1, colorGUI);
    DrawLine(12, 2, 17, 2, colorGUI);
    DrawLine(13 + 2 * selector2, 1, 15 + 2 * selector2, 1, colorGUI2);

    DrawPixel(25, 1, colorGUI);
    DrawPixel(26, 1, colorGUI2);
    DrawPixel(27, 1, colorGUI);
    DrawLine(25,2,28,2,colorGUI);

    return;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: highlightUser()                                                            */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
Color highlightUser(Color input, int i)
{
    int r, g, b;
    int offset = i * 6;
    Color output;

    r = input.r + offset;
    g = input.g + offset;
    b = input.b + offset;
    if (r > 255) { output.r = 255; } else { output.r = r; }
    if (g > 255) { output.g = 255; } else { output.g = g; }
    if (b > 255) { output.b = 255; } else { output.b = b; }
    output.a = 255;

    return output;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: highlightGame()                                                            */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
Color highlightGame(Color input,  int i)
{
    Color output;

    output.r = i * (colorGUI2.r - input.r) / TIMER_HIGHLIGHT_GAME + input.r;
    output.g = i * (colorGUI2.g - input.g) / TIMER_HIGHLIGHT_GAME + input.g;
    output.b = i * (colorGUI2.b - input.b) / TIMER_HIGHLIGHT_GAME + input.b;
    output.a = 255;

    return output;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: calculateColorZones()                                                      */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void calculateColorZones(void)
{
    for (int n = 0; n < numberZones; n++)
    {
        //zColors[n] = (Color){ GetRandomValue(0, 255),  GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
        zColors[n] = paletteOranges[GetRandomValue(0, 15)];    
    }
    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: wait()                                                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void wait(double seconds)
{
    double a = GetTime();
    double b = GetTime();
 
    TraceLog(LOG_INFO, FormatText("Wait %f [s]",seconds));
    while (b < a + seconds)
    {
        b = GetTime();
    }
    return;
}
