#include <c64/vic.h>
#include <c64/sprites.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <oscar.h>

inline void printf_xy(unsigned char x, unsigned char y, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    gotoxy(x, y);
    vprintf(fmt, args);
    va_end(args);
}

inline byte peek(unsigned addr)
{
    byte b = ((volatile byte *)addr)[0];
	return b;
}

const char SpriteRes[] = {
	#embed spd_sprites lzo "../assets/gfx/sprites.spd"
};

char *const Screen  = (char *)0x0400;
char *const Sprites = (char *)0x2000;

int JoyPosX = 173;
int JoyPosY = 151;

void fillJoyStateBuffers(char* bitBuffer, char* stringBuffer) {
    byte joyStateInRam = peek(0xdC00);
    bool joyStateChanged = false;
    byte joyStateLen = 0;
    JoyPosX = 173;
    JoyPosY = 151;
    int xDirection = 0;
    int yDirection = 0;

    for(int i = 7; i >= 0; i--) {
        byte bit = joyStateInRam & 1;
        
        bitBuffer[i] = bit ? '1' : '0';

        if(!bit) {
            if(i >= 3) {
                joyStateChanged = true;
            }

            if(i == 7 ) {
                strncat(stringBuffer, " UP", 3);
                yDirection = -1;
                joyStateLen += 3;
            }
            
            if(i == 6) {
                strncat(stringBuffer, " DOWN", 5);
                yDirection = 1;
                joyStateLen += 5;
            }
        
            if(i == 5) 
            {
                strncat(stringBuffer, " LEFT", 5);
                xDirection = -1;
                joyStateLen += 5;
            } 
            
            if(i == 4) 
            {
                strncat(stringBuffer, " RIGHT", 6);
                xDirection = 1;
                joyStateLen += 6;
            }  

            if(i == 3) 
            {
                strncat(stringBuffer, " FIRE", 5);
                joyStateLen += 5;
                vic.color_border = VCOL_RED;
            }
        }     
        else 
        {
            if(i == 3) 
            {
                vic.color_border = VCOL_LT_BLUE;
            }
        }

        joyStateInRam >>= 1;
    }

    if(!joyStateChanged) {
        strncat(stringBuffer, " IDLE", 5);
        joyStateLen += 5;
    }
    else {
        if(xDirection < 0) {
            JoyPosX += xDirection * 24;
        }
        else if(xDirection > 0) {
            JoyPosX += xDirection * 23;
        }
        JoyPosY += yDirection * 21;
    }

    for(int i = joyStateLen; i < 16; i++) {
        stringBuffer[i] = ' ';
    }
}

int main() {
    oscar_expand_lzo(Sprites, SpriteRes);

    clrscr();

    printf_xy(12, 2, "JOYSTICK TESTER");
    printf_xy(2, 7, "STATE: ");
    printf_xy(9, 22, "SEBASTIAN HABA (2026)");
    
    char joyStateStringBuffer[17];
    char joyStateBitsBuffer[9];
    joyStateBitsBuffer[8] = '\0';

    spr_init(Screen);

    // Pointer to sprite SpritesMem / 0x40 + sprite index ex 0x2000 / 0x40
    // 0x40 hex = 64 dec, which is the size of one sprite in bytes. So this calculation gives us the correct offset for each sprite in memory. 
    // Sprite 4 0x84

    spr_set(0, true, JoyPosX, JoyPosY, 0x84, VCOL_RED, false, false, false);
    spr_set(1, true, 160, 140, 0x80, VCOL_GREEN, false, false, false);
    spr_set(2, true, 184, 140, 0x81, VCOL_GREEN, false, false, false);
    spr_set(3, true, 160, 161, 0x82, VCOL_GREEN, false, false, false);
    spr_set(4, true, 184, 161, 0x83, VCOL_GREEN, false, false, false);

    for(;;) {
        memset(joyStateStringBuffer, '\0', sizeof(joyStateStringBuffer));
        fillJoyStateBuffers(&joyStateBitsBuffer[0], &joyStateStringBuffer[0]);
        printf_xy(2, 5, "$DC00: %s", joyStateBitsBuffer);
        printf_xy(8, 7, "%s", joyStateStringBuffer);

        spr_move(0, JoyPosX, JoyPosY);

        vic_waitFrame();
    }

    return 0;
}