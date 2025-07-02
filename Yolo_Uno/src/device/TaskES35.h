#ifndef INC_TASKES35_H_
#define INC_TASKES35_H_

#include "globals.h"

// Khai báo các hàm extern
extern void initES35();
extern void getValueES35();
extern void sendRS485Command(byte *command, int commandSize, byte *response, int responseSize);
extern void TaskES35Sensor(void *pvParameters);

#endif /* INC_TASKES35_H_ */