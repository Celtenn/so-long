#include "minilibx-linux/mlx.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define PLAYER_SIZE 50
#define WALL_COUNT 4
#define ITEM_COUNT 5
#define PLAYER_SPEED 0.3

// Oyuncu yapısı
typedef struct {
    double x;
    double y;
    int size;
    int start_x;
    int start_y;
    double dx;
    double dy;
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
    void *img;
    char *img_data;
    int bpp;
    int size_line;
    int endian;
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

// Oyuncuyu başlangıç noktasına döndür ve nesneleri yeniden oluştur
void reset_player(Game *game) {
    game->player.x = game->player.start_x;
    game->player.y = game->player.start_y;
    game->player.size = PLAYER_SIZE;
    game->player.dx = PLAYER_SPEED;
    game->player.dy = 0;
    
    // Toplanabilir nesneleri yeniden oluştur
    for (int i = 0; i < ITEM_COUNT; i++) {
        game->items[i].x = rand() % (WIDTH - PLAYER_SIZE);
        game->items[i].y = rand() % (HEIGHT - PLAYER_SIZE);
        game->items[i].collected = 0;
    }
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
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int pixel = (y * game->size_line) + (x * (game->bpp / 8));
            game->img_data[pixel] = 0;       // Mavi
            game->img_data[pixel + 1] = 0;   // Yeşil
            game->img_data[pixel + 2] = 0;   // Kırmızı
        }
    }
    
    // Duvarları çiz
    for (int i = 0; i < WALL_COUNT; i++) {
        for (int x = 0; x < game->walls[i].width; x++) {
            for (int y = 0; y < game->walls[i].height; y++) {
                int px = game->walls[i].x + x;
                int py = game->walls[i].y + y;
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    int pixel = (py * game->size_line) + (px * (game->bpp / 8));
                    game->img_data[pixel] = 0;       // Mavi
                    game->img_data[pixel + 1] = 0;   // Yeşil
                    game->img_data[pixel + 2] = 139; // Kırmızı (8B0000)
                }
            }
        }
    }
    
    // Toplanabilir nesneleri çiz
    for (int i = 0; i < ITEM_COUNT; i++) {
        if (!game->items[i].collected) {
            for (int x = 0; x < PLAYER_SIZE; x++) {
                for (int y = 0; y < PLAYER_SIZE; y++) {
                    int px = game->items[i].x + x;
                    int py = game->items[i].y + y;
                    if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                        int pixel = (py * game->size_line) + (px * (game->bpp / 8));
                        game->img_data[pixel] = 0;       // Mavi
                        game->img_data[pixel + 1] = 255; // Yeşil (00FF00)
                        game->img_data[pixel + 2] = 0;   // Kırmızı
                    }
                }
            }
        }
    }
    
    // Oyuncuyu çiz (Beyaz kare)
    for (int x = 0; x < game->player.size; x++) {
        for (int y = 0; y < game->player.size; y++) {
            int px = game->player.x + x;
            int py = game->player.y + y;
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                int pixel = (py * game->size_line) + (px * (game->bpp / 8));
                game->img_data[pixel] = 255;     // Mavi
                game->img_data[pixel + 1] = 255; // Yeşil
                game->img_data[pixel + 2] = 255; // Kırmızı (FFFFFF)
            }
        }
    }
    
    // Görüntüyü pencereye çiz
    mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
}

// Klavye girişlerini işleme
int key_hook(int keycode, Game *game) {
    if (keycode == 65307) // ESC tuşu
        exit(0);
    if (keycode == 65361) { // Sol ok
        game->player.dx = -PLAYER_SPEED;
        game->player.dy = 0;
    }
    if (keycode == 65363) { // Sağ ok
        game->player.dx = PLAYER_SPEED;
        game->player.dy = 0;
    }
    if (keycode == 65362) { // Yukarı ok
        game->player.dx = 0;
        game->player.dy = -PLAYER_SPEED;
    }
    if (keycode == 65364) { // Aşağı ok
        game->player.dx = 0;
        game->player.dy = PLAYER_SPEED;
    }
    return 0;
}

// Oyuncu hareket fonksiyonu
int update(Game *game) {
    int old_x = game->player.x;
    int old_y = game->player.y;
    
    game->player.x += game->player.dx;
    game->player.y += game->player.dy;
    
    // Ekran sınırlarını kontrol et
    if (game->player.x < 0) game->player.x = 0;
    if (game->player.x + game->player.size > WIDTH) game->player.x = WIDTH - game->player.size;
    if (game->player.y < 0) game->player.y = 0;
    if (game->player.y + game->player.size > HEIGHT) game->player.y = HEIGHT - game->player.size;
    
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
    srand(time(NULL));
    Game game;
    game.mlx = mlx_init();
    game.win = mlx_new_window(game.mlx, WIDTH, HEIGHT, "MLX Game");
    game.img = mlx_new_image(game.mlx, WIDTH, HEIGHT);
    game.img_data = mlx_get_data_addr(game.img, &game.bpp, &game.size_line, &game.endian);
    
    game.player.x = WIDTH / 2;
    game.player.y = HEIGHT / 2;
    game.player.start_x = WIDTH / 2;
    game.player.start_y = HEIGHT / 2;
    game.player.size = PLAYER_SIZE;
    game.player.dx = PLAYER_SPEED;
    game.player.dy = 0;
    
    // Duvarları tanımla
    game.walls[0] = (Wall){100, 100, 200, 20};
    game.walls[1] = (Wall){300, 200, 20, 200};
    game.walls[2] = (Wall){500, 100, 150, 20};
    game.walls[3] = (Wall){600, 400, 20, 150};
    
    // Toplanabilir nesneleri tanımla
    for (int i = 0; i < ITEM_COUNT; i++) {
        game.items[i].x = rand() % (WIDTH - PLAYER_SIZE);
        game.items[i].y = rand() % (HEIGHT - PLAYER_SIZE);
        game.items[i].collected = 0;
    }
    
    render(&game);
    mlx_key_hook(game.win, key_hook, &game);
    mlx_loop_hook(game.mlx, update, &game);
    mlx_loop(game.mlx);
    return 0;
}
