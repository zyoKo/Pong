#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <./SDL_ttf.h>
#include "Constants.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int game_is_running = false;	// 100% dependency on processInput()
int match_started = false;		// ball is moving?

int last_frame_time = 0;
float deltatime = 0.0f;

int p1Score = 0;
int p2Score = 0;

TTF_Font* font = NULL;
SDL_Surface* p1ScoreTextSurface = NULL;
SDL_Surface* p2ScoreTextSurface = NULL;
SDL_Texture* p1ScoreTextTexture = NULL;
SDL_Texture* p2ScoreTextTexture = NULL;

char* int_to_string(int number)
{
	//char* str = (int*)malloc(sizeof(int));

	//for(int i = 0; i < )
}

struct game_object
{
	float x;
	float y;
	float width;
	float height;
	float velocity_x;
	float velocity_y;
	
} ball, paddle1, paddle2;

void initializeTTF()
{
	printf("\nInitializing SDL_TTF...");
	if (TTF_Init() != 0)
		printf("\nERROR initializing SDL_TTF: %s", TTF_GetError());
	else
		printf("\nSDL_TTF Initialized!");

	// Loading Font
	font = TTF_OpenFont("../Dependencies/Fonts/FFFFORWA.TTF", 32);
	if (!font)
		printf("\nError Loading Font: %s", TTF_GetError());
	else
		printf("\nFont Initialized!");

	int style;
	style = TTF_GetFontStyle(font);
	printf("\nThe font style is:");
	if (style == TTF_STYLE_NORMAL)
		printf(" normal!");
	else {
		if (style & TTF_STYLE_BOLD)
			printf(" bold!");
		if (style & TTF_STYLE_ITALIC)
			printf(" italic!");
		if (style & TTF_STYLE_UNDERLINE)
			printf(" underline!");
	}
	printf("\n");

	// To set Font Style
	//TTF_SetFontStyle(font, TTF_STYLE_BOLD | TTF_STYLE_ITALIC);
}

int initializeWindow()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error Initializing SDL!");
		return false;
	}

	window = SDL_CreateWindow(
		"Pong Game", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0);

	if (window == NULL)
	{
		printf("Could not create window: %s", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		fprintf(stderr, "Cannot create renderer: %s", SDL_GetError());
		return false;
	}

	return true;
}

void setup()
{
	paddle1.width = 10.0f;
	paddle1.height = 70.0f;
	paddle1.x = 20.0f;
	paddle1.y = (WINDOW_HEIGHT / 2.0f) - (paddle1.height / 2.0f);
	paddle1.velocity_x = 0.0f;

	paddle2.width = paddle1.width;
	paddle2.height = paddle1.height;
	paddle2.x = WINDOW_WIDTH - (1.5f * paddle1.x);
	paddle2.y = paddle1.y;
	paddle2.velocity_x = paddle1.velocity_x;

	ball.width = 10.0f;
	ball.height = 10.0f;
	ball.x = paddle1.x + 20.0f;
	ball.y = paddle1.y + (paddle1.height / 2.0f) - (ball.height / 2.0f);
	ball.velocity_x = 0.0f;
	ball.velocity_y = 0.0f;
}

void processInput()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT: 
		game_is_running = false;
		break;

	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
			game_is_running = false;

		// Start Game
		if (event.key.keysym.sym == SDLK_SPACE) {
			match_started = true;
			ball.velocity_x = 250.0f;
			ball.velocity_y = 250.0f;
		}

		// Set velocity for P1
		if (event.key.keysym.sym == SDLK_w)
			paddle1.velocity_y = -400.0f;
		else if (event.key.keysym.sym == SDLK_s)
			paddle1.velocity_y =  400.0f;

		// Set velocity for P2
		if (event.key.keysym.sym == SDLK_UP)
			paddle2.velocity_y = -400.0f;
		else if (event.key.keysym.sym == SDLK_DOWN)
			paddle2.velocity_y = 400.0f;
		break;

	case SDL_KEYUP:
		if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_s)
			paddle1.velocity_y = 0.0f;

		if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
			paddle2.velocity_y = 0.0f;
		break;
	}
}

void resetgame(int point)
{
	if (point == 1)
		p1Score += 1;
	else if (point  == 2)
		p2Score += 1;

	printf("\nP1 SCORE: %d", p1Score);
	printf("\nP2 SCORE: %d", p2Score);

	match_started = false;

	setup();
}

void update()
{
	/* 
	// For Capped Framerates ////////////////////////////////////////////////////////////////////////////////
	
	// Waste some time / sleep and wait until we reach the frame target time
	
	// 1. Using while loop (Bad for CPU, locks CPU into doing nothing)
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));

	// 2. Using SDL Delay (Redirects CPU cycles to focus on other tasks until the execution is complete)
	float time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
		SDL_Delay(time_to_wait);
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/

	// Get a delta time factor converted to seconds to be used to update my object
	deltatime = (SDL_GetTicks() - last_frame_time) / 1000.0f;
	last_frame_time = SDL_GetTicks();

	// game_object movement
	// 1. If ball doesn't move, stick with the paddle until press "Space" to move
	if (match_started == true){
		ball.x += ball.velocity_x * deltatime;
		ball.y += ball.velocity_y * deltatime;
	}
	else
		ball.y += paddle1.velocity_y * deltatime;
	// 2. Paddle Movement
	paddle1.y += paddle1.velocity_y * deltatime;
	paddle2.y += paddle2.velocity_y * deltatime;

	// Constraint Paddle1 movement to the screen
	if (paddle1.y <= 0.0f)
	{
		paddle1.velocity_y = 0.0f;
		paddle1.y = 0.0f;
	}
	if ((paddle1.y + paddle1.height) >= WINDOW_HEIGHT)
	{
		paddle1.velocity_y = 0.0f;
		paddle1.y = WINDOW_HEIGHT - paddle1.height;
	}

	// Constraint Paddle2 movement to the screen
	if (paddle2.y <= 0.0f)
	{
		paddle2.velocity_y = 0.0f;
		paddle2.y = 0.0f;
	}
	if ((paddle2.y + paddle2.height) >= WINDOW_HEIGHT)
	{
		paddle2.velocity_y = 0.0f;
		paddle2.y = WINDOW_HEIGHT - paddle2.height;
	}

	// Bounces ball on walls next to the paddles, i.e., upper and lower walls respectively
	if (ball.y >= WINDOW_HEIGHT - ball.height)
	{
		ball.y = WINDOW_HEIGHT - ball.height;
		ball.velocity_y = ball.velocity_y * -1;
	}
	else if (ball.y <= 0.0f)
	{
		ball.y = 0.0f;
		ball.velocity_y = ball.velocity_y * -1;
	}

	// Bounce ball on Paddle
	if (ball.y >= paddle1.y && ball.y <= (paddle1.y + paddle1.height))
	{
		if (ball.x >= paddle1.x && ball.x <= paddle1.x + paddle1.width)
		{
			ball.x = paddle1.x + paddle1.width;
			ball.velocity_x = ball.velocity_x * -1;
		}
	}

	if (ball.y >= paddle2.y && ball.y <= (paddle2.y + paddle2.height))
	{
		if (ball.x >= paddle2.x && ball.x <= paddle2.x + paddle2.width)
		{
			ball.x = paddle2.x;
			ball.velocity_x = ball.velocity_x * -1;
		}
	}

	// Game Over Scenario
	// If ball goes behind the paddle1 or paddle2 the game resets
	if (ball.x < (0.0f - ball.width))
		resetgame(2);
	else if (ball.x > WINDOW_WIDTH)
		resetgame(1);
}

void render()
{
	char strP1Score[50];
	char strP2Score[50];
	SDL_Color textColor = { 255, 255, 255, 255 };
	sprintf_s(strP1Score, sizeof(strP1Score), "%10d", p1Score);
	sprintf_s(strP2Score, sizeof(strP2Score), "%10d", p2Score);

	p1ScoreTextSurface = TTF_RenderText_Solid(font, strP1Score, textColor);
	//SDL_FreeSurface(p1ScoreTextSurface);
	//p1ScoreTextSurface = NULL;

	p2ScoreTextSurface = TTF_RenderText_Solid(font, strP2Score, textColor);
	//SDL_FreeSurface(p2ScoreTextSurface);
	//p2ScoreTextSurface = NULL;

	p1ScoreTextTexture = SDL_CreateTextureFromSurface(renderer, p1ScoreTextSurface);
	p2ScoreTextTexture = SDL_CreateTextureFromSurface(renderer, p2ScoreTextSurface);

	SDL_Rect p1ScoreTextRect;
	p1ScoreTextRect.x = 0.15 * WINDOW_HEIGHT;
	p1ScoreTextRect.y = 50;
	SDL_QueryTexture(p1ScoreTextTexture, NULL, NULL, &p1ScoreTextRect.w, &p1ScoreTextRect.h);

	SDL_Rect p2ScoreTextRect;
	p2ScoreTextRect.x = 0.85 * WINDOW_HEIGHT;
	p2ScoreTextRect.y = 50;
	SDL_QueryTexture(p2ScoreTextTexture, NULL, NULL, &p2ScoreTextRect.w, &p2ScoreTextRect.h);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_Rect ball_rect = {
		(int)ball.x,
		(int)ball.y,
		(int)ball.width,
		(int)ball.height
	};

	SDL_Rect paddle1_rect = {
		(int)paddle1.x,
		(int)paddle1.y,
		(int)paddle1.width,
		(int)paddle1.height
	};

	SDL_Rect paddle2_rect = {
		(int)paddle2.x,
		(int)paddle2.y,
		(int)paddle2.width,
		(int)paddle2.height
	};

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &ball_rect);
	SDL_RenderFillRect(renderer, &paddle1_rect);
	SDL_RenderFillRect(renderer, &paddle2_rect);
	SDL_RenderCopy(renderer, p1ScoreTextTexture, NULL, &p1ScoreTextRect);
	SDL_RenderCopy(renderer, p2ScoreTextTexture, NULL, &p2ScoreTextRect);

	SDL_RenderPresent(renderer);	// this basically swaps front and back buffers (i.e. the two virtual screen buffers)
}

void destroyWindow()
{
	SDL_DestroyTexture(p1ScoreTextTexture);
	SDL_DestroyTexture(p2ScoreTextTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	fprintf(stdout, "Welcome to Pong!");

	// Initializing neccesary files and functionalities
	game_is_running = initializeWindow();
	initializeTTF();

	setup();

	while (game_is_running)
	{
		processInput();
		update();
		render();
	}

	destroyWindow();

	return 0;
}