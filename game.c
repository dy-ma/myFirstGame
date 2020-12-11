// written by dylan ang

/*
 * TODOs
	 * Draw board *
	 * Get dimensions from file *
	 * Show player *
	 * Map and move player *
	 * Buildings *
	 * Enemies *
	 * power orbs *
	 * collision detection *
	 	* buildings *
	 	* enemies *
	 	* items *
	 	* power orbs *
 * TO FIX *
 * FIXED
	*  change objarr to writable memory. Causes seg fault rn. *
	*  need to initialize it, but this breaks it. *
 * To REMOVE
	* //printf
 * Dynamically allocated
 	* board *
 	* objarr *
 * To ASK
	* writing to array of structs *
	* why does mallocing the array cause a seg fault *
*/

#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

//Macros
#define BORDER '*'
#define FILL ' '

//structs
typedef struct {
	int h;
	int w;
} dimensions;

typedef struct {
	char left;
	char right;
	char up;
	char down;
	char quit;
} controls;

typedef struct {
	char player;
	char powered;
	int y;
	int x;
} play;

typedef struct {
	char type;
	int y;
	int x;
} obj;

//Globals
int linecount, objcount = 0;
unsigned itemcount = 0;
dimensions dims;
controls conts;
play ply;
play move;
obj* objarr;
char** board;

//function declarations
int playGame(const char* filename);

int readFile(const char* filename);

int createBoard();

int playLoop();

int noMemLeakPls();


int playGame(const char* filename)
{
	readFile(filename);
	createBoard();
	playLoop();
	noMemLeakPls();
	return 0;
}

int readFile(const char* filename)
{
	FILE* fp;

	//open file for reading
	fp = fopen(filename, "r");
	if (fp == NULL) return -1;

	char buf[30];

	linecount = 0;
	while (fgets(buf, 30, fp) != NULL) linecount++;
	rewind(fp);

	objarr = (obj*)malloc(sizeof(obj) * (linecount - 3));
	objcount = 0;

	//assign structs
	for (int j = 0; j < linecount; j++) {
		fgets(buf, 20, fp);

		switch (j) {
		case 0:
			sscanf(buf, "%d %d", &dims.h, &dims.w);
			break;
		case 1:
			sscanf(buf, "%c %c %c %c %c", &conts.left, &conts.right, &conts.up, &conts.down, &conts.quit);
			if (conts.left == '\n') {
				conts = (controls){.left = 'a', .right = 'd', .up = 'w', .down = 's', .quit = 'q'};
			}
			break;
		case 2:
			sscanf(buf, "%c %c %d %d", &ply.player, &ply.powered, &ply.y, &ply.x);
			break;
		default:
			sscanf(buf, "%c %d %d", &objarr[objcount].type, &objarr[objcount].y, &objarr[objcount].x);
			objcount++;

		}
	}
	fclose(fp);
	return 0;
}

int createBoard()
{
	int hh = dims.h + 2;
	int ww = dims.w + 2;
	itemcount = 0;

	//creating the board
	board = malloc(hh * sizeof(char*));
	for (int i = 0; i < hh; i++) 
		board[i] = malloc(ww * sizeof(char));
	
	//skeleton
	for (int i = 0; i < hh; i++) {
		for (int j = 0; j < ww; j++)
			board[i][j] = FILL;
		for (int m = 0; m < ww; m++) {
			board[0][m] = BORDER;
			board[hh - 1][m] = BORDER;
		}
		board[i][0] = BORDER;
		board[i][ww - 1] = BORDER;
	}

	//player
	if(ply.x < dims.w && ply.y < dims.h)
		board[ply.y + 1][ply.x + 1] = ply.player;

	//objects
	for (int i = 0; i < objcount; i++) {
		//enemies
		if (objarr[i].type == 'E' && objarr[i].y < dims.h && objarr[i].x < dims.w) {
			board[objarr[i].y + 1][objarr[i].x + 1] = 'X';
		}

		//items
		if (objarr[i].type == 'I' && objarr[i].y < dims.h && objarr[i].x < dims.w) {
			board[objarr[i].y + 1][objarr[i].x + 1] = '$';
			itemcount++;
		}

		//power orbs
		if (objarr[i].type == 'P' && objarr[i].y < dims.h && objarr[i].x < dims.w) {
			board[objarr[i].y + 1][objarr[i].x + 1] = '*';
		}

		//buildings
		if (objarr[i].type == 'B' && objarr[i].y < dims.h && objarr[i].x < dims.w) {
			//dashes
			for (int j = 1; j < 5; j++) {
				for (int s = 1; s < 7; s++) {
					board[objarr[i].y + j][objarr[i].x + s] = '-';

				}
			}
			//doors
			for (int j = 3; j < 5; j++) {
				for (int s = 3; s < 5; s++) {
					board[objarr[i].y + j][objarr[i].x + s] = '&';

				}
			}
		}
	}

	return 0;
}

int playLoop()
{
	int hh = dims.h + 2;
	int ww = dims.w + 2;
	unsigned score = 0, powerlevel = 0;
	int itemtotal = itemcount;
	bool dead = false;

	move = (play){.player = ply.player, .powered = ply.powered, .y = ply.y, .x = ply.x};

	while(true) {
		printf("Score: %u\n", score);
		printf("Items remaining: %u\n", itemcount);

		if (dead) {
			board[move.y + 1][move.x + 1] = '@';
		}

		//printing the board
		for (int i = 0; i < hh; i++) {
			for (int j = 0; j < ww; j++)
				printf("%c", board[i][j]);
			printf("\n");
		}

		//death
		if (dead) {
			printf("You have died.\n");
			break;
		}

		//movement
		printf("Enter input: ");
		char ent = '\0';
		scanf(" %c", &ent);

		if (ent == conts.quit) {
			printf("You have quit.\n");
			break;
		} 

		else if (ent == conts.up) {
			//dimensions
			if (move.y > 0 && board[move.y][move.x + 1] != '-') {
				//buildings
				if (board[move.y][move.x + 1] != '-' && board[move.y][move.x + 1] != '&') {
					//items
					if (board[move.y][move.x + 1] == '$') {
						itemcount -= 1;
						score++;
					}
					//power orbs
					else if (board[move.y][move.x + 1] == '*') {
						powerlevel = 7;
					}
					//enemies
					else if (board[move.y][move.x + 1] == 'X') {
						if (powerlevel == 0) dead = true;
					}

					//swap chars
					board[move.y + 1][move.x + 1] = FILL;
					move.y -= 1;
					board[move.y + 1][move.x + 1] = move.player;
				}
			}
		}

		else if (ent == conts.down) {
			//dimensions
			if (move.y < (dims.h - 1)) {
				//buildings
				if (board[move.y + 2][move.x + 1] != '-') {
					//items
					if (board[move.y + 2][move.x + 1] == '$') {
						itemcount -= 1;
						score++;
					}
					//power orbs
					else if (board[move.y + 2][move.x + 1] == '*') {
						powerlevel = 7;
					}
					//enemies
					else if (board[move.y + 2][move.x + 1] == 'X') {
						if (powerlevel == 0) dead = true;
					}

					//swap chars
					board[move.y +1][move.x + 1] = FILL;
					move.y += 1;
					board[move.y + 1][move.x + 1] = move.player;
				}
			}
		}

		else if (ent == conts.left) {
			//dimensions
			if (move.x > 0) {
				//buildings
				if (board[move.y + 1][move.x] != '-') {
					//items
					if (board[move.y + 1][move.x] == '$') {
						itemcount -= 1;
						score++;
					}
					//power orbs
					else if (board[move.y + 1][move.x] == '*') {
						powerlevel = 7;
					}
					//enemies
					else if (board[move.y + 1][move.x] == 'X') {
						if (powerlevel == 0) dead = true;
					}
					//swap chars
					board[move.y + 1][move.x + 1] = FILL;
					move.x -= 1;
					board[move.y + 1][move.x + 1] = move.player;
				}
			}
		}

		else if (ent == conts.right) {
			//dimensions
			if (move.x < (dims.w - 1)) {
				//buildings
				if (board[move.y + 1][move.x + 2] != '-') {
					//items
					if (board[move.y + 1][move.x + 2] == '$') {
						itemcount -= 1;
						score++;
					}
					//power orbs
					else if (board[move.y + 1][move.x + 2] == '*') {
						powerlevel = 7;
					}
					//enemies
					else if (board[move.y + 1][move.x + 2] == 'X') {
						if (powerlevel == 0) dead = true;
					}

					//swap chars
					board[move.y + 1][move.x + 1] = FILL;
					move.x += 1;
					board[move.y + 1][move.x + 1] = move.player;
				}
			}
		}

		//power level for wall cases
		if(powerlevel > 0) {
			board[move.y + 1][move.x + 1] = move.powered;
			powerlevel -= 1;
		}
		else if(powerlevel == 0) board[move.y + 1][move.x + 1] = move.player;

		//exit cases
		if (itemtotal > 0 && score == itemtotal) {
			printf("Congratulations! You have won.\n");
			break;
		}
	}
	printf("Final score: %u\n", score);

	return 0;
}

int noMemLeakPls()
{
		free(objarr);
		for (int i = 0; i < (dims.h + 2); i++)
			free(board[i]);
		free(board);
		return 0;
}









