#include "drive.h"
#include <stdbool.h>

//writen by Dries Blontrock & Oliver Nyssen
//geschreven uitbreiding: dynamisch bepalen van threshold value

sint16 kp;
sint16 kd;
sint16 ki;
sint16 imax;
//mm nog berekenen, omtrek (2*r*pi) delen door 360°
float mmPerDeg;
int correction;
int minimum;
int maximum;
int counter;
int threshold;
int stopvalue;
int ignore;
bool debug;

//bereken het verschil in terug gegeven waarden van de sensoren voor de zelfde zwartwaarde (benadering)
void callibration()
{
    uint16 right = 0;
    uint16 left = 0;
    int i;

    float diff[100] = {0};
    int avg = 0;

    // gemiddeld verschil berekenen
    for(i=0; i<100; i++)
    {
        LegoSensorGetLSensorData(&LegoSensor, 1, &right);
        LegoSensorGetLSensorData(&LegoSensor, 2, &left);
        diff[i] = right-left;
        avg += (right+left);
    }

    float sum = 0;
    // bij afwezigheid van Mean --> gemiddelde berekenen
    for(i=0; i<100; i++)
    {
        sum += diff[i];
    }
    correction = sum/100.0;
    avg = avg/200;

    if(debug == true) {printf("average: %d\n", avg);}
    // minimum & maximum worden als average ingesteld, hiermee kan een eerste thresholdvalue berekend worden.
    minimum = avg;
    maximum = avg;
}

//neemt minimum en maximum gemeten waarde en pakt de helft voor gemiddelde waarde
//geeft deze waarde als threshold terug
int adaptiveThreshold(int RSensor, int LSensor)
{
    // indien nieuw maximum gevonden wordt
    if(RSensor > maximum)
    {
        maximum = RSensor;
    }
    // indien nieuw minimum gevonden wordt
    else if(RSensor < minimum)
    {
        minimum = RSensor;
    }

    // idem RSensor
    if(LSensor > maximum)
    {
        maximum = LSensor;
    }
    // idem RSensor
    else if(LSensor < minimum)
    {
        minimum = LSensor;
    }

    if(debug == true) {
        printf("minimum: %d\n", minimum);
        printf("maximum: %d\n", maximum);
    }

    // bereken nieuw gemiddelde
    int averageValue = (minimum + maximum)/2;
    return averageValue;
}


int DriveInit()
{
    //printf("DriveInit\n");
    kp = 2220; //origneel 1800
    kd = 5550; //origneel 4200
    ki = 0;
    imax = 0;
    counter = 0;
    correction = 0;
    ignore = 0;
    //mm nog berekenen, omtrek (2*r*pi) delen door 360°
    mmPerDeg = (53*M_PI)/360;
    threshold = 0;
    stopvalue = 0;
    // debug mode
    debug = false;

    //dan staat deze zeker stil
    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);

    //set podPos to 0
    LegoMotorPosPIDControl(&LegoMotor, 1, 0);
    LegoMotorPosPIDControl(&LegoMotor, 2, 0);

    //zet hoekcallibratie op 0
    LegoMotorSetPos(&LegoMotor, 1, 0);
    LegoMotorSetPos(&LegoMotor, 2, 0);

    // sensoren activeren
    LegoSensorSetupLSensor(&LegoSensor, 1, 1);
    LegoSensorSetupLSensor(&LegoSensor, 2, 1);

    callibration();

    return 0;
}

//distance is in mm, speed is in mm/s
void DriveStraightDistance(float Distance,float Speed)
{
    //printf("DriveStraightDistance\n");
    LegoMotorSetPos(&LegoMotor, 1, 0);
    LegoMotorSetPos(&LegoMotor, 2, 0);
    //te bepalen, mm per graden (via omtrek van wiel) meet diameter. Distance wordt doorgegeven in cm
    //times 2 for half degrees.
    float TotalMoveAngle = (Distance/mmPerDeg)*2;
    //speed wordt doorgegeven in mm/s
    float rotateSpeed = ((fabs(Speed)/100)/mmPerDeg)*2;
    float currentAngle = 0;

    TimeStepInit(10000);

    //achteruit rijden
    if(Distance<0)
    {
        while(currentAngle>TotalMoveAngle)
        {
            currentAngle -= rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, currentAngle, kp, kd, ki, imax);
            LegoMotorPIDControl(&LegoMotor, 2, currentAngle, kp, kd, ki, imax);
            TimeStep(0);
        }
    }
    // vooruit rijden
    else
    {
        while(currentAngle<TotalMoveAngle)
        {
            currentAngle += rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, currentAngle, kp, kd, ki, imax);
            LegoMotorPIDControl(&LegoMotor, 2, currentAngle, kp, kd, ki, imax);
            TimeStep(0);
        }
    }

    // zet motoren in break modus
    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);

}

// draai enkel linker wiel
void DriveRotateLWheel(float Angle,float Speed)
{
    //printf("DriveRotateLWheel\n");
    LegoMotorSetup(&LegoMotor,1,0,1);       // Linker motor rem af
    LegoMotorSetup(&LegoMotor,2,1,1);       // rechter motor rem op
    LegoMotorSetPos(&LegoMotor, 1, 0);
    LegoMotorSetPos(&LegoMotor, 2, 0);      // redudant code
    LegoMotorPosPIDControl(&LegoMotor, 1, 0);
    LegoMotorPosPIDControl(&LegoMotor, 2, 0);       // redudant code

    //times 2 for half degrees.
    float TotalMoveAngle = (Angle*2)/mmPerDeg;
    //speed wordt doorgegeven in mm/s
    float rotateSpeed = ((fabs(Speed)/100)/mmPerDeg);
    float currentAngle = 0;

    TimeStepInit(10000);
    //rotate right
    if(Angle<0)
    {
        while(currentAngle>TotalMoveAngle)
        {
            currentAngle -= rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, currentAngle*2, kp, kd, ki, imax);
            TimeStep(0);
        }
    }
    //rotate left
    else
    {
        while(currentAngle<TotalMoveAngle)
        {
            currentAngle += rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, currentAngle*2, kp, kd, ki, imax);
            TimeStep(0);
        }
    }

    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);
}

// draai enkel rechterwiel
void DriveRotateRWheel(float Angle,float Speed)
{
    //printf("DriveRotateLWheel\n");
    LegoMotorSetup(&LegoMotor,1,1,1);       // zet linker rem op
    LegoMotorSetup(&LegoMotor,2,0,1);       // zet rechter rem af
    LegoMotorSetPos(&LegoMotor, 1, 0);      // redudant code
    LegoMotorSetPos(&LegoMotor, 2, 0);
    LegoMotorPosPIDControl(&LegoMotor, 1, 0);       // redudant code
    LegoMotorPosPIDControl(&LegoMotor, 2, 0);

    //times 2 for half degrees.
    float TotalMoveAngle = (Angle*2)/mmPerDeg;
    //speed wordt doorgegeven in mm/s
    float rotateSpeed = ((fabs(Speed)/100)/mmPerDeg);
    float currentAngle = 0;

    TimeStepInit(10000);
    //rotate left
    if(Angle<0)
    {
        while(currentAngle>TotalMoveAngle)
        {
            currentAngle -= rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 2, currentAngle*2, kp, kd, ki, imax);
            TimeStep(0);
        }
    }
    //rotate right
    else
    {
        while(currentAngle<TotalMoveAngle)
        {
            currentAngle += rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 2, currentAngle*2, kp, kd, ki, imax);
            TimeStep(0);
        }
    }

    // zet beide motoren in rem modus
    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);
}

void DriveRotateCenter(float Angle,float Speed)
{
    //printf("DriveRotateCenter\n");
    // zet beide motoren uit rem modus
    LegoMotorSetup(&LegoMotor,1,0,1);
    LegoMotorSetup(&LegoMotor,2,0,1);

    // zet actuele hoekposities op nul
    LegoMotorSetPos(&LegoMotor, 1, 0);
    LegoMotorSetPos(&LegoMotor, 2, 0);

    // zet PId setpoint op nul
    LegoMotorPosPIDControl(&LegoMotor, 1, 0);
    LegoMotorPosPIDControl(&LegoMotor, 2, 0);

    //times 2 for half degrees.
    float TotalMoveAngle = (Angle*2)/mmPerDeg;
    //speed wordt doorgegeven in mm/s
    float rotateSpeed = ((fabs(Speed)/100)/mmPerDeg);
    float currentAngle = 0;

    TimeStepInit(10000);
    //links achteruit, recht vooruit
    if(Angle<0)
    {
        while(currentAngle>TotalMoveAngle)
        {
            currentAngle -= rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, -currentAngle, kp, kd, ki, imax);
            LegoMotorPIDControl(&LegoMotor, 2, currentAngle, kp, kd, ki, imax);
            TimeStep(0);
        }
    }
    //links vooruit, rechts achteruit
    else
    {
        while(currentAngle<TotalMoveAngle)
        {
            currentAngle += rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, -currentAngle, kp, kd, ki, imax);
            LegoMotorPIDControl(&LegoMotor, 2, currentAngle, kp, kd, ki, imax);
            TimeStep(0);
        }
    }

    // zet motoren in rem modus
    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);
}

// rij een vaste afstand rechtdoor
void DriveLineFollowDistance(int Distance, float Speed)
{
    //printf("DriveLineFollowDistance\n");
    LegoMotorSetup(&LegoMotor,1,0,1);
    LegoMotorSetup(&LegoMotor,2,0,1);

    // zet actuele hoekposities op nul
    LegoMotorSetPos(&LegoMotor, 1, 0);
    LegoMotorSetPos(&LegoMotor, 2, 0);

    // zet PId setpoint op nul
    LegoMotorPosPIDControl(&LegoMotor, 1, 0);
    LegoMotorPosPIDControl(&LegoMotor, 2, 0);

    //channel 1
    uint16 RSensor = 0;
    //channel 2
    uint16 LSensor = 0;
    LegoSensorSetupLSensor(&LegoSensor, 1, 1);
    LegoSensorSetupLSensor(&LegoSensor, 2, 1);

    float TotalMoveAngle = (Distance/mmPerDeg)*2;
    //speed wordt doorgegeven in mm/s
    float rotateSpeed = ((fabs(Speed)/100)/mmPerDeg)*2;
    //individuele hoeken bijhouden, voor correctie
    float currentAngleR = 0;
    float currentAngleL = 0;
    //totale hoek bijhouden, voor afstand
    float currentAngle = 0;
    //correctiermen per wiel
    float correctionL = 0;
    float correctionR = 0;
    float correctieTerm = 1100;     // arbitrair bepaalt

    TimeStepInit(10000);
    // rij achteruit
    if(Distance<0)
    {
        while(currentAngle>TotalMoveAngle)
        {
            // sensoren uitlezen
            LegoSensorGetLSensorData(&LegoSensor, 1, &RSensor);
            LegoSensorGetLSensorData(&LegoSensor, 2, &LSensor);
            LSensor +=correction;

            // maak correctie
            if(RSensor != LSensor)
            {
                correctionL = (RSensor - LSensor)/correctieTerm;
                correctionR = -correctionL;
                if(debug == true) {printf("correction left: %f\n", correctionL);}
            }
            // maak geen correctie
            else
            {
                correctionL = 0;
                correctionR = 0;
            }

            currentAngleR -= (rotateSpeed + correctionR);
            currentAngleL -= (rotateSpeed + correctionL);
            currentAngle -= rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, currentAngleR, kp, kd, ki, imax);
            LegoMotorPIDControl(&LegoMotor, 2, currentAngleL, kp, kd, ki, imax);
            TimeStep(0);
        }
    }

    // vooruit rijden
    else
    {
        while(currentAngle<TotalMoveAngle)
        {
            LegoSensorGetLSensorData(&LegoSensor, 1, &RSensor);
            LegoSensorGetLSensorData(&LegoSensor, 2, &LSensor);
            LSensor +=correction;

            // maak een correctie
            if(RSensor != LSensor)
            {
                correctionL = (RSensor - LSensor)/correctieTerm;
                correctionR = -correctionL;
                if(debug == true) {printf("correction left: %f\n", correctionL);}
            }
            // moet geen correctie gemaakt worden
            else
            {
                correctionL = 0;
                correctionR = 0;
            }

            currentAngleR += (rotateSpeed + correctionR);
            currentAngleL += (rotateSpeed + correctionL);
            currentAngle += rotateSpeed;
            LegoMotorPIDControl(&LegoMotor, 1, currentAngleR, kp, kd, ki, imax);
            LegoMotorPIDControl(&LegoMotor, 2, currentAngleL, kp, kd, ki, imax);
            TimeStep(0);
        }
    }

    // zet de motoren in break modus
    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);
}

// rij rechtdoor terwijl je een lijn volgt
void DriveLineFollow(float Speed)
{
    //printf("DriveLineFollow\n");
    LegoMotorSetup(&LegoMotor,1,0,1);
    LegoMotorSetup(&LegoMotor,2,0,1);

    // zet actuele hoekposities op nul
    LegoMotorSetPos(&LegoMotor, 1, 0);
    LegoMotorSetPos(&LegoMotor, 2, 0);

    // zet PId setpoint op nul
    LegoMotorPosPIDControl(&LegoMotor, 1, 0);
    LegoMotorPosPIDControl(&LegoMotor, 2, 0);

    //channel 1
    uint16 RSensor = 0;
    //channel 2
    uint16 LSensor = 0;
    LegoSensorSetupLSensor(&LegoSensor, 1, 1);
    LegoSensorSetupLSensor(&LegoSensor, 2, 1);

    //speed wordt doorgegeven in mm/s
    float rotateSpeed = ((fabs(Speed)/100)/mmPerDeg)*2;
    //hoeken van hoeken bijhouden
    float currentAngleR = 0;
    float currentAngleL = 0;
    //correctie per wiel
    float correctionL = 0;
    float correctionR = 0;
    float correctieTerm = 1100;
    int stop = 0;

    TimeStepInit(10000);
    // rij tot een wit lijn
    while(stop == 0)
    {
        // haal data op van sensoren
        LegoSensorGetLSensorData(&LegoSensor, 1, &RSensor);
        LegoSensorGetLSensorData(&LegoSensor, 2, &LSensor);
        LSensor +=correction;
        if(debug == true)
        {
            printf("left: %d\n", LSensor);
            printf("right: %d\n", RSensor);
        }

        // maak een correctie om terug recht te rijden
        if(RSensor != LSensor)
        {
            correctionL = (RSensor - LSensor)/correctieTerm;
            correctionR = -correctionL;
        }
        // maak geen correctie
        else
        {
            correctionL = 0;
            correctionR = 0;
        }
        currentAngleR += (rotateSpeed + correctionR);
        currentAngleL += (rotateSpeed + correctionL);
        LegoMotorPIDControl(&LegoMotor, 1, currentAngleR, kp, kd, ki, imax);
        LegoMotorPIDControl(&LegoMotor, 2, currentAngleL, kp, kd, ki, imax);

        // indien er een nieuw extrema is, threshold aanpassen
        if(RSensor> maximum || RSensor<minimum || LSensor>maximum || LSensor<minimum)
        {
            threshold = adaptiveThreshold(RSensor, LSensor);
            stopvalue = threshold-fabs((threshold/15));
            _delay_ms(10);
            ignore += 1;        // eerst 10 thresholds zullen genegeerd worden wegens initiele calibratie
            if(debug == true) {printf("stopvalue: %d\n", stopvalue);}
        }
        //if(LSensor<14000 && RSensor<14000)

        // stopvoorwaarde
        if(ignore > 10 && LSensor<stopvalue && RSensor<stopvalue)
        {
            if(counter >= 5)
            {
                stop = 1;
                printf("stop!\n");
                counter = 0;
            }
            else
            {
                counter +=1;
            }
        }
        else
        {
            counter = 0;
        }
        TimeStep(0);
    }

    // zet motoren in break modus
    LegoMotorSetup(&LegoMotor,1,1,1);
    LegoMotorSetup(&LegoMotor,2,1,1);
}
