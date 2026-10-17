#include "raylib.h"
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_SIZE 32
#define COIN_SIZE 18
#define TARGET_SCORE 100
#define GAME_TIME 200.0f

#define JOYSTICK_RADIUS 65.0f
#define JOYSTICK_KNOB_RADIUS 27.0f
#define JOYSTICK_MARGIN 95.0f
#define PLAYER_SPEED 240.0f

typedef struct {
    Vector2 center;
    Vector2 knob;
    float radius;
    bool active;
    int touchId;
} VirtualJoystick;

static void SpawnCoin(Rectangle *coin, Rectangle player)
{
    for (int tries = 0; tries < 1000; tries++)
    {
        coin->x = (float)GetRandomValue(20, SCREEN_WIDTH - COIN_SIZE - 20);
        coin->y = (float)GetRandomValue(70, SCREEN_HEIGHT - COIN_SIZE - 20);
        coin->width = COIN_SIZE;
        coin->height = COIN_SIZE;

        if (!CheckCollisionRecs(*coin, player)) return;
    }

    coin->x = SCREEN_WIDTH / 2.0f;
    coin->y = SCREEN_HEIGHT / 2.0f;
}

static Vector2 ClampToCircle(Vector2 center, Vector2 point, float radius)
{
    Vector2 delta = {
        point.x - center.x,
        point.y - center.y
    };

    float length = sqrtf(delta.x * delta.x + delta.y * delta.y);

    if (length > radius && length > 0.0f)
    {
        delta.x = delta.x / length * radius;
        delta.y = delta.y / length * radius;
    }

    return (Vector2) {
        center.x + delta.x,
        center.y + delta.y
    };
}

static void ResetGame(
    Rectangle *player,
    Rectangle *coin,
    int *score,
    float *timer,
    bool *gameOver,
    bool *won)
{
    *player = (Rectangle) {
        SCREEN_WIDTH / 2.0f - PLAYER_SIZE / 2.0f,
        SCREEN_HEIGHT / 2.0f - PLAYER_SIZE / 2.0f,
        PLAYER_SIZE,
        PLAYER_SIZE
    };

    *score = 0;
    *timer = GAME_TIME;
    *gameOver = false;
    *won = false;

    SpawnCoin(coin, *player);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Coin Collector");
    SetTargetFPS(60);

    // Mobile devices can use the whole screen without a desktop window.
    // Keep the virtual game world at 800x600.
    SetConfigFlags(FLAG_VSYNC_HINT);

    Rectangle player = {
        SCREEN_WIDTH / 2.0f - PLAYER_SIZE / 2.0f,
        SCREEN_HEIGHT / 2.0f - PLAYER_SIZE / 2.0f,
        PLAYER_SIZE,
        PLAYER_SIZE
    };

    Rectangle coin = { 100, 100, COIN_SIZE, COIN_SIZE };

    int score = 0;
    float timer = GAME_TIME;
    bool gameOver = false;
    bool won = false;

    VirtualJoystick joystick = { 0 };
    joystick.center = (Vector2) {
        JOYSTICK_MARGIN,
        SCREEN_HEIGHT - JOYSTICK_MARGIN
    };
    joystick.knob = joystick.center;
    joystick.radius = JOYSTICK_RADIUS;
    joystick.touchId = -1;

    SpawnCoin(&coin, player);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ---------- Touch joystick ----------
        Vector2 movement = { 0.0f, 0.0f };

        int touchCount = GetTouchPointCount();

        if (!gameOver && !won)
        {
            if (touchCount > 0)
            {
                for (int i = 0; i < touchCount; i++)
                {
                    Vector2 touch = GetTouchPosition(i);
                    int id = GetTouchPointId(i);

                    // Claim a touch when it begins inside the joystick area.
                    if (!joystick.active)
                    {
                        float dx = touch.x - joystick.center.x;
                        float dy = touch.y - joystick.center.y;

                        if (dx * dx + dy * dy <=
                            (JOYSTICK_RADIUS * 1.8f) *
                            (JOYSTICK_RADIUS * 1.8f))
                        {
                            joystick.active = true;
                            joystick.touchId = id;
                        }
                    }

                    if (joystick.active && id == joystick.touchId)
                    {
                        joystick.knob = ClampToCircle(
                            joystick.center,
                            touch,
                            joystick.radius
                        );

                        movement.x =
                            (joystick.knob.x - joystick.center.x) /
                            joystick.radius;

                        movement.y =
                            (joystick.knob.y - joystick.center.y) /
                            joystick.radius;
                    }
                }

                // Release joystick when its finger is lifted.
                bool trackedFingerFound = false;

                for (int i = 0; i < touchCount; i++)
                {
                    if (GetTouchPointId(i) == joystick.touchId)
                    {
                        trackedFingerFound = true;
                        break;
                    }
                }

                if (joystick.active && !trackedFingerFound)
                {
                    joystick.active = false;
                    joystick.touchId = -1;
                    joystick.knob = joystick.center;
                }
            }
            else
            {
                joystick.active = false;
                joystick.touchId = -1;
                joystick.knob = joystick.center;
            }

            // ---------- Keyboard fallback for desktop ----------
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) movement.x += 1.0f;
            if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) movement.x -= 1.0f;
            if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) movement.y += 1.0f;
            if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) movement.y -= 1.0f;

            // Normalize diagonal movement.
            float length = sqrtf(
                movement.x * movement.x +
                movement.y * movement.y
            );

            if (length > 1.0f)
            {
                movement.x /= length;
                movement.y /= length;
            }

            player.x += movement.x * PLAYER_SPEED * dt;
            player.y += movement.y * PLAYER_SPEED * dt;

            // Keep the player inside the game world.
            if (player.x < 0) player.x = 0;
            if (player.y < 0) player.y = 0;

            if (player.x + player.width > SCREEN_WIDTH)
                player.x = SCREEN_WIDTH - player.width;

            if (player.y + player.height > SCREEN_HEIGHT)
                player.y = SCREEN_HEIGHT - player.height;

            // ---------- Collect coins ----------
            if (CheckCollisionRecs(player, coin))
            {
                score++;

                if (score >= TARGET_SCORE)
                {
                    won = true;
                    gameOver = true;
                }
                else
                {
                    SpawnCoin(&coin, player);
                }
            }

            // ---------- Timer ----------
            timer -= dt;

            if (timer <= 0.0f)
            {
                timer = 0.0f;
                gameOver = true;
            }
        }

        // ---------- Restart ----------
        if (gameOver && (IsKeyPressed(KEY_ENTER) || GetTouchPointCount() > 0))
        {
            ResetGame(
                &player, &coin, &score, &timer, &gameOver, &won
            );

            joystick.active = false;
            joystick.touchId = -1;
            joystick.knob = joystick.center;
        }

        // ---------- Draw ----------
        BeginDrawing();
        ClearBackground((Color){ 22, 25, 35, 255 });

        DrawText("COIN COLLECTOR", 20, 16, 28, RAYWHITE);

        DrawText(
            TextFormat("Coins: %d / %d", score, TARGET_SCORE),
            20, 55, 20, GOLD
        );

        DrawText(
            TextFormat("Time: %d", (int)ceilf(timer)),
            SCREEN_WIDTH - 150, 20, 24,
            timer <= 30.0f ? RED : RAYWHITE
        );

        DrawLine(20, 90, SCREEN_WIDTH - 20, 90, DARKGRAY);

        if (!gameOver)
        {
            // Coin
            DrawCircle(
                (int)(coin.x + COIN_SIZE / 2.0f),
                (int)(coin.y + COIN_SIZE / 2.0f),
                COIN_SIZE / 2.0f,
                GOLD
            );

            DrawCircleLines(
                (int)(coin.x + COIN_SIZE / 2.0f),
                (int)(coin.y + COIN_SIZE / 2.0f),
                COIN_SIZE / 2.0f,
                ORANGE
            );

            // Player
            DrawRectangleRounded(player, 0.25f, 6, SKYBLUE);
            DrawRectangleRoundedLines(player, 0.25f, 6, WHITE);

            // Virtual joystick base
            DrawCircleV(
                joystick.center,
                JOYSTICK_RADIUS,
                (Color){ 180, 190, 210, 65 }
            );

            DrawCircleLines(
                (int)joystick.center.x,
                (int)joystick.center.y,
                JOYSTICK_RADIUS,
                (Color){ 220, 230, 245, 180 }
            );

            // Joystick knob
            DrawCircleV(
                joystick.knob,
                JOYSTICK_KNOB_RADIUS,
                (Color){ 90, 170, 255, 220 }
            );

            DrawCircleLines(
                (int)joystick.knob.x,
                (int)joystick.knob.y,
                JOYSTICK_KNOB_RADIUS,
                RAYWHITE
            );

            DrawText(
                "MOVE",
                (int)joystick.center.x - 25,
                (int)joystick.center.y + JOYSTICK_RADIUS + 10,
                16,
                LIGHTGRAY
            );
        }
        else
        {
            const char *title = won ? "YOU WIN!" : "TIME'S UP!";
            Color titleColor = won ? LIME : RED;

            int titleWidth = MeasureText(title, 48);

            DrawText(
                title,
                SCREEN_WIDTH / 2 - titleWidth / 2,
                SCREEN_HEIGHT / 2 - 70,
                48,
                titleColor
            );

            DrawText(
                TextFormat("Final score: %d / %d", score, TARGET_SCORE),
                SCREEN_WIDTH / 2 - 115,
                SCREEN_HEIGHT / 2,
                22,
                RAYWHITE
            );

            DrawText(
                "Touch the screen or press ENTER to restart",
                SCREEN_WIDTH / 2 - 220,
                SCREEN_HEIGHT / 2 + 45,
                18,
                LIGHTGRAY
            );
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
