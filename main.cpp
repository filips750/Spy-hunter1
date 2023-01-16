#define _USE_MATH_DEFINES

#define MAX_ANGLE 0.8
#define DELTA_ANGLE 0.2
#define TEXT_CONST 26

#define myblack  SDL_MapRGB(screen->format, 0x00, 0x00, 0x00)
#define mygreen  SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00)
#define myred  SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00)
#define myblue  SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC)
#define mywhites  SDL_MapRGB(my_printer.screen->format, 0xFF, 0xFF, 0xFF)
#define myblacks  SDL_MapRGB(my_printer.screen->format, 0x00, 0x00, 0x00)
#define mygreens  SDL_MapRGB(my_printer.screen->format, 0x00, 0xFF, 0x00)
#define myreds  SDL_MapRGB(my_printer.screen->format, 0xFF, 0x00, 0x00)
#define myblues  SDL_MapRGB(my_printer.screen->format, 0x11, 0x11, 0xCC)

#define NUMBER_OF_CHARS_TO_SAVE_NUMBER 33

#define MIN_Y_CAR_POS 525
#define MAX_Y_CAR_POS 600
#define CAR_START_POS 550

#define MAX_VELOCITY 15
#define MIN_VELOCITY 5
#define VELO_CONST_DEINCREMENTATOR 0.3

#define STARTING_ACCELERATION 0.4
#define RECT_MULTIPLIER 18

#define BMP_PLAYER_CAR_WIDTH 35
#define BMP_PLAYER_CAR_HEIGHT 54

#define CONST_TO_MOVE_BOARD 100
#define START_OF_ASCII_NUMBERS 48
#define MAX_PAGES 4
#define SIZE_OF_MAP 30

#define TB_LEFT_BORDER 700
#define TB_RIGHT_BORDER 200	
#define TB_UP_BORDER 50	
#define TB_DOWN_BORDER 400	
#define TB_LENGHT 500
#define TB_WIDTH 20



#include<math.h>
#include<stdio.h>
#include<string.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	1000
#define SCREEN_HEIGHT	675

struct game_clock {
	int t1;
	int t2;
	double delta;
	double world_time;
	double fps_timer;
	double frames;
	double to_rewind;
	int fps;

};

struct printer {
	SDL_Window* window;
	SDL_Surface* charset;
	SDL_Surface* screen;
	SDL_Texture* scrtex;
	SDL_Renderer* renderer;
};

int print_filenames(char* filename, printer printer, int page, int number_of_files);

int atoi(char* str) // function converting array of digits to integer
{
	int to_return = 0;
	for (int i = 0; str[i] != '\0'; i++)
		to_return = to_return * 10 + str[i] - START_OF_ASCII_NUMBERS;
	return to_return;
}

void itoa(int to_convert, char* dst) // function converting integer to array of digits
{
	int c_to_convert = to_convert;
	int outside_for_i = 0;
	int size = 0;
	for (int i = 0; to_convert != 0; i++) {
		size++;
		to_convert /= 10;
	}
	dst[size] = '\0';
	for (int i = 1; c_to_convert != 0; i++) {
		int rem = c_to_convert % 10;
		dst[size - i] = rem + START_OF_ASCII_NUMBERS;
		c_to_convert = c_to_convert / 10;
		outside_for_i++;
	}

}

void convert_int_to_buffer(char* to_return, int to_convert) {
	itoa(to_convert, to_return);
	to_return[32] = '\n';
}



int wait_for_number(char* filenames, printer printer, int page, int number_of_files) {
	int quit = 0;
	SDL_Event event;
	while (true) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_0 && page > 0) return print_filenames(filenames, printer, page-1, number_of_files);
				else if (event.key.keysym.sym == SDLK_1) return 1;
				else if (event.key.keysym.sym == SDLK_2) return 2;
				else if (event.key.keysym.sym == SDLK_3) return 3;
				else if (event.key.keysym.sym == SDLK_4) return 4;
				else if (event.key.keysym.sym == SDLK_5) return 5;
				else if (event.key.keysym.sym == SDLK_6) return 6;
				else if (event.key.keysym.sym == SDLK_7) return 7;
				else if (event.key.keysym.sym == SDLK_8) return 8;
				else if (event.key.keysym.sym == SDLK_9 && page < MAX_PAGES) return print_filenames(filenames, printer, page+1, number_of_files);
				break;
			case SDL_QUIT:
				return 1;
				break;
			};
		};
	}
}

void render(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer) {
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int xy_index(int x, int y, int size) { // because of using one-dimensional array we have to use this func
	return y * size + x + y;			   // to get element of array[x][y] of size 'size'
}

void clear_array(int* array, int size) {
	for (int i = -1; i < size * size + 1; i++)
		if (i % (size+1) == 0 || (i + size - 27) % (size+1) == 0)
			array[i+1] = 1;
		else
			array[i+1] = 0;
}

void clear_rect_array(SDL_Rect* rect_array, int size) {
	for (int i = 0; i < size * size; i++) {
		rect_array[i] = SDL_Rect{ 0,0,0,0 };
	}
}


void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);

struct Coordinates
{
	double x;
	double y;
};

void delete_map(int* map) {
	delete[] map;
	map = NULL;
}

class map {
public:
	int* new_map;
	int size;
	int counter = 0;
	SDL_Rect* all_rects;
	SDL_Surface* surf = SDL_LoadBMP("./eti.bmp");
	map(int size) {
		SDL_Rect* array_of_rectangles= new SDL_Rect[size * size];
		clear_rect_array(array_of_rectangles, size);
		this->all_rects = array_of_rectangles;
		this->size = size;
		int* ptr_to_map = new int[size * size + size];
		clear_array(ptr_to_map, size);
		this->new_map = ptr_to_map;
	}

	

	void create_next_block(){
		int size = this->size;

		if (rand() % 2 == 0 && this->counter < 7) {
			this->counter++;
			for (int i = 0; i < this->counter; i++) {
				this->new_map[i + 1] = 1;
				this->new_map[size - 2 - i] = 1;
			}
		}
		if (rand() % 100 < 45 && this->counter > 0) {
			this->counter--;
			this->new_map[this->counter + 1] = 0;
			this->new_map[size - this->counter - 2] = 0;
		}
		this->new_map[1] = 1;
		this->new_map[size - 2] = 1;
	/*	for (int i = 2; i < size - 2; i++) {
			if (!(rand() % 10) && this->count_obstacles() < 1)
					this->new_map[i] = 1;
			if (this->new_map[i] == 1 && !(rand() % 10))
				this->new_map[i] = 0;
	}*/
	}

	void make_place_for_new_row() {
		int size = this->size;
		int* temp_row = new int[size];
		

		for (int i = size - 3; i >= 0; i--)
			for (int j = 0; j < size; j++) {
				temp_row[j] = this->new_map[xy_index(j, i, size)];
				this->new_map[xy_index(j, i+1, size)] = temp_row[j];
			};
		for (int i = 0; i < size; i++)
			temp_row[i] = 0;

		delete[] temp_row;
	}

	int count_obstacles() {
		int size = this->size;
		int obstacles = 0;
		for (int i = size; i < 2 * size; i++)
			if (this->new_map[i])
				obstacles++;
		return obstacles;
	};

	void update_all_rects(int counter) {
		int temp_map = 0;
		int number_rects_added = 0;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				int x = RECT_MULTIPLIER * j + CONST_TO_MOVE_BOARD;
				int y = RECT_MULTIPLIER * i + CONST_TO_MOVE_BOARD;
				int temp_map = this->new_map[xy_index(j, i, size)];
				if (temp_map == 1) {
					SDL_Rect rect = { x, y + counter, RECT_MULTIPLIER, RECT_MULTIPLIER };
					this->all_rects[number_rects_added] = rect;
					number_rects_added++;
				}
			}
		}
	}

	void draw(SDL_Surface* screen, int counter) {
		SDL_Surface* grass = SDL_LoadBMP("grass.bmp");
		size = this->size;
		for(int i = 1; i < size; i++)
			for (int j = 1; j < size; j++) {
				int x = RECT_MULTIPLIER * j + CONST_TO_MOVE_BOARD;
				int y = RECT_MULTIPLIER * i + CONST_TO_MOVE_BOARD;
				switch (this->new_map[xy_index(j, i, size)]) {
				case 0:
					DrawRectangle(screen, x, y + counter,RECT_MULTIPLIER, RECT_MULTIPLIER, myblack, myblack);
					break;
				case 1:
					SDL_Rect rect = { x, y + counter, RECT_MULTIPLIER, RECT_MULTIPLIER };
					DrawSurface(screen, grass, rect.x + RECT_MULTIPLIER /2, rect.y + RECT_MULTIPLIER / 2);
					break;
				}
			}
		DrawRectangle(screen, CONST_TO_MOVE_BOARD, CONST_TO_MOVE_BOARD, RECT_MULTIPLIER*size, RECT_MULTIPLIER*2, myblack, myblack);
		DrawRectangle(screen, CONST_TO_MOVE_BOARD, CONST_TO_MOVE_BOARD + RECT_MULTIPLIER * (size-1), RECT_MULTIPLIER * size, RECT_MULTIPLIER * 2, myblack, myblack);


		SDL_FreeSurface(grass);
	}
};

class player_car {
public:
	int score = 0;
	double velocity = 0;
	double acceleration = 5;
	double angle = 0;
	SDL_Surface* surf = SDL_LoadBMP("./eti.bmp");
	SDL_Surface* surf2;
	Coordinates coords{ 50, 50 };
	SDL_Point top_left_point{};
	SDL_Point top_right_point{};
	SDL_Point bot_left_point{};
	SDL_Point bot_right_point{};
	SDL_Point center_point{};

	player_car(Coordinates coords, char* surf, char* surf2, double velocity, double acceleration, double angle) {
		this->score = 0;
		this->coords = coords;
		this->surf = SDL_LoadBMP(surf);
		this->surf2 = SDL_LoadBMP(surf2);
		this->velocity = velocity;
		this->acceleration = acceleration;
		this->angle = angle;
	}

	void increase_velocity() {
		if (this->velocity < MIN_VELOCITY)
			this->controll_velocity();
			this->velocity += this->acceleration;
	}

	void decrease_velocity() {
		if (this->velocity > -MAX_VELOCITY)
			this->velocity -= this->acceleration;
	}

	void slow_car() {
		if (this->velocity <= -VELO_CONST_DEINCREMENTATOR)
			this->velocity += VELO_CONST_DEINCREMENTATOR;
	}

	void controll_velocity(){
		if (this->coords.y > MAX_Y_CAR_POS - 10)
			this->velocity = 0;
	}

	void set_acceleration(int new_acceleration) {
		this->acceleration = new_acceleration;
	}

	void change_angle(double angle_to_change) {
		double updated_angle = this->angle + angle_to_change;
		if(updated_angle > -MAX_ANGLE && updated_angle < MAX_ANGLE)
			this->angle += angle_to_change;
	}

	void update_coords() {
		double x, y;
		x = this->coords.x + this->velocity * sin(this->angle);
		y = this->coords.y + this->velocity * cos(this->angle);
		bool x_valid = 0 < x && x < SCREEN_WIDTH;
		bool y_valid = MIN_Y_CAR_POS < y && y < MAX_Y_CAR_POS;
		if (x_valid) 
			this->coords.x += this->velocity * sin(this->angle);
		if (y_valid)
			this->coords.y += this->velocity * cos(this->angle);
	}

	void update_sdl_points() {
		this->top_left_point.x = this->coords.x - BMP_PLAYER_CAR_WIDTH/2 + 5;
		this->top_left_point.y = this->coords.y - BMP_PLAYER_CAR_HEIGHT/2;

		this->top_right_point.x = this->coords.x + BMP_PLAYER_CAR_WIDTH/2 - 5;
		this->top_right_point.y = this->coords.y - BMP_PLAYER_CAR_HEIGHT/2;

		this->bot_left_point.x = this->coords.x - BMP_PLAYER_CAR_WIDTH/2;
		this->bot_left_point.y = this->coords.y + BMP_PLAYER_CAR_HEIGHT/2;

		this->bot_right_point.x = this->coords.x + BMP_PLAYER_CAR_WIDTH/2;
		this->bot_right_point.y = this->coords.y + BMP_PLAYER_CAR_HEIGHT/2;

		this->center_point.x = this->coords.x;
		this->center_point.y = this->coords.y;


	}

	int check_collision(SDL_Rect* rect_array, int size) {
		int to_return = 0;
		for (int i = 0; i < size * size; i++) {
			SDL_Rect* temp_rect = &rect_array[i];
			if (rect_array[i].x == 0 && rect_array[i].y == 0)
				return to_return;
			SDL_bool bot_left = SDL_PointInRect(&this->bot_left_point, temp_rect);
			SDL_bool bot_right = SDL_PointInRect(&this->bot_right_point, temp_rect);
			SDL_bool top_left = SDL_PointInRect(&this->top_left_point, temp_rect);
			SDL_bool top_right = SDL_PointInRect(&this->top_right_point, temp_rect);
			SDL_bool center = SDL_PointInRect(&this->center_point, temp_rect);
			if (center)
				return 2;
			if (bot_left || bot_right || top_left || top_right)
				to_return = 1;
		}
		return to_return;
	}

	void draw(SDL_Surface *screen) {
		DrawSurface(screen, this->surf, this->coords.x, this->coords.y);
	}

	void draw_pix(SDL_Surface* screen) {
		DrawPixel(screen, this->bot_left_point.x, this->bot_left_point.y, mygreen);
		DrawPixel(screen, this->bot_right_point.x, this->bot_right_point.y, mygreen);
		DrawPixel(screen, this->top_left_point.x, this->top_left_point.y, mygreen);
		DrawPixel(screen, this->top_right_point.x, this->top_right_point.y, mygreen);
	}

	void new_game() {
		Coordinates coords{ CAR_START_POS, CAR_START_POS };
		this->coords = coords;
		this->velocity = 0;
		this->acceleration = STARTING_ACCELERATION;
		this->update_coords();
		this->update_sdl_points();
		this->angle = 0;
		this->score = 0;
	}

	int on_colission(SDL_Surface* screen, SDL_Texture* scrtex, SDL_Renderer* renderer ) {
		SDL_Surface* boom = SDL_LoadBMP("boom.bmp");
		this->velocity = 0;
		this->acceleration = 0;
		DrawSurface(screen, boom, this->coords.x, this->coords.y);
		render(screen, scrtex, renderer);
		_sleep(250);
		SDL_Surface* boom2 = SDL_LoadBMP("boom2.bmp");
		DrawSurface(screen, boom2, this->coords.x, this->coords.y);
		render(screen, scrtex, renderer);
		_sleep(250);

		SDL_Surface* boom3 = SDL_LoadBMP("boom3.bmp");
		DrawSurface(screen, boom3, this->coords.x, this->coords.y);
		render(screen, scrtex, renderer);
		_sleep(250);

		SDL_FreeSurface(boom);
		SDL_FreeSurface(boom2);
		SDL_FreeSurface(boom3);

		return 0;
	}
};




// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
// charset to bitmapa 128x128 zawierajπca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt úrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostokπta o d≥ugoúci bokÛw l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};


void play_new_game(map* map, player_car* car, game_clock* clock, double *counter) {
	clear_rect_array(map->all_rects, map->size);
	clear_array(map->new_map, map->size);
	car->new_game();
	clock->world_time = 0;
	*counter = 0;
}

struct game {
	map map;
	player_car player_car;
};

void load_new_game(map* map, player_car* car, double* time, double* counter, game* game) {

	int size = map->size;
	*car = game->player_car;
	for (int i = 0; i < size * size; i++) {
		map->new_map[i] = game->map.new_map[i];
	}
	time = 0;
	counter = 0;
}

game read_board_from_file(const char* file_path, game_clock* my_clock);

void copy_to(char* to_copy, char* destination, int size, int counter, int second_counter) {
	int i_outside_loop = 0;
	for (int i = 0; i < size; i++) {
		if (to_copy[i + second_counter] == '\n')
			break;
		destination[counter + i] = to_copy[i + second_counter];
		i_outside_loop++;
	}

	destination[counter + second_counter + i_outside_loop] = '\n';
}

int get_sys_result(const char* cmd, char* sys_result) {
	int counter = 0;
	int result = 0;
	char buffer[256] = {};
	FILE* pipe = _popen(cmd, "r");
	if (!pipe)
		return -1;
	while (fgets(buffer, 256, pipe)) {
		copy_to(buffer, sys_result, TEXT_CONST, counter * TEXT_CONST, 0);
		counter++;
	}
	_pclose(pipe);
	return counter;
}

void fill_with_spaces(char* to_fill, int size) {
	for (int i = 0; i < size; i++) {
		to_fill[i] = ' ';
	}
}

void print_textbox(printer my_printer) {
	DrawRectangle(my_printer.screen, my_printer.screen->w - TB_LEFT_BORDER, TB_UP_BORDER, TB_LENGHT, TB_WIDTH, mywhites, mywhites);
	DrawRectangle(my_printer.screen, my_printer.screen->w - TB_LEFT_BORDER, TB_DOWN_BORDER, TB_LENGHT, TB_WIDTH, mywhites, mywhites);
	DrawRectangle(my_printer.screen, my_printer.screen->w - TB_LEFT_BORDER, TB_UP_BORDER, TB_WIDTH, TB_LENGHT, mywhites, mywhites);
	DrawRectangle(my_printer.screen, my_printer.screen->w - TB_RIGHT_BORDER, TB_UP_BORDER, TB_WIDTH, TB_LENGHT, mywhites, mywhites);

	DrawRectangle(my_printer.screen, my_printer.screen->w - 680, 70, 480, 330, myblacks, myblacks);

	render(my_printer.screen, my_printer.scrtex, my_printer.renderer);

}

int print_filenames(char* filenames, printer printer, int page, int number_of_files) {
	char buffer[27] = {};
	char buffer_number[16] = {};
	int second_i = 0;
	int third_i = 0;
	print_textbox(printer);


	for (int i = page * TEXT_CONST * 8; i < (page + 1) * TEXT_CONST * 8 && third_i/TEXT_CONST < number_of_files; i += TEXT_CONST) {
		second_i = third_i;
		copy_to(filenames, buffer, TEXT_CONST, 0, i);
		second_i = third_i;
		sprintf(buffer_number, "%i. ", second_i / TEXT_CONST + 1);
		second_i = third_i;
		DrawString(printer.screen, printer.screen->w / 2 - TEXT_CONST*5 , second_i + 100, buffer_number, printer.charset);
		second_i = third_i;
		DrawString(printer.screen, printer.screen->w / 2 - strlen(buffer) * 8 / 2, second_i + 100, buffer, printer.charset);
		third_i += TEXT_CONST;
	}
	DrawString(printer.screen, printer.screen->w / 2 - TEXT_CONST * 5, TEXT_CONST * 8 + 100, "9. ", printer.charset);
	DrawString(printer.screen, printer.screen->w / 2 - strlen(buffer) * 8 / 2, TEXT_CONST * 8 + 100, "Kolejna strona", printer.charset);
	render(printer.screen, printer.scrtex, printer.renderer);
	return wait_for_number(filenames, printer, page, number_of_files);
}

void convert_int_to_filename(char* filename, char* filenames, int file_number) {
	int second_i = 0;
	for (int i = file_number * TEXT_CONST; i < (file_number + 1) * TEXT_CONST; i++) {
		filename[second_i] = filenames[i];
		second_i++;
	}
}

void cut_leftovers(char* filename) {
	for (int i = 0; i < TEXT_CONST; i++) {
		if (filename[i] == '\n') {
			filename[i] = '\0';
			return;
		}

	}
}

game get_filenames(char* filename, printer printer, game_clock* my_clock) {
	{	
		int number_of_files;
		int choice = 0;
		char* final_command = { "dir /a-d /b *.txt" };
		// Sending the final command as the argument to the
		// system function
		char sys_result[50000] = {};
		fill_with_spaces(sys_result, 50000);
		number_of_files = get_sys_result(final_command, sys_result);
		choice = print_filenames(sys_result, printer, 0, number_of_files);
		convert_int_to_filename(filename, sys_result, choice-1);
		cut_leftovers(filename);
		return read_board_from_file(filename, my_clock);
	}
}

void save_board_to_file(player_car& car, map& play_map, game_clock clock) { // saves game to given filepath
	char buffer_velo[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_time[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_score[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_angle[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_x[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_y[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_acceleration[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_size[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char file_path[50] = {};
	char buffer[1000] = {};
	fill_with_spaces(buffer_size, NUMBER_OF_CHARS_TO_SAVE_NUMBER);
	sprintf(buffer_x, "%f", car.coords.x);
	sprintf(buffer_y, "%f", car.coords.y);
	sprintf(buffer_velo, "%f", car.velocity);
	sprintf(buffer_angle, "%f", car.angle);
	sprintf(buffer_acceleration, "%f", car.acceleration);
	sprintf(buffer_score, "%i", car.score);
	sprintf(buffer_size, "%i", play_map.size);
	sprintf(buffer_time, "%f", clock.world_time);
	buffer_x[32] = '\n';
	buffer_y[32] = '\n';
	buffer_velo[32] = '\n';
	buffer_score[32] = '\n';
	buffer_angle[32] = '\n';
	buffer_time[32] = '\n';
	FILE* my_file;
	time_t time_to_make_file_title;
	struct tm* data;
	time(&time_to_make_file_title);
	data = localtime(&time_to_make_file_title);
	mktime(data);
	sprintf(file_path, "%i%i%i%i%i%i.txt", data->tm_mday, data->tm_mon, data->tm_year, data->tm_hour, data->tm_min, data->tm_sec);
	my_file = fopen(file_path, "w");
	int iter = 0;
	buffer[0] = '\n';
	int temp = 0;
	int temp2 = 0;
	for (int i = 0; i < play_map.size-1; i++) {
		for (int j = 0; j < play_map.size; j++) {
			iter++;
			temp2 = xy_index(j, i, play_map.size);
			temp = play_map.new_map[temp2];
			if (temp)
				buffer[iter] = temp + START_OF_ASCII_NUMBERS;
			else
				buffer[iter] = ' ';
		}
		iter++;
		buffer[iter] = '\n';
	}
	buffer[iter + 1] = '\0';
	fwrite(buffer_x, sizeof(char), sizeof(buffer_x), my_file);
	fwrite(buffer_y, sizeof(char), sizeof(buffer_y), my_file);
	fwrite(buffer_velo, sizeof(char), sizeof(buffer_velo), my_file);
	fwrite(buffer_score, sizeof(char), sizeof(buffer_score), my_file);
	fwrite(buffer_angle, sizeof(char), sizeof(buffer_angle), my_file);
	fwrite(buffer_acceleration, sizeof(char), sizeof(buffer_acceleration), my_file);
	fwrite(buffer_size, sizeof(char), sizeof(buffer_size), my_file);
	fwrite(buffer_time, sizeof(char), sizeof(buffer_time), my_file);
	fwrite(buffer, sizeof(char), sizeof(buffer), my_file);

	fclose(my_file);
}

game read_board_from_file(const char* file_path, game_clock* my_clock) {

	char buffer_velo[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_time[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_score[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_angle[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_x[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_size[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer_y[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	char buffer[1000] = {};
	char buffer_acceleration[NUMBER_OF_CHARS_TO_SAVE_NUMBER] = {};
	double velo, acceleration, angle, double_x, double_y, world_time;
	int score;

	Coordinates null_coords = { CAR_START_POS, CAR_START_POS};
	player_car null_car{ null_coords, "./eti.bmp", "", 0, STARTING_ACCELERATION, 0};
	map null_play_map{ 30 };
	game null_game{ null_play_map, null_car };
	FILE* my_file;
	
	my_file = fopen(file_path, "r");
	if (my_file == NULL) {
		return null_game;
	}
	int size = 0;
	int iter = 0;
	fread(buffer_x, sizeof(char), sizeof(buffer_x), my_file);
	fread(buffer_y, sizeof(char), sizeof(buffer_y), my_file);
	fread(buffer_velo, sizeof(char), sizeof(buffer_velo), my_file);
	fread(buffer_score, sizeof(char), sizeof(buffer_score), my_file);
	fread(buffer_angle, sizeof(char), sizeof(buffer_angle), my_file);
	fread(buffer_acceleration, sizeof(char), sizeof(buffer_acceleration), my_file);
	fread(buffer_size, sizeof(char), sizeof(buffer_size), my_file);
	fread(buffer_time, sizeof(char), sizeof(buffer_time), my_file);
	fread(buffer, sizeof(char), sizeof(buffer), my_file);

	velo = atof(buffer_velo);
	score = atoi(buffer_score);
	acceleration = atof(buffer_acceleration);
	velo = atof(buffer_velo);
	angle = atof(buffer_angle);
	double_x = atof(buffer_x);
	double_y = atof(buffer_y);
	size = atoi(buffer_size);
	world_time = atof(buffer_time);
	map play_map{ SIZE_OF_MAP };
	play_map.size = size;
	Coordinates coords = { double_x, double_y};
	player_car car{ coords, "./eti.bmp", "", velo, acceleration, angle };
	car.score = score;
	my_clock->world_time = world_time;
	

	fread(buffer, sizeof(char), sizeof(buffer), my_file);
	play_map.size = size;
	clear_array(play_map.new_map, play_map.size);
	for (int i = 0; i < size-1; i++) {
		for (int j = 0; j < size; j++) {
			iter++;
			if (buffer[iter] != ' ') {
				if (buffer[iter] == '1')
					play_map.new_map[xy_index(j, i, size)] = 1;
				else if (buffer[iter] == '2')
					play_map.new_map[xy_index(j, i, size)] = 2;
			}
			else
				play_map.new_map[xy_index(j, i, size)] = 0;
		}
		iter++;
	}
	fclose(my_file);
	game game{ play_map, car };
	return game;
}

int failed_to_load_charset(printer my_printer) {
	printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
	SDL_FreeSurface(my_printer.screen);
	SDL_DestroyTexture(my_printer.scrtex);
	SDL_DestroyWindow(my_printer.window);
	SDL_DestroyRenderer(my_printer.renderer);
	SDL_Quit();
	return 1;
}

int free_all(printer my_printer) {
	printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
	SDL_FreeSurface(my_printer.charset);
	SDL_FreeSurface(my_printer.screen);
	SDL_DestroyTexture(my_printer.scrtex);
	SDL_DestroyWindow(my_printer.window);
	SDL_DestroyRenderer(my_printer.renderer);
	SDL_Quit();
	return 1;
}

void init_all(SDL_Renderer *renderer, SDL_Window* window, SDL_Surface* screen, SDL_Texture* scrtex) {
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Filip Stankiewicz, 193512");
	
	SDL_ShowCursor(SDL_DISABLE);

}

void update_game(player_car* car, map* map, printer my_printer, double *counter, bool *start_new_game, int *collision_result) {
	map->draw(my_printer.screen, *counter);
	map->update_all_rects(*counter);
	car->draw(my_printer.screen);
	car->draw_pix(my_printer.screen);
	car->update_coords();
	car->update_sdl_points();
	*collision_result = car->check_collision(map->all_rects, map->size * map->size);
	if (*collision_result == 2) {
		if (car->on_colission(my_printer.screen, my_printer.scrtex, my_printer.renderer) == 0)
			*start_new_game = true;
	}
	render(my_printer.screen, my_printer.scrtex, my_printer.renderer);

}

void draw_texts(printer my_printer, player_car* car, double world_time, int fps) {
	char text[128];
	DrawRectangle(my_printer.screen, 4, 4, SCREEN_WIDTH - 8, 36, myreds, myblues);
	sprintf(text, "Czas trwania = %.1lf s  %i klatek / s, wynik :%i ", world_time, fps, car->score);
	DrawString(my_printer.screen, my_printer.screen->w / 2 - strlen(text) * 8 / 2, 10, text, my_printer.charset);
	sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
	DrawString(my_printer.screen, my_printer.screen->w / 2 - strlen(text) * 8 / 2, TEXT_CONST, text, my_printer.charset);

	DrawRectangle(my_printer.screen, 800, 4, SCREEN_WIDTH - 800, SCREEN_HEIGHT- 4, myreds, myblues);

	sprintf(text, "strzalki - jezdzenie");
	DrawString(my_printer.screen, my_printer.screen->w - strlen(text) * 8-20, TEXT_CONST, text, my_printer.charset);
	sprintf(text, "p - pauza");
	DrawString(my_printer.screen, my_printer.screen->w - strlen(text) * 8 - 20, TEXT_CONST*2, text, my_printer.charset);
	sprintf(text, "s - zapis");
	DrawString(my_printer.screen, my_printer.screen->w - strlen(text) * 8 - 20, TEXT_CONST*3, text, my_printer.charset);
	sprintf(text, "l - zaladuj gre");
	DrawString(my_printer.screen, my_printer.screen->w - strlen(text) * 8 - 20, TEXT_CONST*4, text, my_printer.charset);
	sprintf(text, "n - nowa gra");
	DrawString(my_printer.screen, my_printer.screen->w - strlen(text) * 8 - 20, TEXT_CONST*5, text, my_printer.charset);

}




void clockclock(bool* on_pause, game_clock* my_clock,  printer my_printer, player_car* car, int collision_result) {
	my_clock->frames++;
	int ticks = SDL_GetTicks();
	my_clock->t2 = ticks;
	double result = (my_clock->t2 - my_clock->t1) * 0.001;
	my_clock->delta = result;
	my_clock->t1 = my_clock->t2;
	if (!*on_pause) {
		my_clock->world_time += result;
		SDL_FillRect(my_printer.screen, NULL, myblacks);
		my_clock->fps_timer += my_clock->delta;
		if (my_clock->fps_timer > 0.5) {
			my_clock->fps = my_clock->frames * 2;
			my_clock->frames = 0;
			my_clock->fps_timer -= 0.5;
			car->slow_car();
			if (collision_result == 1) {
				car->velocity += VELO_CONST_DEINCREMENTATOR;
			}
		};
	}

	if (*on_pause) {
		my_clock->to_rewind += my_clock->delta;
	}
	
	
}

int my_pauseclock(bool* on_pause, game_clock* my_clock, printer printer, player_car* car, int collision_result) {
	int quit = 0;
	SDL_Event event;
	*on_pause = true;
	while (true) {
		clockclock(on_pause, my_clock, printer, car, collision_result);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) return 2;
				else if (event.key.keysym.sym == SDLK_n) {
					*on_pause = false;
					return 1;
				}
				else if (event.key.keysym.sym == SDLK_p) {
					*on_pause = false;
					return 0;
				}
				break;
			case SDL_QUIT:
				return 1;
				break;
			};
		};
	}
}
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {
	srand(time(NULL));
	bool on_pause = false; bool start_new_game = false;
	int pause_result = 0;
	int t1, t2, quit, frames, rc, collision_result;
	double delta, world_time, fps_timer, fps, distance, counter, to_rewind = 0;
	frames = 0; fps_timer = 0; fps = 0; quit = 0; world_time = 0; distance = 0; counter = 0; delta = 0; collision_result = 0; to_rewind = 0;
	char* eti = "./eti.bmp";
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Coordinates beggining_coord{ CAR_START_POS, CAR_START_POS };
	player_car car{ beggining_coord, eti, "", 0, STARTING_ACCELERATION, 0};
	map play_map{ 30 };
	game game{ play_map, car };
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	init_all(renderer, window, screen, scrtex);
	charset = SDL_LoadBMP("./cs8x8.bmp");
	SDL_SetColorKey(charset, true, 0x000000);
	printer printer{ window, charset, screen, scrtex, renderer };
	if(charset == NULL)
		return failed_to_load_charset(printer);
	if(eti == NULL)
		return free_all(printer);
	t1 = SDL_GetTicks();
	t2 = 0;
	game_clock my_clock{ t1, t2, delta, world_time, fps_timer, frames, to_rewind, fps };

	while(!quit) {
		clockclock(&on_pause, &my_clock, printer, &car, collision_result);
		if (!on_pause) {
			draw_texts(printer, &car, my_clock.world_time, my_clock.fps);
			if (counter > 19 && car.coords.y < MIN_Y_CAR_POS + BMP_PLAYER_CAR_HEIGHT / 2) {
				play_map.make_place_for_new_row();
				play_map.create_next_block();
				counter = 0;
				if(collision_result != 1)
					car.score += 15;
			}
			else if (car.velocity < 0 && car.coords.y < MIN_Y_CAR_POS + BMP_PLAYER_CAR_HEIGHT / 2)
				counter -= car.velocity;
			update_game(&car, &play_map, printer, &counter, &start_new_game, &collision_result);
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_DOWN) car.increase_velocity();
					else if (event.key.keysym.sym == SDLK_UP) car.decrease_velocity();
					else if (event.key.keysym.sym == SDLK_LEFT) car.change_angle(DELTA_ANGLE);
					else if (event.key.keysym.sym == SDLK_RIGHT) car.change_angle(-DELTA_ANGLE);
					else if (event.key.keysym.sym == SDLK_n || start_new_game) {
						play_new_game(&play_map, &car, &my_clock, &counter);
						start_new_game = false;
					}
					else if (event.key.keysym.sym == SDLK_p) {
						pause_result = my_pauseclock(&on_pause, &my_clock, printer, &car, collision_result);
						if (pause_result == 1)
							play_new_game(&play_map, &car, &my_clock, &counter);
						else if (pause_result == 2)
							quit = 1;
					}
					else if (event.key.keysym.sym == SDLK_s) save_board_to_file(car, play_map, my_clock);
					else if (event.key.keysym.sym == SDLK_l) {
						on_pause = true;
						char filename[32] = {};
						get_filenames(filename, printer, &my_clock);
						game = read_board_from_file(filename, &my_clock);
						load_new_game(&play_map, &car, &world_time, &counter, &game);
						on_pause = false;
						;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		};
	}
	delete_map(play_map.new_map);
	free_all(printer);
	return 0;
	};
