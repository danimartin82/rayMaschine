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


/*------------------------------------- Function headers -------------------------------*/
void generate_sin(short audio_samples[], int samples_length, int samplerate, float frequency);
Color highlightColor(Color input, int i);
Color lowlightColor(Color input, int i);
int find_max(int a[], int n);


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
    int samplerate = 22050;
    InitAudioDevice();
    AudioStream stream = InitAudioStream(samplerate, 16, 1);
    float frequencies[] = {
        261.63,	//C4
        277.18,	//C#4
        293.66,	//D4
        311.13,	//D#4
        329.63,	//E4
        349.23,	//F4
        369.99,	//F#4
        392.00,	//G4
        415.30,	//G#4
        440.00,	//A4
        466.16,	//A#4
        493.88,	//B4
        523.25,	//C5
        554.37,	//C#5
        587.33,	//D5
        622.25	//D#5
    };
    #define zRows 4
    #define zCols 4
    int Yoffset = 4;
    #define numberZones  zRows* zCols
    int zWidth = gameScreenWidth / zCols;
    int zHeight = (gameScreenHeight- Yoffset/ zRows) / zRows;
    int timers[numberZones] = { 0 };
    Color zColors[numberZones];
    Rectangle zones[numberZones];

    int n = 0;

    for (int i = 0; i < zRows; i++)
    {
        for (int j = 0; j < zCols; j++)
        {
            zColors[n] = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0, 255),255 };
            zones[n] = (Rectangle){ j * zWidth, i * zHeight + Yoffset, zWidth, zHeight };
            n++;
        }
    }


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

                int x = (int)GetMouseX()/scale;
                int y = (int)GetMouseY()/scale;
                Color temp;
             

                for (int i = 0; i < numberZones; i++)
                {
                    if ((x > zones[i].x) && (x < (zones[i].x + zones[i].width)) && (y > zones[i].y) && (y < (zones[i].y + zones[i].height)))
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            timers[i] = 10;
                           // TraceLog(LOG_INFO, FormatText(" timers[%d]=%d,", i, timers[i]));
                        }
                    }
                }

                int max=find_max(timers, numberZones);
                for (int i = 0; i < numberZones; i++)
                {
                    temp = zColors[i];
    
                    if (max > 0)
                    {
            
                        if (timers[i] > 0)
                        {
                            temp = highlightColor(zColors[i], timers[i]);
                            if (timers[i] == 10)
                            {
                                short audio_samples[MAX_SAMPLES] = { 0 };
                                generate_sin(audio_samples, MAX_SAMPLES, samplerate, frequencies[i]);
                                UpdateAudioStream(stream, audio_samples, MAX_SAMPLES);
                                PlayAudioStream(stream);
                            }

                            timers[i] -= 1;
                        }


                    }
                    
                    DrawRectangle(zones[i].x, zones[i].y, zones[i].width, zones[i].height, temp);
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
    CloseAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}

/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: highlightColor()                                                           */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
Color highlightColor(Color input, int i)
{
    int r, g, b;
    int offset = i*8;
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
/* Function: lowlightColor()                                                            */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
Color lowlightColor(Color input, int i)
{
    int r, g, b;
    int offset = i*4;
    Color output;

    r = input.r - offset;
    g = input.g - offset;
    b = input.b - offset;
    if (r < 0) { output.r = 0; }
    else { output.r = r; }
    if (g < 0) { output.g = 0; }
    else { output.g = g; }
    if (b < 0) { output.b = 0; }
    else { output.b = b; }
    output.a = 255;

    return output;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: find_max()                                                                 */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
int find_max(int a[], int n)
{
    int c, max, index;

    max = a[0];
    index = 0;

    for (c = 1; c < n; c++) {
        if (a[c] > max) {
            index = c;
            max = a[c];
        }
    }

    return max;
}


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: generate_sin()                                                             */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void generate_sin(short audio_samples[], int samples_length, int samplerate, float frequency)
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
