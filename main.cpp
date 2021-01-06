#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<iostream>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define STARTPOSITIONY  0
#define STARTPOSITIONX 0
#define DEFAULTJUMP    5
#define DEFAULTSPEED   4
#define GRAVITY   5



// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
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
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};



void goLeft(int& mapPosX,  double& speed, SDL_Rect& camera) {
	mapPosX -= speed;
	camera.x -= speed;
	if (camera.x <= 0)
		camera.x = 4000 - 640;
}

void goRight(int& mapPosX,  double& speed, SDL_Rect& camera ) {
	mapPosX += speed;
	camera.x += speed;
	if (camera.x >= 4000 - 640)
		camera.x = 0;
}

void goUp(int& unicornPosY, int jumpDistance, SDL_Rect& camera) {

	camera.y -= jumpDistance;

}
void goDown(int& unicornPosY, int jumpDistance, int checkJump, SDL_Rect& camera) {

		camera.y += jumpDistance;

}



void startNewGame(int& unicornPosY, int& advance, int& mapPosY, int& points, SDL_Rect& camera,  double& unicornSpeed, double& time) {
	unicornPosY = STARTPOSITIONY;
	mapPosY = STARTPOSITIONX;
	camera.x = STARTPOSITIONX;
	unicornSpeed = DEFAULTSPEED;
	advance = 0;
	points = 0;
	time = 0;

}



void goLeftAdvance(SDL_Rect& camera, int& mapPosX,  double unicornSpeed) {
	camera.x += unicornSpeed;
	mapPosX += unicornSpeed;
	if (camera.x >= 4000 - 640) {
		camera.x = 0;
		camera.x += unicornSpeed;
	}
	if (camera.x <= 0) {
		camera.x = 4000 - 640;
		camera.x += unicornSpeed;
	}
}


int fallingDown(SDL_Rect camera, int floor, int jump, int hang) {
	if (!jump && camera.y < floor && !hang) {
			camera.y += GRAVITY;
			if (camera.y > floor) {
				camera.y = floor;
			}
	}
	return camera.y;
}

void accelerating( double& unicornSpeed, double worldTime, long double acceleration) {
		acceleration = worldTime / 1000000;
		unicornSpeed += acceleration;

	printf("%Lf\n", unicornSpeed); 
}


void dash( SDL_Rect& camera, int dashSpeed) {
		camera.x += dashSpeed;
	
}


void collisionDetection(SDL_Rect camera, int& floor, int& ceilling, int& passage) {

	if ((camera.x >= 0 && camera.x <= 1550) || (camera.x >= 2800 && camera.x <= 3360))
		ceilling = 50, passage = 0;
	else if (camera.x >= 1840 && camera.x <= 2130)
		ceilling = 110, passage = 0;
	else if (camera.x >= 2150 && camera.x <= 2570)
		ceilling = -30, passage = 0;
	else if (camera.x >= 1560 && camera.x <= 1830)
		passage = 1;
	else if (camera.x >= 2580 && camera.x <= 2790)
		passage = 2;

	if (camera.y > ceilling) {
		if ((camera.x >= 0 && camera.x <= 715) || (camera.x >= 990 && camera.x <= 1155) || (camera.x >= 1530 && camera.x <= 1600) || (camera.x >= 1780 && camera.x <= 1830) || (camera.x >= 2730 && camera.x <= 2770) || (camera.x >= 3250 && camera.x <= 3360))
			floor = 295;
		else if ((camera.x >= 720 && camera.x <= 980) || (camera.x >= 2120 && camera.x <= 2480))
			floor = 235;
		else if (camera.x >= 1165 && camera.x <= 1520)
			floor = 170;
		else if (camera.x >= 1610 && camera.x <= 1770)
			floor = 105;
		else if (camera.x >= 1860 && camera.x <= 2110)
			floor = 175;
		else if (camera.x >= 2490 && camera.x <= 2720)
			floor = 75;
		else if (camera.x >= 2790 && camera.x <= 3240)
			floor = 125;
	}

	if (camera.y < ceilling) {
		if ((camera.x >= 0 && camera.x <= 565) || (camera.x >= 735 && camera.x <= 900) || (camera.x >= 1330 && camera.x <= 1540) || (camera.x >= 2820 && camera.x <= 2905) || (camera.x >= 3115 && camera.x <= 3280) || (camera.x >= 3336 && camera.x <= 3360))
			floor = -65;
		else if (camera.x >= 570 && camera.x <= 720)
			floor = -125;
		else if (camera.x >= 930 && camera.x <= 1310)
			floor = -180;
		else if ((camera.x >= 2040 && camera.x <= 2180) || (camera.x >= 2280 && camera.x <= 2300) || (camera.x >= 2420 && camera.x <= 2435) || (camera.x >= 2480 && camera.x <= 2530))
			floor = -110;
		else if ((camera.x >= 2230 && camera.x <= 2270) || (camera.x >= 2440 && camera.x <= 2470))
			floor = -150;
		else if (camera.x >= 2330 && camera.x <= 2410)
			floor = -170;
		else if (camera.x >= 2930 && camera.x <= 3100)
			floor = -130;
		else if (camera.x == 3310)
			floor = -215;
		else if (camera.x >= 1860 && camera.x <= 1990)
			floor = 0;
	
	}

	if (passage == 1) {
		if (camera.x >= 1620 && camera.x <= 1770)
			floor = 105;
		else
			floor = 295;
	}
	else if (passage == 2) {
		if (camera.x >= 2570 && camera.x <= 2720)
			floor = 75;
		else
			floor = 295;
	}


	if (((camera.y > floor) || (camera.y == ceilling)) && !passage) {
		printf("%s", "error");
		//	quit = 1;
	}

}
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	int mapPosX = 0;

	int unicornPosY = STARTPOSITIONY;
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance;
	int firstJump = 1;
	int jumpDistance = DEFAULTJUMP;
	int floor = 0;
	int ceiling = 0;
	int advance = 0;
	int secondJump = 0;
	int jump = 0;
	int hang = 0;
	int y = 0, x = 0;
	int points;
	int range = 50*jumpDistance;
	double acceleration = 0.0, unicornSpeed = DEFAULTSPEED;
	int dashSpeed = unicornSpeed+5;
	int passage = 0;
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Surface* map;
	SDL_Surface* unicorn;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	// okno konsoli nie jest widoczne, je¿eli chcemy zobaczyæ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniæ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	unicorn = SDL_LoadBMP("./unicorn.bmp");
	if (unicorn == NULL) {
		printf("SDL_LoadBMP(unicorn.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	map = SDL_LoadBMP("./background1.bmp");
	if (map == NULL) {
		printf("SDL_LoadBMP(unicorn.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};


	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;

	SDL_Rect camera;
	camera.x = 0;
	camera.y = 290;
	camera.w = 640;
	camera.h = 480;

	while (!quit) {
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		distance += unicornSpeed * delta;

		SDL_FillRect(screen, NULL, czarny);

		SDL_BlitSurface(map, &camera, screen, NULL);

		DrawSurface(screen, unicorn, 46, SCREEN_HEIGHT/2-35);


		// tekst informacyjny / info text
		DrawRectangle(screen, SCREEN_WIDTH, SCREEN_HEIGHT - 120, 300, 100, czerwony, zielony);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		//sprintf(text, "DISTANCE: %d", mapPosX);
		sprintf(text, "camera y: %d", camera.y);
		DrawString(screen, 10, SCREEN_HEIGHT - 100, text, charset);
		//sprintf(text, "TIMIE ALIVE: %.1lf s", worldTime);
		sprintf(text, "floor:%d", floor);
		//	      "Esc - exit, \030 - faster, \031 - slower"
		DrawString(screen, 10, SCREEN_HEIGHT - 110, text, charset);
		sprintf(text, "camera x:%d", camera.x);
		//	      "Esc - exit, \030 - faster, \031 - slower"
		DrawString(screen, 10, SCREEN_HEIGHT - 90, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		


		


		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while (SDL_PollEvent(&event)) {
			if (!advance) {
				switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_UP:
							jump = 1;
							goUp(unicornPosY, jumpDistance, camera);
						break;
					case SDLK_LEFT:
						goLeft(mapPosX, unicornSpeed, camera);
						break;
					case SDLK_RIGHT:
						goRight(mapPosX, unicornSpeed, camera);
						break;
					case SDLK_DOWN:
						goDown(unicornPosY, jumpDistance, firstJump, camera);
						break;
					case SDLK_ESCAPE:
						quit = 1;
						break;
					case SDLK_d:
						advance = 1;
						break;
					case SDLK_n:
						startNewGame(unicornPosY, advance, mapPosX, points, camera, unicornSpeed, worldTime);
						break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_UP:
						jump = 0;
						break;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				}
				
			}
			else {
				switch (event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_z:
						if (y <= range && (firstJump || secondJump)) {
							jump = 1;
							goUp(unicornPosY, jumpDistance, camera);
							y += jumpDistance;
						}
						else if (y >= range) jump = 0;
						break;
					case SDLK_ESCAPE:
						quit = 1;
						break;
					case SDLK_x:
						while (x < 3) {
							hang = 1;
							dash(camera, dashSpeed);
							x++;
						}
						break;
					case SDLK_d:
						advance = 0;
						break;
					}

					break;
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_z:
						if (firstJump) {
							secondJump = 1;
							firstJump = 0;
						}
						else if (secondJump) secondJump = 0;
						jump = 0;
						y = 0;
						break;
					case SDLK_x:
						hang = 0;
						secondJump = 1;
						jump = 0;
						x = 0;
					}
				}
			}
		}


		collisionDetection(camera, floor, ceiling, passage);
		camera.y = fallingDown(camera, floor, jump, hang);

		if (advance) {
			goLeftAdvance(camera, mapPosX, unicornSpeed);
				if (!hang) {
					accelerating(unicornSpeed, worldTime, acceleration);
				}
		}
		

		 if (camera.y == floor) {
			firstJump = 1;
			secondJump = 0;
		}

				 SDL_Delay(16);
	}

	//zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};
