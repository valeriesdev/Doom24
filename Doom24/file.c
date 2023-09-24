#define _CRT_SECURE_NO_DEPRECATE
#include "main.h"
#include "file.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/** # Example save file
* H 2, 2, 2, 9 # a map with camera starting at 2,2, with 2 sectors, and 9 walls
* S 0, 0, 5    # a sector with the first wall being 0, id 0, and 5 walls
* S 5, 1, 4    # a sector with the first wall being 5, id 1, and 4 walls
* W 0.0,0.0,0.0,5.0, 0, -1, -1, 0xFF000000
* W 0.0,5.0,3.0,10.0, 0, -1, -1, 0xFFFF0000
* W 3.0,10.0,10.0,10.0, 1, 1 , 5 , 0x00000000
* W 10.0,10.0,10.0,0.0, 0, -1, -1, 0xFF00FF00
* W 10.0,0.0,0.0,0.0, 0, -1, -1, 0xFFFFAAAA
* W 3.0,10.0,10.0,10.0, 1, 0 , 2 , 0x00000000
* W 10.0,10.0,13.0,15.0, 0, -1, -1, 0xFFFFFFFF
* W 13.0,15.0,1.5,15.0, 0, -1, -1, 0xFF0000FF
* W 1.5,15.0,3.0,10.0, 0, -1, -1, 0xFFAAAAFF
*/

// load sectors from file -> state
int load_sectors(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) { return -1; }

    int retval = 0;
    enum { SCAN_SECTOR, SCAN_WALL, SCAN_NONE } ss = SCAN_NONE;

    char line[1024], buf[64];
    while (fgets(line, sizeof(line), f)) {
        const char* p = line;
        while (isspace(*p)) {
            p++;
        }

        // skip line, empty or comment
        if (!*p || *p == '#') {
            continue;
        }
        else if (*p == '[') {
            strncpy(buf, p + 1, sizeof(buf));
            const char* section = strtok(buf, "]");
            if (!section) { retval = -2; goto done; }

            if (!strcmp(section, "SECTOR")) { ss = SCAN_SECTOR; }
            else if (!strcmp(section, "WALL")) { ss = SCAN_WALL; }
            else { retval = -3; goto done; }
        }
        else {
            switch (ss) {
            case SCAN_WALL: {
                struct wall* wall = &state.walls.arr[state.walls.n++];
                if (sscanf(
                    p,
                    "%d %d %d %d %d",
                    &wall->a.x,
                    &wall->a.y,
                    &wall->b.x,
                    &wall->b.y,
                    &wall->portal)
                    != 5) {
                    retval = -4; goto done;
                }
            }; break;
            case SCAN_SECTOR: {
                struct sector* sector = &state.sectors.arr[state.sectors.n++];
                if (sscanf(
                    p,
                    "%d %zu %zu %f %f",
                    &sector->id,
                    &sector->firstwall,
                    &sector->nwalls,
                    &sector->zfloor,
                    &sector->zceil)
                    != 5) {
                    retval = -5; goto done;
                }
            }; break;
            default: retval = -6; goto done;
            }
        }
    }

    if (ferror(f)) { retval = -128; goto done; }
done:
    fclose(f);
    return retval;
}