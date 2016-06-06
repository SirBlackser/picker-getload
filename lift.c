#include "lift.h"
#include <stdbool.h>

//writen by Dries Blontrock and Oliver Nyssen

float currentHeight; //in mm
//int initHeight = 32; //initial height, arbitrairly found/measured (in mm)
float initHeight;
sint16 initPos;
bool debug = false;

int LiftInit()
{
    //alles initialiseren
    uint8 state = 0;
    initPos = 0;
    initHeight = 0;
    currentHeight = 0;
    // liftmotor uit brake modus zetten (PID)
    LegoMotorSetup(&LegoMotor,3,0,0);
    // liftmotor laten zakken tot beginpositie
    LegoMotorDirectControl(&LegoMotor,3,-25000);
    LegoSensorGetSwitchData(&LegoSensor, 3, &state);

    // Blijven dalen tot drukknup is ingedrukt
    while (state != 1) {
        LegoSensorGetSwitchData(&LegoSensor, 3, &state);
    }
    LegoMotorDirectControl(&LegoMotor,3,0); //leg motor stil
    LegoMotorSetup(&LegoMotor,3,1,0);   //brake opzetten

    if(state == 1)
    {
        LegoMotorSetup(&LegoMotor,3,0,0);
        //beginPosition is 2 cm above ground (measured the top)
        LegoMotorDirectControl(&LegoMotor,3,25000);
        _delay_ms(1000);
        LegoMotorDirectControl(&LegoMotor,3,0); //leg motor still
        LegoMotorSetup(&LegoMotor,3,1,0);   //brake opzetten
        //endPosition is 3.3 cm, measured the top
        LegoMotorSetPos(&LegoMotor, 3, initPos);    //initiele postie instellen
        LiftGetHeight(&initHeight); // initiele hoogte insetllen
        if(debug == true)
        {
            printf("initHeigt: %f\n", initHeight);
        }
        system ("espeak -ven+f2 -k5 -a50 -s150 \"ForkLift Ready.\" --stdout | aplay");
        return 0;
    }
    else
    {
        return 1;
    }
    return 2;
}

//the input should be in milimeters
int LiftGoto(float Height)
{
    float Goto = Height;        // hoogte die je wenst te bereiken.
    int speed = 0;
    //haal de huidige hoogte op
    LiftGetHeight(&currentHeight);
    // indien gewenste hoogte groter is dan huidige hoogte
    if(Goto >= currentHeight)
    {
        speed = 25000;  //snelheid voor omhoog
    }
    // indien gewenste hoogte kleiner is dan huidige hoogte
    else
    {
        speed = -25000; //snelheid voor omlaag
    }
    if (debug == true)
    {
        printf("goto: %f\n", Goto);
        printf("current: %f\n", currentHeight);
        printf("speed: %d\n", speed);
    }

    LegoMotorSetup(&LegoMotor,3,0,0);   //liftmotor ui brake
    LegoMotorDirectControl(&LegoMotor,3,speed); //snelheid instellen
    LiftGetHeight(&currentHeight);

    //nakijken of hoogte al breikt is
    if(speed > 0)
    {
        while(Goto > currentHeight)
        {
            // lift naar boven
            LiftGetHeight(&currentHeight);
            if(debug == true) { printf("current: %f\n", currentHeight); }
        }
    }
    else
    {
        while(Goto < currentHeight)
        {
            // lift naar beneden
            LiftGetHeight(&currentHeight);
            if(debug == true) { printf("current: %f\n", currentHeight); }
        }
    }

    LegoMotorDirectControl(&LegoMotor,3,0); // leg de motor stil
    LegoMotorSetup(&LegoMotor,3,1,0);   // brake inschakelen
    return 1;
}

// haal hoogte op van de lift
int LiftGetHeight(float *Height)
{
    LegoMotorGetPos(&LegoMotor, 3, &initPos);
    float temp = initPos/80.0;  //1 mm is 80 halve graden
    Height = &temp;
    currentHeight = temp;
    if(debug == 1)
    {
        printf("initpos: %d\n", initPos);
        printf("height: %f\n", *Height);
        printf("Ready!\n");
    }
    return 2;
}
