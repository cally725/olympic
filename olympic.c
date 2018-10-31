#include <simple2d.h>
#include <wiringPi.h>
#include <stdio.h>
#include <time.h>
#include "phone.h"

/*
Game    Room                Device                  WiringPi    Pi Pin  I/O

Olympic	Chambre	            Anneaux Sensors	        0	        11	    In 
                            Anneaux Sensors Bypass	1, 2	    12, 13	Out
                            Anneaux Dual Dors	    1, 2	    12, 13	Out
                            Phone Buttons	        5	        18	    Out
                            Phone Buttons	        6	        22	    Out
                            Phone Buttons	        7	        7	    Out
                            Phone Buttons	        10	        24	    In 
                            Phone Buttons	        11	        26	    In 
                            Phone Buttons	        12	        19	    In 
                            Phone Buttons	        13	        21	    In 
                            Phone Bypass	        No	        No	
                            Fixation Sensors	    3	        15	    In 
                            Fixation Sensors Bypass	4	        16	    Out
                            Fixation Trap	        4	        16	    Out
                            Magnetic Card Bypass	21	        29	    Out
*/

/*
 * Constant definition
 */

#define ANNEAUX_SENSORS                 0
#define BYPASS_ANNEAUX_SENSORS_LEFT     1
#define BYPASS_ANNEAUX_SENSORS_RIGHT    2
#define ANNEAUX_DOOR_LEFT               1
#define ANNEAUX_DOOR_RIGHT              2
#define FIXATION_SENSORS                3
#define FIXATION_DOOR                   4
#define BYPASS_FIXATION_DOOR            4
#define BYPASS_PUCE_MAGNET              21
#define BYPASS_PHONE                    99
/*
 * Variable definition
 */
 
char anneauxSensorsLeftBypass[30] = {"BYPASS_ANNEAUX_SENSORS_LEFT"};
char anneauxSensorsRightBypass[30] = {"BYPASS_ANNEAUX_SENSORS_RIGHT"};
char phoneBypass[30] = {"BYPASS_PHONE"};
int phoneBypassActive = 0;
time_t anneauxSensorsBypassTimer = 0;
char puceMagnetBypass[30] = {"BYPASS_PUCE_MAGNET"};
time_t puceMagnetBypassTimer = 0;
char fixationDoorBypass[30] = {"BYPASS_FIXATION_DOOR"};
time_t fixationDoorBypassBypassTimer = 0;

time_t noTimer = -1;

char key = ' ';

/*
 * Function :   checkBypass
 * Description: Check if the corresponding bypass file is present in the current directory
 *              if it is it means that we need to bypass the correcponding switch
 * 
 * Parameters:  file        Name of the file to check
 *              pin         Pin to drive for the bypass
 *              state       State of the pin to set
 *              startTime   Needed when a 10 second delay is required before restoring initial value
 *
 * Return       No return value
 * 
 */
void checkBypass(char *file, int pin, int state, time_t *startTime)
{
	FILE *file1;
    

	file1 = fopen(file, "rb");
	if (file1)
	{
        if (pin == BYPASS_PHONE)
        {
            phoneBypassActive = 1;
        }   
        else if (*startTime == noTimer)
        {
            digitalWrite(pin, state);
        }
        else if (*startTime == 0)
        {
            *startTime = time(NULL);
            digitalWrite(pin, state);
        }
        else
        {
            time_t curTime = time(NULL);
            if ((curTime - *startTime) > 10)
                {
                    if (state == LOW)
                        digitalWrite(pin, HIGH);
                    else
                        digitalWrite(pin, LOW);

                }
        }
	}
}

/*
 * Function :   TimedActivate
 * Description: Activate n IO pin for 10 seconds1
 * 
 * Parameters:  pin         Pin to drive for the bypass
 *              state       State of the pin to set
 *              startTime   Needed when a 10 second delay is required before restoring initial value
 *
 * Return       No return value
 * 
 */
void TimedActivate(int pin, int state, time_t *startTime)
{

    if (*startTime == 0)
    {
        *startTime = time(NULL);
        digitalWrite(pin, state);
    }
    else
    {
        time_t curTime = time(NULL);
        if ((curTime - *startTime) > 10)
            {
                if (state == LOW)
                    digitalWrite(pin, HIGH);
                else
                    digitalWrite(pin, LOW);
            }
    }
}

/*
 * Function :   CheckControls
 * Description: Activate n IO pin for 10 seconds1
 * 
 * Parameters:  pin         Pin to drive for the bypass
 *              state       State of the pin to set
 *              startTime   Needed when a 10 second delay is required before restoring initial value
 *
 * Return       No return value
 * 
 */
void CheckControls()
{
    // Anneaux management
    // Check if anneaux sensors are all in place (set to ground)
    // If closed then open both doors
    if (digitalRead(ANNEAUX_SENSORS) == 0)
    {
        digitalWrite(ANNEAUX_DOOR_LEFT, LOW);
        digitalWrite(ANNEAUX_DOOR_RIGHT, LOW);
    }
    // Fixation management
    // Check if fixations sensors are all in place (set to ground)
    // If closed then open the door
    if (digitalRead(FIXATION_SENSORS) == 0)
        digitalWrite(FIXATION_DOOR, LOW);
            
    // Bypass management    
    // Check if any of the bypass file exist
    checkBypass(anneauxSensorsLeftBypass, BYPASS_ANNEAUX_SENSORS_LEFT, LOW, &noTimer);
    checkBypass(anneauxSensorsRightBypass, BYPASS_ANNEAUX_SENSORS_RIGHT, LOW, &noTimer);
    checkBypass(puceMagnetBypass, BYPASS_PUCE_MAGNET, LOW, &noTimer);
    checkBypass(fixationDoorBypass, BYPASS_FIXATION_DOOR, LOW, &noTimer);
    checkBypass(phoneBypass, BYPASS_PHONE, LOW, &noTimer);

}

//=======================================================

S2D_Image *voice;

S2D_Text *txt;

S2D_Window *window;


void ClearScreen()
{
    S2D_DrawTriangle(
        320,  90, 0, 0, 0, 1,
        150, 165, 0, 0, 0, 1,
        150, 315, 0, 0, 0, 1);

    S2D_DrawTriangle(
        320,  90, 0, 0, 0, 1,
        150, 315, 0, 0, 0, 1,
        320, 390, 0, 0, 0, 1);

    S2D_DrawTriangle(
        320,  90, 0, 0, 0, 1,
        320, 390, 0, 0, 0, 1,
        490, 315, 0, 0, 0, 1);

    S2D_DrawTriangle(
        320,  90, 0, 0, 0, 1,
        490, 315, 0, 0, 0, 1,
        490, 165, 0, 0, 0, 1);
}

void DisplayNumber()
{
    S2D_SetText(txt, "123456");
    txt->x = 250;
    txt->y = 220;
    S2D_DrawText(txt);
}

char ReadFakePhone()
{
    if (key == ' ')
        return ' ';
    else
    {
        char aKey = key;
        key = ' ';
        return aKey;
    }
}
void Render() {
	
    CheckControls();
    char c = ReadPhone();
    if (c != ' ')
        InsertPhoneDidgit(c);
    if ((CheckPhoneNumber() == 0) && (phoneBypassActive == 0))
        ClearScreen();
    else
        DisplayNumber();
}

void on_key(S2D_Event e) 
{
    switch (e.type) 
    {
    case S2D_KEY_DOWN:
        printf("Key down: %s\n", e.key);
        if (strcmp(e.key, "Escape") == 0) S2D_Close(window);
        if (strcmp(e.key, "Return") == 0) 
        {
            printf("Key held: %s\n", e.key);
        }
        printf("Key held: %s\n", e.key);        
        key = e.key[0];
        break;

        case S2D_KEY_HELD:
            printf("Key held: %s\n", e.key);
            break;

        case S2D_KEY_UP:
            printf("Key up: %s\n", e.key);
            break;
    }
}

int main() 
{    
    wiringPiSetup() ;
    
    pinMode(ANNEAUX_SENSORS, INPUT);
    pinMode(BYPASS_ANNEAUX_SENSORS_LEFT, OUTPUT);
    pinMode(BYPASS_ANNEAUX_SENSORS_RIGHT, OUTPUT);
    pinMode(ANNEAUX_DOOR_LEFT, OUTPUT);
    pinMode(ANNEAUX_DOOR_RIGHT, OUTPUT);
    pinMode(FIXATION_SENSORS, INPUT);
    pinMode(FIXATION_DOOR, OUTPUT);
    pinMode(BYPASS_FIXATION_DOOR, OUTPUT);
    pinMode(BYPASS_PUCE_MAGNET, OUTPUT);
    pinMode(BYPASS_PUCE_MAGNET, OUTPUT);

    
    pullUpDnControl(ANNEAUX_SENSORS, PUD_UP);
    pullUpDnControl(BYPASS_ANNEAUX_SENSORS_LEFT, PUD_UP);
    digitalWrite(BYPASS_ANNEAUX_SENSORS_LEFT, HIGH);
    pullUpDnControl(BYPASS_ANNEAUX_SENSORS_RIGHT, PUD_UP);
    digitalWrite(BYPASS_ANNEAUX_SENSORS_RIGHT, HIGH);
    pullUpDnControl(FIXATION_SENSORS, PUD_UP);
    pullUpDnControl(FIXATION_DOOR, PUD_UP);
    digitalWrite(FIXATION_DOOR, HIGH);
    pullUpDnControl(BYPASS_FIXATION_DOOR, PUD_UP);
    digitalWrite(BYPASS_FIXATION_DOOR, HIGH);
    pullUpDnControl(BYPASS_PUCE_MAGNET, PUD_UP);
    digitalWrite(BYPASS_PUCE_MAGNET, HIGH);

    InitPhone();
    phoneBypassActive = 0;
    
    txt = S2D_CreateText("Alien-Encounters-Regular.ttf", "", 40);

    window = S2D_CreateWindow("Hello Triangle", 640, 480, NULL, Render, 0);

    voice = S2D_CreateImage("Voice.png");    

    txt = S2D_CreateText("Alien-Encounters-Regular.ttf", "Missile Launched", 40);

    txt->color.r = 0.0;
    txt->color.g = 0.8;
    txt->color.b = 0.0;
    txt->color.a = 1.0;

    window->on_key = on_key;

    S2D_Show(window);

    S2D_FreeImage(voice);

    S2D_FreeText(txt);
  
    S2D_FreeWindow(window);

    S2D_Close(window);

    return 0;
}



 
