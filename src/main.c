/* ----------------------------------------------------------------------------------------------------------------
	Importation des bibliothèques
---------------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>


/* ----------------------------------------------------------------------------------------------------------------
	Variables globales
---------------------------------------------------------------------------------------------------------------- */

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900
#define CARD_WIDTH_HEIGHT 206
#define NAME_WIDTH 440
#define LABEL_HEIGHT 69


/* ----------------------------------------------------------------------------------------------------------------
	Déclaration des fonctions
---------------------------------------------------------------------------------------------------------------- */

int memory_game();
int SDL_exit_with_error(const char *message);
void SDL_destroy_window_renderer(SDL_Renderer *renderer, SDL_Window *window);
void SDL_clear_name_texture(SDL_Texture *texture_name);
void SDL_clear_theme_texture(SDL_Texture *texture_animal_theme, SDL_Texture *texture_pastry_theme, SDL_Texture *texture_painting_theme);
void SDL_clear_level_texture(SDL_Texture *texture_level_1, SDL_Texture *texture_level_2, SDL_Texture *texture_level_3);
void SDL_clear_back_texture(SDL_Texture *texture_back);
void SDL_clear_renderer(SDL_Renderer *renderer);
void SDL_texture_renderer(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture, int x, int y);
int in_zone(int x, int y, int x_min, int x_max, int y_min, int y_max);
void shuffle(int rows, int columns, int game_board[rows][columns]);

// -------------------------À trier--------------------------------
/*
bool isTwoSelect(int rows, int columns, bool tab[rows][columns], bool judge[rows][columns], int game_board[rows][columns]);
bool hasWon(int rows, int columns, bool judge[rows][columns]);
*/


// /* ----------------------------------------------------------------------------------------------------------------
// 	Fonction main
// ---------------------------------------------------------------------------------------------------------------- */

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

	// Création de la fenêtre et du rendu
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0) {
        SDL_exit_with_error("creation fenetre et rendu");
    }


	// Couleur de fond
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);


	// Chargement nom du jeu
    SDL_Surface *image_name = NULL;
    image_name = SDL_LoadBMP("src/images/game_name.bmp");
    if (image_name == NULL) {
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image nom");
    }
	SDL_Texture *texture_name = NULL;
    texture_name = SDL_CreateTextureFromSurface(renderer, image_name);
    SDL_FreeSurface(image_name);
    if (texture_name == NULL) {
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture nom");
    }
	int name_x = (WINDOW_WIDTH / 2) - (NAME_WIDTH / 2);
    int name_y = (WINDOW_HEIGHT / 2) - 200;


	// Chargement image dos de la carte
	SDL_Surface *image_back = NULL;
	image_back = SDL_LoadBMP("src/images/back.bmp");
	if (image_back == NULL) {
		SDL_clear_name_texture(texture_name);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image couverture carte");
    }
	SDL_Texture *texture_back = NULL;
	texture_back = SDL_CreateTextureFromSurface(renderer, image_back);
	SDL_FreeSurface(image_back);
	if (texture_back == NULL) {
		SDL_clear_name_texture(texture_name);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture couverture carte");
    }
	int back_x = 0;
	int back_y = 0;


	// Choix du theme et niveau
	int selected_theme = 0;
	int selected_level = 0;


	// Chargement image thème Animaux
	SDL_Surface *image_animal_theme = NULL;
	image_animal_theme = SDL_LoadBMP("src/images/animals/2.bmp");
	if (image_animal_theme == NULL) {
		SDL_clear_name_texture(texture_name);
		SDL_clear_back_texture(texture_back);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image theme animaux");
    }
	SDL_Texture *texture_animal_theme = NULL;
	texture_animal_theme = SDL_CreateTextureFromSurface(renderer, image_animal_theme);
	SDL_FreeSurface(image_animal_theme);
	if (texture_animal_theme == NULL) {
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
		SDL_clear_back_texture(texture_back);
		SDL_DestroyTexture(texture_animal_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image theme patisseries");
    }
	SDL_Texture *texture_pastry_theme = NULL;
	texture_pastry_theme = SDL_CreateTextureFromSurface(renderer, image_pastry_theme);
	SDL_FreeSurface(image_pastry_theme);
	if (texture_pastry_theme == NULL) {
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("chargement image niveau 1");
    }
	SDL_Texture *texture_level_1 = NULL;
    texture_level_1 = SDL_CreateTextureFromSurface(renderer, image_level_1);
    SDL_FreeSurface(image_level_1);
    if (texture_level_1 == NULL) {
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
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
		SDL_clear_name_texture(texture_name);
		SDL_clear_back_texture(texture_back);
		SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
		SDL_DestroyTexture(texture_level_1);
		SDL_DestroyTexture(texture_level_2);
        SDL_destroy_window_renderer(renderer, window);
        SDL_exit_with_error("creation texture niveau 3");
    }
	int level_3_x = painting_theme_x;
    int level_3_y = painting_theme_y + CARD_WIDTH_HEIGHT;


	// Matrices représentant le plateau de jeu
	int game_board_size = 0;

	int game_board_level_1[4][4] = {
			{1, 1, 2, 2},
			{3, 3, 4, 4},
			{5, 5, 6, 6},
			{7, 7, 8, 8}
		};

	int game_board_level_2[4][5] = {
		{1, 1, 2, 2, 3},
		{3, 4, 4, 5, 5},
		{6, 6, 7, 7, 8},
		{8, 9, 9, 10, 10}
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
			SDL_clear_name_texture(texture_name);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
			for (int j = 0; j < i; j++) {
            	SDL_DestroyTexture(textures_animal[j]);
			}
			SDL_destroy_window_renderer(renderer, window);
			SDL_exit_with_error("chargement image carte animal");
	    }

		textures_animal[i] = SDL_CreateTextureFromSurface(renderer, image_animal);
		SDL_FreeSurface(image_animal);
		if (textures_animal[i] == NULL) {
			SDL_clear_name_texture(texture_name);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
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
			SDL_clear_name_texture(texture_name);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
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
			SDL_clear_name_texture(texture_name);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
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
			SDL_clear_name_texture(texture_name);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
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
			SDL_clear_name_texture(texture_name);
			SDL_clear_back_texture(texture_back);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
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






/*


	// - A trier ------------------------------------------------------------------------------------------------v--------------


	char scoreStringPlayerOne[6];

	sprintf(scoreStringPlayerOne, "%d", scorePlayerOne);

	int startedSelect = 1;
	int won = 0;

	bool lettersSelect[4][4] = {false};
	bool judgeLetters[4][4] = {false};

	bool flowersSelect[4][5] = {false};
	bool judgeFlowers[4][5] = {false};

	bool catsSelect[4][6] = {false};
	bool judgeCats[4][6] = {false};

	bool forTwo;
	int countForTwo = 0;


*/
//------------------------------------------------------------------------------------------------------------------------------

    // Boucle infinie du programme
    SDL_bool program_launched = SDL_TRUE;

    while(program_launched) {
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
                        } else if (in_zone(event.button.x, event.button.y, pastry_theme_x, pastry_theme_x + CARD_WIDTH_HEIGHT, pastry_theme_y, pastry_theme_y + CARD_WIDTH_HEIGHT)) {
                            selected_theme = 2;
                        } else if (in_zone(event.button.x, event.button.y, painting_theme_x, painting_theme_x + CARD_WIDTH_HEIGHT, painting_theme_y, painting_theme_y + CARD_WIDTH_HEIGHT)) {
                            selected_theme = 3;
                        }

                        // Choix du niveau
                        if (!selected_level && (in_zone(event.button.x, event.button.y, level_1_x, level_1_x + CARD_WIDTH_HEIGHT, level_1_y, level_1_y + LABEL_HEIGHT))) {
                            selected_level = 1;
                        } else if (in_zone(event.button.x, event.button.y, level_2_x, level_2_x + CARD_WIDTH_HEIGHT, level_2_y, level_2_y + LABEL_HEIGHT)) {
                            selected_level = 2;
                        } else if (in_zone(event.button.x, event.button.y, level_3_x, level_3_x + CARD_WIDTH_HEIGHT, level_3_y, level_3_y + LABEL_HEIGHT)) {
                            selected_level = 3;
                        }
						break;
					}
			}
		}


		// Instruction si le thème et le niveau n'ont pas encore été choisis
		if (!selected_theme && !selected_level) {
			SDL_texture_renderer(renderer, window, texture_name, name_x, name_y);
			SDL_texture_renderer(renderer, window, texture_animal_theme, animal_theme_x, animal_theme_y);
			SDL_texture_renderer(renderer, window, texture_pastry_theme, pastry_theme_x, pastry_theme_y);
			SDL_texture_renderer(renderer, window, texture_painting_theme, painting_theme_x, painting_theme_y);
			SDL_texture_renderer(renderer, window, texture_level_1, level_1_x, level_1_y);
			SDL_texture_renderer(renderer, window, texture_level_2, level_2_x, level_2_y);
			SDL_texture_renderer(renderer, window, texture_level_3, level_3_x, level_3_y);
			SDL_RenderPresent(renderer);

		}

		// Instruction une fois que le thème et le niveau ont été choisis
		if (selected_theme && selected_level) {
			// Effacement des boutons de nom, thème et niveau du rendu
			SDL_clear_name_texture(texture_name);
			SDL_clear_theme_texture(texture_animal_theme, texture_pastry_theme, texture_painting_theme);
			SDL_clear_level_texture(texture_level_1, texture_level_2, texture_level_3);
			SDL_clear_renderer(renderer);
		/*
			// Mélange des cartes en fonction du niveau
			if (selected_level == 1) {
				game_board_size = 4;
				shuffle(4, game_board_size, game_board_level_1);
			} else if (selected_level == 2) {
				game_board_size = 5;
				shuffle(4, game_board_size, game_board_level_2);
			} else if (selected_level == 3) {
				game_board_size = 6;
				shuffle(4, game_board_size, game_board_level_3);
			}
		}
/*
							// Vérification clic sur la carte
							for (int i = 0; i < 4; i++) {
								for (int j = 0; j < game_board_size; j++) {
									if (in_zone(event.button.x, event.button.y,
												  xStartCard + 350 * j, xStartCard + 350 * j + CARD_WIDTH_HEIGHT,
												  yStartCard - 100 + 200 * i, yStartCard - 100 + 200 * i + CARD_WIDTH_HEIGHT)) //1825
									{
										if(!judgeLetters[i][j])
										{
											forTwo = isTwoSelect(4, 4, lettersSelect, judgeLetters, game_boardLetters);
											if(!forTwo)
												countForTwo++;
											if(twoPlayerSelect && countForTwo == 2)
											{
												if(isPlayerOne)
												{
													isPlayerOne = 0;
													isPlayerTwo = 1;
													countForTwo = 0;
												}

												else if(isPlayerTwo)
												{
													isPlayerTwo = 0;
													isPlayerOne = 1;
													countForTwo = 0;
												}
											}
											if(twoPlayerSelect && forTwo)
											{
												if(isPlayerOne)
												{
													scorePlayerOne+= 100;
													isPlayerOne = 0;
													isPlayerTwo = 1;

													SDL_DestroyTexture(textScorePlayerOne);
													sprintf(scoreStringPlayerOne, "%d", scorePlayerOne);

													textScorePlayerOne = renderText(scoreStringPlayerOne, fontPath, white, 60, renderer);
													if(!textScorePlayerOne)
													{
														SDL_DestroyTexture(background);
														SDL_DestroyTexture(textMemory);
														SDL_DestroyTexture(textWon);			
														SDL_DestroyTexture(onePlayerButton);
														SDL_DestroyTexture(twoPlayerButton);
														SDL_DestroyTexture(exitButton);
														SDL_DestroyTexture(animal_theme_button);
														SDL_DestroyTexture(flowerButton);
														SDL_DestroyTexture(catButton);
														SDL_DestroyTexture(returnButton);
														SDL_DestroyTexture(backgroundCardLettre);
														SDL_DestroyTexture(backgroundCardFleur);
														SDL_DestroyTexture(backgroundCardChat);
														SDL_DestroyTexture(verticalBar);
														SDL_DestroyTexture(textPlayerOne);
														SDL_DestroyTexture(textPlayerTwo);
														SDL_DestroyTexture(textScorePlayerTwo);
														SDL_DestroyTexture(textPlayerOneWon);
														SDL_DestroyTexture(textPlayerTwoWon);
														SDL_DestroyTexture(textDraw);
														for(int i = 0; i < 8; ++i)
															SDL_DestroyTexture(letters[i]);
														for(int i = 0; i < 10; ++i)
															SDL_DestroyTexture(flowers[i]);
														for(int i = 0; i < 12; ++i)
															SDL_DestroyTexture(cats[i]);
														cleanUp(window, renderer);
														return EXIT_FAILURE;		
													}
												}
												else if(isPlayerTwo)
												{
													scorePlayerTwo+= 100;
													isPlayerTwo = 0;
													isPlayerOne = 1;

													SDL_DestroyTexture(textScorePlayerTwo);
													sprintf(scoreStringPlayerTwo, "%d", scorePlayerTwo);

													textScorePlayerTwo = renderText(scoreStringPlayerTwo, fontPath, white, 60, renderer);
													if(!textScorePlayerTwo)
													{
														SDL_DestroyTexture(background);
														SDL_DestroyTexture(textMemory);
														SDL_DestroyTexture(textWon);			
														SDL_DestroyTexture(onePlayerButton);
														SDL_DestroyTexture(twoPlayerButton);
														SDL_DestroyTexture(exitButton);
														SDL_DestroyTexture(animal_theme_button);
														SDL_DestroyTexture(flowerButton);
														SDL_DestroyTexture(catButton);
														SDL_DestroyTexture(returnButton);
														SDL_DestroyTexture(backgroundCardLettre);
														SDL_DestroyTexture(backgroundCardFleur);
														SDL_DestroyTexture(backgroundCardChat);
														SDL_DestroyTexture(verticalBar);
														SDL_DestroyTexture(textPlayerOne);
														SDL_DestroyTexture(textPlayerTwo);
														SDL_DestroyTexture(textScorePlayerOne);
														SDL_DestroyTexture(textPlayerOneWon);
														SDL_DestroyTexture(textPlayerTwoWon);
														SDL_DestroyTexture(textDraw);
														for(int i = 0; i < 8; ++i)
															SDL_DestroyTexture(letters[i]);
														for(int i = 0; i < 10; ++i)
															SDL_DestroyTexture(flowers[i]);
														for(int i = 0; i < 12; ++i)
															SDL_DestroyTexture(cats[i]);
														cleanUp(window, renderer);
														return EXIT_FAILURE;																
													}
												}
											}

											lettersSelect[i][j] = !lettersSelect[i][j];
										}
									}
						}

						}


                    }







						



						else if(flowerSelect)
						{
							for(int i = 0; i < 4; ++i)
								for(int j = 0; j < 5; ++j)
									if(in_zone(event.button.x, event.button.y,
												  xStartCard + 300 * j, xStartCard + 300 * j + CARD_WIDTH_HEIGHT,
												  yStartCard + 200 * i, yStartCard + 200 * i + CARD_WIDTH_HEIGHT)) //1845
									{
										if(!judgeFlowers[i][j])
										{
											forTwo = isTwoSelect(4, 5, flowersSelect, judgeFlowers, game_boardFlowers);
											if(!forTwo)
												countForTwo++;
											if(twoPlayerSelect && countForTwo == 2)
											{
												if(isPlayerOne)
												{
													isPlayerOne = 0;
													isPlayerTwo = 1;
													countForTwo = 0;
												}

												else if(isPlayerTwo)
												{
													isPlayerTwo = 0;
													isPlayerOne = 1;
													countForTwo = 0;
												}
											}											
											if(twoPlayerSelect && forTwo)
											{
												if(isPlayerOne)
												{
													scorePlayerOne+= 100;
													isPlayerOne = 0;
													isPlayerTwo = 1;

													SDL_DestroyTexture(textScorePlayerOne);
													sprintf(scoreStringPlayerOne, "%d", scorePlayerOne);

													textScorePlayerOne = renderText(scoreStringPlayerOne, fontPath, white, 60, renderer);
													if(!textScorePlayerOne)
													{
														SDL_DestroyTexture(background);
														SDL_DestroyTexture(textMemory);
														SDL_DestroyTexture(textWon);			
														SDL_DestroyTexture(onePlayerButton);
														SDL_DestroyTexture(twoPlayerButton);
														SDL_DestroyTexture(exitButton);
														SDL_DestroyTexture(animal_theme_button);
														SDL_DestroyTexture(flowerButton);
														SDL_DestroyTexture(catButton);
														SDL_DestroyTexture(returnButton);
														SDL_DestroyTexture(backgroundCardLettre);
														SDL_DestroyTexture(backgroundCardFleur);
														SDL_DestroyTexture(backgroundCardChat);
														SDL_DestroyTexture(verticalBar);
														SDL_DestroyTexture(textPlayerOne);
														SDL_DestroyTexture(textPlayerTwo);
														SDL_DestroyTexture(textScorePlayerTwo);
														SDL_DestroyTexture(textPlayerOneWon);
														SDL_DestroyTexture(textPlayerTwoWon);
														SDL_DestroyTexture(textDraw);
														for(int i = 0; i < 8; ++i)
															SDL_DestroyTexture(letters[i]);
														for(int i = 0; i < 10; ++i)
															SDL_DestroyTexture(flowers[i]);
														for(int i = 0; i < 12; ++i)
															SDL_DestroyTexture(cats[i]);
														cleanUp(window, renderer);
														return EXIT_FAILURE;		
													}
												}
												else if(isPlayerTwo)
												{
													scorePlayerTwo+= 100;
													isPlayerTwo = 0;
													isPlayerOne = 1;

													SDL_DestroyTexture(textScorePlayerTwo);
													sprintf(scoreStringPlayerTwo, "%d", scorePlayerTwo);

													textScorePlayerTwo = renderText(scoreStringPlayerTwo, fontPath, white, 60, renderer);
													if(!textScorePlayerTwo)
													{
														SDL_DestroyTexture(background);
														SDL_DestroyTexture(textMemory);
														SDL_DestroyTexture(textWon);			
														SDL_DestroyTexture(onePlayerButton);
														SDL_DestroyTexture(twoPlayerButton);
														SDL_DestroyTexture(exitButton);
														SDL_DestroyTexture(animal_theme_button);
														SDL_DestroyTexture(flowerButton);
														SDL_DestroyTexture(catButton);
														SDL_DestroyTexture(returnButton);
														SDL_DestroyTexture(backgroundCardLettre);
														SDL_DestroyTexture(backgroundCardFleur);
														SDL_DestroyTexture(backgroundCardChat);
														SDL_DestroyTexture(verticalBar);
														SDL_DestroyTexture(textPlayerOne);
														SDL_DestroyTexture(textPlayerTwo);
														SDL_DestroyTexture(textScorePlayerOne);
														SDL_DestroyTexture(textPlayerOneWon);
														SDL_DestroyTexture(textPlayerTwoWon);
														SDL_DestroyTexture(textDraw);
														for(int i = 0; i < 8; ++i)
															SDL_DestroyTexture(letters[i]);
														for(int i = 0; i < 10; ++i)
															SDL_DestroyTexture(flowers[i]);
														for(int i = 0; i < 12; ++i)
															SDL_DestroyTexture(cats[i]);
														cleanUp(window, renderer);
														return EXIT_FAILURE;																
													}
												}
											}

											flowersSelect[i][j] = !flowersSelect[i][j];
										}
									}
						}

						else if(catSelect)
						{
							for(int i = 0; i < 4; ++i)
								for(int j = 0; j < 6; ++j)
									if(in_zone(event.button.x, event.button.y,
												  xStartCard + 250 * j, xStartCard + 250 * j + CARD_WIDTH_HEIGHT,
												  yStartCard + 200 * i, yStartCard + 200 * i + CARD_WIDTH_HEIGHT)) //1797
									{
										if(!judgeCats[i][j])
										{
											forTwo = isTwoSelect(4, 6, catsSelect, judgeCats, game_boardCats);
											if(!forTwo)
												countForTwo++;
											if(twoPlayerSelect && countForTwo == 2)
											{
												if(isPlayerOne)
												{
													isPlayerOne = 0;
													isPlayerTwo = 1;
													countForTwo = 0;
												}

												else if(isPlayerTwo)
												{
													isPlayerTwo = 0;
													isPlayerOne = 1;
													countForTwo = 0;
												}
											}											
											if(twoPlayerSelect && forTwo)
											{
												if(isPlayerOne)
												{
													scorePlayerOne+= 100;
													isPlayerOne = 0;
													isPlayerTwo = 1;

													SDL_DestroyTexture(textScorePlayerOne);
													sprintf(scoreStringPlayerOne, "%d", scorePlayerOne);

													textScorePlayerOne = renderText(scoreStringPlayerOne, fontPath, white, 60, renderer);
													if(!textScorePlayerOne)
													{
														SDL_DestroyTexture(background);
														SDL_DestroyTexture(textMemory);
														SDL_DestroyTexture(textWon);			
														SDL_DestroyTexture(onePlayerButton);
														SDL_DestroyTexture(twoPlayerButton);
														SDL_DestroyTexture(exitButton);
														SDL_DestroyTexture(animal_theme_button);
														SDL_DestroyTexture(flowerButton);
														SDL_DestroyTexture(catButton);
														SDL_DestroyTexture(returnButton);
														SDL_DestroyTexture(backgroundCardLettre);
														SDL_DestroyTexture(backgroundCardFleur);
														SDL_DestroyTexture(backgroundCardChat);
														SDL_DestroyTexture(verticalBar);
														SDL_DestroyTexture(textPlayerOne);
														SDL_DestroyTexture(textPlayerTwo);
														SDL_DestroyTexture(textScorePlayerTwo);
														SDL_DestroyTexture(textPlayerOneWon);
														SDL_DestroyTexture(textPlayerTwoWon);
														SDL_DestroyTexture(textDraw);
														for(int i = 0; i < 8; ++i)
															SDL_DestroyTexture(letters[i]);
														for(int i = 0; i < 10; ++i)
															SDL_DestroyTexture(flowers[i]);
														for(int i = 0; i < 12; ++i)
															SDL_DestroyTexture(cats[i]);
														cleanUp(window, renderer);
														return EXIT_FAILURE;		
													}
												}
												else if(isPlayerTwo)
												{
													scorePlayerTwo+= 100;
													isPlayerTwo = 0;
													isPlayerOne = 1;

													SDL_DestroyTexture(textScorePlayerTwo);
													sprintf(scoreStringPlayerTwo, "%d", scorePlayerTwo);

													textScorePlayerTwo = renderText(scoreStringPlayerTwo, fontPath, white, 60, renderer);
													if(!textScorePlayerTwo)
													{
														SDL_DestroyTexture(background);
														SDL_DestroyTexture(textMemory);
														SDL_DestroyTexture(textWon);			
														SDL_DestroyTexture(onePlayerButton);
														SDL_DestroyTexture(twoPlayerButton);
														SDL_DestroyTexture(exitButton);
														SDL_DestroyTexture(animal_theme_button);
														SDL_DestroyTexture(flowerButton);
														SDL_DestroyTexture(catButton);
														SDL_DestroyTexture(returnButton);
														SDL_DestroyTexture(backgroundCardLettre);
														SDL_DestroyTexture(backgroundCardFleur);
														SDL_DestroyTexture(backgroundCardChat);
														SDL_DestroyTexture(verticalBar);
														SDL_DestroyTexture(textPlayerOne);
														SDL_DestroyTexture(textPlayerTwo);
														SDL_DestroyTexture(textScorePlayerOne);
														SDL_DestroyTexture(textPlayerOneWon);
														SDL_DestroyTexture(textPlayerTwoWon);
														SDL_DestroyTexture(textDraw);
														for(int i = 0; i < 8; ++i)
															SDL_DestroyTexture(letters[i]);
														for(int i = 0; i < 10; ++i)
															SDL_DestroyTexture(flowers[i]);
														for(int i = 0; i < 12; ++i)
															SDL_DestroyTexture(cats[i]);
														cleanUp(window, renderer);
														return EXIT_FAILURE;																
													}
												}
											}

											catsSelect[i][j] = !catsSelect[i][j];
										}
									}
						}


					}
				break;
			}
		}
}


	
		
		if(twoPlayerSelect && (letterSelect || flowerSelect || catSelect))
		{
		
			renderTexture(textPlayerOne, renderer, xTextPlayerOne, yTextPlayerOne, NULL);
			renderTexture(textPlayerTwo, renderer, xTextPlayerTwo, yTextPlayerTwo, NULL);
			//renderTexture(textScorePlayerOne, renderer, xTextPlayerOne, yTextPlayerOne + 100, NULL);
		}


		if(hasWon(4, 5, judgeFlowers))
		{
			flowerSelect = 0;
			letterSelect = 0;
			catSelect = 0;
			returnSelect = 1;
			won = 1;
			if(twoPlayerSelect)
			{
				if(scorePlayerOne > scorePlayerTwo)
					renderTexture(textPlayerOneWon, renderer, xTextWon, yTextWon, NULL);
				else if(scorePlayerOne < scorePlayerTwo)
					renderTexture(textPlayerTwoWon, renderer, xTextWon, yTextWon, NULL);
				else
					renderTexture(textDraw, renderer, xTextWon, yTextWon, NULL);
				scorePlayerOne = 0;
				scorePlayerTwo = 0;
				isPlayerOne = 1;
				isPlayerTwo = 0;
			}
			for(int i = 0; i < 4; ++i)
				for(int j = 0; j < 5; ++j)
				{
					judgeFlowers[i][j] = false;
					flowersSelect[i][j] = false;
				}

			if(onePlayerSelect)	
				renderTexture(textWon, renderer, xTextWon, yTextWon, NULL);
			goto renderPresent;
		}



		if(flowerSelect)
		{
			if(startedSelect)
			{
				for(int i = 0; i < 4; ++i)
					for(int j = 0; j < 5; ++j)
						switch(game_boardFlowers[i][j])
						{
							case 1:
								renderTexture(flowers[0], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);
							break;
							case 2:
								renderTexture(flowers[1], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 3:
								renderTexture(flowers[2], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 4:
								renderTexture(flowers[3], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 5:
								renderTexture(flowers[4], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 6:
								renderTexture(flowers[5], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 7:
								renderTexture(flowers[6], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 8:
								renderTexture(flowers[7], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);
							break;
							case 9:
								renderTexture(flowers[8], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 10:
								renderTexture(flowers[9], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
						}
				goto renderPresent;				
			}
			for(int i = 0; i < 4; ++i)
				for(int j = 0; j < 5; ++j)
				{
					if(!flowersSelect[i][j])
						renderTexture(backgroundCardFleur, renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);
					else
						switch(game_boardFlowers[i][j])
						{
							case 1:
								renderTexture(flowers[0], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);
							break;
							case 2:
								renderTexture(flowers[1], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 3:
								renderTexture(flowers[2], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 4:
								renderTexture(flowers[3], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 5:
								renderTexture(flowers[4], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 6:
								renderTexture(flowers[5], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 7:
								renderTexture(flowers[6], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 8:
								renderTexture(flowers[7], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);
							break;
							case 9:
								renderTexture(flowers[8], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
							case 10:
								renderTexture(flowers[9], renderer, xStartCard + 300 * j, yStartCard + 200 * i, NULL);							
							break;
						}
				}
		}


		if(!onePlayerSelect && !twoPlayerSelect)
		{
			renderTexture(onePlayerButton, renderer, xOnePlayer, yOnePlayer, NULL);
			renderTexture(twoPlayerButton, renderer, xTwoPlayer, yTwoPlayer, NULL);
			renderTexture(exitButton, renderer, xExit, yExit, NULL);
		}

		else if((onePlayerSelect || twoPlayerSelect) && (!flowerSelect && !letterSelect && !catSelect))
		{
			renderTexture(animal_theme_button, renderer, xLetter, yLetter, NULL);
			renderTexture(flowerButton, renderer, xFlower, yFlower, NULL);
			renderTexture(catButton, renderer, xCat, yCat, NULL);
			renderTexture(returnButton, renderer, xReturn, yReturn, NULL);
		}

		if(!flowerSelect && !letterSelect && !catSelect)
			renderTexture(textMemory, renderer, xTextMemory, yTextMemory, NULL);
		renderPresent:SDL_RenderPresent(renderer);
		if(startedSelect && (flowerSelect || letterSelect || catSelect))
		{
			SDL_Delay(1500);
			startedSelect = 0;
		}

		if(won)
		{
			SDL_Delay(1500);
			won = 0;
			startedSelect = 1;
		}

		frameTime = SDL_GetTicks() - frameStart;
		if(frameTime < DELAY_TIME)
			SDL_Delay((int)(DELAY_TIME - frameTime));
	}

	SDL_DestroyTexture(background);
	SDL_DestroyTexture(textMemory);
	SDL_DestroyTexture(textWon);	
	SDL_DestroyTexture(onePlayerButton);
	SDL_DestroyTexture(twoPlayerButton);
	SDL_DestroyTexture(exitButton);
	SDL_DestroyTexture(animal_theme_button);
	SDL_DestroyTexture(flowerButton);
	SDL_DestroyTexture(catButton);
	SDL_DestroyTexture(returnButton);
	SDL_DestroyTexture(backgroundCardLettre);
	SDL_DestroyTexture(backgroundCardFleur);
	SDL_DestroyTexture(backgroundCardChat);
	SDL_DestroyTexture(verticalBar);
	SDL_DestroyTexture(textPlayerOne);
	SDL_DestroyTexture(textPlayerTwo);
	SDL_DestroyTexture(textScorePlayerOne);
	SDL_DestroyTexture(textScorePlayerTwo);
	SDL_DestroyTexture(textPlayerOneWon);
	SDL_DestroyTexture(textPlayerTwoWon);
	SDL_DestroyTexture(textDraw);
	for(int i = 0; i < 8; ++i)
		SDL_DestroyTexture(letters[i]);
	for(int i = 0; i < 10; ++i)
		SDL_DestroyTexture(flowers[i]);
	for(int i = 0; i < 12; ++i)
		SDL_DestroyTexture(cats[i]);
	cleanUp(window, renderer);
	
	return EXIT_SUCCESS;*/

		}}}

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

// Fonction permettant de vérifier la zone de clic
int in_zone(int x, int y, int x_min, int x_max, int y_min, int y_max) {
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

// Fonction permettant d'effacer la texture du nom
void SDL_clear_name_texture(SDL_Texture *texture_name) {
	SDL_DestroyTexture(texture_name);
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













/*
// Fonction à trier  ---------------------------------------------------------------------------------
SDL_Texture * renderText(const char * text, const char * fontName, SDL_Color color, int fontSize, SDL_Renderer * renderer)
{
	// TTF_Font * font = TTF_OpenFont(fontName, fontSize);
	// if(!font)
	// {
	// 	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[!] Unable to open font: %s\n", TTF_GetError());
	// 	return NULL;
	// }

	// SDL_Surface * surface = TTF_RenderText_Blended(font, text, color);
	// if(!surface)
	// {
	// 	TTF_CloseFont(font);
	// 	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[!] Unable to apply text on surface: %s\n", TTF_GetError());
	// 	return NULL;
	// }

	// SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
	// if(!texture)
	// 	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[!] Unable to create texture: %s\n", SDL_GetError());

	// SDL_FreeSurface(surface);
	// //TTF_CloseFont(font);
	// return texture;
}


// Fonction à trier  ---------------------------------------------------------------------------------
bool isTwoSelect(int rows, int columns, bool tab[rows][columns], bool judge[rows][columns], int game_board[rows][columns])
{
	int i1 = -1, j1 = -1, i2 = -1, j2 = -1;
	for(int i = 0; i < rows; ++i)
		for(int j = 0; j < columns; ++j)
		{	
			if(tab[i][j] && !judge[i][j])
			{
				if(i1 == -1)
					i1 = i, j1 = j;
				else
				{
					i2 = i, j2 = j;
					if(game_board[i1][j1] == game_board[i2][j2])
					{
						judge[i1][j1] = judge[i2][j2] = true;
						return true;
					}
					else
					{
						tab[i1][j1] = tab[i2][j2] = false;
						return false;
					}

				}
			}
		}
	return false;	
}




// Fonction à trier  ---------------------------------------------------------------------------------
bool hasWon(int rows, int columns, bool judge[rows][columns])
{
	for(int i = 0; i < rows; ++i)
		for(int j = 0;  j < columns; ++j)
			if(!judge[i][j])
				return false;
	return true;
}

*/