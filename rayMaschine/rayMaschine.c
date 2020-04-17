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
#define MAX_SAMPLES               4096

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
int samplerate = 22050;
AudioStream stream;

int gameMelody[70][2] = {
{1,14},{1,13},{1,14},{1,13},{1,14},{1,9},{1,12},{1,10},{3,7},{1,2},{1,2},{1,7},{3,9},{1,2},{1,6},{1,9},{3,10},{1,2},{1,14},{1,13},{1,14},{1,13},{1,14},{1,9},{1,12},
{1,10},{3,7},{1,2},{1,2},{1,7},{1,9},{1,2},{1,6},{1,12},{1,10},{1,9},{3,7},{1,9},{1,10},{1,12},{3,14},{1,5},{1,15},{1,14},{3,12},{1,3},{1,14},{1,12},{3,10},{1,2},
{1,12},{1,10},{2,9},{1,7},{1,2},{3,14},{1,14},{3,14},{1,13},{3,14},{1,13},{1,14},{1,13},{1,14},{1,13},{1,14},{1,9},{1,12},{1,10},{4,7}};

int userMelody[70] = { 0 };

int audioTimer = 0;
int gameActualNote = 100;
int gameMaxNote = 0;
int userActualNote = 0;
bool gameInterpretationDone = false;
bool userDrawingDone = false;

enum gameStatus{START,FREE_MODE,PLAYING,LOOSE} gameStatus;
enum userStatus { USER_NOT_STARTED, USER_PLAYING, USER_LEVEL_DONE,USER_FAILED} userStatus;

gameStatus = FREE_MODE;
userStatus = USER_NOT_STARTED;

#define TIMER_HIGHLIGHT_GAME 10

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


/*------------------------------------- Function headers -------------------------------*/
void detectUserInteraction(void);
void checkUserFollowingMelody(int note);
void drawZones(void);
void playUserInteraction(void);
void playMelody(void);
void drawGUI(void);
void wait(int seconds);
void generateSin(short audio_samples[], int samples_length, int samplerate, float frequency);
void generateSquare(short audio_samples[], int samples_length, int samplerate, float frequency);
void generateNoise(short audio_samples[], int samples_length);
Color highlightUser(Color input, int i);
Color highlightGame(Color input, int i);
void userStatusFSM(void);


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
    InitAudioDevice();
    stream = InitAudioStream(samplerate, 16, 1);
    
    int zWidth = gameScreenWidth / zCols;
    int zHeight = (gameScreenHeight - Yoffset / zRows) / zRows;

    int n = 0;

    for (int i = 0; i < zRows; i++)
    {
        for (int j = 0; j < zCols; j++)
        {
            zColors[n] = (Color){ GetRandomValue(0, 255),  GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
            zones[n] = (Rectangle){ j * zWidth, i * zHeight + Yoffset, zWidth, zHeight };
            n++;
        }
    }
    TraceLog(LOG_INFO, "userStatus = USER_NOT_STARTED");
    TraceLog(LOG_INFO, "gameStatus = FREE_MODE");
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
                //TraceLog(LOG_INFO, FormatText("%d,%d", x, y));

                switch (gameStatus)
                {
                    case FREE_MODE:
                    {
                        detectUserInteraction();
                        drawZones();
                        drawGUI();
                        playUserInteraction();
                    }
                    break;

                    case START:
                    {
                        audioTimer = 0;
                        gameActualNote = 0;
                        gameInterpretationDone = false;
                        userActualNote = 0;
                        memset(userMelody, 0, sizeof(userMelody));

                        gameStatus = PLAYING;
                        TraceLog(LOG_INFO, "gameStatus = PLAYING");
                    }
                    break;

                    case PLAYING:
                    {
                        detectUserInteraction();
                        drawZones();
                        drawGUI();
                        playUserInteraction();
                        playMelody();
                        userStatusFSM();
                    }
                    break;

                    case LOOSE:
                    {
                        DrawRectangle(0, 0, 32, 32, (Color) { 0, 0, 0, 255 });
                      
                    }
                    break;
                }
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
/* Function: playMelody()                                                              */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void playMelody(void)
{
    short audio_samples[MAX_SAMPLES] = { 0 };

    if ((gameActualNote <= gameMaxNote) && (gameInterpretationDone != true))
    {       

        if (audioTimer == 0)
        {
            timersGame[gameMelody[gameActualNote][1]] = TIMER_HIGHLIGHT_GAME;
            //TraceLog(LOG_INFO, FormatText(" gameActualNote=%d, gameMaxNote=%d, timersGame[%d][1]] %d", gameActualNote, gameMaxNote, gameMelody[gameActualNote][1], timersGame[gameMelody[gameActualNote][1]]));
            if (selector2 == 0)
            {
                generateSin(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][gameMelody[gameActualNote][1]]);
            }
            else
            {
                generateSquare(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][gameMelody[gameActualNote][1]]);
            }
            if (IsAudioStreamPlaying(stream))
            {
                StopAudioStream(stream);
            }
            UpdateAudioStream(stream, audio_samples, MAX_SAMPLES);
            PlayAudioStream(stream);

        }
        if ((audioTimer / 10 == gameMelody[gameActualNote][0]))
        {
            audioTimer = 0;
            if (gameActualNote == gameMaxNote)
            {
                gameInterpretationDone = true;
                TraceLog(LOG_INFO, "gameInterpretationDone");
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
        if (timersGame[gameMelody[gameActualNote][1]] > 0)
        {
            timersGame[gameMelody[gameActualNote][1]] -= 1;
        }
    }
    

    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: drawGUI()                                                                  */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void drawGUI(void)
{
    DrawPixel(1, 1, colorGUI);
    DrawLine(1, 2, 10, 2, colorGUI);
    DrawLine(2 + 2 * selector1, 1, 4 + 2 * selector1, 1, colorGUI2);

    DrawPixel(12, 1, colorGUI);
    DrawLine(12, 2, 19, 2, colorGUI);
    DrawLine(13 + 2 * selector2, 1, 15 + 2 * selector2, 1, colorGUI2);

    DrawPixel(25, 1, colorGUI);
    DrawPixel(26, 1, colorGUI2);
    DrawPixel(27, 1, colorGUI);
    DrawLine(25,2,28,2,colorGUI);

    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: detect_user_interction()                                                   */
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
                timersUser[i] = 10;
                checkUserFollowingMelody(i);

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

        if (selector2 == 3)
        {
            selector2 = 0;
        }
    }

    if ((x >= 25) && (x <= 28) && (y >= 1) && (y <= 2) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        gameStatus = START;
        TraceLog(LOG_INFO, "gameStatus = START");
        
    }

    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: checkUserFollowingMelody()                                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void checkUserFollowingMelody(int note)
{
    bool mistake = false;

    if (gameInterpretationDone == true)
    {
        userMelody[userActualNote] = note;

        for (int i = 0; i <= userActualNote; i++)
        {

            if (userMelody[i] != gameMelody[i][1])
            {
                mistake = true;
                userStatus = USER_FAILED;
                TraceLog(LOG_INFO, "userStatus = USER_FAILED");
                userActualNote = 0;
                memset(userMelody, 0, sizeof(userMelody));
                gameMaxNote = 0;
                break;
            }
        }

        if (mistake == false)
        {
            if (userActualNote == gameActualNote)
            {
                userStatus = USER_LEVEL_DONE;
                TraceLog(LOG_INFO, "userStatus = USER_LEVEL_DONE");
                userActualNote = 0;
                memset(userMelody, 0, sizeof(userMelody));
                gameMaxNote++;
            }
            else
            {
             userActualNote += 1;
             userStatus = USER_PLAYING;
             TraceLog(LOG_INFO, "userStatus = USER_PLAYING");
            }
        }

     }
    return;
}
/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: drawZones()                                                                */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void drawZones(void)
{
    Color temp;
    for (int i = 0; i < numberZones; i++)
    {
        temp = zColors[i];

        if (i == gameMelody[gameActualNote][1])
        {
            temp = highlightGame(zColors[i], timersGame[i]);
        }

        if (timersUser[i] > 0)
        {
            temp = highlightUser(zColors[i], timersUser[i]);
        }

        DrawRectangle(zones[i].x, zones[i].y, zones[i].width, zones[i].height, temp);

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

            if (timersUser[i] == 10)
            {
                short audio_samples[MAX_SAMPLES] = { 0 };

                if (selector2 == 0)
                {
                    generateSin(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][i]);
                }
                else
                {
                    generateSquare(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][i]);
                }
                if (IsAudioStreamPlaying(stream))
                {
                    StopAudioStream(stream);
                }
                UpdateAudioStream(stream, audio_samples, MAX_SAMPLES);
                PlayAudioStream(stream);
            }

            timersUser[i] -= 1;
            if (timersUser[i]==0)
            {
                userDrawingDone = true;
            }
        }


    }
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
    int offset = i * 8;
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

    output.r = i * (255 - input.r) / TIMER_HIGHLIGHT_GAME + input.r;
    output.g = i * (-1) * input.g / TIMER_HIGHLIGHT_GAME + input.g;
    output.b = i * (-1) * input.b / TIMER_HIGHLIGHT_GAME + input.b;
    output.a = 255;

    return output;
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
        int amplitude = 32000 - 8 * i;

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
/* Function: generateNoise()                                                            */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void generateNoise(short audio_samples[], int samples_length)
{
 
    for (int i = 0; i < samples_length; i++)
    {
        int amplitude = abs(3200 - 10 * i);

        audio_samples[i] = amplitude * GetRandomValue(0,100);
    }
    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: wait()                                                                     */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void wait(int seconds)
{
    double a = GetTime();
    double b = GetTime();
    while (b < a + seconds)
    {
        b = GetTime();
    }
    return;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: userStatusFSM()                                                            */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void userStatusFSM(void)
{
    switch (userStatus)
    {
        case USER_LEVEL_DONE:
        {
            if (userDrawingDone == true)
            {
                wait(1);
                audioTimer = 0;
                gameActualNote = 0;
                gameInterpretationDone = false;
                userDrawingDone = false;
                userStatus = USER_NOT_STARTED;
                TraceLog(LOG_INFO, "userStatus = USER_NOT_STARTED");
            }


        }
        break;
        case USER_FAILED:
        {
            if (userDrawingDone == true)
            {
                userStatus = USER_NOT_STARTED;
                gameStatus = LOOSE;
                TraceLog(LOG_INFO, "gameStatus = LOOSE");
                TraceLog(LOG_INFO, "userStatus = USER_NOT_STARTED");

            }


        }
        break;
    }

    

    return;

}