/* XDS v2.60: Copyright (c) 1999-2011 Excelsior, LLC. All Rights Reserved. */
/* Generated by XDS Oberon-2 to ANSI C v4.20 translator */

#ifndef In_H_
#define In_H_
#ifndef X2C_H_
#include "X2C.h"
#endif

extern X2C_BOOLEAN In_Done;

extern void In_Open(void);

extern void In_Char(X2C_CHAR *);

extern void In_String(X2C_CHAR [], X2C_CARD32);

extern void In_Name(X2C_CHAR [], X2C_CARD32);

extern void In_Int(X2C_INT16 *);

extern void In_LongInt(X2C_INT32 *);

extern void In_Real(X2C_REAL *);

extern void In_LongReal(X2C_LONGREAL *);


extern void In_BEGIN(void);


#endif /* In_H_ */