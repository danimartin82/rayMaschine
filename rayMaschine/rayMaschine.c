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
void generate_hihat(short audio_samples[], int samples_length);
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
    float frequencies[4][16] = {
        {41.20, // E1
        43.65, // F1
        46.25, // F#1
        49.00, // G1
        51.91, // G#1 
        55.00, // A1
        58.27, // A#1
        61.74, // B1
        65.41, // C2
        69.30, // C#2
        73.42, // D2
        77.78, // D#2
        82.41, // E2
        87.31, // F2
        92.50, // F#2
        98.00}, //	G2

        {103.83, //	G#2
        110.00, //	A2
        116.54, //	A#2
        123.47, //	B2
        130.81, //	C3
        138.59, //	C#3
        146.83, //	D3
        155.56, //	D#3
        164.81, //	E3
        174.61, //	F3
        185.00, //	F#3
        196.00, //	G3
        207.65, //	G#3
        220.00, //	A3
        233.08, //	A#3
        246.94 }, //B3

        {261.63,//C4
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
        622.25},//D#5

        {659.25,//	E5
        698.46,	//  F5
        739.99,	//	F#5
        783.99,	//	G5
        830.61,	//	G#5 
        880.00,	//	A5
        932.33,	//	A#5
        987.77,	//	B5
        1046.50, //	C6
        1108.73, //	C#6
        1174.66, //	D6
        1244.51, //	D#6
        1318.51, //	E6
        1396.91, //	F6
        1479.98, //	F#6 
        1567.98} //	G6

    };

    int selector1 = 0;
    int selector2 = 0;
    #define zRows 4
    #define zCols 4
    #define numberZones  zRows* zCols
    int Yoffset = 4;
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
            zColors[n] = (Color){ GetRandomValue(0, 255),  GetRandomValue(0, 255), GetRandomValue(0, 255), 255 };
            zones[n] = (Rectangle){ j * zWidth, i * zHeight + Yoffset, zWidth, zHeight };
            n++;
        }
    }

    Color colorGUI = { 0, 153, 255, 255 };
    Color colorGUI2 = { 255, 0, 102, 255 };
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
             
                // Detect if clicked zones
                for (int i = 0; i < numberZones; i++)
                {
                    if ((x > zones[i].x) && (x < (zones[i].x + zones[i].width)) && (y > zones[i].y) && (y < (zones[i].y + zones[i].height)))
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            timers[i] = 10;
                        }
                    }
                }
                // Detect if clicked Selector
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


                // Draw zones 
                for (int i = 0; i < numberZones; i++)
                {
                    temp = zColors[i];

                    if (timers[i] > 0)
                    {
                        temp = highlightColor(zColors[i], timers[i]);
                        if (timers[i] == 10)
                        {
                            short audio_samples[MAX_SAMPLES] = { 0 };

                             generate_sin(audio_samples, MAX_SAMPLES, samplerate, frequencies[selector1][i]);

                            if (IsAudioStreamPlaying(stream))
                            {
                                StopAudioStream(stream);
                            }
                            UpdateAudioStream(stream, audio_samples, MAX_SAMPLES);
                            PlayAudioStream(stream);
                         }

                        timers[i] -= 1;
                    }
   
                    DrawRectangle(zones[i].x, zones[i].y, zones[i].width, zones[i].height, temp);
       

                }


                DrawPixel(1, 1, colorGUI);
                DrawLine(1, 2, 10, 2, colorGUI);
                DrawLine(2 + 2 * selector1, 1, 4 + 2 * selector1, 1, colorGUI2);

                DrawPixel(12, 1, colorGUI);
                DrawLine(12, 2, 19, 2, colorGUI);
                DrawLine(13 + 2 * selector2, 1, 15 + 2 * selector2, 1, colorGUI2);

     
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
/* Function: highlightColor()                                                           */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
Color highlightColor(Color input, int i)
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


/*--------------------------------------------------------------------------------------*/
/*                                                                                      */
/* Function: generate_hihat()                                                           */
/*                                                                                      */
/*--------------------------------------------------------------------------------------*/
void generate_hihat(short audio_samples[], int samples_length)
{
 
    for (int i = 0; i < samples_length; i++)
    {
        int amplitude = abs(3200 - 10 * i);

        audio_samples[i] = amplitude * GetRandomValue(0,100);
    }
    return;
}
