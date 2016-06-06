#include "travel.h"
#include <stdbool.h>

//writen by Dries Blontrock
//bereken van route aan de hand van dijkstra

int Travel(NodeStruct *Map,int MapSize,int Start,int Finish,float Speed)
{
    int failure = -1;
    int succes = 1;
    int hops = 0;
    bool debug = true;

    // Initialize map
    InitMap(Map, MapSize);

    // Calculate Dijkstra algorithm
    hops = Dijkstra(Map, MapSize, Start, Finish);

    // Print nodes status
    //DijkstraNodesPrint(Map, MapSize);

    //if hops = -1, then invalid start or end point
    if(hops == -1)
    {
        return failure;
    }
    else
    {
        int CurrentNode = Start;

        DriveLineFollow(Speed);
        _delay_ms(10);
        DriveStraightDistance(50, Speed);
        _delay_ms(10);

        // Print relative directions between nodes
        while(Map[CurrentNode].Next != -1)
        {
            if(debug == true) {printf("current node is: %d\n ", CurrentNode);}

            switch(Map[CurrentNode].NextRelDir)
            {
                //rechts af
                case 1 :
                    DriveStraightDistance(15,Speed);
                    DriveRotateLWheel(90, Speed);
                    DriveLineFollow(Speed);
                    if(debug == true) {printf("Turn left to %d.\n", Map[CurrentNode].Next);}
                    break;
                //rechtdoor
                case 2 :
                    DriveStraightDistance(110,Speed);
                    DriveLineFollow(Speed);
                    if(debug == true) {printf("Straight to %d.\n", Map[CurrentNode].Next);}
                    break;
                //links
                case 3 :
                    DriveStraightDistance(15,Speed);
                    DriveRotateRWheel(90, Speed);
                    DriveLineFollow(Speed);
                    if(debug == true) {printf("Turn right to %d.\n", Map[CurrentNode].Next);}
                    break;
                default :
                    // Do nothing
                    break;
            }

            // Select next node
            CurrentNode = Map[CurrentNode].Next;
        }
        return succes;
    }

}


