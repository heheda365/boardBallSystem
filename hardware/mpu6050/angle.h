#ifndef ANGLE_H__
#define ANGLE_H__

int initMPU(void);
int readAngleData(float *outPitch, float *outRoll, float *outYaw, short accelData[3]);

#endif

