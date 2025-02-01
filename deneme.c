#include "minilibx-linux/mlx.h"
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600
#define PLAYER_SIZE 50

// Oyuncu yapısı
typedef struct {
    int x;
    int y;
} Player;

typedef struct {
    void *mlx;
    void *win;
    Player player;
} Game;

// Ekranı güncelleme fonksiyonu
void render(Game *game) {
    // Ekranı temizle
    mlx_clear_window(game->mlx, game->win);
    
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
    
    render(game);
    return 0;
}

int main() {
    Game game;
    game.mlx = mlx_init();
    game.win = mlx_new_window(game.mlx, WIDTH, HEIGHT, "MLX Game");
    game.player.x = WIDTH / 2;
    game.player.y = HEIGHT / 2;
    
    render(&game);
    
    mlx_key_hook(game.win, key_hook, &game);
    mlx_loop(game.mlx);
    return 0;
}
