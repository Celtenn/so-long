#include "minilibx-linux/mlx.h"
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600
#define PLAYER_SIZE 50
#define WALL_COUNT 4

// Oyuncu yapısı
typedef struct {
    int x;
    int y;
} Player;

// Duvar yapısı
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Wall;

typedef struct {
    void *mlx;
    void *win;
    Player player;
    Wall walls[WALL_COUNT];
} Game;

// Çakışma kontrolü
int check_collision(Player *player, Wall *walls) {
    for (int i = 0; i < WALL_COUNT; i++) {
        if (player->x < walls[i].x + walls[i].width &&
            player->x + PLAYER_SIZE > walls[i].x &&
            player->y < walls[i].y + walls[i].height &&
            player->y + PLAYER_SIZE > walls[i].y) {
            return 1;
        }
    }
    return 0;
}

// Ekranı güncelleme fonksiyonu
void render(Game *game) {
    // Ekranı temizle
    mlx_clear_window(game->mlx, game->win);
    
    // Duvarları çiz
    for (int i = 0; i < WALL_COUNT; i++) {
        for (int x = 0; x < game->walls[i].width; x++) {
            for (int y = 0; y < game->walls[i].height; y++) {
                mlx_pixel_put(game->mlx, game->win, game->walls[i].x + x, game->walls[i].y + y, 0xFF0000);
            }
        }
    }
    
    // Oyuncuyu çiz (Beyaz kare)
    int x = game->player.x;
    int y = game->player.y;
    int size = PLAYER_SIZE;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            mlx_pixel_put(game->mlx, game->win, x + i, y + j, 0xFFFFFF);
        }
    }
}

// Klavye girişlerini işleme
int key_hook(int keycode, Game *game) {
    int old_x = game->player.x;
    int old_y = game->player.y;
    
    if (keycode == 65307) // ESC tuşu
        exit(0);
    if (keycode == 65361) // Sol ok
        game->player.x -= 10;
    if (keycode == 65363) // Sağ ok
        game->player.x += 10;
    if (keycode == 65362) // Yukarı ok
        game->player.y -= 10;
    if (keycode == 65364) // Aşağı ok
        game->player.y += 10;
    
    if (check_collision(&game->player, game->walls)) {
        game->player.x = old_x;
        game->player.y = old_y;
    }
    
    render(game);
    return 0;
}

int main() {
    Game game;
    game.mlx = mlx_init();
    game.win = mlx_new_window(game.mlx, WIDTH, HEIGHT, "MLX Game");
    game.player.x = WIDTH / 2;
    game.player.y = HEIGHT / 2;
    
    // Duvarları tanımla
    game.walls[0] = (Wall){100, 100, 200, 20};
    game.walls[1] = (Wall){300, 200, 20, 200};
    game.walls[2] = (Wall){500, 100, 150, 20};
    game.walls[3] = (Wall){600, 400, 20, 150};
    
    render(&game);
    
    mlx_key_hook(game.win, key_hook, &game);
    mlx_loop(game.mlx);
    return 0;
}
