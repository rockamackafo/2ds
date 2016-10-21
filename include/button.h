#ifndef BUTTON_H
#define BUTTON_H

#include <3ds.h>
#include <sf2d.h>
#include <string>

#include "titledata.h"
#include "textbox.h"

/*
This is those buttons I use.
sText = what button says
sX = X position
sY = Y position
*/
class button
{
    public:
        button(const char *sText, int sX, int sY, int sWidth, int sHeight);
        button(titleData *d, int sX, int sY);
        const titleData *getDat();
        void update(const touchPosition p);
        void draw();
        bool isOver();
        bool released();
        int getX();
        int getY();

    private:
        bool Pressed, initial;
        int X, Y, width, height;
        int textX, textY;
        std::string text;
        touchPosition prev, cur;
        titleData *dat = NULL;
};

#endif // BUTTON_H
