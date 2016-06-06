#ifndef RFCOMMS_H
#define RFCOMMS_H

#include "robotapp.h"
#include "rf-cc1101.h"

#define RFCOMMS_MAX_DATA_LEN 32
#define FinishBlock(X) (*Code_ptr = (X), Code_ptr = Dest++, Code = 0x01)    //Step 1: Set pointer value to X (=Code), Step 2: Shift pointer to next destination address, Step 3: Set code to default value

#pragma pack(push)
#pragma pack(1)

//Packet structure
typedef struct
{
    sint16 Rssi;    ///<Received signal strength
    uint8 Lqi;      ///<Link Quality Indicator
    uint8 DstRfAddr;///<Destination RF address

    //Transmitted OTA
    uint8 DataLen;  ///<Number of data bytes in packet (Data array)
    uint8 SrcRfAddr;///<Source RF address
    uint8 Cmd;      ///<Command byte
    uint8 Data[RFCOMMS_MAX_DATA_LEN];///<Data array. 'DataLen' bytes are used
} RfCommsPacket;
#pragma pack(pop)

/**
 * \brief Initialize RF communications system
 * \param RfCC1101 :pointer to RfCC1101 structure containing the module settings
 * \param Frequency :RF base working frequency (Hz)
 * \param ModType :modulation type (CC1101_MOD_OOK or CC1101_MOD_FSK)
 * \param Rate :RF data rate (CC1101_RATE_1200, CC1101_RATE_10000 or CC1101_RATE_250000)
 * \param RFAddr :RF packet address
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int RfCommsInit(RfCC1101Struct *RfCC1101,uint32 Frequency,uint8 ModType,uint8 Rate,uint8 RFAddr);

/**
 * \brief Send data packet in RfCommsPacket struct form
 * \param Packet :Pointer to packet to be transmitted
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int RfCommsSendPacket(RfCommsPacket *Packet);

/**
 * \brief Send response packet(ACK or NACK) in RfCommsPacket struct form
 * \param Packet :Pointer to packet to be transmitted
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int RfCommsSendResponse(RfCommsPacket *Packet);

/**
 * \brief Receive data packet in RfCommsPacket struct form. Blocks until packet is received.
 * \param Packet :Pointer to packet receive buffer
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int RfCommsReceivePacket(RfCommsPacket *Packet);


/**
 * \brief Polls whether packet is ready in receive buffer
 * \param PollStatus :Pointer to poll status variable. 0: No packet ready 1: Packet ready
 * \return
 * 0:OK \n
 * 1:Error \n
*/
int RfCommsReceivePoll(uint8 *PollStatus);

/**
 * \brief Clear receive buffer before communication
*/
void RfCommsClearRcvBuffer();

/**
 * \brief Encodes “Length” bytes of data at the location pointed to by “Data”, writing the output to the location pointed to by “Dest”.
          encoded data starts with an overhead byte, non-NULL bytes are left unchanged, NULL bytes are replaced by the amount of consecutive
          non-NULL bytes +1 and encoded data is finished with a NULL byte
 * \param Data :Data to encode
 * \param Length :The length of the data
 * \param Dest :Buffer to store the encoded data
*/
void EncodeCOBS(const unsigned char *Data, unsigned long Length, unsigned char *Dest);

/**
 * \brief Decodes “Length” bytes of data at the location pointed to by “Data”, writing the output to the location pointed to by “Dest”.
 * \param Data :Data to decode
 * \param Length :The length of the data
 * \param Dest :Buffer to store the decoded data
*/
void DecodeCOBS(const unsigned char *Data, unsigned long Length, unsigned char *Dest);
#endif
