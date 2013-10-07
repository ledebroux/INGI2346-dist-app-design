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

struct msgHeader{
	int length;
	int type;
};
typedef struct msgHeader msgHeader;