/* ----------------------------------------------------------------------------------------------------------------
	Importation des bibliothèques
---------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>



/* ----------------------------------------------------------------------------------------------------------------
	Variables globales et structures
---------------------------------------------------------------------------------------------------------------- */

#define WINDOW_WIDTH 1236
#define WINDOW_HEIGHT 900
#define CARD_WIDTH_HEIGHT 206
#define NAME_WIDTH 440
#define LABEL_HEIGHT 69
#define MARGIN_TOP_1 244
#define MARGIN_TOP_2 141
#define MARGIN_TOP_3 38
#define BOUTON_BW_HEIGHT 50

// Structure carte
typedef struct {
    SDL_Texture *card_image;
    int card_number, card_revealed, card_x_1, card_y_1, card_x_2, card_y_2, card_positioned;
} Card;



/* ----------------------------------------------------------------------------------------------------------------
	Déclaration des fonctions
---------------------------------------------------------------------------------------------------------------- */

int memory_game();
int SDL_exit_with_error(const char *message);
void SDL_destroy_window_renderer(SDL_Renderer *renderer, SDL_Window *window);
void SDL_clear_name_texture(SDL_Texture *texture_name);
void SDL_clear_bravo_texture(SDL_Texture *texture_bravo);
void SDL_clear_theme_texture(SDL_Texture *texture_animal_theme, SDL_Texture *texture_pastry_theme, SDL_Texture *texture_painting_theme);
void SDL_clear_level_texture(SDL_Texture *texture_level_1, SDL_Texture *texture_level_2, SDL_Texture *texture_level_3);
void SDL_clear_back_texture(SDL_Texture *texture_back);
void SDL_clear_renderer(SDL_Renderer *renderer);
void SDL_texture_renderer(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture, int x, int y);
void SDL_all_cards_display(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture **textures_group, SDL_Texture *texture, Card *cards, int game_board_size);
void SDL_card_display(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture, Card card, int x, int y);
void create_cards(Card *cards, SDL_Texture **textures);
void position_cards(Card *cards, int rows, int game_board[rows][6], int selected_level);
void first_card_selection(int x, int y, Card *cards, SDL_Texture **textures_group, int *first_selection, int *first_selection_x, int *first_selection_y, int game_board_size, SDL_Renderer *renderer, SDL_Window *window);
void second_card_selection(int x, int y, Card *cards, SDL_Texture **textures_group, int *second_selection, int first_selection_x, int first_selection_y, int game_board_size, SDL_Renderer *renderer, SDL_Window *window);
void cards_verification(Card *cards, int *second_selection, int *first_selection, int game_board_size, SDL_Renderer *renderer, SDL_Window *window, SDL_Texture **textures_group, SDL_Texture *texture);
int in_zone(int x, int y, int x_min, int x_max, int y_min, int y_max);
void shuffle(int rows, int columns, int game_board[rows][columns]);
int has_won(int game_board_size, Card *cards);
int end_game(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture_bravo, int bravo_x, int bravo_y, SDL_Texture **textures_animal, SDL_Texture **textures_pastry, SDL_Texture **textures_painting);
void save_game_data(Card *cards, int rows, int game_board[rows][6], int selected_level, int selected_theme);
void load_game_data(Card *animal_cards, Card *pastry_cards, Card *painting_cards, int *game_board_size, int game_board_level_1[2][6], int game_board_level_2[3][6], int game_board_level_3[4][6], int *selected_level, int *selected_theme);
void save_game_state(int game_finished);
void load_game_state(int *game_finished);
void change_mode(int dark_clear,SDL_Renderer *renderer);
int decrypt();
void encrypt(int dark_clear);


/* ----------------------------------------------------------------------------------------------------------------
	Fonction main
---------------------------------------------------------------------------------------------------------------- */

// Lancement de la fonction principale permettant de jouer
int main(int argc, char **argv) {
	if(!memory_game()) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}



/* ----------------------------------------------------------------------------------------------------------------
	Définition des fonctions
---------------------------------------------------------------------------------------------------------------- */

// Fonction principale de jeu
int memory_game()
{
	// Initialisation générateur nombre aléatoire
	srand(time(NULL));

	// Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_exit_with_error("initialisation SDL");
    }

	
	// Création icone
	SDL_Surface *memory_icon = NULL;
	memory_icon = SDL_LoadBMP("src/images/icone.bmp");
	if (memory_icon == NULL) {
        SDL_exit_with_error("chargement icone");
    }


	// Création de la fenêtre et du rendu
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0) {
        SDL_exit_with_error("creation fenetre et rendu");
    }
	SDL_SetWindowIcon(window, memory_icon);
	SDL_SetWindowTitle(window, "Memory Matching Game");


	// Couleur de fond
	int dark_clear = decrypt();
    change_mode(dark_clear,renderer);
	SDL_RenderClear(renderer);


	// Chargement nom du jeu
    SDL_Surface *image_name = NULL;
    image_name = SDL_LoadBMP("src/images/game_name.bmp");
    if (image_name == NULL) {
		SDL_FreeSurface(memory_icon);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image nom");
    }
	SDL_Texture *texture_name = NULL;
    texture_name = SDL_CreateTextureFromSurface(renderer, image_name);
    SDL_FreeSurface(image_name);
    if (texture_name == NULL) {
		SDL_FreeSurface(memory_icon);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture nom");
    }
	int name_x = (WINDOW_WIDTH / 2) - (NAME_WIDTH / 2);
    int name_y = (WINDOW_HEIGHT / 2) - 200;


	// Chargement bravo
    SDL_Surface *image_bravo = NULL;
    image_bravo = SDL_LoadBMP("src/images/bravo.bmp");
    if (image_bravo == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image bravo");
    }
	SDL_Texture *texture_bravo = NULL;
    texture_bravo = SDL_CreateTextureFromSurface(renderer, image_bravo);
    SDL_FreeSurface(image_bravo);
    if (texture_bravo == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture bravo");
    }
	int bravo_x = (WINDOW_WIDTH / 2) - 300;
    int bravo_y = (WINDOW_HEIGHT / 2) - 300;


	// Chargement image dos de la carte
	SDL_Surface *image_back = NULL;
	image_back = SDL_LoadBMP("src/images/back.bmp");
	if (image_back == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image couverture carte");
    }
	SDL_Texture *texture_back = NULL;
	texture_back = SDL_CreateTextureFromSurface(renderer, image_back);
	SDL_FreeSurface(image_back);
	if (texture_back == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture couverture carte");
    }


	// Choix du theme et niveau
	int selected_theme = 0;
	int selected_level = 0;


	// Chargement image thème Animaux
	SDL_Surface *image_animal_theme = NULL;
	image_animal_theme = SDL_LoadBMP("src/images/animals/2.bmp");
	if (image_animal_theme == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image theme animaux");
    }
	SDL_Texture *texture_animal_theme = NULL;
	texture_animal_theme = SDL_CreateTextureFromSurface(renderer, image_animal_theme);
	SDL_FreeSurface(image_animal_theme);
	if (texture_animal_theme == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture theme animaux");
    }
	int animal_theme_x = (WINDOW_WIDTH / 2) - 344;
	int animal_theme_y = (WINDOW_HEIGHT / 2) - 50;


	// Chargement image thème Pâtisseries
	SDL_Surface *image_pastry_theme = NULL;
	image_pastry_theme = SDL_LoadBMP("src/images/pastries/1.bmp");
	if (image_pastry_theme == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_DestroyTexture(texture_animal_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image theme patisseries");
    }
	SDL_Texture *texture_pastry_theme = NULL;
	texture_pastry_theme = SDL_CreateTextureFromSurface(renderer, image_pastry_theme);
	SDL_FreeSurface(image_pastry_theme);
	if (texture_pastry_theme == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_DestroyTexture(texture_animal_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture theme patisseries");
    }
	int pastry_theme_x = (WINDOW_WIDTH / 2) - (CARD_WIDTH_HEIGHT / 2);
	int pastry_theme_y = (WINDOW_HEIGHT / 2) - 50;


	// Chargement image thème Peintures
	SDL_Surface *image_painting_theme = NULL;
	image_painting_theme = SDL_LoadBMP("src/images/paintings/3.bmp");
	if (image_painting_theme == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_DestroyTexture(texture_animal_theme);
		SDL_DestroyTexture(texture_pastry_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image theme peintures");
    }
	SDL_Texture *texture_painting_theme = NULL;
	texture_painting_theme = SDL_CreateTextureFromSurface(renderer, image_painting_theme);
	SDL_FreeSurface(image_painting_theme);
	if (texture_painting_theme == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_DestroyTexture(texture_animal_theme);
		SDL_DestroyTexture(texture_pastry_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture theme peintures");
    }
	int painting_theme_x = (WINDOW_WIDTH / 2) + 133;
	int painting_theme_y = (WINDOW_HEIGHT / 2) - 50;


	// Chargement bouton niveau 1
    SDL_Surface *image_level_1 = NULL;
    image_level_1 = SDL_LoadBMP("src/images/level1.bmp");
    if (image_level_1 == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image niveau 1");
    }
	SDL_Texture *texture_level_1 = NULL;
    texture_level_1 = SDL_CreateTextureFromSurface(renderer, image_level_1);
    SDL_FreeSurface(image_level_1);
    if (texture_level_1 == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture niveau 1");
    }
	int level_1_x = animal_theme_x;
    int level_1_y = animal_theme_y + CARD_WIDTH_HEIGHT;


	// Chargement bouton niveau 2
    SDL_Surface *image_level_2 = NULL;
    image_level_2 = SDL_LoadBMP("src/images/level2.bmp");
    if (image_level_2 == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_DestroyTexture(texture_level_1);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image niveau 2");
    }
	SDL_Texture *texture_level_2 = NULL;
    texture_level_2 = SDL_CreateTextureFromSurface(renderer, image_level_2);
    SDL_FreeSurface(image_level_2);
    if (texture_level_2 == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_DestroyTexture(texture_level_1);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture niveau 2");
    }
	int level_2_x = pastry_theme_x;
    int level_2_y = pastry_theme_y + CARD_WIDTH_HEIGHT;


	// Chargement bouton niveau 3
    SDL_Surface *image_level_3 = NULL;
    image_level_3 = SDL_LoadBMP("src/images/level3.bmp");
    if (image_level_3 == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_DestroyTexture(texture_level_1);
		SDL_DestroyTexture(texture_level_2);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image niveau 3");
    }
	SDL_Texture *texture_level_3 = NULL;
    texture_level_3 = SDL_CreateTextureFromSurface(renderer, image_level_3);
    SDL_FreeSurface(image_level_3);
    if (texture_level_3 == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_DestroyTexture(texture_level_1);
		SDL_DestroyTexture(texture_level_2);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture niveau 3");
    }
	int level_3_x = painting_theme_x;
    int level_3_y = painting_theme_y + CARD_WIDTH_HEIGHT;


    // Chargement bouton continue
	SDL_Surface *image_continue_button = NULL;
    image_continue_button = SDL_LoadBMP("src/images/continue.bmp");
    if (image_continue_button == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image bouton continue");
    }
	SDL_Texture *texture_continue_button = NULL;
    texture_continue_button = SDL_CreateTextureFromSurface(renderer, image_continue_button);
    SDL_FreeSurface(image_continue_button);
    if (texture_continue_button == NULL) {
		SDL_FreeSurface(memory_icon);
		SDL_clear_name_texture(texture_name);
		SDL_clear_bravo_texture(texture_bravo);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture bouton continue");
    }
	int continue_button_x = (WINDOW_WIDTH / 2) - (CARD_WIDTH_HEIGHT / 2);
	int continue_button_y = (WINDOW_HEIGHT / 2) + 300;

	// chargement bouton du dark theme 
	SDL_Surface *image_Sun = NULL;
	image_Sun = SDL_LoadBMP("src/images/sun.bmp");
	if (image_Sun == NULL) {
		SDL_destroy_window_renderer(renderer, window);
		SDL_exit_with_error("chargement image sun");
	}

	SDL_Texture *texture_Sun = NULL;
	texture_Sun = SDL_CreateTextureFromSurface(renderer, image_Sun);
	SDL_FreeSurface(image_Sun);
	if (texture_Sun == NULL) {
		SDL_destroy_window_renderer(renderer, window);
		SDL_exit_with_error("creation texture sun");
	}


	SDL_Surface *image_Moon = NULL;
	image_Moon = SDL_LoadBMP("src/images/moon.bmp");
	if (image_Moon == NULL) {
		SDL_destroy_window_renderer(renderer, window);
		SDL_exit_with_error("chargement image moon");
	}

	SDL_Texture *texture_Moon = NULL;
	texture_Moon = SDL_CreateTextureFromSurface(renderer, image_Moon);
	SDL_FreeSurface(image_Moon);
	if (texture_Moon == NULL) {
		SDL_destroy_window_renderer(renderer, window);
		SDL_exit_with_error("creation texture moon");
	}

	int Sun_Moon_x = WINDOW_WIDTH  - ((BOUTON_BW_HEIGHT / 2) * 3);
	int Sun_Moon_y = (WINDOW_HEIGHT / 4) - 100;



	// Matrices représentant le plateau de jeu
	int game_board_size = 0;

	int game_board_level_1[2][6] = {
		{1, 1, 2, 2, 3, 3},
		{4, 4, 5, 5, 6, 6}
	};

	int game_board_level_2[3][6] = {
		{1, 1, 2, 2, 3, 3},
		{4, 4, 5, 5, 6, 6},
		{7, 7, 8, 8, 9, 9}
	};

	int game_board_level_3[4][6] = {
		{1, 1, 2, 2, 3, 3},
		{4, 4, 5, 5, 6, 6},
		{7, 7, 8, 8, 9, 9},
		{10, 10, 11, 11, 12, 12}
	};


	// Chargement images des cartes thème Pâtisserie
	SDL_Texture *textures_animal[12];

	const char *path_animal[12] = {
    	"src/images/animals/1.bmp",
		"src/images/animals/2.bmp",
		"src/images/animals/3.bmp",
		"src/images/animals/4.bmp",
		"src/images/animals/5.bmp",
		"src/images/animals/6.bmp",
		"src/images/animals/7.bmp",
		"src/images/animals/8.bmp",
		"src/images/animals/9.bmp",
		"src/images/animals/10.bmp",
		"src/images/animals/11.bmp",
		"src/images/animals/12.bmp"
	};

	for (int i = 0; i < 12; i++) {
		SDL_Surface *image_animal = NULL;
		image_animal = SDL_LoadBMP(path_animal[i]);
		if (image_animal == NULL) {
			SDL_FreeSurface(memory_icon);
			SDL_clear_name_texture(texture_name);
			SDL_clear_bravo_texture(texture_bravo);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_animal[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("chargement image carte animal");
	    }

		textures_animal[i] = SDL_CreateTextureFromSurface(renderer, image_animal);
		SDL_FreeSurface(image_animal);
		if (textures_animal[i] == NULL) {
			SDL_FreeSurface(memory_icon);
			SDL_clear_name_texture(texture_name);
			SDL_clear_bravo_texture(texture_bravo);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_animal[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("creation texture carte animal");
		}
	}


	// Chargement images des cartes thème Pâtisserie
	SDL_Texture *textures_pastry[12];

	const char *path_pastry[12] = {
    	"src/images/pastries/1.bmp",
		"src/images/pastries/2.bmp",
		"src/images/pastries/3.bmp",
		"src/images/pastries/4.bmp",
		"src/images/pastries/5.bmp",
		"src/images/pastries/6.bmp",
		"src/images/pastries/7.bmp",
		"src/images/pastries/8.bmp",
		"src/images/pastries/9.bmp",
		"src/images/pastries/10.bmp",
		"src/images/pastries/11.bmp",
		"src/images/pastries/12.bmp"
	};

	for (int i = 0; i < 12; i++) {
		SDL_Surface *image_pastry = NULL;
		image_pastry = SDL_LoadBMP(path_pastry[i]);
		if (image_pastry == NULL) {
			SDL_FreeSurface(memory_icon);
			SDL_clear_name_texture(texture_name);
			SDL_clear_bravo_texture(texture_bravo);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			for (int x = 0; x < 12; x++) {
            	SDL_DestroyTexture(textures_animal[x]);
			}
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_pastry[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("chargement image carte pâtisserie");
	    }

		textures_pastry[i] = SDL_CreateTextureFromSurface(renderer, image_pastry);
		SDL_FreeSurface(image_pastry);
		if (textures_pastry[i] == NULL) {
			SDL_FreeSurface(memory_icon);
			SDL_clear_name_texture(texture_name);
			SDL_clear_bravo_texture(texture_bravo);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			for (int x = 0; x < 12; x++) {
            	SDL_DestroyTexture(textures_animal[x]);
			}
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_pastry[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("creation texture carte pâtisserie");
		}
	}


	// Chargement images des cartes thème Peintures
	SDL_Texture *textures_painting[12];

	const char *path_painting[12] = {
    	"src/images/paintings/1.bmp",
		"src/images/paintings/2.bmp",
		"src/images/paintings/3.bmp",
		"src/images/paintings/4.bmp",
		"src/images/paintings/5.bmp",
		"src/images/paintings/6.bmp",
		"src/images/paintings/7.bmp",
		"src/images/paintings/8.bmp",
		"src/images/paintings/9.bmp",
		"src/images/paintings/10.bmp",
		"src/images/paintings/11.bmp",
		"src/images/paintings/12.bmp"
	};

	for (int i = 0; i < 12; i++) {
		SDL_Surface *image_painting = NULL;
		image_painting = SDL_LoadBMP(path_painting[i]);
		if (image_painting == NULL) {
			SDL_FreeSurface(memory_icon);
			SDL_clear_name_texture(texture_name);
			SDL_clear_bravo_texture(texture_bravo);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			for (int x = 0; x < 12; x++) {
            	SDL_DestroyTexture(textures_animal[x]);
				SDL_DestroyTexture(textures_pastry[x]);
			}
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_painting[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("chargement image carte peinture");
	    }

		textures_painting[i] = SDL_CreateTextureFromSurface(renderer, image_painting);
		SDL_FreeSurface(image_painting);
		if (textures_pastry[i] == NULL) {
			SDL_FreeSurface(memory_icon);
			SDL_clear_name_texture(texture_name);
			SDL_clear_bravo_texture(texture_bravo);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			for (int x = 0; x < 12; x++) {
            	SDL_DestroyTexture(textures_animal[x]);
				SDL_DestroyTexture(textures_pastry[x]);
			}
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_painting[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("creation texture carte peinture");
		}
	}


	// Création des cartes
	Card animal_cards[12];
	Card pastry_cards[12];
	Card painting_cards[12];
	create_cards(animal_cards, textures_animal);
	create_cards(pastry_cards, textures_pastry);
	create_cards(painting_cards, textures_painting);


	// Sélection des cartes
	int first_selection = 0;
	int first_selection_x = 0;
	int first_selection_y = 0;
	int second_selection = 0;


	// Etat du jeu
	int game_started = 0;
	int game_finished = 0;
	load_game_state(&game_finished);

	// Affichage choix du niveau et du thème
	if (!selected_theme && !selected_level && !game_started) {
		if (dark_clear == 0){
			SDL_texture_renderer(renderer, window, texture_Sun, Sun_Moon_x, Sun_Moon_y);
			change_mode(dark_clear,renderer);
		}
		else if (dark_clear == 0){
			SDL_texture_renderer(renderer, window, texture_Moon, Sun_Moon_x, Sun_Moon_y);
			change_mode(dark_clear,renderer);
		}
		SDL_texture_renderer(renderer, window, texture_name, name_x, name_y);
		SDL_texture_renderer(renderer, window, texture_animal_theme, animal_theme_x, animal_theme_y);
		SDL_texture_renderer(renderer, window, texture_pastry_theme, pastry_theme_x, pastry_theme_y);
		SDL_texture_renderer(renderer, window, texture_painting_theme, painting_theme_x, painting_theme_y);
		SDL_texture_renderer(renderer, window, texture_level_1, level_1_x, level_1_y);
		SDL_texture_renderer(renderer, window, texture_level_2, level_2_x, level_2_y);
		SDL_texture_renderer(renderer, window, texture_level_3, level_3_x, level_3_y);
		if (!game_finished) {
			SDL_texture_renderer(renderer, window, texture_continue_button, continue_button_x, continue_button_y);
		}
		SDL_RenderPresent(renderer);
	}


    // Boucle infinie du programme
    SDL_bool program_launched = SDL_TRUE;
    while(program_launched) {
		// Boucle de rendu SDL
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                // Clic icone de fermeture de la fenêtre
                case SDL_QUIT:
                    program_launched = SDL_FALSE;
                    break;

				// Clic de la souris
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        // Choix du thème
                        if (!selected_theme && (in_zone(event.button.x, event.button.y, animal_theme_x, animal_theme_x + CARD_WIDTH_HEIGHT, animal_theme_y, animal_theme_y + CARD_WIDTH_HEIGHT))) {
                            selected_theme = 1;
                        } else if (!selected_theme && (in_zone(event.button.x, event.button.y, pastry_theme_x, pastry_theme_x + CARD_WIDTH_HEIGHT, pastry_theme_y, pastry_theme_y + CARD_WIDTH_HEIGHT))) {
                            selected_theme = 2;
                        } else if (!selected_theme && (in_zone(event.button.x, event.button.y, painting_theme_x, painting_theme_x + CARD_WIDTH_HEIGHT, painting_theme_y, painting_theme_y + CARD_WIDTH_HEIGHT))) {
                            selected_theme = 3;
                        }

                        // Choix du niveau
                        if (!selected_level && (in_zone(event.button.x, event.button.y, level_1_x, level_1_x + CARD_WIDTH_HEIGHT, level_1_y, level_1_y + LABEL_HEIGHT))) {
                            selected_level = 1;
                        } else if (!selected_level && (in_zone(event.button.x, event.button.y, level_2_x, level_2_x + CARD_WIDTH_HEIGHT, level_2_y, level_2_y + LABEL_HEIGHT))) {
                            selected_level = 2;
                        } else if (!selected_level && (in_zone(event.button.x, event.button.y, level_3_x, level_3_x + CARD_WIDTH_HEIGHT, level_3_y, level_3_y + LABEL_HEIGHT))) {
                            selected_level = 3;
                        } else if (!selected_level && !game_finished && (in_zone(event.button.x, event.button.y, continue_button_x, continue_button_x + CARD_WIDTH_HEIGHT, continue_button_y, continue_button_y + LABEL_HEIGHT))) {
							selected_level = 4;
                        }

						// Changement de mode
						
						if (!dark_clear && (in_zone(event.button.x, event.button.y, Sun_Moon_x, Sun_Moon_x + BOUTON_BW_HEIGHT,  Sun_Moon_y, Sun_Moon_y + BOUTON_BW_HEIGHT))){
							dark_clear = 1;
							encrypt(dark_clear); 
							printf("pos moon x :%d\n",Sun_Moon_x+BOUTON_BW_HEIGHT);
							printf("pos moon y :%d\n",Sun_Moon_y+BOUTON_BW_HEIGHT);
							printf("dark: %d\n",dark_clear);
							
						}else if(in_zone(event.button.x, event.button.y, Sun_Moon_x, Sun_Moon_x + BOUTON_BW_HEIGHT, Sun_Moon_y, Sun_Moon_y + BOUTON_BW_HEIGHT)){
							dark_clear = 0;
							encrypt(dark_clear); 
							printf("clear: %d\n",dark_clear);
							
						}
						

						// Sélection de la première carte
						if ((game_started) && (!first_selection) && (!second_selection)) {
							if (selected_theme == 1) {
								first_card_selection(event.button.x, event.button.y, animal_cards, textures_animal, &first_selection, &first_selection_x, &first_selection_y, game_board_size, renderer, window);
							} else if (selected_theme == 2) {
								first_card_selection(event.button.x, event.button.y, pastry_cards, textures_pastry, &first_selection, &first_selection_x, &first_selection_y, game_board_size, renderer, window);
							} else if (selected_theme == 3) {
								first_card_selection(event.button.x, event.button.y, painting_cards, textures_painting, &first_selection, &first_selection_x, &first_selection_y, game_board_size, renderer, window);
							}

						// Selection de la deuxième carte
						} else if ((game_started) && (first_selection) && (!second_selection)) {
							if (selected_theme == 1) {
								second_card_selection(event.button.x, event.button.y, animal_cards, textures_animal, &second_selection, first_selection_x, first_selection_y, game_board_size, renderer, window);
							} else if (selected_theme == 2) {
								second_card_selection(event.button.x, event.button.y, pastry_cards, textures_pastry, &second_selection, first_selection_x, first_selection_y, game_board_size, renderer, window);
							} else if (selected_theme == 3) {
								second_card_selection(event.button.x, event.button.y, painting_cards, textures_painting, &second_selection, first_selection_x, first_selection_y, game_board_size, renderer, window);
							}
						}
					}
					break;
				}
			}
		

		// Instruction une fois que le thème et le niveau ont été choisis
		if ((!game_started && selected_theme && selected_level)  || (!game_started && (selected_level == 4))){
			// Effacement des boutons de nom, thème et niveau du rendu
			SDL_Delay(500);
			SDL_clear_name_texture(texture_name);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
            SDL_DestroyTexture(texture_continue_button);
			SDL_clear_renderer(renderer);

            if (selected_level != 4) {		
                // Mélange des cartes en fonction du niveau
                if (selected_level == 1) {
                    game_board_size = 2;
                    shuffle(game_board_size, 6, game_board_level_1);
                } else if (selected_level == 2) {
                    game_board_size = 3;
                    shuffle(game_board_size, 6, game_board_level_2);
                } else if (selected_level == 3) {
                    game_board_size = 4;
                    shuffle(game_board_size, 6, game_board_level_3);
                }


                // Affichage du plateau de jeu
                if (selected_theme == 1) {
                    if (selected_level == 1) {
                        position_cards(animal_cards, game_board_size, game_board_level_1, selected_level);
                        save_game_data(animal_cards, game_board_size, game_board_level_1, selected_level, selected_theme);
                    } else if (selected_level == 2) {
                        position_cards(animal_cards, game_board_size, game_board_level_2, selected_level);
                        save_game_data(animal_cards, game_board_size, game_board_level_2, selected_level, selected_theme);
                    } else if (selected_level == 3) {
                        position_cards(animal_cards, game_board_size, game_board_level_3, selected_level);
                        save_game_data(animal_cards, game_board_size, game_board_level_3, selected_level, selected_theme);
                    }
                    for (int i = 0; i < ((6 * game_board_size) / 2); i++) {
                        SDL_all_cards_display(renderer, window, textures_animal, texture_back, animal_cards, game_board_size);
                    }
                } else if (selected_theme == 2) {
                    if (selected_level == 1) {
                        position_cards(pastry_cards, game_board_size, game_board_level_1, selected_level);
                        save_game_data(pastry_cards, game_board_size, game_board_level_1, selected_level, selected_theme);
                    } else if (selected_level == 2) {
                        position_cards(pastry_cards, game_board_size, game_board_level_2, selected_level);
                        save_game_data(pastry_cards, game_board_size, game_board_level_2, selected_level, selected_theme);
                    } else if (selected_level == 3) {
                        position_cards(pastry_cards, game_board_size, game_board_level_3, selected_level);
                        save_game_data(pastry_cards, game_board_size, game_board_level_3, selected_level, selected_theme);
                    }
                    for (int i = 0; i < ((6 * game_board_size) / 2); i++) {
                        SDL_all_cards_display(renderer, window, textures_pastry, texture_back, pastry_cards, game_board_size);
                    }
                } else if (selected_theme == 3) {
                    if (selected_level == 1) {
                        position_cards(painting_cards, game_board_size, game_board_level_1, selected_level);
                        save_game_data(painting_cards, game_board_size, game_board_level_1, selected_level, selected_theme);
                    } else if (selected_level == 2) {
                        position_cards(painting_cards, game_board_size, game_board_level_2, selected_level);
                        save_game_data(painting_cards, game_board_size, game_board_level_2, selected_level, selected_theme);
                    } else if (selected_level == 3) {
                        position_cards(painting_cards, game_board_size, game_board_level_3, selected_level);
                        save_game_data(painting_cards, game_board_size, game_board_level_3, selected_level, selected_theme);
                    }
                    for (int i = 0; i < ((6 * game_board_size) / 2); i++) {
                        SDL_all_cards_display(renderer, window, textures_painting, texture_back, painting_cards, game_board_size);
                    }
                }
            } else if (selected_level = 4) {
                load_game_data(animal_cards, pastry_cards, painting_cards, &game_board_size, game_board_level_1, game_board_level_2, game_board_level_3, &selected_level, &selected_theme);
				for (int i = 0; i < 6 * game_board_size / 2; i++) {
                    if (selected_theme == 1) {
                        SDL_all_cards_display(renderer, window, textures_animal, texture_back, animal_cards, game_board_size);
                    } else if (selected_theme == 2) {
                        SDL_all_cards_display(renderer, window, textures_pastry, texture_back, pastry_cards, game_board_size);
                    } else if (selected_theme == 3) {
                        SDL_all_cards_display(renderer, window, textures_painting, texture_back, painting_cards, game_board_size);
                    }
                }
            }

			SDL_RenderPresent(renderer);
			game_started = 1;
		}


		// Instruction si deux cartes ont été sélectionnées
		if (game_started && first_selection && second_selection) {
			game_finished = 0;
			if (selected_theme == 1) {
				cards_verification(animal_cards, &second_selection, &first_selection, game_board_size, renderer, window, textures_animal, texture_back);
                if (selected_level == 1) {
                    save_game_data(animal_cards, game_board_size, game_board_level_1, selected_level, selected_theme);
                } else if (selected_level == 2) {
                    save_game_data(animal_cards, game_board_size, game_board_level_2, selected_level, selected_theme);
                } else if (selected_level == 3) {
                    save_game_data(animal_cards, game_board_size, game_board_level_3, selected_level, selected_theme);
                }
				save_game_state(game_finished);
                
                if (has_won(game_board_size, animal_cards)) {
					game_finished = 1;
					save_game_state(game_finished);
					end_game(renderer, window, texture_bravo, bravo_x, bravo_y, textures_animal, textures_pastry, textures_painting);
					return 1;
				}
			} else if (selected_theme == 2) {
				cards_verification(pastry_cards, &second_selection, &first_selection, game_board_size, renderer, window, textures_pastry, texture_back);
                if (selected_level == 1) {
                    save_game_data(pastry_cards, game_board_size, game_board_level_1, selected_level, selected_theme);
                } else if (selected_level == 2) {
                    save_game_data(pastry_cards, game_board_size, game_board_level_2, selected_level, selected_theme);
                } else if (selected_level == 3) {
                    save_game_data(pastry_cards, game_board_size, game_board_level_3, selected_level, selected_theme);
                }
				save_game_state(game_finished);

                if (has_won(game_board_size, pastry_cards)) {
					game_finished = 1;
					save_game_state(game_finished);
					end_game(renderer, window, texture_bravo, bravo_x, bravo_y, textures_animal, textures_pastry, textures_painting);
					return 1;
				}
			} else if (selected_theme == 3) {
				cards_verification(painting_cards, &second_selection, &first_selection, game_board_size, renderer, window, textures_painting, texture_back);
				if (selected_level == 1) {
                    save_game_data(painting_cards, game_board_size, game_board_level_1, selected_level, selected_theme);
                } else if (selected_level == 2) {
                    save_game_data(painting_cards, game_board_size, game_board_level_2, selected_level, selected_theme);
                } else if (selected_level == 3) {
                    save_game_data(painting_cards, game_board_size, game_board_level_3, selected_level, selected_theme);
                }
				save_game_state(game_finished);

                if (has_won(game_board_size, painting_cards)) {
					game_finished = 1;
					save_game_state(game_finished);
					end_game(renderer, window, texture_bravo, bravo_x, bravo_y, textures_animal, textures_pastry, textures_painting);
					return 1;
				}
			}

			SDL_RenderPresent(renderer);
			first_selection = 0;
			second_selection = 0;
		}
	}

	for (int x = 0; x < 12; x++) {
		SDL_DestroyTexture(textures_animal[x]);
		SDL_DestroyTexture(textures_pastry[x]);
		SDL_DestroyTexture(textures_painting[x]);
		}
	SDL_FreeSurface(memory_icon);
	SDL_clear_renderer(renderer);
	SDL_DestroyTexture(texture_bravo);
	return 1;
}


// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de quitter le programme en cas d'erreur liée à SDL
int SDL_exit_with_error(const char *message) {
	SDL_Log("Erreur %s : %s\n", message, SDL_GetError());
	SDL_Quit();
	return 0;
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de fermer la fenêtre et le rendu
void SDL_destroy_window_renderer(SDL_Renderer *renderer, SDL_Window *window) {
	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'effacer la texture du nom
void SDL_clear_name_texture(SDL_Texture *texture_name) {
	SDL_DestroyTexture(texture_name);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'effacer le message bravo
void SDL_clear_bravo_texture(SDL_Texture *texture_bravo) {
	SDL_DestroyTexture(texture_bravo);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'effacer les textures de thème
void SDL_clear_theme_texture(SDL_Texture *texture_animal_theme, SDL_Texture *texture_pastry_theme, SDL_Texture *texture_painting_theme) {
	SDL_DestroyTexture(texture_animal_theme);
	SDL_DestroyTexture(texture_pastry_theme);
	SDL_DestroyTexture(texture_painting_theme);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'effacer les textures de niveau
void SDL_clear_level_texture(SDL_Texture *texture_level_1, SDL_Texture *texture_level_2, SDL_Texture *texture_level_3) {
	SDL_DestroyTexture(texture_level_1);
	SDL_DestroyTexture(texture_level_2);
	SDL_DestroyTexture(texture_level_3);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'effacer la texture de l'arrière de la carte
void SDL_clear_back_texture(SDL_Texture *texture_back) {
	SDL_DestroyTexture(texture_back);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'effacer les éléments du rendu
void SDL_clear_renderer(SDL_Renderer *renderer) {
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'afficher une texture dans le rendu
void SDL_texture_renderer(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture, int x, int y) {
	SDL_Rect rectangle;
	if (SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h) != 0) {
    	SDL_destroy_window_renderer(renderer, window);
    	SDL_exit_with_error("chargement texture");
    }
	rectangle.x = x;
	rectangle.y = y;
	if (SDL_RenderCopy(renderer, texture, NULL, &rectangle) != 0) {
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("affichage texture");
    }
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'afficher toutes les cartes
void SDL_all_cards_display(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture **textures_group, SDL_Texture *texture, Card *cards, int game_board_size) {
	for (int i = 0; i < ((6 * game_board_size) / 2); i++) {
		if (cards[i].card_revealed == 1) {
			SDL_texture_renderer(renderer, window, textures_group[i], cards[i].card_x_1, cards[i].card_y_1);
			SDL_texture_renderer(renderer, window, textures_group[i], cards[i].card_x_2, cards[i].card_y_2);
		} else {
			SDL_texture_renderer(renderer, window, texture, cards[i].card_x_1, cards[i].card_y_1);
			SDL_texture_renderer(renderer, window, texture, cards[i].card_x_2, cards[i].card_y_2);
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant d'afficher une carte
void SDL_card_display(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture, Card card, int x, int y) {
	SDL_texture_renderer(renderer, window, texture, x, y);
	SDL_RenderPresent(renderer);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de créer les cartes
void create_cards(Card *cards, SDL_Texture **textures) {
    for (int i = 0; i < 12; i++) {
        cards[i].card_image = textures[i];
        cards[i].card_number = i + 1;
        cards[i].card_revealed = 0;
        cards[i].card_x_1 = 0;
        cards[i].card_y_1 = 0;
		cards[i].card_x_2 = 0;
        cards[i].card_y_2 = 0;
		cards[i].card_positioned = 0;
    }
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de positionner les cartes
void position_cards(Card *cards, int rows, int game_board[rows][6], int selected_level) {
	int margin;
	if (selected_level == 1) {
		margin = MARGIN_TOP_1;
	} else if (selected_level == 2) {
		margin = MARGIN_TOP_2;
	} else if (selected_level == 3) {
		margin = MARGIN_TOP_3;
	}

    for (int i = 0; i < rows; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < ((6 * rows) / 2); k++) {
				if ((game_board[i][j] == cards[k].card_number) && (!cards[k].card_positioned)) {
					cards[k].card_x_1 = j * CARD_WIDTH_HEIGHT;
					cards[k].card_y_1 = (i * CARD_WIDTH_HEIGHT) + margin;
					cards[k].card_positioned = 1;
				} else if ((game_board[i][j] == cards[k].card_number) && (cards[k].card_positioned)) {
					cards[k].card_x_2 = j * CARD_WIDTH_HEIGHT;
					cards[k].card_y_2 = (i * CARD_WIDTH_HEIGHT) + margin;
				}
			}
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de vérifier la zone de clic
int in_zone(int x, int y, int x_min, int x_max, int y_min, int y_max) {
	printf("x: %d, y:%d, x_min:%d x_max:%d\n");
	if ((x >= x_min) && (x <= x_max) && (y >= y_min) && (y <= y_max)) {
		return 1;
	}
	return 0;
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de mélanger les cartes (algorithme de Fisher Yates)
void shuffle(int rows, int columns, int game_board[rows][columns]) {
    int total_cards = rows * columns;
    for (int i = total_cards - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int row_i = i / columns;
        int column_i = i % columns;
        int row_j = j / columns;
        int column_j = j % columns;
        int temp = game_board[row_i][column_i];
        game_board[row_i][column_i] = game_board[row_j][column_j];
        game_board[row_j][column_j] = temp;
    }
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de vérifier si le joueur a gagné 
int has_won(int game_board_size, Card *cards) {
	for (int i = 0; i < 6 * game_board_size / 2; i++) {
		if(cards[i].card_revealed == 0) {
			return 0;
		}
	}
	return 1;
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de sélectioner la première carte
void first_card_selection(int x, int y, Card *cards, SDL_Texture **textures_group, int *first_selection, int *first_selection_x, int *first_selection_y, int game_board_size, SDL_Renderer *renderer, SDL_Window *window) {
	for (int i = 0; i < 6 * game_board_size / 2; i++) {
		if (in_zone(x, y, cards[i].card_x_1, cards[i].card_x_1 + CARD_WIDTH_HEIGHT, cards[i].card_y_1, cards[i].card_y_1 + CARD_WIDTH_HEIGHT)) {
			SDL_card_display(renderer, window, textures_group[i], cards[i], cards[i].card_x_1, cards[i].card_y_1);
			*first_selection = cards[i].card_number;
			*first_selection_x = cards[i].card_x_1;
			*first_selection_y = cards[i].card_y_1;
		}
		if (in_zone(x, y, cards[i].card_x_2, cards[i].card_x_2 + CARD_WIDTH_HEIGHT, cards[i].card_y_2, cards[i].card_y_2 + CARD_WIDTH_HEIGHT)) {
			SDL_card_display(renderer, window, textures_group[i], cards[i], cards[i].card_x_2, cards[i].card_y_2);
			*first_selection = cards[i].card_number;
			*first_selection_x = cards[i].card_x_2;
			*first_selection_y = cards[i].card_y_2;
		}
	}
	SDL_Delay(1000);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de sélectioner la deuxième carte
void second_card_selection(int x, int y, Card *cards, SDL_Texture **textures_group, int *second_selection, int first_selection_x, int first_selection_y, int game_board_size, SDL_Renderer *renderer, SDL_Window *window) {
	for (int i = 0; i < (6 * game_board_size / 2); i++) {
		if (in_zone(x, y, cards[i].card_x_1, cards[i].card_x_1 + CARD_WIDTH_HEIGHT, cards[i].card_y_1, cards[i].card_y_1 + CARD_WIDTH_HEIGHT)) {
			if ((cards[i].card_x_1 != first_selection_x) || (cards[i].card_y_1 != first_selection_y)) {
				SDL_card_display(renderer, window, textures_group[i], cards[i], cards[i].card_x_1, cards[i].card_y_1);
				*second_selection = cards[i].card_number;
			}
		}
		if (in_zone(x, y, cards[i].card_x_2, cards[i].card_x_2 + CARD_WIDTH_HEIGHT, cards[i].card_y_2, cards[i].card_y_2 + CARD_WIDTH_HEIGHT)) {
			if ((cards[i].card_x_2 != first_selection_x) || (cards[i].card_y_2 != first_selection_y)) {
				SDL_card_display(renderer, window, textures_group[i], cards[i], cards[i].card_x_2, cards[i].card_y_2);
				*second_selection = cards[i].card_number;
			}
		}
	}
	SDL_Delay(1000);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de faire la vérification des cartes
void cards_verification(Card *cards, int *second_selection, int *first_selection, int game_board_size, SDL_Renderer *renderer, SDL_Window *window, SDL_Texture **textures_group, SDL_Texture *texture) {
	if (*first_selection == *second_selection) {
		for (int i = 0; i < (6 * game_board_size / 2); i++) {
			if (cards[i].card_number == *first_selection) {
				cards[i].card_revealed = 1;
			}
		}
	}

	for (int i = 0; i < ((6 * game_board_size) / 2); i++) {
		SDL_all_cards_display(renderer, window, textures_group, texture, cards, game_board_size);
	}
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de quitter le jeu en cas de victoire
int end_game(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture_bravo, int bravo_x, int bravo_y, SDL_Texture **textures_animal, SDL_Texture **textures_pastry, SDL_Texture **textures_painting) {
	for (int x = 0; x < 12; x++) {
		SDL_DestroyTexture(textures_animal[x]);
		SDL_DestroyTexture(textures_pastry[x]);
		SDL_DestroyTexture(textures_painting[x]);
	}
	SDL_clear_renderer(renderer);
	SDL_texture_renderer(renderer, window, texture_bravo, bravo_x, bravo_y);
	SDL_RenderPresent(renderer);
	SDL_Delay(1000);
	SDL_clear_renderer(renderer);
	SDL_DestroyTexture(texture_bravo);
	return 1;
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de sauvegarder le jeu
void save_game_data(Card *cards, int rows, int game_board[rows][6], int selected_level, int selected_theme) {
	FILE *file = fopen("save_data.txt", "w");

    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier de sauvegarde.\n");
        return;
    }

    // Sauvegarde thème et niveau choisi
    fprintf(file, "%d\n", selected_level);
    fprintf(file, "%d\n", selected_theme);

    // Sauvegarde des données des cartes
    for (int i = 0; i < (6 * rows / 2); i++) {
        fprintf(file, "%d %d %d %d %d %d %d\n",
            cards[i].card_number, cards[i].card_revealed, cards[i].card_x_1, cards[i].card_y_1,
            cards[i].card_x_2, cards[i].card_y_2, cards[i].card_positioned);
    }

    // Sauvegarde des données de la matrice game_board
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 6; j++) {
            fprintf(file, "%d ", game_board[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de récupérer la sauvegarde du jeu
void load_game_data(Card *animal_cards, Card *pastry_cards, Card *painting_cards, int *game_board_size, int game_board_level_1[2][6], int game_board_level_2[3][6], int game_board_level_3[4][6], int *selected_level, int *selected_theme) {
    FILE *file = fopen("save_data.txt", "r");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier de sauvegarde.\n");
        return;
    }

    // Chargement du niveau et du thème choisis
    fscanf(file, "%d\n", selected_level);
    fscanf(file, "%d\n", selected_theme);

    // Chargement des données des cartes
	if (*selected_level == 1) {
		*game_board_size = 2;
	} else if (*selected_level == 2) {
		*game_board_size = 3;
	} else if (*selected_level == 3) {
		*game_board_size = 4;
	}

    for (int i = 0; i < 6 * (*game_board_size) / 2; i++) {
        if (*selected_theme == 1) {
            fscanf(file, "%d %d %d %d %d %d %d\n",
            &animal_cards[i].card_number, &animal_cards[i].card_revealed, &animal_cards[i].card_x_1, &animal_cards[i].card_y_1,
            &animal_cards[i].card_x_2, &animal_cards[i].card_y_2, &animal_cards[i].card_positioned);
        } else if (*selected_theme == 2) {
            fscanf(file, "%d %d %d %d %d %d %d\n",
            &pastry_cards[i].card_number, &pastry_cards[i].card_revealed, &pastry_cards[i].card_x_1, &pastry_cards[i].card_y_1,
            &pastry_cards[i].card_x_2, &pastry_cards[i].card_y_2, &pastry_cards[i].card_positioned);
        } else if (*selected_theme == 3) {
            fscanf(file, "%d %d %d %d %d %d %d\n",
            &painting_cards[i].card_number, &painting_cards[i].card_revealed, &painting_cards[i].card_x_1, &painting_cards[i].card_y_1,
            &painting_cards[i].card_x_2, &painting_cards[i].card_y_2, &painting_cards[i].card_positioned);
        }
    }

	// Chargement des données du plateau de jeu
    if (*selected_level == 1) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 6; j++) {
                fscanf(file, "%d ", &game_board_level_1[i][j]);
            }
            fscanf(file, "\n");
        }
    } else if (*selected_level == 2) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 6; j++) {
                fscanf(file, "%d ", &game_board_level_2[i][j]);
            }
            fscanf(file, "\n");
        }
    } else if (*selected_level == 3) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 6; j++) {
                fscanf(file, "%d ", &game_board_level_3[i][j]);
            }
            fscanf(file, "\n");
        }
    }

    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de sauvegarder l'état du jeu
void save_game_state(int game_finished) {
	FILE *file = fopen("save_state.txt", "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier de sauvegarde de l'état du jeu.\n");
        return;
    }
    fprintf(file, "%d\n", game_finished);
    fclose(file);
}

// ----------------------------------------------------------------------------------------------------------------

// Fonction permettant de récupérer l'état du jeu
void load_game_state(int *game_finished) {
    FILE *file = fopen("save_state.txt", "r");
    if (file == NULL) {
        *game_finished = 1;
    }
    fscanf(file, "%d\n", game_finished);
    fclose(file);
}

void change_mode(int theme,SDL_Renderer *renderer){
	if (theme == 0){
		SDL_SetRenderDrawColor(renderer,255,255,255,255);
	
	}else if (theme == 1){
		SDL_SetRenderDrawColor(renderer,30,30,30,255);
	}else{
		exit(EXIT_FAILURE);
	}
}

int decrypt() {
        system("openssl aes-256-cbc -d -a -pbkdf2 -in conf.txt.enc -out conf.txt.new -pass pass:esgi");
        FILE *file = fopen("conf.txt.new", "r");
        int theme = -1;
        fscanf(file, "%d", &theme);
        fclose(file);
        printf("Decrypted theme: %d\n", theme);
		return theme;
		
}




void encrypt(int theme) {
    FILE *file = fopen("conf.txt", "w");
    
    fprintf(file, "%d", theme);
    fclose(file);

    system( "openssl aes-256-cbc -a -pbkdf2 -in conf.txt -out conf.txt.enc -pass pass:esgi");

    
    system("del conf.txt");
}
