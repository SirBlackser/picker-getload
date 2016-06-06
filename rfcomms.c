#include "rfcomms.h"

int RfCommsInit(RfCC1101Struct *RfCC1101,uint32 Frequency,uint8 ModType,uint8 Rate,uint8 RFAddr)
{
    // Variables
    int Status = -1;

    // Re-initialize module
    Status = RfCC1101Init(RfCC1101);

    // Check if re-initialization was successful
    switch(Status)
    {
        case 0 :
            printf("RfCC1101 module successfully re-initialized.\n");
            //system ("espeak -ven+f2 -k5 -a50 -s150 \"Communication module ready.\" --stdout | aplay");
            switch(ModType)
            {
                // OOK
                case 0 :
                    switch(Rate)
                    {
                        // 1200
                        case 0 :
                            printf("Connection parameters: frequency %lu Hz, modulation type OOK, data rate 1200 baud, RF address %d.\n", Frequency, RFAddr);
                            Status = 0;
                            break;
                        // 10000
                        case 1 :
                            printf("Connection parameters: frequency %lu Hz, modulation type OOK, data rate 10000 baud, RF address %d.\n", Frequency, RFAddr);
                            Status = 0;
                            break;
                        // 250000
                        case 2 :
                            printf("Connection parameters: frequency %lu Hz, modulation type OOK, data rate 250000 baud, RF address %d.\n", Frequency, RFAddr);
                            Status = 0;
                            break;
                        default :
                            // Do nothing
                            break;
                    }
                // FSK
                case 1 :
                    switch(Rate)
                    {
                        // 1200
                        case 0 :
                            printf("Connection parameters: frequency %lu Hz, modulation type FSK, data rate 1200 baud, RF address %d.\n", Frequency, RFAddr);
                            Status = 0;
                            break;
                        // 10000
                        case 1 :
                            printf("Connection parameters: frequency %lu Hz, modulation type FSK, data rate 10000 baud, RF address %d.\n", Frequency, RFAddr);
                            Status = 0;
                            break;
                        // 250000
                        case 2 :
                            printf("Connection parameters: frequency %lu Hz, modulation type FSK, data rate 250000 baud, RF address %d.\n", Frequency, RFAddr);
                            Status = 0;
                            break;
                        default :
                            // Do nothing
                            break;
                    }
                default :
                    // Do nothing
                    break;
            }
            break;
        case 1 :
            printf("Invalid settings.\n");
            Status = 1;
            break;
        case 2 :
            printf("RS485 Client driver not initialized.\n");
            Status = 1;
            break;
        case 3 :
            printf("Cannot connect to module.\n");
            Status = 1;
            break;
        default :
            // Do nothing
            break;
    }

    return Status;
}


int RfCommsSendPacket(RfCommsPacket *Packet)
{
    // Define timers
    time_t StartTime = 0;
    time_t EndTime = 0;

    // Elapsed time in seconds
    time_t Sec = 0;

    int Status = -1;
    int ReturnCode = RfCC1101FIFOSendData(&RfCC1101, Packet -> Data, Packet -> DstRfAddr);

    switch(ReturnCode)
        {
            case 0 :
                printf("Status send: OK\n");
                Status = 0;

                // Start time-out timer
                StartTime = time(NULL);

                // Run timer until time-out is reached
                while(Sec < 3)
                {
                    // Check time-out timer
                    EndTime = time(NULL);

                    Sec = EndTime - StartTime;
                    //printf("SEC: %d\n", Sec);
                }
                printf("Timed out\n");
                break;
            case 1 :
                printf("RF-CC1101 not initialized.\n");
                Status = 1;
                break;
            case 2 :
                printf("Data is too long.\n");
                Status = 1;
                break;
            case 3 :
                printf("Other error.\n");
                Status = 1;
                break;
            default :
                // Do nothing
                break;
        }
        return Status;
}

int RfCommsSendResponse(RfCommsPacket *Packet)
{
    int Status = -1;

    int ReturnCode = RfCC1101FIFOSendData(&RfCC1101, Packet -> Data, Packet -> DstRfAddr);

    switch(ReturnCode)
        {
            case 0 :
                printf("Status send: OK\n");
                Status = 0;
                break;
            case 1 :
                printf("RF-CC1101 not initialized.\n");
                Status = 1;
                break;
            case 2 :
                printf("Data is too long.\n");
                Status = 1;
                break;
            case 3 :
                printf("Other error.\n");
                Status = 1;
                break;
            default :
                // Do nothing
                break;
        }
        return Status;
}

int RfCommsReceivePacket(RfCommsPacket *Packet)
{
    // Variables
    int Status = -1;
    int ReturnCode = -1;
    int DataLen = 0;
    int MaxDataLen = 60;
    int Counter = 0;
    unsigned char Decoded[MaxDataLen];
    const unsigned char Ack[3] = {0x01, 0x01, 0x00};
    const unsigned char Nack[3] = {0xFF, 0xFF, 0x00};
    RfCommsPacket AckPacket;
    RfCommsPacket NackPacket;

    // Receive data
    ReturnCode = RfCC1101FIFOReceiveData(&RfCC1101, Packet -> Data, Packet -> Rssi, Packet -> Lqi);

    // Check if receiving was successful
    switch(ReturnCode)
    {
        case 0 :
            Counter = 0;

            while(Packet -> Data[Counter] != 0)
            {
                Counter++;
            }

            // ACK or NACK frame, not encoded and given an arbitrary value: ACK = [0x01 0x01], NACK = [0xFF 0xFF]
            if(Counter < 2)
            {
                if(Packet -> Data[0] == 1 && Packet -> Data[1] == 1)
                {
                    printf("ACK frame received.\n");
                }
                else if(Packet -> Data[0] == 255 && Packet -> Data[1] == 255)
                {
                    printf("NACK frame received.\n");
                }
                else
                {
                    // Do nothing
                }
            }
            // Minimum length encoded data frame is 3
            else
            {
                Counter = 0;
                printf("Data packet: \n");
                while(Packet -> Data[Counter] != 0)
                {
                    printf("%d\n", Packet -> Data[Counter]);
                    Counter++;
                }
                printf("0\n");
                printf("was successfully received.\n");
                DataLen = Counter + 1;

                // Decode data
                DecodeCOBS(Packet -> Data, DataLen, Decoded);

                printf("Decoded data: \n");

                for(Counter = 0; Counter < DataLen - 2; Counter++)
                {
                    printf("%d\n", Decoded[Counter]);
                }

                // Set packet's data field to decoded data (which contains a start or end node)
                Packet -> Data[0] = Decoded[0];
            }

            // Initialize packet fields
            AckPacket.Rssi = 0;
            AckPacket.Lqi = 0;
            AckPacket.DataLen = 60;
            AckPacket.Cmd = 0;
            for(Counter = 0; Counter < 3; Counter++)
            {
                AckPacket.Data[Counter] = Ack[Counter];
            }
            AckPacket.DstRfAddr = 8;
            AckPacket.SrcRfAddr = 9;
            AckPacket.DataLen = sizeof(Ack);

            RfCommsSendResponse(&AckPacket);

            Status = 0;
            break;
        case 1 :
            printf("RF-CC1101 not initialized.\n");
            Status = 1;
            break;
        case 2 :
            // Initialize packet fields
            NackPacket.Rssi = 0;
            NackPacket.Lqi = 0;
            NackPacket.DataLen = 60;
            NackPacket.Cmd = 0;
            for(Counter = 0; Counter < 3; Counter++)
            {
                NackPacket.Data[Counter] = Nack[Counter];
            }
            NackPacket.DstRfAddr = 8;
            NackPacket.SrcRfAddr = 9;
            NackPacket.DataLen = sizeof(Nack);

            RfCommsSendResponse(&NackPacket);
            printf("Invalid frame received.\n");
            Status = 1;
            break;
        case 3 :
            printf("Other error.\n");
            Status = 1;
            break;
        default :
            // Do nothing
            break;
    }

    return Status;
}

int RfCommsReceivePoll(uint8 *PollStatus)
{
    // Variables
    int Status = -1;
    int ReturnCode = -1;

    // Checks whether data is present in the receive buffer
    ReturnCode = RfCC1101FIFOReceivePoll(&RfCC1101, PollStatus);

    // Check if sending was successful
    switch(ReturnCode)
    {
        case 0 :
            //printf("Poll OK.\n");
            Status = 0;
            break;
        case 1 :
            //printf("RF-CC1101 not initialized.\n");
            Status = 1;
            break;
        case 2 :
            //printf("Other error.\n");
            Status = 1;
            break;
        default :
            // Do nothing
            break;
    }

    return Status;
}

void RfCommsClearRcvBuffer()
{
    // Variables
    int PollCheck = 0;
    int PollStatus = 0;
    int PacketStatus = 0;
    int Counter;
    RfCommsPacket DropPacket;

    // Try polling receive buffer
    PollStatus = RfCommsReceivePoll(&PollCheck);

    // Initialize packet fields
    DropPacket.Rssi = 0;
    DropPacket.Lqi = 0;
    DropPacket.DataLen = 60;
    DropPacket.Cmd = 0;
    for(Counter = 0; Counter < 10; Counter++)
    {
        DropPacket.Data[Counter] = 0;
    }
    DropPacket.DstRfAddr = 8;
    DropPacket.SrcRfAddr = 9;

    // If successful
    if(PollStatus == 0)
    {
        // While polling is successful and there are packets ready
        while(PollStatus == 0 && PollCheck == 1)
        {
            // Receive packet
            PacketStatus = RfCommsReceivePacket(&DropPacket);

            // Successfully retrieved packet to drop from receive buffer
            if(PacketStatus == 0)
            {
               printf("Packet dropped while clearing receive buffer.\n");
            }
            // Error occurred with packet from receive buffer
            else
            {
                // Do nothing
            }
            // Get poll status and check if there are more packets waiting
            PollStatus = RfCommsReceivePoll(&PollCheck);
        }
        // Polling failed
        if(PollStatus == 1)
        {
            printf("Polling to clear receive buffer failed due to an error.\n");
        }
    }
    // Polling failed
    else
    {
        printf("Polling to clear receive buffer failed due to an error.\n");
    }
}

void EncodeCOBS(const unsigned char *Data, unsigned long Length, unsigned char *Dest)
{
    // End address of data array
    const unsigned char *End = Data + Length;
    // Address iterator
    unsigned char *Code_ptr = Dest++;
    // Default value overhead byte
    unsigned char Code = 0x01;
    // Destination length (data length + overhead and end byte (0x00)
    int DestLength = Length + 2;

    // Loop over original data
    while(Data < End)
    {
        // Check for 0-byte
        if(*Data == 0)
        {
            FinishBlock(Code);
        }
        else
        {
            // Unchanged Data value is set to Dest when non-zero byte occurred
            *Dest++ = *Data;
            // Count non-zero bytes
            Code++;
            // Counter overflow
            if (Code == 0xFF)
            {
                FinishBlock(Code);
            }
        }
        // Next Data address
        Data++;
    }
    FinishBlock(Code);
}

void DecodeCOBS(const unsigned char *Data, unsigned long Length, unsigned char *Dest)
{
    // End address of data array
    const unsigned char *End = Data + Length;
    // Loop over data
    while(Data < End)
    {
        int Counter;
        // Assign value from current Data location to Code and increment Data location
        int Code = *Data++;
        // Keep every original non-zero element (assign & increment)
        for(Counter = 1; Counter < Code; Counter++)
        {
            *Dest++ = *Data++;
        }
        // Set element after non-zero row to zero and increment Data location
        if(Code < 0xFF)
        {
            *Dest++ = 0;
        }
    }
}
