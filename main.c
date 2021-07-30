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

typedef struct Portal {
    Vector2 position;
    float size;
    Vector2 point1, point2;
} Portal;

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
    const int screenWidth = 1024;
    const int screenHeight = 768;

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
            if (Vector2Distance(playerPosition, portals[i].position) < 50) continue;
            UpdatePortalPoints(playerPosition, &portals[i]);
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPosition.x += 4.0f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerPosition.x -= 4.0f;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) playerPosition.y -= 4.0f;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) playerPosition.y += 4.0f;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            num_portals++;
            portals[num_portals-1] = mousePortal;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(backgroundTexture, 0, 0, WHITE);
            

            // DrawLineEx(playerPosition, portal.point1, 1.0f, BLACK);
            // DrawLineEx(playerPosition, portal.point2, 1.0f, BLACK);
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

                // DrawLineEx(playerPosition, portals[i].point1, 1.0f, BLACK);
                // DrawLineEx(playerPosition, portals[i].point2, 1.0f, BLACK);
                DrawLineEx(portals[i].point1, portals[i].point2, 3.0f, BLUE);

                Vector2 points[4] = {portals[i].point1, portals[i].point2, line2, line1};
                DrawTriangleFan(points, 4, GetColor(0xdd773380));
                Vector2 points2[4] = {portals[i].point2, portals[i].point1, line1, line2};
                DrawTriangleFan(points2, 4, GetColor(0x4466ff80));
                
                // DrawCircleV(Vector2Scale(Vector2Add(portals[i].point1, portals[i].point2), 0.5f), portalSize/2, GetColor(0xdd333380));
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