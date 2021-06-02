#include "Includes.h"

typedef struct{
	Length len;
	Tile **map;

	uint numTileVarients;
	Tile *tileVarients;
}TileMap;

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
	Coord m = coordSub(mouse.pos, coordMod(coordSub(mouse.pos, cam.gridOffset), cam.scale));
	setColor(CYAN);
	fillBorderCoordSquare(m, cam.scale, 4);
}

int main(int argc, char const *argv[])
{
	init();
	setFontSize(16);
	Camera cam = {
		.window = getWindowLen(),
		.origin = (Offset){0,0},
		.gridOffset = (Offset){0,0},
		.scale = 40
	};

	while(1){
		Ticks frameStart = getTicks();
		clear();

		if(windowResized())
			cam.window = getWindowLen();
		if(mouseBtnState(MOUSE_L))
			cam.origin = coordOffset(cam.origin, mouse.vec);
		cam.gridOffset = calcGridOffset(cam.origin, cam.scale);

		drawGridLines(cam);
		outlineMouseTile(cam);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
