#include <wiringPi.h>
#include <stdio.h>

#include "phone.h"

#define PHONE_ROW_1     5
#define PHONE_ROW_2     6
#define PHONE_ROW_3     7
#define PHONE_COLUMN_1  12
#define PHONE_COLUMN_2  13
#define PHONE_COLUMN_3  10
#define PHONE_COLUMN_4  11

char RefPhoneNumber[11] = {'1', '8', '0', '5', '4', '8', '2', '7', '3', '5', '1'};
char phoneNumber[11] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
int didgitIndex = 0;
int validPhoneNumber = 0;

int inputButtonPressed(int button)
{
    int press = 0;
    while (digitalRead(button) == 0)
    {
        press = 1;
    }
    
    return press;
}

void DisplayPhoneNumber()
{
    
    for (int i = 0; i < MAX_PHONE_NUMBER_DIGIT; i++)
       printf("%c", phoneNumber[i]);  
}

void InsertPhoneDidgit(char c)
{
    if (validPhoneNumber == 0)
    {
        phoneNumber[didgitIndex++] = c;
        if (didgitIndex >= MAX_PHONE_NUMBER_DIGIT)
            didgitIndex = 0;
        DisplayPhoneNumber();
     }
}

int CheckPhoneNumber()
{
    validPhoneNumber = 1;
    int i; 
    for (int j = 0; j < MAX_PHONE_NUMBER_DIGIT; j++)
    {
        validPhoneNumber = 1;
        for (i = 0; i < MAX_PHONE_NUMBER_DIGIT; i++)
            if (phoneNumber[(i+j) % MAX_PHONE_NUMBER_DIGIT] != RefPhoneNumber[i])
            {
                validPhoneNumber = 0;
                break;
            }
        if (validPhoneNumber == 1)
            break;
    }
        
    return validPhoneNumber;
}

void InitPhone()
{
    didgitIndex = 0;
    validPhoneNumber = 0;
    
    pinMode(PHONE_ROW_1, OUTPUT) ;	    // fil 5
    pinMode(PHONE_ROW_2, OUTPUT) ;	    // fil 6
    pinMode(PHONE_ROW_3, OUTPUT) ;	    // fil 7
    pinMode(PHONE_COLUMN_1, INPUT) ;	// fil 8
    pinMode(PHONE_COLUMN_2, INPUT) ;	// fil 9
    pinMode(PHONE_COLUMN_3, INPUT) ;	// fil 10
    pinMode(PHONE_COLUMN_4, INPUT) ;	// fil 11

    pullUpDnControl(PHONE_ROW_1, PUD_UP) ;
    pullUpDnControl(PHONE_ROW_2, PUD_UP) ;
    pullUpDnControl(PHONE_ROW_3, PUD_UP) ;
    pullUpDnControl(PHONE_COLUMN_1, PUD_UP) ;
    pullUpDnControl(PHONE_COLUMN_2, PUD_UP) ;
    pullUpDnControl(PHONE_COLUMN_3, PUD_UP) ;
    pullUpDnControl(PHONE_COLUMN_4, PUD_UP) ;
}

char ReadPhone()
{      
    char keyPress = ' ';
    
    digitalWrite(5, LOW);
    delay(0);
       if (inputButtonPressed(13))
       {
           keyPress = '9';
       }
       else if (inputButtonPressed(12))
       {
           keyPress = '6';
       }
       else if (inputButtonPressed(10))
       {
           keyPress = '3';
       }
       else if (inputButtonPressed(11))
       {
           keyPress = '#';
       }
    digitalWrite(5, HIGH);

    digitalWrite(6, LOW);
    delay(0);
       if (inputButtonPressed(13))
       {
           keyPress = '8';
       }
       else if (inputButtonPressed(12))
       {
           keyPress = '5';
       }
       else if (inputButtonPressed(10))
       {
           keyPress = '2';
       }
       else if (inputButtonPressed(11))
       {
           keyPress = '0';
       }
    digitalWrite(6, HIGH);

    digitalWrite(7, LOW);
    delay(0);
       if (inputButtonPressed(13))
       {
           keyPress = '7';
       }
       else if (inputButtonPressed(12))
       {
           keyPress = '4';
       }
       else if (inputButtonPressed(10))
       {
           keyPress = '1';
       }
       else if (inputButtonPressed(11))
       {
           keyPress = '*';
       }
    digitalWrite(7, HIGH);
    if (keyPress != ' ')
        printf("Key held: %c\n", keyPress);  
    return keyPress;
}
