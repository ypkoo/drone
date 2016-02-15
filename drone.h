#ifndef DRONE_H
#define DRONE_H

#define NODE_NUM 6
#define MSG_LEN 20

struct drone_msg
{
	char msg[MSG_LEN];
	uint8_t received_ids[NODE_NUM];
};

#endif
