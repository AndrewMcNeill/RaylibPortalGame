/*******************************************************************************************
*
*   raylib [core] example - Mouse input
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define WIDTH 1024
#define HEIGHT 768

#define DOTCOLOR 0x882200FF
#define MAXPORTALS 8
#define PORTALSIZE 30

enum Direction {Top, Right, Bottom, Left};

char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

enum Direction RayScreenCollisionPoint(
    Vector2 position, Vector2 direction, 
    float width, float height, Vector2 *intersection)
{   
    
    if (fabs(direction.x) < 0.02 && direction.y < 0) {
        *intersection = (Vector2) {position.x, 0};
        return Top;
    } else if (fabs(direction.x) < 0.02 && direction.y > 0) {
        *intersection = (Vector2) {position.x, height};
        return Bottom;
    } else if (fabs(direction.y) < 0.02 && direction.x < 0) {
        *intersection = (Vector2) {0, position.y};
        return Left;
    } else if (fabs(direction.y) < 0.02 && direction.x > 0) {
        *intersection = (Vector2) {width, position.y};
        return Right;
    }
    
    direction = Vector2Scale(direction, 5000);
    float intersectionX, intersectionY;

    if (get_line_intersection(
        position.x, position.y, 
        direction.x, direction.y,
        0, 0, // Top Left
        WIDTH, 0, // Top Right
        &intersectionX, &intersectionY)) {
        *intersection = (Vector2){intersectionX, intersectionY};
        return Top;
    } else if (get_line_intersection(
        position.x, position.y, 
        direction.x, direction.y,
        WIDTH, 0, // Top Right
        WIDTH, HEIGHT, // Bottom Right
        &intersectionX, &intersectionY)) {
        *intersection = (Vector2){intersectionX, intersectionY};
        return Right;
    } else if (get_line_intersection(
        position.x, position.y, 
        direction.x, direction.y,
        WIDTH, HEIGHT, // Bottom Right
        0, HEIGHT, // Bottom Left
        &intersectionX, &intersectionY)) {
        *intersection = (Vector2){intersectionX, intersectionY};
        return Bottom;
    } else if (get_line_intersection(
        position.x, position.y, 
        direction.x, direction.y,
        0, HEIGHT, // Bottom Left
        0, 0, // Top Left
        &intersectionX, &intersectionY)) {
        *intersection = (Vector2){intersectionX, intersectionY};
        return Left;
    }

    return 0;
}

typedef struct Portal Portal;
struct Portal {
    Vector2 position;
    float size;
    Vector2 point1, point2;
    Portal* linkedPortal;
    Vector2 polyPoints[8];
    int numPolyPoints;
    RenderTexture2D choppedTexture;
    Vector2 topLeft;
};

void UpdatePortalPoints(Vector2 position, Portal* portal) {
    float portalPlayerAngle = Vector2Angle(position, (*portal).position) + 90;
    (*portal).point1 = Vector2Rotate((Vector2){ portal->size*-0.5f, 0.0f }, portalPlayerAngle);
    (*portal).point1 = Vector2Add((*portal).point1, (*portal).position);
    (*portal).point2 = Vector2Rotate((Vector2){ (*portal).size*0.5f, 0.0f }, portalPlayerAngle);
    (*portal).point2 = Vector2Add((*portal).point2, (*portal).position);
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = WIDTH;
    const int screenHeight = HEIGHT;
    InitWindow(screenWidth, screenHeight, "raylib [core] example - mouse input");

    float screenSize[2] = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    const Vector2 topLeft = {0,0};
    const Vector2 topRight = {screenWidth,0};
    const Vector2 bottomLeft = {0,screenHeight};
    const Vector2 bottomRight = {screenWidth,screenHeight};


    Image backgroundImage = LoadImage("resources/Background.png");
    Texture2D backgroundTexture = LoadTextureFromImage(backgroundImage);

    /*
    Shader pixelationShader = LoadShader(0, "pixelationShader.fs");

    int pixelationAmount = 1;

    SetShaderValue(pixelationShader, GetShaderLocation(pixelationShader, "size"), &screenSize, SHADER_UNIFORM_VEC2);
    SetShaderValue(pixelationShader, GetShaderLocation(pixelationShader, "pixelationAmount"), &pixelationAmount, SHADER_UNIFORM_INT);
    */

    float playerPosition[8] = { 100.0f, 100.0f };
    Vector2 playerSize = { 20.0f, 20.0f };
    Color playerColor = DARKBLUE;

    Vector2 portalPositions[MAXPORTALS] = {{200,300},{400,600}};
    int numPortals = 2;

    Shader portalDisplacementShader = LoadShader(0, "portalDisplacementShader.fs");
    SetShaderValue(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "size"), &screenSize, SHADER_UNIFORM_VEC2);
    
    SetShaderValue(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "portalPositions"), &portalPositions, SHADER_UNIFORM_VEC2);
    SetShaderValueV(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "portalPositions"), &portalPositions, SHADER_UNIFORM_VEC2, numPortals);
    SetShaderValue(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "numPortals"), &numPortals, SHADER_UNIFORM_INT);
    SetShaderValue(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "playerPosition"), &playerPosition, SHADER_UNIFORM_VEC2);


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPosition[0] += 4.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerPosition[0] -= 4.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) playerPosition[1] -= 4.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) playerPosition[1] += 4.0f;
        SetShaderValue(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "playerPosition"), &playerPosition, SHADER_UNIFORM_VEC2);


        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (numPortals < MAXPORTALS) {
                portalPositions[numPortals++] = GetMousePosition();
                SetShaderValueV(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "portalPositions"), &portalPositions, SHADER_UNIFORM_VEC2, (numPortals/2)*2);
                SetShaderValue(portalDisplacementShader, GetShaderLocation(portalDisplacementShader, "numPortals"), &numPortals, SHADER_UNIFORM_INT);
            }
        }
        /*if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            pixelationAmount = (pixelationAmount > 1) ? pixelationAmount-1 : pixelationAmount;
            SetShaderValue(pixelationShader, GetShaderLocation(pixelationShader, "pixelationAmount"), &pixelationAmount, SHADER_UNIFORM_INT);
        }*/
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            BeginShaderMode(portalDisplacementShader);

                DrawTexture(backgroundTexture, 0, 0, WHITE);

            EndShaderMode();

            DrawRectangle(playerPosition[0]-playerSize.x*0.5, playerPosition[1]-playerSize.y*0.5, playerSize.x, playerSize.y, playerColor);
            // DrawRectangleV(Vector2Subtract(playerPosition, Vector2Scale(playerSize, 0.5f)), playerSize, playerColor);

            for(int i = 0; i < numPortals; i++) {
                DrawCircleV(portalPositions[i], PORTALSIZE, Fade(BLUE, 0.5f));
            }

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}