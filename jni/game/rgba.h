#pragma once

class CRGBA
{
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
	
   /* uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;*/

    CRGBA(unsigned char red, unsigned char green, unsigned blue);
    CRGBA(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha);
    CRGBA(CRGBA const &rhs);
    CRGBA(unsigned int intValue);
    CRGBA();

    void Set(unsigned char red, unsigned char green, unsigned blue);
    void Set(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha);
    void Set(CRGBA const &rhs);
    void Set(CRGBA const &rhs, unsigned char alpha);
    void Set(unsigned int intValue);

    CRGBA ToRGB() const;
    unsigned int ToInt() const;
    unsigned int ToIntARGB() const;

    void Invert();
    CRGBA Inverted() const;

    bool operator==(CRGBA const &rhs) const;
    CRGBA &operator=(CRGBA const &rhs);
};