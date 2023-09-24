#include <stdlib.h>
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


typedef struct v2 { float x, y; } v2;

void file_creator() {
	struct wall {
		v2 a, b;
		int portal;
		int bsector;
		int bwall;
		int color;
	};

	struct sector {
		int id;
		size_t firstwall, nwalls;
		float zfloor, zceil;
	};
	
	struct sector_group { struct sector arr[32]; size_t n; } sectors;
	struct wall_group   { struct wall arr[128];  size_t n; } walls;


	char value = ' ';
	int current_sector = 0;
	int current_wall = -1;
	int junk = 0;
	printf("Sector %d\n", current_sector);
	while (1) {
		sectors.arr[current_sector].id = current_sector;
		sectors.arr[current_sector].firstwall = current_wall + 1;

		while (1) {
			printf("Add another wall? (Y/N)");
			junk = scanf(" %c", &value);
			if (value == 'Y') current_wall++;
			else break;

			printf("Input a.x: ");
			junk = scanf(" %f", &walls.arr[current_wall].a.x);
			printf("Input a.y: ");
			junk = scanf(" %f", &walls.arr[current_wall].a.y);
			printf("Input b.x: ");
			junk = scanf(" %f", &walls.arr[current_wall].b.x);
			printf("Input b.y: ");
			junk = scanf(" %f", &walls.arr[current_wall].b.y);

			printf("Is wall a portal (Y/N)?");
			junk = scanf(" %c", &value);
			if (value == 'Y') {
				walls.arr[current_wall].portal = 1;
				printf("What sector does the wall boundary: ");
				junk = scanf(" %d", &walls.arr[current_wall].bsector);
				printf("What wall does this wall share: ");
				junk = scanf(" %d", &walls.arr[current_wall].bwall);
			} else {
				walls.arr[current_wall].portal = 0;
				walls.arr[current_wall].bsector = -1;
				walls.arr[current_wall].bwall = -1;
			}
			printf("Input color: ");
			//junk = scanf(" %i", &walls.arr[current_wall].color);
			walls.arr[current_wall].color = 0xFFFFFFFF;
		}

		sectors.arr[current_sector].nwalls = current_wall - sectors.arr[current_sector].firstwall+1; // ????

		

		printf("Add another sector? (Y/N)");
		junk = scanf(" %c", &value);
		if (value == 'Y') current_sector++;
		else break;
	}

	sectors.n = current_sector+1;
	walls.n = current_wall+1; // ????

	struct map {
		struct sector_group sectors;
		struct wall_group walls;
	};

	struct map this_map = { sectors, walls };
	FILE* write_ptr;
	write_ptr = fopen("map.bin", "wb");
	fwrite(&this_map, sizeof(this_map), 1, write_ptr);
}