#include "ixhundred_Move.h"
#include <LiquidCrystal_I2C.h>

void ixhundred_Move::init() 
{
}

void ixhundred_Move::init(char ch)
{
    _char = ch;
}

void ixhundred_Move::init(char ch, float tx, float ty)
{
    init(ch);
    settxy(tx,ty);
    //Serial.printf("#char=%c, tx=%d, cx=%d, ty=%d, cy=%d, maxx=%d, maxy=%d\r\n",_char, (int)_tx,(int)_cx,(int)_ty,(int)_cy,(int)_maxx,(int)_maxy);
}

void ixhundred_Move::init(char ch, float tx, float ty, float maxx, float maxy)
{
    init(ch,tx,ty);
    _maxx = maxx;
    _maxy = maxy;
}

/*
ixhundred_Move::ixhundred_Move()
{

}

ixhundred_Move::ixhundred_Move(char ch)
{
    init(ch);
}

ixhundred_Move::ixhundred_Move(char ch, float tx, float ty)
{
    init(ch,tx,ty);
    Serial.printf("#init %c,%d,%d,%d,%d\r\n",_char,(int)_tx,(int)_ty,(int)_cx,(int)_cy);    
}

ixhundred_Move::ixhundred_Move(char ch, float tx, float ty, float maxx, float maxy)
{
    init(ch,tx,ty,maxx,maxy);
}
*/

void ixhundred_Move::setmaxx(float maxx)
{
    _maxx = maxx;
}

void ixhundred_Move::setmaxy(float maxy)
{
    _maxy = maxy;
}

void ixhundred_Move::setmax(float maxx, float maxy)
{
    setmaxx(maxx);
    setmaxy(maxy);
}

void ixhundred_Move::settx(float tx)
{
    _tx = tx;
    _cx = _tx;
}

void ixhundred_Move::setty(float ty)
{
    _ty = ty;
    _cy = _ty;
}

void ixhundred_Move::settxy(float tx, float ty)
{
    settx(tx);
    setty(ty);
}

void ixhundred_Move::setspeedx(float speedx)
{
    _speedx = speedx;
}

void ixhundred_Move::setspeedy(float speedy)
{
    _speedy = speedy;
}

void ixhundred_Move::sespeed(float speedx, float speedy)
{
    setspeedx(speedx);
    setspeedy(speedy);
}

bool ixhundred_Move::move_finished()
{
    //Serial.printf("#tx %d == cx %d && ty %d == dy %d\r\n",(int)_tx,(int)_cx,(int)_ty,(int)_cy);
    return (int)_tx == (int)_cx && (int)_ty == (int)_cy;
}

void ixhundred_Move::randompos(LiquidCrystal_I2C &lcd)
{
    _cx = random(0,_maxx);
    _cy = random(0,_maxy);
}

void ixhundred_Move::move_next(LiquidCrystal_I2C &lcd, ixhundred_Move* titles)
{
    if ((int)_cx != (int)_tx || (int)_cy != (int)_ty) {
        float diffx = abs(_tx-_cx);
        float diffy = abs(_ty-_cy);
        if (diffx >= diffy) {
            //move x-axis
            if ((int)_cx != (int)_tx) {
                lcd.setCursor(_cx,_cy);
                lcd.print(' ');
                //for(int i = 0; i < 12; ++i) {
                //    if (titles[i]._tx != _tx && titles[i]._ty != _ty)
                //        if (titles[i]._cx == _cx && titles[i]._cy == _cy) {
                //            lcd.setCursor((int)titles[i]._cx,(int)titles[i]._cy);
                //            lcd.print((char)titles[i]._char);
                //            break;
                //        }
                //}
                if((int)_tx > (int)_cx) {
                    _cx += _speedx;
                    show(lcd);
                }
                else {
                    _cx -= _speedx;
                    show(lcd);
                }
            }
        }
        else {
            //mov y-axis
            if ((int)_cy != (int)_ty) {
                lcd.setCursor(_cx,_cy);
                lcd.print(' ');
                if((int)_ty > (int)_cy) {
                    _cy += _speedy;
                    show(lcd);
                }
                else {
                    _cy -= _speedy;
                    show(lcd);
                }
            }
        }
    }
}

void ixhundred_Move::move_nextxy(LiquidCrystal_I2C &lcd)
{
    
}

void ixhundred_Move::show(LiquidCrystal_I2C &lcd)
{
    lcd.setCursor((int)_cx,(int)_cy);
    lcd.print((char)_char);
    //Serial.printf("#show %c at %d,%d\r\n",_char,(int)_cx,(int)_cy);
}