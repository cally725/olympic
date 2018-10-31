#include <wiringPi.h>
#include <stdio.h>
#include <time.h>
#include "scales.h"

/*
Game    Room                Device                  WiringPi    Pi Pin  I/O

Asile   Chambre             Bouton Gache Bypass	    0	        11	    Out
                            Puce Magnet Bypass	    1	        12	    Out
                            Balance	                2	        13	    Out
                            Balance Bypass	        2	        13	    Out
					
        Salle electrique	Breaker switch	        3	        15	    In 
                            Breaker Switch Bypass	4	        16	    Out
                            Breaker Door	        4	        16	    Out
					
        Exterieur	        Camera Switch       	5	        18	    In 
                            Camera Switch Bypass	6	        22	    Out
                            Camera Door	            6	        22	    Out
                            Rocks Sensors	        7	        7	    In 
                            Rocks Sensors Bypass	10	        24	    Out
                            Rocks Trap	            10	        24	    Out    
*/

/*
 * Constant definition
 */

#define GACHE_BOUTON_BYPASS     0
#define PUCE_MAGNET_BYPASS      1
#define BALANCE_DOOR            2
#define BALANCE_BAYPASS         2
#define BREAKER_SWITCH          3
#define BREAKER_SWITCH_BYPASS   4
#define BREAKER_DOOR            4
#define CAMERA_SWITCHES         5
#define CAMERA_SWITCHED_BYPASS  6
#define CAMERA_DOOR             6 
#define ROCKS_SENSORS           7
#define ROCKS_SENSORS_BYPASS    10
#define ROCKS_TRAP              10
#define PILS_WEIGHT_MIN         80.0
#define PILS_WEIGHT_MAX         100.0
    
/*
 * Variable definition
 */
 
char gacheButtonBypass[30] = {"GACHE_BOUTON_BYPASS"};
time_t gacheButtonBypassTimer = 0;
char puceMagnetBypass[30] = {"PUCE_MAGNET_BYPASS"};
time_t puceMagnetBypassTimer = 0;
char balanceBypass[30] = {"BALANCE_BAYPASS"};
time_t balanceDoorTimer = 0;
time_t balanceStableTimer = 0;
time_t balanceBypassTimer = 0;
char breakerSwitchBypass[30] = {"BREAKER_SWITCH_BYPASS"};
time_t breakerSwitchTimer = 0;
time_t breakerSwitchBypassTimer = 0;
char cameraSwitchBypass[30] = {"CAMERA_SWITCHED_BYPASS"};
time_t cameraSwitchBypassTimer = 0;
char rocksSensorBypass[20] = {"ROCKS_SENSORS_BYPASS"};
time_t rocksSensorBypassTimer = 0;
time_t noTimer = -1;

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
        if (*startTime == noTimer)
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
 * Function :   main
 * Description: Program for the Asile
 * 
 * Parameters:  None
 * 
 * Return       int but it never returns
 * 
 */  
int main() 
{    
    int initScaleDone = 0;
    
    wiringPiSetup() ;
  
    pinMode(GACHE_BOUTON_BYPASS, OUTPUT);
    pinMode(PUCE_MAGNET_BYPASS, OUTPUT);
    pinMode(BALANCE_DOOR, OUTPUT);
    pinMode(BALANCE_BAYPASS, OUTPUT);
    pinMode(BREAKER_SWITCH, INPUT);
    pinMode(BREAKER_SWITCH_BYPASS, OUTPUT);
    pinMode(BREAKER_DOOR, OUTPUT);
    pinMode(CAMERA_SWITCHES, INPUT);
    pinMode(CAMERA_SWITCHED_BYPASS, OUTPUT);
    pinMode(CAMERA_DOOR, OUTPUT);
    pinMode(ROCKS_SENSORS, INPUT);
    pinMode(ROCKS_SENSORS_BYPASS, OUTPUT);
    pinMode(ROCKS_TRAP, OUTPUT);
    
    pullUpDnControl(GACHE_BOUTON_BYPASS, PUD_DOWN);
    digitalWrite(GACHE_BOUTON_BYPASS, LOW);
    pullUpDnControl(PUCE_MAGNET_BYPASS, PUD_UP);
    digitalWrite(PUCE_MAGNET_BYPASS, HIGH);
    pullUpDnControl(BALANCE_DOOR, PUD_DOWN);
    digitalWrite(BALANCE_DOOR, LOW);
    pullUpDnControl(BALANCE_BAYPASS, PUD_DOWN);
    digitalWrite(BALANCE_BAYPASS, LOW);
    pullUpDnControl(BREAKER_SWITCH, PUD_UP);
    pullUpDnControl(BREAKER_SWITCH_BYPASS, PUD_DOWN);
    digitalWrite(BREAKER_SWITCH_BYPASS, LOW);
    pullUpDnControl(BREAKER_DOOR, PUD_DOWN);
    digitalWrite(BREAKER_DOOR, LOW);
    pullUpDnControl(CAMERA_SWITCHES, PUD_UP);
    pullUpDnControl(CAMERA_SWITCHED_BYPASS, PUD_UP);
    digitalWrite(CAMERA_SWITCHED_BYPASS, HIGH);
    pullUpDnControl(CAMERA_DOOR, PUD_UP);
    digitalWrite(CAMERA_DOOR, HIGH);
    pullUpDnControl(ROCKS_SENSORS, PUD_UP);
    pullUpDnControl(ROCKS_SENSORS_BYPASS, PUD_UP);
    digitalWrite(ROCKS_SENSORS_BYPASS, HIGH);
    pullUpDnControl(ROCKS_TRAP, PUD_UP);
    digitalWrite(ROCKS_TRAP, HIGH);
    
    while (1)
    {
        // Scale management
        if (initScaleDone == 0)
        {
            int r = initScale(1);
            if (r == 0)
            {
                initScaleDone = 1;
            }
        }
        
        if (initScaleDone == 1)
        {
            double weight;
            weight = runScale();
            if (weight == -1.0)
                initScaleDone = 0;
            
            if ((weight > PILS_WEIGHT_MIN) && (weight < PILS_WEIGHT_MAX))
            {
                if (balanceStableTimer == 0)
                {
                    balanceStableTimer = time(NULL);
                }
                else
                {
                    time_t curTime = time(NULL);
                    if ((curTime - balanceStableTimer) > 10)
                    {

                        TimedActivate(BALANCE_DOOR, HIGH, &balanceDoorTimer);
                    }
                }            
            }
            else
            {
                if (balanceStableTimer != 0)
                {
                    balanceStableTimer = 0;
                }
            }
            printf("weight = %lf Oz\n", weight);
        }
            //printf("toto\n");        
        // Breaker management
        // Check if braker swith is closed (set to ground)
        // If closed then open the door
        if (digitalRead(BREAKER_SWITCH) == 0)
            TimedActivate(BREAKER_DOOR, HIGH, &breakerSwitchTimer);

        // Camera management
        // Check if camera swith is closed (set to ground)
        // If closed then open the door
        if (digitalRead(CAMERA_SWITCHES) == 0)
            digitalWrite(CAMERA_DOOR, LOW);
            
        // Rocks management    
        // Check if rocks swith is closed (set to ground)
        // If closed then open the trsp
        if (digitalRead(ROCKS_SENSORS) == 0)
            digitalWrite(ROCKS_TRAP, LOW);
            
        // Bypass management    
        // Check if any of the bypass file exist
        checkBypass(gacheButtonBypass, GACHE_BOUTON_BYPASS, HIGH, &gacheButtonBypassTimer);
        checkBypass(puceMagnetBypass, PUCE_MAGNET_BYPASS, LOW, &noTimer);
        checkBypass(balanceBypass, BALANCE_BAYPASS, HIGH, &balanceBypassTimer);
        checkBypass(breakerSwitchBypass, BREAKER_SWITCH_BYPASS, HIGH, &breakerSwitchBypassTimer);
        checkBypass(cameraSwitchBypass, CAMERA_SWITCHED_BYPASS, LOW, &noTimer);
        checkBypass(rocksSensorBypass, ROCKS_SENSORS_BYPASS, LOW, &noTimer);
    }
  
    closeScale();
    
    return 0;
}
