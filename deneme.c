#include "minilibx-linux/mlx.h"
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600
#define PLAYER_SIZE 50
#define WALL_COUNT 4
#define ITEM_COUNT 5

// Oyuncu yapısı
typedef struct {
    int x;
    int y;
    int size;
    int start_x;
    int start_y;
} Player;

// Duvar yapısı
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Wall;

// Toplanabilir nesne yapısı
typedef struct {
    int x;
    int y;
    int collected;
} Item;

typedef struct {
    void *mlx;
    void *win;
    Player player;
    Wall walls[WALL_COUNT];
    Item items[ITEM_COUNT];
} Game;

// Çakışma kontrolü
int check_collision(Player *player, Wall *walls) {
    for (int i = 0; i < WALL_COUNT; i++) {
        if (player->x < walls[i].x + walls[i].width &&
            player->x + player->size > walls[i].x &&
            player->y < walls[i].y + walls[i].height &&
            player->y + player->size > walls[i].y) {
            return 1;
        }
    }
    return 0;
}

// Oyuncuyu başlangıç noktasına döndür
void reset_player(Game *game) {
    game->player.x = game->player.start_x;
    game->player.y = game->player.start_y;
    game->player.size = PLAYER_SIZE;
}

// Toplanabilir nesne kontrolü
void check_items(Game *game) {
    for (int i = 0; i < ITEM_COUNT; i++) {
        if (!game->items[i].collected &&
            game->player.x < game->items[i].x + PLAYER_SIZE &&
            game->player.x + game->player.size > game->items[i].x &&
            game->player.y < game->items[i].y + PLAYER_SIZE &&
            game->player.y + game->player.size > game->items[i].y) {
            
            game->items[i].collected = 1;
            game->player.size += 10;
        }
    }
}

// Ekranı güncelleme fonksiyonu
void render(Game *game) {
    // Ekranı temizle
    mlx_clear_window(game->mlx, game->win);
    
    // Duvarları çiz (Dikenli)
    for (int i = 0; i < WALL_COUNT; i++) {
        for (int x = 0; x < game->walls[i].width; x++) {
            for (int y = 0; y < game->walls[i].height; y++) {
                mlx_pixel_put(game->mlx, game->win, game->walls[i].x + x, game->walls[i].y + y, 0xFF0000);
            }
        }
    }
    
    // Toplanabilir nesneleri çiz
    for (int i = 0; i < ITEM_COUNT; i++) {
        if (!game->items[i].collected) {
            for (int x = 0; x < PLAYER_SIZE; x++) {
                for (int y = 0; y < PLAYER_SIZE; y++) {
                    mlx_pixel_put(game->mlx, game->win, game->items[i].x + x, game->items[i].y + y, 0x00FF00);
                }
            }
        }
    }
    
    // Oyuncuyu çiz (Beyaz kare)
    int x = game->player.x;
    int y = game->player.y;
    int size = game->player.size;
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
        reset_player(game);
    }
    
    check_items(game);
    render(game);
    return 0;
}

int main() {
    Game game;
    game.mlx = mlx_init();
    game.win = mlx_new_window(game.mlx, WIDTH, HEIGHT, "MLX Game");
    game.player.x = WIDTH / 2;
    game.player.y = HEIGHT / 2;
    game.player.start_x = WIDTH / 2;
    game.player.start_y = HEIGHT / 2;
    game.player.size = PLAYER_SIZE;
    
    // Duvarları tanımla
    game.walls[0] = (Wall){100, 100, 200, 20};
    game.walls[1] = (Wall){300, 200, 20, 200};
    game.walls[2] = (Wall){500, 100, 150, 20};
    game.walls[3] = (Wall){600, 400, 20, 150};
    
    // Toplanabilir nesneleri tanımla
    game.items[0] = (Item){150, 150, 0};
    game.items[1] = (Item){400, 300, 0};
    game.items[2] = (Item){550, 200, 0};
    game.items[3] = (Item){650, 450, 0};
    game.items[4] = (Item){300, 500, 0};
    
    render(&game);
    
    mlx_key_hook(game.win, key_hook, &game);
    mlx_loop(game.mlx);
    return 0;
}
