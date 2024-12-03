#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 10;
const int PLAYER_LIVES = 3;
const float PLAYER_SPEED = 5.0f;
const float BULLET_SPEED = 7.0f;
const int BULLET_WIDTH = 5;
const int BULLET_HEIGHT = 10;
const int ENEMY_WIDTH = 40;
const int ENEMY_HEIGHT = 20;
const int ENEMY_ROWS = 3;
const int ENEMY_COLS = 8;
const int ENEMY_ATTACK_INTERVAL = 240;
const int MAX_ENEMY_BULLETS_START = 2;
const int BUNKER_WIDTH = 60;
const int BUNKER_HEIGHT = 30;
const int BUNKER_COUNT = 3;
const int BUNKER_MAX_HEALTH = 5;
const int BUNKER_DAMAGE = 1;

// Structs
struct Bullet {
    Rectangle rect;
    bool active;
    bool isEnemy;
};

struct Enemy {
    Rectangle rect;
    bool active;
    int cooldown;
};

struct Bunker {
    Rectangle rect;
    int health;
};

// Function to reset enemies and bunkers
void ResetGameEntities(std::vector<Enemy>& enemies, std::vector<Bunker>& bunkers) {
    enemies.clear();
    for (int i = 0; i < ENEMY_ROWS; i++) {
        for (int j = 0; j < ENEMY_COLS; j++) {
            enemies.push_back({
                { j * (ENEMY_WIDTH + 10) + 50, i * (ENEMY_HEIGHT + 10) + 50, ENEMY_WIDTH, ENEMY_HEIGHT },
                true,
                0
            });
        }
    }

    bunkers.clear();
    for (int i = 0; i < BUNKER_COUNT; i++) {
        bunkers.push_back({
            { SCREEN_WIDTH / (BUNKER_COUNT + 1) * (i + 1) - BUNKER_WIDTH / 2.0f, SCREEN_HEIGHT - 150, BUNKER_WIDTH, BUNKER_HEIGHT },
            BUNKER_MAX_HEALTH
        });
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders Clone");
    SetTargetFPS(60);

    // Game state variables
    Rectangle player = { SCREEN_WIDTH / 2.0f - PLAYER_WIDTH / 2.0f, SCREEN_HEIGHT - 50, PLAYER_WIDTH, PLAYER_HEIGHT };
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Bunker> bunkers;
    ResetGameEntities(enemies, bunkers);

    int lives = PLAYER_LIVES;
    int level = 1;
    int score = 0;
    int maxEnemyBullets = MAX_ENEMY_BULLETS_START;
    float enemySpeed = 1.0f;
    float enemyDirection = 2.0f;
    bool gameOver = false;
    bool victory = false;
    bool startScreen = true;

    srand(static_cast<unsigned int>(time(0)));

    while (!WindowShouldClose()) {
        // Start screen handling
        if (startScreen && IsKeyPressed(KEY_ENTER)) {
            startScreen = false;
        }

        // Game controls
        if (!gameOver && !victory && !startScreen) {
            // Player movement
            if (IsKeyDown(KEY_LEFT)) player.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_RIGHT)) player.x += PLAYER_SPEED;
            if (IsKeyPressed(KEY_SPACE)) {
                bullets.push_back({
                    { player.x + PLAYER_WIDTH / 2 - BULLET_WIDTH / 2, player.y - BULLET_HEIGHT, BULLET_WIDTH, BULLET_HEIGHT },
                    true,
                    false
                });
            }

            // Keep player within screen bounds
            if (player.x < 0.0f) {
                player.x = 0.0f;
            } else if (player.x > SCREEN_WIDTH - PLAYER_WIDTH) {
                player.x = SCREEN_WIDTH - PLAYER_WIDTH;
            }

            // Bullet updates
            for (auto& bullet : bullets) {
                bullet.rect.y += bullet.isEnemy ? BULLET_SPEED : -BULLET_SPEED;
                if (bullet.rect.y < 0 || bullet.rect.y > SCREEN_HEIGHT) {
                    bullet.active = false;
                }
            }

            // Enemy updates
            bool changeDirection = false;
            for (auto& enemy : enemies) {
                if (enemy.active) {
                    enemy.rect.x += enemyDirection * enemySpeed;
                    if (enemy.rect.x < 0 || enemy.rect.x + ENEMY_WIDTH > SCREEN_WIDTH) {
                        changeDirection = true;
                    }
                    if (enemy.rect.y + ENEMY_HEIGHT >= SCREEN_HEIGHT - PLAYER_HEIGHT) {
                        gameOver = true;
                    }
                    if (enemy.cooldown > 0) {
                        enemy.cooldown--;
                    } else if (rand() % 100 < 5 && std::count_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.isEnemy && b.active; }) < maxEnemyBullets) {
                        bullets.push_back({
                            { enemy.rect.x + ENEMY_WIDTH / 2 - BULLET_WIDTH / 2, enemy.rect.y + ENEMY_HEIGHT, BULLET_WIDTH, BULLET_HEIGHT },
                            true,
                            true
                        });
                        enemy.cooldown = ENEMY_ATTACK_INTERVAL + rand() % 60;
                    }
                }
            }
            if (changeDirection) {
                enemyDirection = -enemyDirection;
                for (auto& enemy : enemies) {
                    enemy.rect.y += ENEMY_HEIGHT;
                }
            }

            // Bullet collisions
            for (auto& bullet : bullets) {
                if (!bullet.active) continue;

                // Player bullets hitting enemies
                if (!bullet.isEnemy) {
                    for (auto& enemy : enemies) {
                        if (enemy.active && CheckCollisionRecs(bullet.rect, enemy.rect)) {
                            bullet.active = false;
                            enemy.active = false;
                            score += 10;
                        }
                    }
                }

                // Bullets hitting bunkers
                for (auto& bunker : bunkers) {
                    if (bunker.health > 0 && CheckCollisionRecs(bullet.rect, bunker.rect)) {
                        bullet.active = false;
                        bunker.health -= BUNKER_DAMAGE;
                    }
                }

                // Enemy bullets hitting player
                if (bullet.isEnemy && CheckCollisionRecs(bullet.rect, player)) {
                    lives--;
                    bullet.active = false;
                    if (lives <= 0) {
                        gameOver = true;
                    }
                }
            }

            // Check for victory
            if (std::all_of(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.active; })) {
                victory = true;
            }

            // Remove inactive bullets
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
        }

        // Level up handling
        if (victory && IsKeyPressed(KEY_ENTER)) {
            level++;
            enemySpeed += 0.5f;
            maxEnemyBullets += 1;
            ResetGameEntities(enemies, bunkers);
            victory = false;
        }

        // Restart game handling
        if (gameOver && IsKeyPressed(KEY_ENTER)) {
            lives = PLAYER_LIVES;
            level = 1;
            score = 0;
            enemySpeed = 1.0f;
            maxEnemyBullets = MAX_ENEMY_BULLETS_START;
            gameOver = false;
            ResetGameEntities(enemies, bunkers);
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        if (startScreen) {
            DrawText("Press ENTER to start", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to start", 20) / 2, SCREEN_HEIGHT / 2, 20, WHITE);
        } else if (gameOver) {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, SCREEN_HEIGHT / 2 - 80, 40, RED);
            DrawText(("Last Level: " + std::to_string(level)).c_str(), SCREEN_WIDTH / 2 - MeasureText(("Last Level: " + std::to_string(level)).c_str(), 20) / 2, SCREEN_HEIGHT / 2, 20, WHITE);
            DrawText(("Final Score: " + std::to_string(score)).c_str(), SCREEN_WIDTH / 2 - MeasureText(("Final Score: " + std::to_string(score)).c_str(), 20) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
            DrawText("Press ENTER to restart", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to restart", 20) / 2, SCREEN_HEIGHT / 2 + 80, 20, WHITE);
        } else if (victory) {
            DrawText(("Level " + std::to_string(level) + " Complete!").c_str(), SCREEN_WIDTH / 2 - MeasureText(("Level " + std::to_string(level) + " Complete!").c_str(), 20) / 2, SCREEN_HEIGHT / 2, 20, GREEN);
            DrawText("Press ENTER for next level", SCREEN_WIDTH / 2 - MeasureText("Press ENTER for next level", 20) / 2, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
        } else {
            DrawRectangleRec(player, WHITE);
            for (const auto& bullet : bullets) {
                DrawRectangleRec(bullet.rect, bullet.isEnemy ? YELLOW : RED);
            }
            for (const auto& enemy : enemies) {
                if (enemy.active) {
                    DrawRectangleRec(enemy.rect, GREEN);
                }
            }
            for (const auto& bunker : bunkers) {
                if (bunker.health > 0) {
                    DrawRectangleRec(bunker.rect, DARKGRAY);
                    DrawText(std::to_string(bunker.health).c_str(), bunker.rect.x + BUNKER_WIDTH / 2 - 10, bunker.rect.y - 15, 20, WHITE);
                }
            }
            DrawText(("Score: " + std::to_string(score)).c_str(), 10, 10, 20, WHITE);
            DrawText(("Lives: " + std::to_string(lives)).c_str(), SCREEN_WIDTH - 100, 10, 20, WHITE);
            DrawText(("Level: " + std::to_string(level)).c_str(), SCREEN_WIDTH / 2 - 30, 10, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}