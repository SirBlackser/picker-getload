#include "maplabo.h"

// Function to initialize(fill) the node map
//writen by Thomas Molkens and Nick De clerq
void InitMap(NodeStruct *Map, int MapSize)
{
    // Common part for all nodes
    int Counter;

    for(Counter = 0; Counter < MapSize; Counter++)
    {
        Map[Counter].Visited = 0;
        Map[Counter].DV = 1000000;

        Map[Counter].Previous = -1;
        Map[Counter].Next = -1;
        Map[Counter].NextAbsDir = -1;
        Map[Counter].NextRelDir = -1;
    }

    // Node 0
    Map[0].Neighbours[0] = -1;
    Map[0].Neighbours[1] = 4;
    Map[0].Neighbours[2] = -1;
    Map[0].Neighbours[3] = -1;

    Map[0].Distance[0] = -1;
    Map[0].Distance[1] = 4;
    Map[0].Distance[2] = -1;
    Map[0].Distance[3] = -1;

    // Node 1
    Map[1].Neighbours[0] = -1;
    Map[1].Neighbours[1] = -1;
    Map[1].Neighbours[2] = -1;
    Map[1].Neighbours[3] = -1;

    Map[1].Distance[0] = -1;
    Map[1].Distance[1] = -1;
    Map[1].Distance[2] = -1;
    Map[1].Distance[3] = -1;

    // Node 2
    Map[2].Neighbours[0] = -1;
    Map[2].Neighbours[1] = 3;
    Map[2].Neighbours[2] = -1;
    Map[2].Neighbours[3] = -1;

    Map[2].Distance[0] = -1;
    Map[2].Distance[1] = 2;
    Map[2].Distance[2] = -1;
    Map[2].Distance[3] = -1;

    // Node 3
    Map[3].Neighbours[0] = -1;
    Map[3].Neighbours[1] = 4;
    Map[3].Neighbours[2] = 7;
    Map[3].Neighbours[3] = 2;

    Map[3].Distance[0] = -1;
    Map[3].Distance[1] = 1;
    Map[3].Distance[2] = 2;
    Map[3].Distance[3] = 2;

    // Node 4
    Map[4].Neighbours[0] = 0;
    Map[4].Neighbours[1] = 5;
    Map[4].Neighbours[2] = -1;
    Map[4].Neighbours[3] = 3;

    Map[4].Distance[0] = 4;
    Map[4].Distance[1] = 1;
    Map[4].Distance[2] = -1;
    Map[4].Distance[3] = 1;

    // Node 5
    Map[5].Neighbours[0] = 11;
    Map[5].Neighbours[1] = 9;
    Map[5].Neighbours[2] = 7;
    Map[5].Neighbours[3] = 4;

    Map[5].Distance[0] = 2;
    Map[5].Distance[1] = 1;
    Map[5].Distance[2] = 6;
    Map[5].Distance[3] = 1;

    // Node 6
    Map[6].Neighbours[0] = -1;
    Map[6].Neighbours[1] = 7;
    Map[6].Neighbours[2] = -1;
    Map[6].Neighbours[3] = -1;

    Map[6].Distance[0] = -1;
    Map[6].Distance[1] = 2;
    Map[6].Distance[2] = -1;
    Map[6].Distance[3] = -1;

    // Node 7
    Map[7].Neighbours[0] = 3;
    Map[7].Neighbours[1] = 5;
    Map[7].Neighbours[2] = -1;
    Map[7].Neighbours[3] = 6;

    Map[7].Distance[0] = 2;
    Map[7].Distance[1] = 6;
    Map[7].Distance[2] = -1;
    Map[7].Distance[3] = 2;

    // Node 8
    Map[8].Neighbours[0] = 9;
    Map[8].Neighbours[1] = -1;
    Map[8].Neighbours[2] = -1;
    Map[8].Neighbours[3] = -1;

    Map[8].Distance[0] = 2;
    Map[8].Distance[1] = -1;
    Map[8].Distance[2] = -1;
    Map[8].Distance[3] = -1;

    // Node 9
    Map[9].Neighbours[0] = 11;
    Map[9].Neighbours[1] = -1;
    Map[9].Neighbours[2] = 8;
    Map[9].Neighbours[3] = 5;

    Map[9].Distance[0] = 1;
    Map[9].Distance[1] = -1;
    Map[9].Distance[2] = 2;
    Map[9].Distance[3] = 1;

    // Node 10
    Map[10].Neighbours[0] = 11;
    Map[10].Neighbours[1] = -1;
    Map[10].Neighbours[2] = -1;
    Map[10].Neighbours[3] = -1;

    Map[10].Distance[0] = 4;
    Map[10].Distance[1] = -1;
    Map[10].Distance[2] = -1;
    Map[10].Distance[3] = -1;

    // Node 11
    Map[11].Neighbours[0] = -1;
    Map[11].Neighbours[1] = 10;
    Map[11].Neighbours[2] = 9;
    Map[11].Neighbours[3] = 5;

    Map[11].Distance[0] = -1;
    Map[11].Distance[1] = 4;
    Map[11].Distance[2] = 1;
    Map[11].Distance[3] = 2;
}
