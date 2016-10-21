#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include <string>

#include "global.h"
#include "button.h"
#include "util.h"

button::button(const char * sText, int sX, int sY, int sWidth, int sHeight)
{
    X = sX;
    Y = sY;
    width = sWidth;
    height = sHeight;
    text = sText;

    textX =  X + 8;
    textY = Y + 8;

    Pressed = false;
    initial = false;
}

button::button(titleData *d, int sX, int sY)
{
    dat = d;
    X = sX;
    Y = sY;
    width = 52;
    height = 52;
}

const titleData *button::getDat()
{
    return dat;
}

void button::update(const touchPosition p)
{
    prev = cur;
    cur = p;

    if(prev.px == 0 && prev.py == 0 && isOver())
        initial = true;
    else if(initial && !isOver())
        initial = false;
}

void button::draw()
{
    if(dat == NULL)
    {
        sf2d_draw_rectangle(X - 1, Y - 1, width + 2, height + 2, RGBA8(64, 64, 64, 255));
        if(Pressed)
            sf2d_draw_rectangle(X, Y, width, height, RGBA8(200, 200, 200, 255));
        else
            sf2d_draw_rectangle(X, Y, width, height, RGBA8(244, 244, 244, 255));

        sftd_draw_text(font, textX, textY, RGBA8(0, 0, 0, 255), 12, text.c_str());
    }
    else
        dat->drawIcn(X, Y);
}

//This checks whether the stylus is inside the button
bool button::isOver()
{
    return (cur.px > X && cur.px < X + width) && (cur.py > Y && cur.py < Y + height);
}

//Checks if the stylus was removed.
bool button::released()
{
    if(isOver() && initial)
    {
        Pressed = true;
    }
    else
    {
        //This should now only return true if
        //the stylus is lifted directly off.
        //should return false now if moved away to different part
        if(Pressed && !touchPressed(cur))
        {
            Pressed = false;
            return true;
        }
        else
        {
            Pressed = false;
        }
    }
    return false;
}


int button::getX()
{
    return X;
}

int button::getY()
{
    return Y;
}
