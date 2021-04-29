#ifndef __API_PMSGQUEUE__
#define __API_PMSGQUEUE__

int InitQueue(char *mqName, long size, int type);
int RcvQueue(int fd, char *ptr, long size);
int SendQueue(int fd, char *ptr, long size);


#endif
