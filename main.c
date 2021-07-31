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

#define WIDTH 600
#define HEIGHT 400

#define DOTCOLOR 0x882200FF

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
    const Vector2 topLeft = {0,0};
    const Vector2 topRight = {screenWidth,0};
    const Vector2 bottomLeft = {0,screenHeight};
    const Vector2 bottomRight = {screenWidth,screenHeight};

    InitWindow(screenWidth, screenHeight, "raylib [core] example - mouse input");

    Image backgroundImage = LoadImage("resources/Background.png");
    Texture2D backgroundTexture = LoadTextureFromImage(backgroundImage);

    Vector2 playerPosition = { 100.0f, 100.0f };
    Vector2 playerSize = { 20.0f, 20.0f };
    Color playerColor = DARKBLUE;

    float portalSize = 100.0f;
    Portal mousePortal = { .position = Vector2Zero(), .size = portalSize };

    const int MAX_PORTALS = 20;
    Portal portals[MAX_PORTALS];
    int num_portals = 0;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        mousePortal.position = GetMousePosition();
        UpdatePortalPoints(playerPosition, &mousePortal);
        for (int i = 0; i < num_portals; i++) {
            //if (Vector2Distance(playerPosition, portals[i].position) < 50) continue;
            UpdatePortalPoints(playerPosition, &portals[i]);

        }
        
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPosition.x += 4.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerPosition.x -= 4.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) playerPosition.y -= 4.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) playerPosition.y += 4.0f;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            num_portals++;
            portals[num_portals-1] = mousePortal;
            if (num_portals % 2 == 0) {
                portals[num_portals-2].linkedPortal = &portals[num_portals-1];
                portals[num_portals-1].linkedPortal = &portals[num_portals-2];
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(backgroundTexture, 0, 0, WHITE);
            
            DrawLineEx(mousePortal.point1, mousePortal.point2, 3.0f, RED);

            for (int i = 0; i < num_portals; i++) {
                Vector2 line1 = Vector2Subtract(portals[i].point1, playerPosition);
                line1 = Vector2Scale(line1, 1000);
                line1 = Vector2Add(line1, playerPosition);
                DrawLineEx(playerPosition, line1, 1.0f, BLACK);
                
                Vector2 line2 = Vector2Subtract(portals[i].point2, playerPosition);
                line2 = Vector2Scale(line2, 1000);
                line2 = Vector2Add(line2, playerPosition);
                DrawLineEx(playerPosition, line2, 1.0f, BLACK);

                DrawLineEx(portals[i].point1, portals[i].point2, 3.0f, BLUE);

                Vector2 points[4] = {portals[i].point1, portals[i].point2, line2, line1};
                DrawTriangleFan(points, 4, GetColor(0xdd773360));
                Vector2 points2[4] = {portals[i].point2, portals[i].point1, line1, line2};
                DrawTriangleFan(points2, 4, GetColor(0xdd773360));

                Vector2 intersection;
                
                Vector2 ray = Vector2Normalize(Vector2Subtract(portals[i].point1, playerPosition));
                ray = Vector2Scale(ray, 2000);
                enum Direction point1Dir = RayScreenCollisionPoint(playerPosition, ray, WIDTH, HEIGHT, &intersection);
                DrawCircleV(intersection, 8.0f, GetColor(DOTCOLOR));
                
                
                ray = Vector2Normalize(Vector2Subtract(portals[i].point2, playerPosition));
                ray = Vector2Scale(ray, 2000);
                enum Direction point2Dir = RayScreenCollisionPoint(playerPosition, ray, WIDTH, HEIGHT, &intersection);
                DrawCircleV(intersection, 8.0f, GetColor(DOTCOLOR));
                
                /*if (point1Dir == Top && point2Dir == Right) {
                    DrawCircleV(topRight, 8.0f, GetColor(DOTCOLOR));
                } else if (point1Dir == Right && point2Dir == Bottom) {
                    DrawCircleV(bottomRight, 8.0f, GetColor(DOTCOLOR));
                } else if (point1Dir == Bottom && point2Dir == Left) {
                    DrawCircleV(bottomLeft, 8.0f, GetColor(DOTCOLOR));
                } else if (point1Dir == Left && point2Dir == Top) {
                    DrawCircleV(topLeft, 8.0f, GetColor(DOTCOLOR));
                }*/
                if (point1Dir == Top) {
                    if (point2Dir == Right) DrawCircleV(topRight, 8.0f, GetColor(DOTCOLOR));
                    else if (point2Dir == Bottom) {
                        DrawCircleV(topRight, 8.0f, GetColor(DOTCOLOR));
                        DrawCircleV(bottomRight, 8.0f, GetColor(DOTCOLOR));
                    }
                } else if (point1Dir == Right) {
                    if (point2Dir == Bottom) DrawCircleV(bottomRight, 8.0f, GetColor(DOTCOLOR));
                    else if (point2Dir == Left) {
                        DrawCircleV(bottomRight, 8.0f, GetColor(DOTCOLOR));
                        DrawCircleV(bottomLeft, 8.0f, GetColor(DOTCOLOR));
                    }
                } else if (point1Dir == Bottom) {
                    if (point2Dir == Left) DrawCircleV(bottomLeft, 8.0f, GetColor(DOTCOLOR));
                    else if (point2Dir == Top) {
                        DrawCircleV(bottomLeft, 8.0f, GetColor(DOTCOLOR));
                        DrawCircleV(topLeft, 8.0f, GetColor(DOTCOLOR));
                    }
                } else if (point1Dir == Left) {
                    if (point2Dir == Top) DrawCircleV(topLeft, 8.0f, GetColor(DOTCOLOR));
                    else if (point2Dir == Right) {
                        DrawCircleV(topLeft, 8.0f, GetColor(DOTCOLOR));
                        DrawCircleV(topRight, 8.0f, GetColor(DOTCOLOR));
                    }
                }

            }

            DrawRectangleV(Vector2Subtract(playerPosition, Vector2Scale(playerSize, 0.5f)), playerSize, playerColor);


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