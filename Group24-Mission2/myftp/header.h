/*
 * Thibaut Knop & Lenoard Debroux
 * Group 24
 * INGI2146 - Mission 1
 * header.h
 */

#include <stdint.h>

/*
Each message sent by the client will be preceded by the sending of
a header. That header contains the length and the type of the message to receive,
Types are defined as follows
*/

#define PWD 1
#define LPWD 2
#define CD 3
#define LCD 4
#define LS 5
#define LLS 6
#define GET 7
#define PUT 8
#define BYE 9
#define GET_SIZE 70
#define GET_LAST 71
#define ERRNO_RET 10
#define TELNETD_PORT 7000
#define PACKET_SIZE 1072

struct msgHeader{
	uint32_t length;
	uint32_t type;
};
typedef struct msgHeader msgHeader;