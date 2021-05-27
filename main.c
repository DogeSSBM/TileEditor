#include "Includes.h"

void drawGridLines(const Offset origin, const uint scale)
{
	const Length window = getWindowLen();
	const Length gridlen = coordDiv(window, scale);
	const Offset firstLineOff = coordMod(origin, scale);

	setColor(WHITE);
	for(uint y = 0; y < gridlen.y; y++){
		drawHLine(0, firstLineOff.y+y*scale, window.x);
	}
	for(uint x = 0; x < gridlen.x; x++){
		drawVLine(firstLineOff.x+x*scale, 0, window.y);
	}

	setColor(RED);
	if(inBound(origin.y, 0, window.y))
		drawHLine(0, origin.y, window.x);
	if(inBound(origin.x, 0, window.x))
		drawVLine(origin.x, 0, window.y);
}

int main(int argc, char const *argv[])
{
	init();

	uint scale = 20;
	Offset off = {0};
	Offset offstart = {0};
	Offset offstop = {0};
	Offset offrel = {0};
	while(1){
		Ticks frameStart = getTicks();
		clear();

		setColor(WHITE);
		if(mouseBtnPressed(MOUSE_L))
			offstart = off;

		if(mouseBtnHeld(MOUSE_L) && mouseMoving()){
			off = coordOffset(off, mouse.vec);
			offrel = coordSub(off, offstart);
			offstop = off;
		}

		drawLineCoords(offstart, offstop);
		fillCircleCoord(off, scale);
		drawGridLines(off, scale);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
