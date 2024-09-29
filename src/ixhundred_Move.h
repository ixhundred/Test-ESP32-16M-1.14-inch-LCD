
#ifndef ixhundred_Move_h
#define ixhundred_Move_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

class ixhundred_Move
{
    public:
        float _maxx = 20;
        float _maxy = 4;
        float _tx = 0;
        float _ty = 0;
        float _cx = 0;
        float _cy = 0;
        int _char = ' ';
/*
        ixhundred_Move();
        ixhundred_Move(char ch);
        ixhundred_Move(char ch, float tx, float ty);
        ixhundred_Move(char ch, float tx, float ty, float maxx, float maxy);
*/        
        void init();
        void init(char ch);
        void init(char ch, float tx, float ty);
        void init(char ch, float tx, float ty, float maxx, float maxy);
        void init(char ch, float tx, float ty, float maxx, float maxy, float speedx, float speedy);
        void setmaxx(float maxx);
        void setmaxy(float maxy);
        void setmax(float maxx, float maxy);
        void setspeedx(float speedx);
        void setspeedy(float speedy);
        void sespeed(float speedx, float speedy);
        void settx(float tx);
        void setty(float ty);
        void settxy(float tx, float ty);
        void randompos();
        bool move_finished();
        void move_next(LiquidCrystal_I2C &lcd, ixhundred_Move* titles);
        void move_nextxy(Adafruit_ST7789 &tft);
        void show(LiquidCrystal_I2C &lcd);
        void showxy(Adafruit_ST7789 &tft);
    private:
        float _speedx = 1;
        float _speedy = 1;
        float _difff = 0;
};

#endif

