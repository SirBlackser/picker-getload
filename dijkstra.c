#include "dijkstra.h"
#include "maplabo.h"
#include "robotapp.h"

//writen by Thomas Molkens

int Dijkstra(NodeStruct *Map, int MapSize, int Start, int Finish)
{
    // Variables
    int Hops = -1;

    // Check for a valid starting point and end point
    if(Start >= 0 && Start < MapSize && Start != 1 && Finish >= 0 && Finish < MapSize && Finish != 1)
    {
        // Set starting point flags
        Map[Start].Visited = 1;
        Map[Start].DV = 0;

        // Execute Dijkstra sub-functions
        DijkstraPathCost(Map, Start);
        Hops = DijkstraPathHops(Map, Finish);
        DijkstraPathDirections(Map, Start);

        // When all nodes were visited
        printf("All %d nodes were visited!\n", MapSize);
        printf("Path cost between node %d and %d is %d over %d hops.\n", Start, Finish, Map[Finish].DV, Hops);

        // Print path
        //DijkstraPathPrint(Map, Start);
    }
    else
    {
        // Invalid starting point
        if(Start < 0 || Start >= MapSize)
        {
            printf("Invalid starting point: %d, choose a value from 0 to %d.\n", Start, MapSize - 1);
        }
        // Invalid end point
        if(Finish < 0 || Finish >= MapSize)
        {
            printf("Invalid end point: %d, choose a value from 0 to %d.\n", Finish, MapSize - 1);
        }
        // Invalid node 1
        if(Start == 1 || Finish == 1)
        {
            printf("Node 1 is an invalid node.\n");
        }
        Hops = -1;
    }
    return Hops;
}

void DijkstraPathCost(NodeStruct *Map, int Start)
{
    // Iterator
    int Counter = 0;

    // Check neighbours
    for(Counter = 0; Counter < 4; Counter++)
    {
        // Visit neighbour if it exists and is not visited yet
        if(Map[Start].Neighbours[Counter] != -1 && Map[Map[Start].Neighbours[Counter]].Visited == 0)
        {
            // Check if there's a shorter path
            int Distance = Map[Start].DV + Map[Start].Distance[Counter];

            // Visit another node, set fields and use the Dijkstra algorithm
            if(Distance < Map[Map[Start].Neighbours[Counter]].DV)
            {
                Map[Map[Start].Neighbours[Counter]].DV = Distance;
                Map[Map[Start].Neighbours[Counter]].Previous = Start;
                Map[Map[Start].Neighbours[Counter]].Visited = 1;

                DijkstraPathCost(Map, Map[Start].Neighbours[Counter]);
            }
        }
        // Look for a shorter path for already visited neighbours
        else if(Map[Start].Neighbours[Counter] != -1 && Map[Map[Start].Neighbours[Counter]].Visited == 1)
        {
            // Check if there's a shorter path
            int Distance = Map[Start].DV + Map[Start].Distance[Counter];

            // Set fields for another node and use the Dijkstra algorithm
            if(Distance < Map[Map[Start].Neighbours[Counter]].DV)
            {
                Map[Map[Start].Neighbours[Counter]].DV = Distance;
                Map[Map[Start].Neighbours[Counter]].Previous = Start;

                DijkstraPathCost(Map, Map[Start].Neighbours[Counter]);
            }
        }
    }
}

int DijkstraPathHops(NodeStruct *Map, int Finish)
{
    // Variables
    int Hops = 0;
    int CurrentNode = Finish;

    // Select previous node, increment hop count and set the next node field
    while(Map[CurrentNode].Previous != -1)
    {
        Hops++;
        Map[Map[CurrentNode].Previous].Next = CurrentNode;
        CurrentNode = Map[CurrentNode].Previous;
    }
    return Hops;
}

void DijkstraPathDirections(NodeStruct *Map, int Start)
{
    // Iterator
    int CurrentNode = Start;

    // Iterate over all nodes in the path
    while(Map[CurrentNode].Next != -1)
    {
        // Iterator
        int Counter = 0;

        // Check which neighbour is the next hop
        while(Map[CurrentNode].Next != Map[CurrentNode].Neighbours[Counter] && Counter < 4)
        {
            Counter++;
        }

        // Check if valid neighbour
        if(Counter < 4)
        {
            Map[CurrentNode].NextAbsDir = Counter;

            // Determine absolute directions
            switch(Counter)
            {
                // North
                case 0 :
                    // If starting node: relative direction forward
                    if(Map[CurrentNode].Previous == -1)
                    {
                        Map[CurrentNode].NextRelDir = 2;
                    }
                    else
                    {
                        // Iterator
                        int Iterator = 0;

                        while(Map[CurrentNode].Previous != Map[CurrentNode].Neighbours[Iterator] && Iterator < 4)
                        {
                            Iterator++;
                        }

                        // Determine relative direction
                        switch(Iterator)
                        {
                            // No direction
                            case 0 :
                                Map[CurrentNode].NextRelDir = -1;
                                break;
                            // Turn right
                            case 1 :
                                Map[CurrentNode].NextRelDir = 3;
                                break;
                            // Forward
                            case 2 :
                                Map[CurrentNode].NextRelDir = 2;
                                break;
                            // Turn left
                            case 3 :
                                Map[CurrentNode].NextRelDir = 1;
                                break;
                            default :
                                // Do nothing
                                break;
                        }
                    }
                    break;
                // East
                case 1 :
                    // If starting node: relative direction forward
                    if(Map[CurrentNode].Previous == -1)
                    {
                        Map[CurrentNode].NextRelDir = 2;
                    }
                    else
                    {
                        // Iterator
                        int Iterator = 0;

                        while(Map[CurrentNode].Previous != Map[CurrentNode].Neighbours[Iterator] && Iterator < 4)
                        {
                            Iterator++;
                        }

                        // Determine relative direction
                        switch(Iterator)
                        {
                            // Turn left
                            case 0 :
                                Map[CurrentNode].NextRelDir = 1;
                                break;
                            // No direction
                            case 1 :
                                Map[CurrentNode].NextRelDir = -1;
                                break;
                            // Turn right
                            case 2 :
                                Map[CurrentNode].NextRelDir = 3;
                                break;
                            // Forward
                            case 3 :
                                Map[CurrentNode].NextRelDir = 2;
                                break;
                            default :
                                // Do nothing
                                break;
                        }
                    }
                    break;
                // South
                case 2 :
                    // If starting node: relative direction forward
                    if(Map[CurrentNode].Previous == -1)
                    {
                        Map[CurrentNode].NextRelDir = 2;
                    }
                    else
                    {
                        // Iterator
                        int Iterator = 0;

                        while(Map[CurrentNode].Previous != Map[CurrentNode].Neighbours[Iterator] && Iterator < 4)
                        {
                            Iterator++;
                        }

                        // Determine relative direction
                        switch(Iterator)
                        {
                            // Forward
                            case 0 :
                                Map[CurrentNode].NextRelDir = 2;
                                break;
                            // Turn left
                            case 1 :
                                Map[CurrentNode].NextRelDir = 1;
                                break;
                            // No direction
                            case 2 :
                                Map[CurrentNode].NextRelDir = -1;
                                break;
                            // Turn right
                            case 3 :
                                Map[CurrentNode].NextRelDir = 3;
                                break;
                            default :
                                // Do nothing
                                break;
                        }
                    }
                    break;
                // West
                case 3 :
                    // If starting node: relative direction forward
                    if(Map[CurrentNode].Previous == -1)
                    {
                        Map[CurrentNode].NextRelDir = 2;
                    }
                    else
                    {
                        // Iterator
                        int Iterator = 0;

                        while(Map[CurrentNode].Previous != Map[CurrentNode].Neighbours[Iterator] && Iterator < 4)
                        {
                            Iterator++;
                        }

                        // Determine relative direction
                        switch(Iterator)
                        {
                            // Turn right
                            case 0 :
                                Map[CurrentNode].NextRelDir = 3;
                                break;
                            // Forward
                            case 1 :
                                Map[CurrentNode].NextRelDir = 2;
                                break;
                            // Turn left
                            case 2 :
                                Map[CurrentNode].NextRelDir = 1;
                                break;
                            // No direction
                            case 3 :
                                Map[CurrentNode].NextRelDir = -1;
                                break;
                            default :
                                // Do nothing
                                break;
                        }
                    }
                    break;
                default :
                    // Do nothing.
                    break;
            }
        }

        // Select next node
        CurrentNode = Map[CurrentNode].Next;
    }
}

void DijkstraPathPrint(NodeStruct *Map, int Start)
{
    // Iterator
    int CurrentNode = Start;

    // Print relative directions between nodes
    while(Map[CurrentNode].Next != -1)
    {
        //printf("%d: ", CurrentNode);

        switch(Map[CurrentNode].NextRelDir)
        {
            case 1 :
                printf("Turn left to %d.\n", Map[CurrentNode].Next);
                break;
            case 2 :
                printf("Straight to %d.\n", Map[CurrentNode].Next);
                break;
            case 3 :
                printf("Turn right to %d.\n", Map[CurrentNode].Next);
                break;
            default :
                // Do nothing
                break;
        }

        // Select next node
        CurrentNode = Map[CurrentNode].Next;
    }
}

void DijkstraNodesPrint(NodeStruct *Map, int MapSize)
{
    // Iterator
    int Counter;

    // Print every node
	for (Counter = 0; Counter < MapSize; Counter++)
	{
		printf ("Node %d: Distance Value: %d, Previous node: %d, Next node: %d\r\n", Counter, Map[Counter].DV, Map[Counter].Previous, Map[Counter].Next);
	}
}
