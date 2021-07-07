#include "Includes.h"

typedef struct{
    Length window;
    Coord origin;
    Offset gridOffset;
    uint scale;
}Camera;

void drawGridLines(const Camera cam)
{
    const Length gridlen = coordDiv(cam.window, cam.scale);

    setColor(GREY);
    for(uint y = 0; y < gridlen.y+1; y++){
        drawHLine(0, cam.gridOffset.y+y*cam.scale, cam.window.x);
        drawHLine(0, cam.gridOffset.y+y*cam.scale-1, cam.window.x);
    }
    for(uint x = 0; x < gridlen.x+1; x++){
        drawVLine(cam.gridOffset.x+x*cam.scale, 0, cam.window.y);
        drawVLine(cam.gridOffset.x+x*cam.scale-1, 0, cam.window.y);
    }

    setColor(RED);
    if(inBound(cam.origin.y, 0, cam.window.y))
    drawHLine(0, cam.origin.y, cam.window.x);
    if(inBound(cam.origin.x, 0, cam.window.x))
    drawVLine(cam.origin.x, 0, cam.window.y);
}

Offset calcGridOffset(const Coord origin, const uint scale)
{
    const Coord mcoord = coordMod(origin, scale);
    return (const Coord){
        mcoord.x < 0 ? scale - abs(mcoord.x) : mcoord.x,
        mcoord.y < 0 ? scale - abs(mcoord.y) : mcoord.y
    };
}

void outlineMouseTile(const Camera cam)
{
    const Coord m1 = coordMod(coordSub(mouse.pos, cam.gridOffset), cam.scale);
    const Coord m = coordSub(
        mouse.pos,
        (const Coord){
            m1.x < 0 ? cam.scale-abs(m1.x) : m1.x,
            m1.y < 0 ? cam.scale-abs(m1.y) : m1.y
        }
    );
    setColor(CYAN);
    fillBorderCoordSquare(m, cam.scale, 4);
    setTextColor(CYAN);
    char buf[32] = {0};
    sprintf(buf, "{% 4i,% 4i}", m.x, m.y);
    drawTextCoord(m, buf);
}

uint menu(const Length window)
{
    setTextSize(24);
    setTextColor(WHITE);
    setColor(WHITE);
    const char *optionLabel[] = {
        "Exit",
        "New Tile Map",
        "Open Tile Map",
    };
    Rect optionBox[3] = {0};
    spanTextListRectCentered(optionBox, coordDiv(window, 2),(Coord){window.x/2, window.y}, 3, optionLabel);

    while(1){
        Ticks frameStart = getTicks();
        clear();

        spanTextListCentered(coordDiv(window, 2),(Coord){window.x/2, window.y}, 3, optionLabel);

        for(uint i = 0; i < 3; i++){
            setColor(WHITE);
            if(coordInRect(mouse.pos, optionBox[i])){
                setColor(RED);
                if(mouseBtnReleased(MOUSE_L))
                    return i;
            }

            fillBorderRect(optionBox[i], 4);
        }

        draw();
        events(frameStart + TPF);
    }
    return 0;
}

Coord tileAt(const Coord pos, const Camera cam)
{
    char buf[32] = {0};
    const Coord protoTile = coordDiv(coordSub(pos, cam.origin), cam.scale);
    const Coord tile = {
        pos.x < cam.origin.x ? protoTile.x-1 : protoTile.x,
        pos.y < cam.origin.y ? protoTile.y-1 : protoTile.y
    };
    sprintf(buf, "(% 4i,% 4i)", tile.x, tile.y);
    setTextColor(WHITE);
    drawTextCoord(pos, buf);
    return tile;
}

void drawCamInfo(const Coord pos, const Camera cam)
{
    char buf0[32] = {0};
    char buf1[32] = {0};
    char buf2[32] = {0};
    char buf3[32] = {0};
    char buf4[32] = {0};
    setTextColor(WHITE);
    sprintf(buf0, "window:(Length)    {% 4i,% 4i}", cam.window.x, cam.window.y);
    sprintf(buf1, "origin:(Coord)     {% 4i,% 4i}", cam.origin.x, cam.origin.y);
    sprintf(buf2, "gridOffset:(Offset){% 4i,% 4i}", cam.gridOffset.x, cam.gridOffset.y);
    sprintf(buf3, "scale:(uint)       {%3u}", cam.scale);
    sprintf(buf4, "mouse.pos:(Coord)  {% 4i,% 4i}", mouse.pos.x, mouse.pos.y);
    spanTextListLeft(pos, coordShift(pos, DIR_D, getTextSize()*5), 5,
        (const char*[5]){buf0, buf1, buf2, buf3, buf4}
    );
}

int main(int argc, char const *argv[])
{
    init();
    Camera cam = {
        .origin = (Offset){0,0},
        .window = (Length){800, 600},
        .gridOffset = (Offset){0,0},
        .scale = 40,
    };

    setWindowLen(cam.window);
    switch(menu(cam.window)){
        case 0:
            printf("Exiting now.\n");
            exit(0);
            break;
        case 1:
            printf("Starting new tile map.\n");
            break;
        case 2:
            printf("Opening tile map.\n");
            break;
        default:
            break;
    }

    while(1){
        Ticks frameStart = getTicks();
        clear();

        if(mouseBtnState(MOUSE_L))
        cam.origin = coordOffset(cam.origin, mouse.vec);
        cam.gridOffset = calcGridOffset(cam.origin, cam.scale);

        if(mouseScrolled(MW_U))
        cam.scale = lbound(cam.scale-5, 5);
        if(mouseScrolled(MW_D))
        cam.scale += 5;

        drawGridLines(cam);
        outlineMouseTile(cam);

        const Coord outlinedTile = tileAt(mouse.pos, cam);
        drawCamInfo(coordDiv(cam.window, 16), cam);

        draw();
        events(frameStart + TPF);
    }
    return 0;
}
