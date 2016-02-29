/*
 * dxlAPRS toolchain
 *
 * Copyright (C) Christian Rabler <oe5dxl@oevsv.at>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef osi_H_
#define osi_H_
#ifndef X2C_H_
#include "X2C.h"
#endif
#include <stdio.h>
#ifndef tcp_H_
#include "tcp.h"
#endif
#ifndef udp_H_
#include "udp.h"
#endif
#include <math.h>
#include <osic.h>

typedef long osi_File;

typedef long osi_SOCKET;

#define osi_pi 3.1415926535898

#define osi_DIRSEP "/"

#define osi_DIRSEP2 "/"

#define osi_InvalidFd (-1)

#define osi_floor floor

#define osi_Flush Flush

#define osi_ln RealMath_ln

#define osi_sin RealMath_sin

#define osi_cos RealMath_cos

#define osi_tan RealMath_tan

#define osi_arctan RealMath_arctan

#define osi_exp RealMath_exp

#define osi_power RealMath_power

#define osi_sqrt RealMath_sqrt

#define osi_Close osic_Close

#define osi_WrFixed osic_WrFixed

#define osi_Seek osic_Seek

#define osi_Seekcur osic_Seekcur

#define osi_WrLn osic_WrLn

#define osi_FdValid osic_FdValid

#define osi_CloseSock osic_CloseSock

#define osi_WrHex osic_WrHex

#define osi_WrStr InOut_WriteString

#define osi_WrCard InOut_WriteCard

#define osi_WrInt InOut_WriteInt

#define osi_readsock readsock

#define osi_sendsock sendsock

#define osi_connectto connectto

#define osi_getunack getunack

#define osi_stoptxrx stoptxrx

#define osi_openudp openudp

#define osi_bindudp bindudp

#define osi_socknonblock socknonblock

#define osi_udpreceive udpreceive

#define osi_udpsend udpsend

#define osi_remove remove

#define osi_Size Size

extern long osi_OpenAppendLong(char [], unsigned long);

extern long osi_OpenAppend(char [], unsigned long);

extern long osi_OpenWrite(char [], unsigned long);

extern long osi_OpenReadLong(char [], unsigned long);

extern long osi_OpenRead(char [], unsigned long);

extern long osi_OpenRW(char [], unsigned long);

extern long osi_OpenNONBLOCK(char [], unsigned long);

extern void osi_Rename(char [], unsigned long, char [], unsigned long);

extern void osi_WerrLn(char [], unsigned long);

extern long osi_RdBin(long, char [], unsigned long, unsigned long);

extern void osi_WrBin(long, char [], unsigned long, unsigned long);

extern void osi_Werr(char [], unsigned long);

extern void osi_NextArg(char [], unsigned long);

extern void osi_WrStrLn(char [], unsigned long);


extern void osi_BEGIN(void);


#endif /* osi_H_ */