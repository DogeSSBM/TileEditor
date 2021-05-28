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

	setColor(WHITE);
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

void outlineMouseTile(const Camera cam)
{
	setColor(BLUE);
	fillBorderCoordSquare(
		cam.gridOffset,
		cam.scale,
		4
	);

	const Coord gridPos = coordDiv(mouse.pos, cam.scale);
	const Coord gridScreenPos = coordMul(gridPos, cam.scale);
	const Coord snapPos = coordOffset(gridScreenPos, cam.gridOffset);

	setColor(YELLOW);
	fillCircleCoord(gridScreenPos, cam.scale/8);

	setColor(GREEN);
	fillBorderCoordSquare(
		snapPos,
		cam.scale,
		4
	);
}

int main(int argc, char const *argv[])
{
	init();

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

		if(mouseBtnState(MOUSE_L)){
			cam.origin = coordOffset(cam.origin, mouse.vec);
			cam.gridOffset = coordMod(cam.origin, cam.scale);
		}

		drawGridLines(cam);
		outlineMouseTile(cam);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
