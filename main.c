#define _CRT_SECURE_NO_WARNINGS
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define ROWS 100
#define COLUMNS 100

int total_rows = 20;
int total_columns = 30;
int grid_size = 20;

void initialize_matrix(int matrix[ROWS][COLUMNS]) 
{
	for (int r = 0; r < total_rows; r++)
	{
		for (int c = 0; c < total_columns; c++)
		{
			matrix[r][c] = 0;
		}
	}
}

int get_cell(int matrix[ROWS][COLUMNS], int r, int c)
{
	r = (r + total_rows) % total_rows;
	c = (c + total_columns) % total_columns;
	return matrix[r][c];
}

void set_cell(int matrix[ROWS][COLUMNS], int r, int c, int value)
{
	r = (r + total_rows) % total_rows;
	c = (c + total_columns) % total_columns;
	matrix[r][c] = value;
}


int get_neighbor_count(int matrix[ROWS][COLUMNS], int r, int c) 
{
	int count = 0;
	for (int dr = -1; dr <= 1; dr++)
	{
		for (int dc = -1; dc <= 1; dc++)
		{
			if (dr == 0 && dc == 0) continue;
			int rr = r + dr;
			int cc = c + dc;
			if (get_cell(matrix, rr, cc))
			{
				count++;
			}
		}
	}
	return count;
}

void copy_matrix(int target[ROWS][COLUMNS], int source[ROWS][COLUMNS])
{
	for (int r = 0; r < total_rows; r++)
	{
		for (int c = 0; c < total_columns; c++)
		{
			target[r][c] = source[r][c];
		}
	}
}


void evolution(int matrix[ROWS][COLUMNS])
{
	int tmp[ROWS][COLUMNS];
	initialize_matrix(tmp);
	for (int r = 0; r < total_rows; r++)
	{
		for (int c = 0; c < total_columns; c++)
		{
			int cnt = get_neighbor_count(matrix, r, c);
			if (cnt == 0 || cnt == 1) 
			{
				if (matrix[r][c])
				{
					tmp[r][c] = 0;
				}
			}
			else if (cnt == 2 || cnt == 3) 
			{
				if (matrix[r][c]) 
				{
					tmp[r][c] = 1;
				}
				else 
				{
					if (cnt == 3)
					{
						tmp[r][c] = 1;
					}
				}
			}
			else if (cnt > 3) 
			{
				if (matrix[r][c])
				{
					tmp[r][c] = 0;
				}
			}
		}
	}
	copy_matrix(matrix, tmp);
}

void load_matrix(char* filename, int matrix[ROWS][COLUMNS])
{
	char line[COLUMNS + 4];
	FILE* file = fopen(filename, "r");
	if (file == NULL) return;
	if (fscanf(file, "%d %d", &total_rows, &total_columns)) 
	{
		if (fgets(line, COLUMNS + 4, file)) {
			char ch;
			for (int r = 0; r < total_rows; r++)
			{
				if (fgets(line, COLUMNS + 4, file)) {
					for (int c = 0; c < total_columns; c++)
					{
						ch = line[c];
						if (ch == '1')
						{
							matrix[r][c] = 1;
						}
						else
						{
							matrix[r][c] = 0;
						}
					}
				}
			}
		}
	}
}

void save_matrix(char* filename, int matrix[ROWS][COLUMNS]) 
{
	FILE* file = fopen(filename, "w");
	fprintf(file, "%d %d\n", total_rows, total_columns);
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLUMNS; c++)
		{
			fprintf(file, "%d", matrix[r][c]);
		}
		fprintf(file, "\n");
	}
	fflush(file);
	fclose(file);
}

void draw_matrix(int matrix[ROWS][COLUMNS], SDL_Renderer* renderer)
{
	
	
	for (int r = 0; r < total_rows; r++)
	{
		for (int c = 0; c < total_columns; c++)
		{
			SDL_Rect rect = { c * grid_size, r * grid_size, grid_size, grid_size };
			if (matrix[r][c])
			{
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderFillRect(renderer, &rect);
			}
			else 
			{
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderDrawRect(renderer, &rect);
			}
		}
	}
}


int main(int argc, char* argv[]) {
	int matrix[ROWS][COLUMNS];

	//load_matrix("glider.txt", matrix);
	initialize_matrix(matrix);


	SDL_Window* window;                    // Declare a pointer
	SDL_Renderer* renderer;

	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"Game of Life",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		800,                               // width, in pixels
		600,                               // height, in pixels
		SDL_WINDOW_SHOWN // flags - see below
	);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s ", SDL_GetError());
			return 1;
	}



	/* We must call SDL_CreateRenderer in order for draw calls to affect this window. */
	renderer = SDL_CreateRenderer(window, -1, 0);


	SDL_Surface* open_img = SDL_LoadBMP("open.bmp");
	SDL_Surface* start_img = SDL_LoadBMP("start.bmp");

	SDL_Texture* open_img_texture = SDL_CreateTextureFromSurface(renderer, open_img);
	SDL_Texture* start_img_texture = SDL_CreateTextureFromSurface(renderer, start_img);

	SDL_Rect open_rect;
	open_rect.h = 100;
	open_rect.w = 100;
	open_rect.x = 300;
	open_rect.y = 500;

	SDL_Rect start_rect;
	start_rect.h = 100;
	start_rect.w = 100;
	start_rect.x = 450;
	start_rect.y = 500;


	int started = 0;
	int done = 0;
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { done = 1; }

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) { done = 1; }
			}
			else if (SDL_MOUSEBUTTONDOWN == event.type)
			{
				if (SDL_BUTTON_LEFT == event.button.button)
				{
					int px = event.button.x;
					int py = event.button.y;
					if (px >= open_rect.x && px <= open_rect.x + open_rect.w && py >= open_rect.y && py <= open_rect.y + open_rect.h) {
						load_matrix("input.txt", matrix);
					}
					else if (px >= start_rect.x && px <= start_rect.x + start_rect.w && py >= start_rect.y && py <= start_rect.y + start_rect.h) {
						started = !started;
					}
					else if (!started) {
						int c = px / grid_size;
						int r = py / grid_size;
						if (r < total_rows && c < total_columns) {
							matrix[r][c] = !matrix[r][c];
						}
					}
				}
			}
		}

		/* Select the color for drawing. It is set to white here. */
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		/* Clear the entire screen to our selected color. */
		SDL_RenderClear(renderer);

		if (started) {
			evolution(matrix);
		}
		draw_matrix(matrix, renderer);

		SDL_RenderCopy(renderer, open_img_texture, NULL, &open_rect);
		SDL_RenderCopy(renderer, start_img_texture, NULL, &start_rect);


		/* Up until now everything was drawn behind the scenes.
		This will show the new, red contents of the window. */
		SDL_RenderPresent(renderer);
		SDL_Delay(200);
	}

	save_matrix("input.txt", matrix);


	//destory renderer
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}

	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();

    return 0;
}
