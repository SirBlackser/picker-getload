#include "robotapp.h"
#include <stdbool.h>

//writen by Dries Blontrock

void RobotApp(int argc, char *argv[])
{
    // Variables
    int Speed = 85;
    int result = -1;
    //deactiveert deel van de code in verband met lift
    bool picker = false;
    //debug false zorgt er voor dat er packeten moeten ontvangen worden.
    //debug op true zal er voor zorgen dat beginPunt, eindPunt en snelheid handmatig wordt ingevoerd.
    //als u debug op true zet veranderd u best de StartNode en EndNode naar een int.
    bool debug = false;

    //verander naar int bij debug == true
    //verander naar char bij debug == false
    char StartNode = -1;
    char EndNode = -1;

    // Define node map (12 nodes)
    NodeStruct Map[12];
    int MapSize = sizeof(Map)/sizeof(Map[0]);

	//receive packets
	if(debug == false)
	{
        int Counter = 0;
        int ReturnCode;
        int PacketsReceived = 0;
        RfCommsPacket Packet;

        // Initialize packet fields
        Packet.Rssi = 0;
        Packet.Lqi = 0;
        Packet.DstRfAddr = 0;
        Packet.DataLen = 60;
        Packet.SrcRfAddr = 0;
        Packet.Cmd = 0;

        for(Counter = 0; Counter < 55; Counter++)
        {
            Packet.Data[Counter] = 0;
        }

        // Connection parameters
        uint32 Frequency = 433000000;
        uint8 ModType = CC1101_MOD_FSK;
        uint8 Rate = CC1101_RATE_10000;
        uint8 RFAddr = 9;
        uint8 PollStatus = 0;

        // Set connection parameters
        RfCC1101.Frequency = Frequency;
        RfCC1101.ModType = ModType;
        RfCC1101.Rate = Rate;
        RfCC1101.RFAddr = RFAddr;

        // Configure connection parameters
        ReturnCode = RfCommsInit(&RfCC1101, Frequency, ModType, Rate, RFAddr);

        // Check if re-initialization was successful
        switch(ReturnCode)
        {
            case 0 :
                printf("RfCC1101 module set.\n");
                printf("Picker robot waiting for start and end node.\n");
                break;
            case 1 :
                printf("RfCC1101 module error.\n");
                break;
            default :
                // Do nothing
                break;
        }

        while(PacketsReceived < 2)
        {
            // Only poll receive buffer when RF-CC1101 module was successfully re-initialized
            if(ReturnCode == 0)
            {
                // Flush receive buffer
                RfCommsClearRcvBuffer();

                // Loop until receive buffer contains data
                while(PollStatus == 0 && ReturnCode != 1)
                {
                    // Check if poll was successful
                    ReturnCode = RfCommsReceivePoll(&PollStatus);
                }

                // Check if sending was successful
                switch(ReturnCode)
                {
                    case 0 :
                        printf("Poll successful.\n");
                        switch(PollStatus)
                        {
                            case 0 :
                                printf("No packet ready.\n");
                                break;
                            case 1 :
                                printf("Packet ready.\n");
                                break;
                            default :
                                // Do nothing
                                break;
                        }
                        break;
                    case 1 :
                        printf("Poll failed.\n");
                        break;
                    default :
                        // Do nothing
                        break;
                }
            }

            // Only receive when RF-CC1101 module was successfully re-initialized, poll was successful and there's a packet ready
            if(ReturnCode == 0 && PollStatus == 1)
            {
                // Receive data
                ReturnCode = RfCommsReceivePacket(&Packet);

                // Check if receiving was successful
                switch(ReturnCode)
                {
                    case 0 :
                        printf("Packet successfully received.\n");
                        if(PacketsReceived == 0)
                        {
                            StartNode = Packet.Data[0];
                            printf("Startnode: %d\n", StartNode);
                        }
                        else if(PacketsReceived == 1)
                        {
                            EndNode = Packet.Data[0];
                            printf("Endnode: %d\n", EndNode);
                        }
                        PacketsReceived++;
                        break;
                    case 1 :
                        printf("Communication failed.\n");
                        break;
                    default :
                        // Do nothing
                        break;
                }
            }
        }
    }
    else
    //manualy enter start and end point and speed
    {
        printf( "Enter start: ");
        scanf("%d", &StartNode);

        printf( "Enter stop: ");
        scanf("%d", &EndNode);

        printf( "Enter speed (no more then 100): ");
        scanf("%d", &Speed);

        if(Speed > 100)
        {
            Speed = 100;
            printf("Speed was more then 100, set to 100\n");
        }
    }

    DriveInit();

    result = Travel(Map, MapSize, StartNode, EndNode, Speed);

    //DriveRotateCenter(180,Speed);
    //result = Travel(&Map, MapSize, Stop, 6, Speed);

    if(result == -1)
    {
        printf("unsuccesfull, start or end point not correct\n");
    }
    else
    {
        if(picker == false) {LiftInit();}

        DriveLineFollowDistance(100, Speed);

        if(picker == false) {LiftGoto(70);}

        DriveRotateCenter(180, Speed);

        result = Travel(Map, MapSize, EndNode, StartNode, Speed);

        if(picker == false) {LiftGoto(0);}

        DriveStraightDistance(-130, Speed);

        //DriveRotateRWheel(180, 50);

        //DriveRotateLWheel(360, 100);

        //DriveRotateCenter(180, 50);

        //DriveLineFollowDistance(2500,100);

        //DriveLineFollow(100);

        printf ("Ready.\n");
    }
}
