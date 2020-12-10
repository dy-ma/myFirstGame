// written by dylan ang

/*
 * TODOs
 ** Draw board
 ** Get dimensions from file
 * Show player 
 * Map and move player
 * Buildings
 * Enemies
 * power orbs

 * To FIX
 * change objarr to writable memory. Causes seg fault rn.
 * need to initialize it, but this breaks it.
 * Dynamically allocated
 	* board
 	* objarr
 * To ASK
	* writing to array of structs
	* why does mallocing the array cause a seg fault 
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
//char** board;
dimensions dims;
controls conts;
play ply;
obj* objarr;
char** board;

//function declarations
int playGame(const char* filename);

int readFile(const char* filename);

int createBoard();

int playLoop();


int playGame(const char* filename)
{
	readFile(filename);
	createBoard();
	playLoop();
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
	//printf("linecount: %d\n", linecount);
	rewind(fp);

	objarr = (obj*)malloc(sizeof(obj) * (linecount - 3));
	objcount = 0;

	//assign structs
	for (int j = 0; j < linecount; j++) {
		fgets(buf, 20, fp);

		switch (j) {
		case 0:
			sscanf(buf, "%d %d", &dims.h, &dims.w);
			//printf("%d \n%d \n", dims.h, dims.w);
			break;
		case 1:
			sscanf(buf, "%c %c %c %c %c", &conts.left, &conts.right, &conts.up, &conts.down, &conts.quit);
			if (conts.left == '\n') {
				conts = (controls){.left = 'a', .right = 'd', .up = 'w', .down = 's', .quit = 'q'};
			}
			//printf("%c \n%c \n%c \n%c \n%c \n", conts.left, conts.right, conts.up, conts.down, conts.quit);
			break;
		case 2:
			sscanf(buf, "%c %c %d %d", &ply.player, &ply.powered, &ply.y, &ply.x);
			//printf("%c \n%c \n%d \n%d \n", ply.player, ply.powered, ply.y, ply.x);
			break;
		default:
			sscanf(buf, "%c %d %d", &objarr[objcount].type, &objarr[objcount].y, &objarr[objcount].x);
			//printf("%c \n%d \n%d \n", objarr[objcount].type, objarr[objcount].y, objarr[objcount].x);
			objcount++;

		}

		//printf("%s\n", buf);
	}
	fclose(fp);
	return 0;
}

int createBoard()
{
	int hh = dims.h + 2;
	int ww = dims.w + 2;

	//creating the board
	board = malloc(hh * sizeof(char*));
	for (int i = 0; i < ww; i++) 
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
		board[i][ww] = '\n';
	}

	//player
	if(ply.x < (ww - 2) && ply.y < (hh - 2))
		board[ply.y + 1][ply.x + 1] = ply.player;
	else printf("bad\n");

	//enemies
	for (int i = 0; i < objcount; i++) {
		//enemies
		if (objarr[i].type == 'E' && objarr[i].y < dims.h && objarr[i].x < dims.w) {
			board[objarr[i].y + 1][objarr[i].x + 1] = 'X';
		}

		//items
		if (objarr[i].type == 'I' && objarr[i].y < dims.h && objarr[i].x < dims.w) {
			board[objarr[i].y + 1][objarr[i].x + 1] = '$';
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

	//printing the board
	for (int i = 0; i < hh; i++) {
		for (int j = 0; j < ww; j++)
			printf("%c", board[i][j]);
		printf("\n");
	}
	return 0;
}





