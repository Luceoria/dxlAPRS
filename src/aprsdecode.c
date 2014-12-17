/*
 * dxlAPRS toolchain
 *
 * Copyright (C) Christian Rabler <oe5dxl@oevsv.at>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#define X2C_int32
#define X2C_index32
#ifndef aprsdecode_H_
#include "aprsdecode.h"
#endif
#define aprsdecode_C_
#ifndef aprspos_H_
#include "aprspos.h"
#endif
#ifndef xosi_H_
#include "xosi.h"
#endif
#ifndef osi_H_
#include "osi.h"
#endif
#ifndef Lib_H_
#include "Lib.h"
#endif
#ifndef InOut_H_
#include "InOut.h"
#endif
#ifndef FileSys_H_
#include "FileSys.h"
#endif
#ifndef tcp_H_
#include "tcp.h"
#endif
#ifndef udp_H_
#include "udp.h"
#endif
#ifndef aprsstr_H_
#include "aprsstr.h"
#endif
#ifndef Storage_H_
#include "Storage.h"
#endif
#ifndef TimeConv_H_
#include "TimeConv.h"
#endif
#ifndef useri_H_
#include "useri.h"
#endif
#ifndef aprstext_H_
#include "aprstext.h"
#endif



































unsigned long aprsdecode_systime;
unsigned long aprsdecode_realtime;
unsigned long aprsdecode_lastlooped;
unsigned long aprsdecode_rxidle;
char aprsdecode_verb;


struct aprsdecode__D0 aprsdecode_click;


struct aprspos_POSITION aprsdecode_mappos;
long aprsdecode_inittilex;
long aprsdecode_inittiley;
long aprsdecode_initxsize;
long aprsdecode_initysize;
long aprsdecode_parmzoom;
long aprsdecode_initzoom;
float aprsdecode_finezoom;
float aprsdecode_parmfinezoom;
char aprsdecode_mapdir[1025];
aprsdecode_pMOUNTAIN aprsdecode_mountains;


struct aprsdecode__D2 aprsdecode_lums;


struct aprsdecode__D3 aprsdecode_tracenew;
aprsdecode_pTXMESSAGE aprsdecode_txmessages;
aprsdecode_pMSGFIFO aprsdecode_msgfifo0;
float aprsdecode_spikesens;
float aprsdecode_maxhop;
aprsdecode_pOPHIST aprsdecode_ophist2;
aprsdecode_pOPHIST aprsdecode_ophist0;


struct aprsdecode__D4 aprsdecode_gateways[10];
aprsdecode_pTCPSOCK aprsdecode_tcpsocks;
struct aprsdecode_UDPSOCK aprsdecode_udpsocks0[4];
unsigned long aprsdecode_lasttcptx;
unsigned long aprsdecode_lastanyudprx;
unsigned long aprsdecode_lastpurge;
unsigned long aprsdecode_lasttcprx;
aprsdecode_FRAMEBUF aprsdecode_testbeaconbuf;
struct xosi_PROCESSHANDLE aprsdecode_serialpid;
struct xosi_PROCESSHANDLE aprsdecode_serialpid2;
struct xosi_PROCESSHANDLE aprsdecode_maploadpid;
/* connect to aprs-is gateway and decode data for archive and map by OE5DXL */
#define aprsdecode_CALLLEN 7

#define aprsdecode_HASHSIZE 65536

#define aprsdecode_GATECONNDELAY 30

#define aprsdecode_WATCH 180

#define aprsdecode_QMAXTIME 15

#define aprsdecode_MIDMOD 62

#define aprsdecode_HOLDT 0x0FFFFFFFF

#define aprsdecode_BEEPINVERVALL 5
/* min time between beeps */

#define aprsdecode_TICKEROFFTIME 60
/* switch off window headline ticker after no data*/

#define aprsdecode_cUSERMESSAGE ":"

#define aprsdecode_cTHIRDPARTY "}"

#define aprsdecode_DUPHASHSIZE 65536

typedef unsigned long SET256[8];

typedef unsigned long CHSET[4];


struct BEACON {
   unsigned long bintervall;
   aprsdecode_FILENAME bfile;
   unsigned long btime;
};

typedef char TICKERCALL[31];

static char sentemptymsg;

static char callsrc;

static unsigned long uptime;

static aprsdecode_FILENAME logframename;
/*    netbeaconintervall,  */

static unsigned long beaconrandomstart;

static unsigned long nextbeep;

static unsigned long rfbecondone;

static unsigned long lastmsgtx;

static unsigned long qwatchtime;

static unsigned long connecttime;

static unsigned long cycleservers;

static unsigned long qas;

static unsigned long qasc;

static long logframes;
/*    passwd                : ARRAY[0..5] OF CHAR; */

static unsigned long trygate;
/*    actudp                : pUDPSOCK; */

static aprsdecode_MONCALL ungate[7];

static unsigned long timehash[65536];

static unsigned long rfbeacont;

static unsigned long dupetime;

static unsigned long msgmid;

static TICKERCALL frameticker[20];

/* calls in ticker headline */
static unsigned long tickertime;

static float maxspeed;

static unsigned short crctable12[256];


static void Err(const char text[], unsigned long text_len)
{
   InOut_WriteString("aprsmap: ", 10ul);
   InOut_WriteString(text, text_len);
   osi_WrStrLn(" error abort", 13ul);
   X2C_ABORT();
} /* end Err() */


extern unsigned long aprsdecode_trunc(float r)
{
   struct X2C_XHandler_STR anonym;
   unsigned long aprsdecode_trunc_ret;
   if (X2C_XTRY(&anonym)) {
      if (r<=0.0f) aprsdecode_trunc_ret = 0UL;
      else if (r>=2.147483647E+9f) aprsdecode_trunc_ret = 2147483647UL;
      else {
         aprsdecode_trunc_ret = (unsigned long)X2C_TRUNCC(r,0UL,
                X2C_max_longcard);
      }
      X2C_XOFF();
   }
   else {
      osi_WrStrLn("error in TRUNC() ", 18ul);
      aprsdecode_trunc_ret = 0UL;
   }
   X2C_XREMOVE();
   return aprsdecode_trunc_ret;
} /* end trunc() */

/*
PROCEDURE trunc(r:REAL):CARDINAL;
VAR n:CARDINAL;
BEGIN
WrStrLn(" t1 ");
  IF r<=0.0 THEN n:=0 ELSIF r>=MAX(INTEGER) THEN n:=MAX(INTEGER)
                ELSE n:=TRUNC(r) END;
WrStrLn(" t2 ");
  RETURN n
END trunc;
*/

extern float aprsdecode_floor(float r)
{
   float f;
   f = (float)(long)X2C_TRUNCI(r,X2C_min_longint,X2C_max_longint);
   if (f>r) f = f-1.0f;
   return f;
} /* end floor() */


extern void aprsdecode_posinval(struct aprspos_POSITION * pos)
{
   pos->long0 = 0.0f;
   pos->lat = 0.0f;
} /* end posinval() */


static char Watchclock(unsigned long * t, unsigned long intervall)
{
   unsigned long tn;
   /*WrInt(t, 10); WrInt(intervall, 10); WrLn; */
   if (intervall>0UL || *t==0UL) {
      /* send once */
      tn = aprsdecode_realtime;
      if (*t<=tn) {
         *t += intervall;
         if (*t<=tn) *t = tn+intervall;
         return 1;
      }
      if (*t>tn+intervall) *t = tn+intervall;
   }
   return 0;
} /* end Watchclock() */


extern void aprsdecode_beeplim(long lev, unsigned long hz, unsigned long ms)
/* beep with minimum quiet time */
{
   if (aprsdecode_realtime>=nextbeep) {
      xosi_beep(lev, hz, ms);
      nextbeep = aprsdecode_realtime+5UL;
   }
} /* end beeplim() */


static void filepath(char s[], unsigned long s_len)
/* get path out of path + filename */
{
   unsigned long j;
   unsigned long i;
   i = 0UL;
   j = 0UL;
   while (i<s_len-1 && s[i]) {
      if (s[i]=='/' || s[i]=='/') j = i;
      ++i;
   }
   s[j] = 0;
} /* end filepath() */


static void alfanum(char s[], unsigned long s_len)
/* clean received call to filename */
{
   unsigned long j;
   unsigned long i;
   char c;
   i = 0UL;
   j = 0UL;
   while (i<s_len-1 && s[i]) {
      c = X2C_CAP(s[i]);
      if (((unsigned char)c>='A' && (unsigned char)c<='Z' || (unsigned char)
                c>='0' && (unsigned char)c<='9') || c=='-') {
         s[j] = c;
         ++j;
      }
      ++i;
   }
   if (j<=s_len-1) s[j] = 0;
} /* end alfanum() */


static long GetNum(char h[], unsigned long h_len, unsigned long * p,
                unsigned long * n)
{
   char ok0;
   long GetNum_ret;
   X2C_PCOPY((void **)&h,h_len);
   h[h_len-1] = 0;
   *n = 0UL;
   ok0 = 0;
   while ((unsigned char)h[*p]>='0' && (unsigned char)h[*p]<='9') {
      ok0 = 1;
      *n = ( *n*10UL+(unsigned long)(unsigned char)h[*p])-48UL;
      ++*p;
   }
   if (!ok0) {
      GetNum_ret = -1L;
      goto label;
   }
   GetNum_ret = 0L;
   label:;
   X2C_PFREE(h);
   return GetNum_ret;
} /* end GetNum() */


static void parms(void)
{
   char h[4096];
   char err;
   char lasth;
   unsigned long n;
   unsigned long i;
   unsigned long gatecnt;
   long ii;
   err = 0;
   aprsdecode_verb = 0;
   gatecnt = 0UL;
   for (;;) {
      Lib_NextArg(h, 4096ul);
      if (h[0U]==0) break;
      if ((h[0U]=='-' && h[1U]) && h[2U]==0) {
         lasth = h[1U];
         /*
               IF (lasth="s") OR (lasth="S") THEN
                 NextArg(h);
                 Assign(servercall, h);
                 IF (h[0]=0C) OR (h[0]="-") OR ((lasth="s") & NOT callok(h))
                THEN Err("-s call-ssid") END;
         */
         /*
               IF lasth="p" THEN
                 NextArg(h);
                 IF (h[0]>="0") & (h[0]<="9") THEN
                   i:=0;  
                   WHILE (i<=HIGH(passwd)) & (h[i]>=" ") DO passwd[i]:=h[i];
                INC(i) END;
                 ELSE
                   fdf:=OpenRead(h);
                   IF NOT FdValid(fdf)
                THEN Err("-p passcode or passwordfile") END;
                   len:=RdBin(fdf, passwd, SIZE(passwd)-1);
                   IF len>=0 THEN passwd[len]
                :=0C ELSE Err("-p error with password file") END;
                   Close(fdf);
                 END;
                 passwd[HIGH(passwd)]:=0C;
                 i:=0; 
                 WHILE (passwd[i]>="0") & (passwd[i]<="9") DO INC(i) END;
                 IF (i=0) OR (passwd[i]<>0C) THEN Err("-p invalid passcode")
                END;
         */
         /*
               ELSIF lasth="n" THEN
                 NextArg(h);
                 i:=0;
                 IF (GetSec(h,i,n)>=0) & (h[i]=":") THEN
                   netbeaconintervall:=n*60;
                   INC(i);
                   FOR n:=0 TO HIGH(netbeaconfn) DO
                     IF i<=HIGH(h) THEN
                       netbeaconfn[n]:=h[i];
                       INC(i);
                     END;
                   END;
                   IF netbeaconfn[0]<=" " THEN Err("-n netbeacon filename")
                END;
                 ELSE Err("-n netbeacon format is minutes:file") END;
         */
         /*
               IF lasth="f" THEN
                 NextArg(h);
                 Assign(serverrangefilter, h);
                 IF (h[0]=0C) OR (h[0]="-") THEN Err("-f rangefilter") END;
                 FOR i:=0 TO Length(serverrangefilter) DO
                   IF serverrangefilter[i]=",
                " THEN serverrangefilter[i]:=" " END;
                 END;  
         
               IF lasth="G" THEN
                 NextArg(h);
                 WITH lums DO
                  IF NOT StrToFix(gamma,
                h) OR (gamma<0.1) OR (gamma>10.0)
                THEN Err("-G gamma (0.1..10)") END;
                 END;
         */
         /*
               IF (lasth="R") OR (lasth="M") THEN
                 NextArg(h);
                 ALLOCATE(usock, SIZE(usock^)); debugmem.req:=SIZE(usock^);
                INC(debugmem.mon, debugmem.req);
                 IF usock=NIL THEN Err("out of memory") END;
         
                 FILL(usock, 0C, SIZE(usock^));
                 WITH usock^ DO
                   IF GetIp2(h, ip, dport, bindport,
                checkip)<0 THEN Err("-R or -M need ip:port:port") END;
                   fds:=openudp();
                   INC(udpch);
                   chan:=udpch;
                   rawread:=lasth="R";
                   IF (CAST(INTEGER,fds)<0) OR (bindudp(fds,
                bindport)<0) THEN Err("-R or -M cannot bind udpport") END;
         
                   len:=socknonblock(fds);
         */
         /*
                   fd:=fds; 
                   ii:=InStr(h, "+");
                   IF ii>0 THEN 
                     i:=ii+1;
                     IF (GetSec(h,i,n)>=0) THEN 
                       maxbytes:=n;
                       INC(i);
                       IF (h[i-1]=":") & (GetSec(h,i,
                n)>=0) THEN torfradius:=FLOAT(n) END;
                     END;
                   END;
         */
         /*
                   next:=NIL;
                 END;
                 IF udpsocks=NIL THEN udpsocks:=usock;
                 ELSE 
                   ush:=udpsocks; 
                   WHILE ush^.next<>NIL DO ush:=ush^.next END; 
                   ush^.next:=usock;
                 END; 
         */
         if (lasth=='x') {
            Lib_NextArg(h, 4096ul);
            i = 0UL;
            if (GetNum(h, 4096ul, &i, &n)>=0L && n>=30UL) {
               aprsdecode_initxsize = (long)n;
            }
            else Err("-x xsize", 9ul);
         }
         else if (lasth=='y') {
            Lib_NextArg(h, 4096ul);
            i = 0UL;
            if (GetNum(h, 4096ul, &i, &n)>=0L && n>=20UL) {
               aprsdecode_initysize = (long)n;
            }
            else Err("-y xsize", 9ul);
         }
         else if (lasth=='z') {
            Lib_NextArg(h, 4096ul);
            if (!aprsstr_StrToFix(&aprsdecode_parmfinezoom, h,
                4096ul) || aprsdecode_parmfinezoom<=1.0f) {
               Err("-z zoomlevel", 13ul);
            }
            n = aprsdecode_trunc(aprsdecode_parmfinezoom);
            aprsdecode_parmfinezoom = (1.0f+aprsdecode_parmfinezoom)-(float)
                n;
            aprsdecode_parmzoom = (long)n;
         }
         else if (lasth=='m') {
            Lib_NextArg(h, 4096ul);
            if (!aprsstr_StrToFix(&aprsdecode_mappos.long0, h, 4096ul)) {
               Err("-m long lat", 12ul);
            }
            Lib_NextArg(h, 4096ul);
            if (!aprsstr_StrToFix(&aprsdecode_mappos.lat, h, 4096ul)) {
               Err("-m long lat", 12ul);
            }
            aprsdecode_mappos.long0 = aprspos_rad0(aprsdecode_mappos.long0);
            aprsdecode_mappos.lat = aprspos_rad0(aprsdecode_mappos.lat);
         }
         else if (lasth=='g') {
            Lib_NextArg(h, 4096ul);
            if (gatecnt>9UL) Err("-g gateway table full", 22ul);
            /*
                    Assign(gateways[gatecnt].url, h);
                    IF h[0]=0C THEN Err("-g url port") END;
                    NextArg(h);
                    Assign(gateways[gatecnt].port, h);
                    IF h[0]=0C THEN Err("-g url port") END;
            */
            h[4095U] = 0;
            if (h[0U]=='[') {
               ii = 1L;
               while (h[ii] && h[ii]!=']') ++ii;
               if (h[ii]!=']' || h[ii+1L]!=':') Err("-g [url]:port", 14ul);
               h[ii] = 0;
               i = 1UL;
               while (i<=4095UL) {
                  h[i-1UL] = h[i];
                  ++i;
               }
            }
            else ii = aprsstr_InStr(h, 4096ul, ":", 2ul);
            if (ii>=0L) h[ii] = 0;
            aprsstr_Assign(aprsdecode_gateways[gatecnt].url, 256ul, h,
                4096ul);
            if (ii>0L) {
               /* port number */
               ++ii;
               i = 0UL;
               while (ii<=4095L) {
                  h[i] = h[ii];
                  ++i;
                  ++ii;
               }
            }
            else Lib_NextArg(h, 4096ul);
            h[4095U] = 0;
            ii = aprsstr_InStr(h, 4096ul, "#", 2ul);
            if (ii>=0L) h[ii] = 0;
            if (h[0U]==0) Err("-g url:port", 12ul);
            aprsstr_Assign(aprsdecode_gateways[gatecnt].port, 6ul, h,
                4096ul);
            if (ii>0L) {
               /* we have a filter string */
               ++ii;
               i = 0UL;
               while (ii<=4095L) {
                  if (h[ii]==',') h[ii] = ' ';
                  h[i] = h[ii];
                  ++i;
                  ++ii;
               }
               aprsstr_Assign(aprsdecode_gateways[gatecnt].filterst, 256ul,
                h, 4096ul);
            }
            ++gatecnt;
         }
         else {
            if (lasth=='h') {
               osi_WrStrLn(" -g <url>:<port>#<filters> connect to APRS-IS gat\
eway, repeat -g to other", 74ul);
               osi_WrStrLn("                gateways write favorites first fo\
r 1 activ link to the best", 76ul);
               osi_WrStrLn("                reachable and a polling link to r\
evert immediately to preferred again", 86ul);
               osi_WrStrLn("                -g www.db0anf.de:14580#m/300 -g 1\
27.0.0.1:3000 -g [::1]:3000", 77ul);
               osi_WrStrLn("                <filters> text sent to out connec\
ted server -f m/50", 68ul);
               osi_WrStrLn("                if blanks dont pass parameter set\
tings use , (-f m/30,-d/CW)", 77ul);
               osi_WrStrLn("                ipv6 if enabled by kernel",
                42ul);
               osi_WrStrLn(" -h             this", 21ul);
               osi_WrStrLn(" -M <ip>:<dport>/<lport>[+<byte/s>[:<radius>]]",
                47ul);
               osi_WrStrLn("                udp rf port (monitor frame format\
) for local (t)rx", 67ul);
               osi_WrStrLn("                <dport>:<lport> read from any ip,\
 dport=0 no tx", 64ul);
               osi_WrStrLn("                +byte/s enable inet to rf for ser\
vices like WLNK, WHO-IS", 73ul);
               osi_WrStrLn("                :radius enable all inet to rf gat\
e (from km around digi)", 73ul);
               osi_WrStrLn("                repeat -M for each radio port wit\
h a tx", 56ul);
               /*        WrStrLn("
                -n <min>:<file> netbeacon minutes:filename -n 10:netbeacon.txt"
                ); */
               /*        WrStrLn("
                beacon file like: !8959.00N/17959.00E&igate mars"); */
               /*        WrStrLn("
                beacon file used by udpgate itself to find out own position")
                ; */
               osi_WrStrLn(" -o <outfile>   out file, delete after read",
                44ul);
               /*        WrStrLn("
                -p <password>  login passwort for aprs-is servers -p 12345");
                 */
               /*        WrStrLn("
                to hide password in commandline use file mode -p pass.txt");
                */
               osi_WrStrLn(" -R             same as -M but axudp format",
                44ul);
               /*        WrStrLn("
                -s <call>      server call of this server -s MYCALL-10 (-S no callcheck)
                "); */
               osi_WrStrLn(" -v             show frames and analytics on stdo\
ut", 52ul);
               osi_WrLn();
               X2C_ABORT();
            }
            if (lasth=='C') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=100UL) {
                  aprsdecode_lums.centering = (long)n;
               }
               else Err("-C center%", 11ul);
            }
            else if (lasth=='X') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.sym = (long)n;
               }
               else Err("-X symbollumen", 15ul);
            }
            else if (lasth=='O') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.obj = (long)n;
               }
               else Err("-O objectlumen", 15ul);
            }
            else if (lasth=='W') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.waypoint = (long)n;
               }
               else Err("-W waypointlumen", 17ul);
            }
            else if (lasth=='A') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.text = (long)n;
               }
               else Err("-A textlumen", 13ul);
            }
            else if (lasth=='T') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.track = (long)n;
               }
               else Err("-T tracklumen", 14ul);
            }
            else if (lasth=='F') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n>0UL) {
                  aprsdecode_lums.firstdim = n*60UL;
               }
               else {
                  Err("-F fadetime (min)", 18ul);
               }
            }
            else if (lasth=='V') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n>0UL) {
                  aprsdecode_lums.fps = (long)n;
               }
               else Err("-V fps", 7ul);
            }
            else if (lasth=='B') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n>0UL) {
                  aprsdecode_lums.maxdim = n*60UL;
               }
               else Err("-B blanktime (min)", 19ul);
            }
            else if (lasth=='P') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n>0UL) {
                  aprsdecode_lums.purgetime = n*60UL;
               }
               else Err("-P purgetime (min)", 19ul);
            }
            else if (lasth=='r') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.rf = (long)n;
               }
               else Err("-r rftracklumen", 16ul);
            }
            else if (lasth=='o') {
               Lib_NextArg(h, 4096ul);
               i = 0UL;
               if (GetNum(h, 4096ul, &i, &n)>=0L && n<=1024UL) {
                  aprsdecode_lums.map = (long)n;
               }
               else Err("-o maplumen", 12ul);
            }
            else if (lasth=='v') aprsdecode_verb = 1;
            else err = 1;
         }
      }
      else {
         /*h[0]:=0C;*/
         err = 1;
      }
      if (err) break;
   }
   if (err) {
      aprsstr_Append(h, 4096ul, " ? use -h", 10ul);
      Err(h, 4096ul);
   }
/*  IF servercall[0]=0C THEN WrLn; WrStrLn("aprsmap: NO SERVERCALL ?"); WrLn;
                 END; */
} /* end parms() */


static char getudp(unsigned long usock, aprsdecode_FRAMEBUF buf)
{
   unsigned long fromport;
   unsigned long ipn;
   char crc2;
   char crc1;
   long len;
   unsigned long mlen;
   aprsdecode_FRAMEBUF mbuf;
   if ((long)aprsdecode_udpsocks0[usock].fd<0L) return 0;
   len = udpreceive(aprsdecode_udpsocks0[usock].fd, buf, 512L, &fromport,
                &ipn);
   if ((len>2L && len<512L) && (!aprsdecode_udpsocks0[usock].checkip || aprsdecode_udpsocks0[usock].ip==ipn)
                ) {
      /*WrInt(udpsend(usock^.fd, buf, len, usock^.dport, usock^.ip), 1);
                WrStrLn(" us"); */
      buf[len] = 0;
      if (aprsdecode_udpsocks0[usock].rawread) {
         crc1 = buf[len-2L];
         crc2 = buf[len-1L];
         aprsstr_AppCRC(buf, 512ul, len-2L);
         if (crc1!=buf[len-2L] || crc2!=buf[len-1L]) {
            if (aprsdecode_verb) osi_WrStrLn(" axudp crc error", 17ul);
            buf[0UL] = 0;
         }
         else {
            aprsstr_raw2mon(buf, 512ul, mbuf, 512ul, (unsigned long)(len-2L),
                 &mlen);
            memcpy(buf,mbuf,512u);
         }
      }
   }
   else return 0;
   return 1;
} /* end getudp() */


static long Gettcp(long fd, aprsdecode_FRAMEBUF line,
                aprsdecode_FRAMEBUF buf, long * pos)
{
   long j;
   long i;
   long len;
   long tmp;
   i = *pos;
   if (i>=512L) i = 0L;
   /*
     len:=RdBin(fd, line, SIZE(line)-i);
     IF len<=0 THEN RETURN -1 END;
                (* disconnected *)
   */
   /*
     IF len<0 THEN len:=0 END;
   */
   len = readsock(fd, line, 512L-i);
   if (len<0L) return -1L;
   if (*pos>=512L) {
      /* hunt mode */
      i = 0L;
      while (i<len && line[i]!='\012') ++i;
      if (i>=len) return 0L;
      /* no line start found */
      *pos = 0L;
      ++i;
      j = 0L;
      while (i<len) {
         line[j] = line[i];
         ++j;
         ++i;
      }
      len = j;
   }
   if (*pos>0L) {
      for (i = len-1L; i>=0L; i--) {
         line[i+*pos] = line[i]; /* move up new */
      } /* end for */
      tmp = *pos-1L;
      i = 0L;
      if (i<=tmp) for (;; i++) {
         line[i] = buf[i]; /* ins buf before */
         if (i==tmp) break;
      } /* end for */
      len += *pos;
      *pos = 0L;
   }
   i = 0L;
   while (i<len && line[i]!='\012') ++i;
   if (i>=len) {
      /* no line end found */
      while (*pos<len) {
         buf[*pos] = line[*pos]; /* move to buf */
         ++*pos;
      }
      len = 0L;
   }
   else {
      j = i+1L;
      while (j<len) {
         buf[*pos] = line[j]; /* move rest to buf */
         ++*pos;
         ++j;
      }
      len = i;
   }
   line[len] = 0;
   return len;
} /* end Gettcp() */


static void saybusy(long * fd, char s[], unsigned long s_len)
/* send good by text and close tcp */
{
   long res;
   X2C_PCOPY((void **)&s,s_len);
   if ((long)*fd>=0L) {
      res = sendsock(*fd, s, (long)aprsstr_Length(s, s_len));
      osi_CloseSock(*fd);
      *fd = -1L;
   }
   X2C_PFREE(s);
} /* end saybusy() */


static void saylinkbad(long s)
{
   char hh[101];
   char h[101];
   strncpy(h,"AprsIs uplink delays ",101u);
   aprsstr_IntToStr(s, 1UL, hh, 101ul);
   aprsstr_Append(h, 101ul, hh, 101ul);
   aprsstr_Append(h, 101ul, "s", 2ul);
   useri_textautosize(0L, 5L, 6UL, 5UL, 'r', h, 101ul);
} /* end saylinkbad() */


static void WatchTXQ(aprsdecode_pTCPSOCK sock)
{
   long sent;
   long j;
   long i;
   long rb;
   long acked;
   long qb;
   char rest;
   long tmp;
   qb = getunack(sock->fd); /* tcp unack sendqueue in byte */
   if (qb==-1L) {
      useri_configbool(useri_fALLOWGATE, 0);
      useri_textautosize(0L, 5L, 6UL, 5UL, 'r', "TCP-Stack on this OS not usa\
ble for Igate, gateing switched off", 64ul);
   }
   sent = (long)(sock->txbytes-sock->qwatch.lasttb);
                /* last second sent bytes */
   if (qb<=0L || sent<0L) sock->qwatch.qsize = 0L;
   else {
      /* we have unack bytes */
      acked = (sock->qwatch.lastqb-qb)+sent; /* acked bytes in last s */
      j = 0L;
      rest = 0;
      tmp = sock->qwatch.qsize-1L;
      i = 0L;
      if (i<=tmp) for (;; i++) {
         rb = sock->qwatch.txbyte[i]-acked;
         sock->qwatch.txbyte[j] = rb;
         if (rb>0L) rest = 1;
         else acked = -rb;
         if (rest) {
            acked = 0L;
            ++j;
         }
         if (i==tmp) break;
      } /* end for */
      sock->qwatch.qsize = j;
      if (sock->qwatch.qsize<=59L) {
         sock->qwatch.txbyte[sock->qwatch.qsize] = sent;
         ++sock->qwatch.qsize;
         if (sock->qwatch.qsize>1L) saylinkbad(sock->qwatch.qsize);
         if (sock->qwatch.qsize>15L) {
            stoptxrx(sock->fd, 2L); /* delete unsent data */
            saybusy(&sock->fd, "\015\012", 3ul); /* close tcp immediately */
            sock->slowlink = 1; /* for log message */
         }
      }
   }
   sock->qwatch.lastqb = qb;
   sock->qwatch.lasttb = sock->txbytes;
} /* end WatchTXQ() */


static void tickermon(const char port[], unsigned long port_len, char dir,
                const char s[], unsigned long s_len)
{
   struct aprsdecode_DAT dat;
   TICKERCALL h2;
   TICKERCALL h1;
   TICKERCALL h;
   unsigned long cnt;
   unsigned long i;
   unsigned long tmp;
   cnt = (unsigned long)useri_conf2int(useri_fWRTICKER, 0UL, 0L, 99L, 1L);
   /*WrInt(cnt, 10); WrLn; */
   if (cnt>0UL && aprsdecode_Decode(s, s_len, &dat)>=0L) {
      aprsstr_Assign(h2, 31ul, port, port_len);
      h2[0U] = X2C_CAP(h2[0U]);
      aprsstr_Append(h2, 31ul, (char *) &dir, 1u/1u);
      aprsstr_Append(h2, 31ul, dat.srccall, 9ul);
      if (dat.wx.temp!=1.E+6f) {
         aprsstr_FixToStr(aprstext_FtoC(dat.wx.temp), 2UL, h, 31ul);
         aprsstr_Append(h2, 31ul, " ", 2ul);
         aprsstr_Append(h2, 31ul, h, 31ul);
         aprsstr_Append(h2, 31ul, "C", 2ul);
      }
      else if ((dat.altitude<X2C_max_longint && dat.altitude>=1000L)
                && dat.altitude>=useri_conf2int(useri_fALTMIN, 0UL, -10000L,
                65535L, -10000L)) {
         aprsstr_Append(h2, 31ul, " ", 2ul);
         aprsstr_IntToStr(dat.altitude, 1UL, h, 31ul);
         aprsstr_Append(h2, 31ul, h, 31ul);
         aprsstr_Append(h2, 31ul, "m", 2ul);
      }
      else if (dat.speed>0UL && dat.speed<2147483647UL) {
         useri_confstr(useri_fKMH, h1, 31ul);
         if (h1[0U]) {
            aprsstr_IntToStr((long)aprsdecode_trunc((float)dat.speed*1.852f),
                 1UL, h, 31ul);
            aprsstr_Append(h2, 31ul, " ", 2ul);
            aprsstr_Append(h2, 31ul, h, 31ul);
            aprsstr_Append(h2, 31ul, h1, 31ul);
         }
      }
      for (i = 19UL; i>=1UL; i--) {
         memcpy(frameticker[i],frameticker[i-1UL],31u);
      } /* end for */
      memcpy(frameticker[0U],h2,31u);
      if (tickertime!=aprsdecode_realtime) {
         aprsstr_Assign(xosi_headmh, 251ul, h2, 31ul);
         if (cnt>19UL) cnt = 19UL;
         tmp = cnt-1UL;
         i = 1UL;
         if (i<=tmp) for (;; i++) {
            aprsstr_Append(xosi_headmh, 251ul, "  *  ", 6ul);
            aprsstr_Append(xosi_headmh, 251ul, frameticker[i], 31ul);
            if (i==tmp) break;
         } /* end for */
         tickertime = aprsdecode_realtime;
      }
   }
} /* end tickermon() */


static void Stopticker(void)
{
   aprsstr_Assign(xosi_headmh, 251ul, "\015", 2ul);
   tickertime = 0x0FFFFFFC3UL;
   memset((char *)frameticker,(char)0,620UL);
} /* end Stopticker() */


static void wrmon(unsigned long port, char dir, const char s[],
                unsigned long s_len)
{
   char mcon[10];
   char ms[31];
   if (port==0UL) strncpy(mcon,"n",10u);
   else aprsstr_IntToStr((long)port, 0UL, mcon, 10ul);
   if (useri_configon(useri_fWRTICKER)) tickermon(mcon, 10ul, dir, s, s_len);
   /*IF configon(fWRINCOM) THEN */
   useri_confstr(useri_fWRINCOM, ms, 31ul);
   if (aprsstr_InStr(ms, 31ul, mcon, 10ul)>=0L) {
      aprsstr_Append(mcon, 10ul, "+", 2ul);
      aprstext_listin(s, s_len, X2C_CAP(mcon[0U]), dir, aprsstr_InStr(ms,
                31ul, mcon, 10ul)>=0L);
   }
/*END; */
} /* end wrmon() */


static char Sendtcp(aprsdecode_pTCPSOCK to, const aprsdecode_FRAMEBUF buf)
{
   long i;
   long len;
   char ok0;
   struct aprsdecode_TCPSOCK * anonym;
   long tmp;
   ok0 = 0;
   if (to) {
      if (to->connt>0UL) {
         wrmon(0UL, '<', buf, 512ul);
                /* do not mon the connected-test-transmissions */
      }
      len = (long)aprsstr_Length(buf, 512ul);
      { /* with */
         struct aprsdecode_TCPSOCK * anonym = to;
         if (anonym->tlen+len<512L) {
            tmp = len-1L;
            i = 0L;
            if (i<=tmp) for (;; i++) {
               anonym->tbuf[anonym->tlen] = buf[i];
               ++anonym->tlen;
               if (i==tmp) break;
            } /* end for */
            ++anonym->txframes;
            anonym->txbytes += (unsigned long)len;
            aprsdecode_lasttcptx = aprsdecode_realtime;
            ok0 = 1;
         }
         else if (aprsdecode_verb) osi_WrStrLn("tx buf overflow", 16ul);
      }
   }
   return ok0;
} /* end Sendtcp() */


static char Sendudp(const char s[], unsigned long s_len,
                unsigned long uport)
{
   long len;
   aprsdecode_FRAMEBUF raw;
   char ok0;
   struct aprsdecode_UDPSOCK * anonym;
   ok0 = 0;
   if ((uport<=3UL && (long)aprsdecode_udpsocks0[uport].fd>=0L)
                && aprsdecode_udpsocks0[uport].dport>0UL) {
      aprsstr_mon2raw(s, s_len, raw, 512ul, &len);
      if (len>1L) {
         len = udpsend(aprsdecode_udpsocks0[uport].fd, raw, len,
                aprsdecode_udpsocks0[uport].dport,
                aprsdecode_udpsocks0[uport].ip);
         if (len>0L) {
            wrmon(uport+1UL, '<', s, s_len);
            { /* with */
               struct aprsdecode_UDPSOCK * anonym = &aprsdecode_udpsocks0[uport]
                ;
               ++anonym->txframes;
               anonym->txbytes += (unsigned long)len;
               anonym->lastudptx = aprsdecode_realtime;
               if (anonym->starttime==0UL) {
                  anonym->starttime = aprsdecode_realtime;
               }
               ok0 = 1;
            }
         }
      }
      else if (aprsdecode_verb) osi_WrStrLn("wrong rf monitor format", 24ul);
   }
   return ok0;
} /* end Sendudp() */


extern char aprsdecode_getmypos(struct aprspos_POSITION * pos)
{
   char s[21];
   useri_confstr(useri_fMYPOS, s, 21ul);
   aprstext_deghtopos(s, 21ul, pos);
   return aprspos_posvalid(*pos);
} /* end getmypos() */

#define aprsdecode_MSYM "\\"


static void beaconmacros(char s[], unsigned long s_len, const char path[],
                unsigned long path_len, char wdata[],
                unsigned long wdata_len, char all)
{
   unsigned long i;
   long j;
   long len;
   char ns[256];
   char ds[256];
   char fn[1024];
   long f;
   char rw;
   X2C_PCOPY((void **)&wdata,wdata_len);
   i = 0UL;
   ns[0U] = 0;
   while (i<s_len-1 && s[i]) {
      if (s[i]=='\\' && s[i+1UL]=='\\') {
         i += 2UL;
         if (s[i]=='z' && all || s[i]=='Z') {
            /* insert day, hour, min */
            aprsstr_DateToStr(aprsdecode_realtime, ds, 256ul);
            ds[0U] = ds[8U];
            ds[1U] = ds[9U];
            ds[2U] = ds[11U];
            ds[3U] = ds[12U];
            ds[4U] = ds[14U];
            ds[5U] = ds[15U];
            ds[6U] = 0;
            aprsstr_Append(ns, 256ul, ds, 256ul);
         }
         else if (s[i]=='h' && all || s[i]=='H') {
            /* insert hour, min, s */
            aprsstr_DateToStr(aprsdecode_realtime, ds, 256ul);
            ds[0U] = ds[11U];
            ds[1U] = ds[12U];
            ds[2U] = ds[14U];
            ds[3U] = ds[15U];
            ds[4U] = ds[17U];
            ds[5U] = ds[18U];
            ds[6U] = 0;
            aprsstr_Append(ns, 256ul, ds, 256ul);
         }
         else if (all) {
            if (s[i]=='<' || s[i]=='>') {
               /* insert or write file */
               rw = s[i];
               fn[0U] = 0;
               ++i;
               while (((i<s_len-1 && s[i]) && s[i]!='<') && s[i]!='>') {
                  aprsstr_Append(fn, 1024ul, (char *) &s[i], 1u/1u);
                  ++i;
               }
               if (rw=='<') {
                  /* read file */
                  f = osi_OpenRead(fn, 1024ul);
                  if (osi_FdValid(f)) {
                     len = osi_RdBin(f, (char *)ds, 256u/1u, 255UL);
                     osi_Close(f);
                     j = 0L;
                     while (((j<len && ds[j]!='\015') && ds[j]!='\012')
                && ds[j]) {
                        aprsstr_Append(ns, 256ul, (char *) &ds[j], 1u/1u);
                        ++j;
                     }
                  }
                  else {
                     aprsstr_Assign(s, s_len, "beacon macro file not readable\
 ", 32ul);
                     aprsstr_Append(s, s_len, fn, 1024ul);
                     useri_xerrmsg(s, s_len);
                     s[0UL] = 0;
                     goto label;
                  }
               }
               else if (wdata[0UL]) {
                  /* write file */
                  f = osi_OpenWrite(fn, 1024ul);
                  if (osi_FdValid(f)) {
                     osi_WrBin(f, (char *)wdata, (wdata_len)/1u,
                aprsstr_Length(wdata, wdata_len));
                     osi_Close(f);
                  }
               }
            }
            else if (s[i]=='\\') aprsstr_Append(ns, 256ul, "\\\\", 3ul);
            else if (s[i]=='v') {
               aprsstr_Append(ns, 256ul, "aprsmap(cu) 0.46", 17ul);
            }
            else if (s[i]=='l') {
               useri_confstr(useri_fMYPOS, ds, 256ul);
               aprsstr_Append(ns, 256ul, ds, 256ul);
            }
            else if (s[i]=='p') aprsstr_Append(ns, 256ul, path, path_len);
            else {
               aprsstr_Assign(s, s_len, "bad beacon macro ", 18ul);
               aprsstr_Append(s, s_len, ns, 256ul);
               aprsstr_Append(s, s_len, "???", 4ul);
               useri_xerrmsg(s, s_len);
               s[0UL] = 0;
               goto label;
            }
         }
         else {
            aprsstr_Append(ns, 256ul, "\\\\", 3ul);
            aprsstr_Append(ns, 256ul, (char *) &s[i], 1u/1u);
         }
      }
      else aprsstr_Append(ns, 256ul, (char *) &s[i], 1u/1u);
      ++i;
   }
   aprsstr_Assign(s, s_len, ns, 256ul);
   label:;
   X2C_PFREE(wdata);
} /* end beaconmacros() */


static void BuildNetBeacon(char h[], unsigned long h_len)
/* "!ddmm.mmNsdddmm.mms....." */
{
   struct aprspos_POSITION mypos;
   char h3[301];
   char h2[301];
   char h1[301];
   useri_confstr(useri_fMYPOS, h1, 301ul);
   useri_confstr(useri_fMYSYM, h3, 301ul);
   useri_confstr(useri_fNBTEXT, h2, 301ul);
   aprstext_degtopos(h1, 301ul, &mypos);
   if (!aprspos_posvalid(mypos)) {
      useri_xerrmsg("netbeacon: wrong my position", 29ul);
      h[0UL] = 0;
      return;
   }
   if (aprsstr_Length(h3, 301ul)!=2UL) {
      useri_xerrmsg("netbeacon: wrong symbol", 24ul);
      h[0UL] = 0;
      return;
   }
   aprsstr_Assign(h, h_len, "!", 2ul);
   h1[8U] = h3[0U];
   h1[18U] = h3[1U];
   h1[19U] = 0;
   aprsstr_Append(h, h_len, h1, 301ul);
   beaconmacros(h2, 301ul, "", 1ul, "", 1ul, 1);
   aprsstr_Append(h, h_len, h2, 301ul);
} /* end BuildNetBeacon() */

/*
PROCEDURE Testbeacon(b:ARRAY OF CHAR):BOOLEAN;
VAR dat:DAT;
BEGIN
  IF (b[0]=0C) OR (Decode(b, dat)<0) THEN RETURN FALSE END;
  IF dat.symt=0C THEN xerrmsg("Beacon: wrong Symbol"); RETURN FALSE END;
  RETURN TRUE
END Testbeacon;
*/

static void SendNetBeacon(aprsdecode_pTCPSOCK cp,
                const aprsdecode_FRAMEBUF b, char manual, char errtxt[],
                unsigned long errtxt_len)
{
   long ret;
   struct aprsdecode_DAT dat;
   if (b[0UL]==0 || aprsdecode_Decode(b, 512ul, &dat)<0L) {
      aprsstr_Assign(errtxt, errtxt_len, "Net Message not Decodable", 26ul);
      return;
   }
   if (!Sendtcp(cp, b)) {
      if (manual) {
         aprsstr_Assign(errtxt, errtxt_len, "Netport not connected", 22ul);
      }
      return;
   }
   if (aprsdecode_lums.logmode) {
      ret = aprsdecode_Stoframe(&aprsdecode_ophist2, b, 512ul,
                aprsdecode_realtime, 0, dat);
   }
   else {
      ret = aprsdecode_Stoframe(&aprsdecode_ophist0, b, 512ul,
                aprsdecode_realtime, 0, dat);
   }
} /* end SendNetBeacon() */


static void SendNet(aprsdecode_pTCPSOCK cp, const char data[],
                unsigned long data_len, char manual, char errtxt[],
                unsigned long errtxt_len)
/* make net header append string and send */
{
   aprsdecode_FRAMEBUF s;
   char pass[51];
   char h[51];
   errtxt[0UL] = 0;
   if (!useri_configon(useri_fALLOWNETTX) || !useri_configon(useri_fCONNECT))
                 {
      if (manual) {
         aprsstr_Assign(errtxt, errtxt_len,
                "Net Port not configured to Send", 32ul);
      }
      return;
   }
   useri_confstr(useri_fMYCALL, s, 512ul);
   if (aprsstr_Length(s, 512ul)<3UL) {
      aprsstr_Assign(errtxt, errtxt_len, "netbeacon: no mycall", 21ul);
      return;
   }
   useri_confstr(useri_fMSGNETDEST, h, 51ul);
   if (h[0U]==0) {
      aprsstr_Assign(errtxt, errtxt_len, "netbeacon: no destination call",
                31ul);
      return;
   }
   if (manual) {
      useri_confstr(useri_fPW, pass, 51ul);
      if (pass[0U]==0) {
         useri_xerrmsg("sending to Net without Passcode", 32ul);
      }
   }
   aprsstr_Append(s, 512ul, ">", 2ul);
   aprsstr_Append(s, 512ul, h, 51ul);
   aprsstr_Append(s, 512ul, ",TCPIP*:", 9ul);
   aprsstr_Append(s, 512ul, data, data_len);
   aprsstr_Append(s, 512ul, "\015\012", 3ul);
   SendNetBeacon(cp, s, manual, errtxt, errtxt_len);
} /* end SendNet() */


static void Timebeacon(aprsdecode_pTCPSOCK cp)
{
   aprsdecode_FRAMEBUF h;
   unsigned long bt;
   char err[100];
   /*WrInt(ORD(configon(fALLOWNETTX)), 5); WrInt(ORD(configon(fCONNECT)), 5);
                WrStrLn(" tb1"); */
   bt = (unsigned long)useri_conf2int(useri_fNETBTIME, 0UL, 0L,
                X2C_max_longint, 0L);
   if (((useri_configon(useri_fALLOWNETTX) && useri_configon(useri_fCONNECT))
                 && (bt>=1UL || cp->beacont==0UL))
                && Watchclock(&cp->beacont, bt)) {
      if ((bt<60UL && bt>0UL) && cp->connt>0UL) {
         useri_xerrmsg("Netbeacon: too fast", 20ul);
      }
      /*
          qai:=TRUE; 
          IF (qas=1) OR (qas>qasc+1) THEN qai:=FALSE;
                INC(qasc) ELSE qasc:=0 END;
      */
      BuildNetBeacon(h, 512ul);
      SendNet(cp, h, 512ul, 0, err, 100ul);
      useri_xerrmsg(err, 100ul);
   }
} /* end Timebeacon() */

/*
PROCEDURE nibb(n:CARDINAL; VAR s:ARRAY OF CHAR);
BEGIN IF n<=9 THEN Append(s, CHR(n+ORD("0"))) ELSE Append(s,
                CHR(n+(ORD("a")-10))) END END nibb;
*/

extern long aprsdecode_knottokmh(long kn)
{
   if (kn>=0L && kn<50000L) {
      return (long)aprsdecode_trunc((float)kn*1.852f+0.5f);
   }
   else return 0L;
   return 0;
} /* end knottokmh() */


static void NoWX(struct aprsdecode_WX * wx)
{
   wx->gust = 1.E+6f;
   wx->temp = 1.E+6f;
   wx->rain1 = 1.E+6f;
   wx->rain24 = 1.E+6f;
   wx->raintoday = 1.E+6f;
   wx->hygro = 1.E+6f;
   wx->baro = 1.E+6f;
   wx->lum = 1.E+6f;
} /* end NoWX() */


static void wpar(unsigned long * p, char buf[], unsigned long buf_len,
                float * v)
{
   char empty;
   char dot;
   char sn;
   char c;
   float x;
   float div0;
   dot = 0;
   empty = 1;
   sn = buf[*p]=='t';
   x = 0.0f;
   div0 = 1.0f;
   for (;;) {
      ++*p;
      c = buf[*p];
      if (sn && c=='-') {
         div0 = -div0;
         empty = 0;
      }
      else if ((unsigned char)c>='0' && (unsigned char)c<='9') {
         x = x*10.0f+(float)((unsigned long)(unsigned char)buf[*p]-48UL);
         if (x>1.E+5f) return;
         if (dot) div0 = div0*0.1f;
         empty = 0;
      }
      else if (empty) {
         if (c!=' ' && c!='.') break;
      }
      else if (c=='.') {
         if (dot) break;
         dot = 1;
      }
      else break;
      sn = 0;
   }
   if (!empty) *v = x*div0;
} /* end wpar() */


static void GetWX(struct aprsdecode_WX * wx, unsigned long * course,
                unsigned long * speed, const char buf[],
                unsigned long buf_len, unsigned long * p)
{
   float wdir;
   float wwind;
   NoWX(wx);
   wwind = 1.E+6f;
   wdir = 1.E+6f;
   if (buf[*p]=='_') *p += 9UL;
   for (;;) {
      switch ((unsigned)buf[*p]) {
      case 'g':
         wpar(p, buf, buf_len, &wx->gust);
         break;
      case 't':
         wpar(p, buf, buf_len, &wx->temp);
         break;
      case 'r':
         wpar(p, buf, buf_len, &wx->rain1);
         break;
      case 'p':
         wpar(p, buf, buf_len, &wx->rain24);
         break;
      case 'P':
         wpar(p, buf, buf_len, &wx->raintoday);
         break;
      case 'h':
         wpar(p, buf, buf_len, &wx->hygro);
         break;
      case 'b':
         wpar(p, buf, buf_len, &wx->baro);
         break;
      case 'L':
         wpar(p, buf, buf_len, &wx->lum);
         break;
      case 'l':
         wpar(p, buf, buf_len, &wx->lum);
         wx->lum = wx->lum+1000.0f;
         break;
      case 'c':
         wpar(p, buf, buf_len, &wdir);
         break;
      case 's':
         wpar(p, buf, buf_len, &wwind);
         break;
      default:;
         goto loop_exit;
      } /* end switch */
   }
   loop_exit:;
   if (wdir>=0.0f && wdir<360.0f) *course = aprsdecode_trunc(wdir);
   if (wwind>=0.0f && wwind<1000.0f) *speed = aprsdecode_trunc(wwind);
} /* end GetWX() */

#define aprsdecode_DATELEN 15


static char wrlog(const char b[], unsigned long b_len, unsigned long time0,
                char wfn[], unsigned long wfn_len, long good)
{
   long f;
   char h[1001];
   unsigned long i;
   unsigned long l;
   char wrlog_ret;
   X2C_PCOPY((void **)&wfn,wfn_len);
   if (wfn[0UL]==0) {
      /*
      WrStrLn("--- wfn0");
      FOR i:=0 TO HIGH(wfn) DO WrInt(ORD(wfn[i]), 4) END;
      WrLn;
      */
      wrlog_ret = 0;
      goto label;
   }
   l = aprsstr_Length(b, b_len);
   while (l>0UL && (unsigned char)b[l-1UL]<='\015') --l;
   if (l>0UL) {
      aprsstr_cleanfilename(wfn, wfn_len);
      f = osi_OpenAppend(wfn, wfn_len);
      if (!osi_FdValid(f)) f = osi_OpenWrite(wfn, wfn_len);
      if (!osi_FdValid(f)) {
         wrlog_ret = 0;
         goto label;
      }
      aprsstr_DateToStr(time0, h, 1001ul);
      h[4U] = h[5U];
      h[5U] = h[6U];
      h[6U] = h[8U];
      h[7U] = h[9U];
      h[8U] = ':';
      h[9U] = h[11U];
      h[10U] = h[12U];
      h[11U] = h[14U];
      h[12U] = h[15U];
      h[13U] = h[17U];
      h[14U] = h[18U];
      if (good>=0L) h[15U] = ' ';
      else h[15U] = '-';
      if (l>=984UL) l = 984UL;
      i = 0UL;
      do {
         h[i+16UL] = b[i];
         ++i;
      } while (i<l);
      h[i+16UL] = '\012';
      ++i;
      osi_WrBin(f, (char *)h, 1001u/1u, i+16UL);
      osi_Close(f);
   }
   wrlog_ret = 1;
   label:;
   X2C_PFREE(wfn);
   return wrlog_ret;
} /* end wrlog() */


static aprsdecode_pOPHIST findop(aprsdecode_MONCALL call, char online)
{
   aprsdecode_pOPHIST op;
   aprsdecode_MONCALL tmp;
   call = (char *)memcpy(tmp,call,9u);
   if (online && aprsdecode_lums.logmode) op = aprsdecode_ophist2;
   else op = aprsdecode_ophist0;
   while (op && X2C_STRCMP(call,9u,op->call,9u)) op = op->next;
   return op;
} /* end findop() */


static void poplogerr(char fn[], unsigned long fn_len)
{
   char h[1001];
   X2C_PCOPY((void **)&fn,fn_len);
   strncpy(h,"logfile write error ",1001u);
   aprsstr_Append(h, 1001ul, fn, fn_len);
   useri_textautosize(0L, 5L, 6UL, 4UL, 'e', h, 1001ul);
   X2C_PFREE(fn);
} /* end poplogerr() */


extern void aprsdecode_savetrack(void)
{
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST pf;
   char h[101];
   char fn[1001];
   char ap;
   aprsdecode_MONCALL name;
   name[0UL] = 0;
   if (aprsdecode_click.mhop[0UL]) memcpy(name,aprsdecode_click.mhop,9u);
   else if (aprsdecode_click.entries>0UL && aprsdecode_click.table[aprsdecode_click.selected]
                .opf) {
      memcpy(name,aprsdecode_click.table[aprsdecode_click.selected].opf->call,
                9u);
   }
   if (name[0UL]) {
      op = findop(name, 0);
      if (op) {
         aprsstr_Assign(fn, 1001ul, op->call, 9ul); /* make 0C terminated */
         alfanum(fn, 1001ul); /* remove bad chars */
         aprsdecode_makelogfn(fn, 1001ul);
         ap = FileSys_Exists(fn, 1001ul);
         pf = op->frames;
         while (pf) {
            if (!wrlog(pf->vardat->raw, 500ul, pf->time0, fn, 1001ul, 0L)) {
               poplogerr(fn, 1001ul);
               return;
            }
            pf = pf->next;
         }
         if (ap) strncpy(h,"Append to logfile ",101u);
         else strncpy(h,"Write logfile ",101u);
         aprsstr_Append(h, 101ul, fn, 1001ul);
         useri_textautosize(0L, 5L, 6UL, 4UL, 'b', h, 101ul);
      }
   }
} /* end savetrack() */


static char replyack(char rep[], unsigned long rep_len, char restack[],
                unsigned long restack_len, const char ack[],
                unsigned long ack_len)
{
   aprsstr_Assign(rep, rep_len, ack, ack_len);
   aprsstr_Assign(restack, restack_len, ack, ack_len);
   if (aprsstr_Length(ack, ack_len)>2UL && ack[2UL]=='}') {
      /* {xx}yy */
      rep[0UL] = ack[3UL];
      rep[1UL] = ack[4UL];
      if (rep_len-1>=2UL) rep[2UL] = 0;
      if (restack_len-1>=2UL) restack[2UL] = 0;
      return 1;
   }
   return 0;
} /* end replyack() */


static char iscall(const aprsdecode_MONCALL b)
{
   unsigned long i;
   unsigned long nu;
   unsigned long li;
   char c;
   li = 0UL;
   nu = 0UL;
   for (i = 0UL; i<=2UL; i++) {
      c = b[i];
      if ((unsigned char)c>='A' && (unsigned char)c<='Z') ++li;
      else if ((unsigned char)c>='0' && (unsigned char)c<='9') {
         if (i==0UL) ++li;
         else ++nu;
      }
   } /* end for */
   return (nu+li==3UL && nu>=1UL) && nu<=2UL;
} /* end iscall() */


static char viaghost(const char b[], unsigned long b_len)
/* ECHO GATE RELAY n=n ELSE ghost */
{
   unsigned long i;
   if ((((b[0UL]=='E' && b[1UL]=='C') && b[2UL]=='H') && b[3UL]=='O')
                && b[4UL]==0) return 0;
   if ((((b[0UL]=='G' && b[1UL]=='A') && b[2UL]=='T') && b[3UL]=='E')
                && b[4UL]==0) return 0;
   if (((((b[0UL]=='R' && b[1UL]=='E') && b[2UL]=='L') && b[3UL]=='A')
                && b[4UL]=='Y') && b[5UL]==0) return 0;
   i = aprsstr_Length(b, b_len);
   if ((((i>3UL && b[i-2UL]=='-') && b[i-1UL]==b[i-3UL]) && (unsigned char)
                b[i-1UL]>='1') && (unsigned char)b[i-1UL]<='7') return 0;
   return 1;
} /* end viaghost() */


static float num(char buf[], unsigned long buf_len, unsigned long * p)
{
   char c;
   char sig;
   float f;
   float r;
   r = 0.0f;
   sig = 0;
   f = 1.0f;
   for (;;) {
      if (*p>buf_len-1 || (float)fabs(r)>=2.147483647E+9f) break;
      c = buf[*p];
      if (sig==0 && c=='-' || c=='+') sig = c;
      else if (f>0.5f && c=='.') f = f*0.1f;
      else if ((unsigned char)c>='0' && (unsigned char)c<='9') {
         if (f>0.5f) {
            r = r*10.0f+(float)((unsigned long)(unsigned char)c-48UL);
         }
         else {
            r = r+(float)((unsigned long)(unsigned char)c-48UL)*f;
            f = f*0.1f;
         }
      }
      else break;
      ++*p;
      if (sig==0) sig = '\001';
   }
   if (r>=2.147483647E+9f) r = 2.147483647E+9f;
   if (sig=='-') r = -r;
   return r;
} /* end num() */


static void skip(char buf[], unsigned long buf_len, unsigned long * p)
{
   if (*p<=buf_len-1 && buf[*p]==',') ++*p;
} /* end skip() */


static unsigned long corrtime(unsigned long syst, unsigned long mt,
                unsigned long span)
/* complete modulo time to absolute */
{
   mt = (span+mt%span)-syst%span;
   if (mt<span/2UL) mt += span;
   else if (mt>span+span/2UL) mt -= span;
   mt += syst;
   if (mt>=span) mt -= span;
   return mt;
} /* end corrtime() */

#define aprsdecode_H 3600

#define aprsdecode_DAY 86400

#define aprsdecode_DEGUNIT 1.7453292519444E-7
/* 1/100000 deg = 1.1111m*/


static void GetHRT(struct aprspos_POSITION * pos, long * altitude,
                unsigned long * speed, const char buf[],
                unsigned long buf_len, unsigned long * compos,
                struct aprsdecode_HRTPOS hrtposes[],
                unsigned long hrtposes_len, unsigned long * hrtlen,
                unsigned long * hrttime)
{
   unsigned long p;
   char cmd;
   unsigned long i;
   unsigned long crc;
   unsigned long duration;
   unsigned long wcnt;
   float r;
   float unit;
   unsigned long ts;
   struct aprspos_POSITION posh;
   struct aprspos_POSITION dao;
   char newcompos;
   struct aprsdecode_HRTPOS * anonym;
   unsigned long tmp;
   /* $HR[t46531u1.5p48.0235,13.1512a367d15,11w-479,622,33w-5,
                2-4w7+2-3ww4-1-5wg435644353-10c27001]
      t s utc modulo 24h modulo 1h or absolut since 1970
      p deg lat/long 
      a m altitude
      v m/s speed
      y m/s climb
      d deg lat/long dao
      w deg,deg,m,s lat long alt time
      u 0.00001 deg,m  unit
      g direct heard gateway
      c crc 
      q qrv mhz
      r rx freq mhz or hz
   */
   /*WrStrLn(buf); */
   *hrtlen = 0UL;
   *hrttime = 0UL;
   p = *compos;
   newcompos = 1;
   /*WrInt(p, 10);WrStrLn(" compos"); */
   for (;;) {
      if (p+3UL>buf_len-1 || buf[p]==0) return;
      if (((buf[p]=='$' && buf[p+1UL]=='H') && buf[p+2UL]=='R')
                && buf[p+3UL]=='[') break;
      /*WrStrLn(" newcompos"); */
      ++p;
      newcompos = 0;
   }
   p += 4UL;
   ts = aprsdecode_systime;
   unit = 1.0f;
   duration = 0UL;
   wcnt = 0UL;
   dao.lat = 0.0f;
   dao.long0 = 0.0f;
   memset((char *)hrtposes,(char)0,
                hrtposes_len*sizeof(struct aprsdecode_HRTPOS));
   crc = 0UL;
   for (;;) {
      cmd = buf[p];
      ++p;
      if (cmd==0 || p>buf_len-1) {
         *hrtlen = 0UL; /* end of string? */
         return;
      }
      if (cmd=='t') {
         ts = aprsdecode_trunc(num(buf, buf_len, &p)+0.5f);
         if (ts<3600UL) {
            ts = corrtime(aprsdecode_systime, ts, 3600UL);
                /* move system time max +-30min */
         }
         else if (ts<90000UL) {
            ts = corrtime(aprsdecode_systime, ts-3600UL, 86400UL);
                /* move system time max +-12h */
         }
         /* absolute time */
         *hrttime = ts;
      }
      else if (cmd=='u') unit = num(buf, buf_len, &p);
      else if (cmd=='p') {
         /* take position from extension if correct */
         posh.lat = aprspos_rad0(num(buf, buf_len, &p));
         skip(buf, buf_len, &p);
         posh.long0 = aprspos_rad0(num(buf, buf_len, &p));
         if (aprspos_posvalid(posh)) *pos = posh;
      }
      else if (cmd=='a') {
         /* take altitude from extension if correct */
         r = num(buf, buf_len, &p);
         if (r>(-1.E+4f) && r<50000.0f) {
            *altitude = (long)X2C_TRUNCI(r+0.5f,X2C_min_longint,
                X2C_max_longint);
         }
      }
      else if (cmd=='v') {
         /* take speed from extension if correct */
         r = num(buf, buf_len, &p);
         if (r>0.0f && r<50000.0f) {
            *speed = (unsigned long)(long)X2C_TRUNCI(r+0.5f,X2C_min_longint,
                X2C_max_longint);
         }
      }
      else if (cmd=='d') {
         /*  ELSIF cmd="i" THEN                              (* define constant time intervall *)
                 */
         /*    duration:=trunc(num()+0.5); */
         /* DAO, correct position */
         dao.lat = num(buf, buf_len, &p);
         skip(buf, buf_len, &p);
         dao.long0 = num(buf, buf_len, &p);
      }
      else if (cmd=='w') {
         /* waypoints, first is difference from now */
         if (wcnt<=hrtposes_len-1) {
            { /* with */
               struct aprsdecode_HRTPOS * anonym = &hrtposes[wcnt];
               anonym->dlat = num(buf, buf_len, &p);
               skip(buf, buf_len, &p);
               anonym->dlong = num(buf, buf_len, &p);
               skip(buf, buf_len, &p);
               anonym->dalt = num(buf, buf_len, &p);
               anonym->notinterpolated = (anonym->dlat!=0.0f || anonym->dlong!=0.0f)
                 || anonym->dalt!=0.0f;
               skip(buf, buf_len, &p);
               r = num(buf, buf_len, &p);
               anonym->dtime = aprsdecode_trunc((float)fabs(r)+0.5f);
               if (r<0.0f) anonym->dtime = 0UL-anonym->dtime;
            }
            ++wcnt;
         }
      }
      else if (cmd=='c') {
         /* crc incl sender call and destination without ssid */
         crc = aprsdecode_trunc(num(buf, buf_len, &p)+0.5f);
      }
      else if (cmd==']') break;
   }
   /*
     IF (wcnt>0) & (hrtposes[0].dtime>0) THEN
       r:=1.0/FLOAT(wcnt);
       kt:=r*FLOAT(hrtposes[0].dtime);
       kx:=r*hrtposes[0].dlong;
       ky:=r*hrtposes[0].dlat;
       kh:=r*hrtposes[0].dalt;
       FOR i:=1 TO wcnt-1 DO
         r:=FLOAT(wcnt-i); 
         hrtposes[i].dtime:=trunc(kt*r + 0.5)-hrtposes[i].dtime;  
         hrtposes[i].dlong:=hrtposes[i].dlong + kx*r;
         hrtposes[i].dlat :=hrtposes[i].dlat  + ky*r;
         hrtposes[i].dalt :=hrtposes[i].dalt  + kh*r;
       END;
    
       r:=unit*DEGUNIT; 
       pos.long:=pos.long + dao.long*r;
       pos.lat :=pos.lat  + dao.lat *r;
   
       FOR i:=0 TO wcnt-1 DO
         hrtposes[i].dtime:=ts - hrtposes[i].dtime;
         hrtposes[i].dlong:=pos.long + hrtposes[i].dlong*r;
         hrtposes[i].dlat :=pos.lat  + hrtposes[i].dlat*r;
         hrtposes[i].dalt :=VAL(REAL, altitude) + hrtposes[i].dalt*unit;
   --WrInt(hrtposes[i].dtime, 6); WrFixed(r, 8,15); WrFixed(dao.lat*r, 8,15);
                 WrFixed(hrtposes[i].dlat*360.0/PI2, 8,15);
                WrFixed(hrtposes[i].dlong*360.0/PI2, 8,15);
                WrInt(VAL(INTEGER, hrtposes[i].dalt), 6);WrStrLn("");
   
       END;
       hrtlen:=wcnt;
     END;
   */
   if (wcnt>0UL && hrtposes[0UL].dtime>0UL) {
      tmp = wcnt-1UL;
      i = 1UL;
      if (i<=tmp) for (;; i++) {
         hrtposes[i].dtime += hrtposes[i-1UL].dtime-hrtposes[i-1UL]
                .dtime/((wcnt-i)+1UL);
         if (hrtposes[i-1UL].dtime>0UL) {
            r = X2C_DIVR((float)hrtposes[i].dtime,
                (float)hrtposes[i-1UL].dtime);
            hrtposes[i].dlong = hrtposes[i].dlong+hrtposes[i-1UL].dlong*r;
            hrtposes[i].dlat = hrtposes[i].dlat+hrtposes[i-1UL].dlat*r;
            hrtposes[i].dalt = hrtposes[i].dalt+hrtposes[i-1UL].dalt*r;
         }
         if (i==tmp) break;
      } /* end for */
      r = unit*1.7453292519444E-7f;
      pos->long0 = pos->long0+dao.long0*r;
      pos->lat = pos->lat+dao.lat*r;
      tmp = wcnt-1UL;
      i = 0UL;
      if (i<=tmp) for (;; i++) {
         hrtposes[i].dtime = ts-hrtposes[i].dtime;
         hrtposes[i].dlong = pos->long0+hrtposes[i].dlong*r;
         hrtposes[i].dlat = pos->lat+hrtposes[i].dlat*r;
         hrtposes[i].dalt = (float)*altitude+hrtposes[i].dalt*unit;
         if (i==tmp) break;
      } /* end for */
      /*WrInt(hrtposes[i].dtime, 6); WrFixed(r, 8,15);
                WrFixed(dao.lat*r, 8,15);
                WrFixed(hrtposes[i].dlat*360.0/PI2, 8,15);
                WrFixed(hrtposes[i].dlong*360.0/PI2, 8,15);
                WrInt(VAL(INTEGER, hrtposes[i].dalt), 6);WrStrLn(""); */
      *hrtlen = wcnt;
   }
   if (newcompos) *compos = p;
/*WrInt(compos, 10);WrStrLn(" composret"); */
} /* end GetHRT() */


extern char aprsdecode_checksymb(char symt, char symb)
/* true for bad symbol */
{
   return (symt!='/' && symt!='\\') && (aprsstr_InStr("#&0>AW^_acnsuvz",
                16ul, (char *) &symb,
                1u/1u)<0L || ((unsigned char)symt<'0' || (unsigned char)
                symt>'9') && ((unsigned char)symt<'A' || (unsigned char)
                symt>'Z'));
} /* end checksymb() */


extern long aprsdecode_Decode(char buf[], unsigned long buf_len,
                struct aprsdecode_DAT * dat)
{
   unsigned long compos;
   unsigned long payload;
   unsigned long iv;
   unsigned long micedest;
   unsigned long len;
   unsigned long p;
   unsigned long i;
   memset((char *)dat,(char)0,sizeof(struct aprsdecode_DAT));
   dat->speed = X2C_max_longcard;
   dat->altitude = X2C_max_longint;
   NoWX(&dat->wx);
   compos = 0UL;
   len = 0UL;
   while ((len<buf_len-1 && buf[len]) && buf[len]!='\015') ++len;
   if (len==0UL || buf[0UL]=='#') return -1L;
   i = 0UL;
   do {
      /* skip 3rd party headers */
      p = i;
      while (i<len && buf[i]!=':') ++i;
      i += 2UL;
   } while (!(i>len || buf[i-1UL]!='}'));
   i = 0UL;
   while (buf[p]!='>') {
      /* source call */
      if (p>=len || i>8UL) return -1L;
      dat->srccall[i] = buf[p];
      ++p;
      ++i;
   }
   ++p;
   micedest = p;
   i = 0UL;
   while (buf[p]!=',' && buf[p]!=':') {
      /* dest call */
      if (p>=len || i>8UL) return -1L;
      dat->dstcall[i] = buf[p];
      ++p;
      ++i;
   }
   iv = 0UL;
   while (buf[p]==',') {
      /*
          IF iv>HIGH(dat.viacalls) THEN RETURN -1 END;
      */
      if (iv>9UL) iv = 9UL;
      ++p;
      if ((buf[p]=='q' && buf[p+1UL]=='A') && buf[p+2UL]!='C') {
         dat->igatep = iv+1UL;
      }
      /*      IF dat.igatep>HIGH(dat.viacalls) THEN dat.igatep:=0;
                RETURN -1 END; */
      if (dat->igatep==iv) dat->igaterawp = p;
      i = 0UL;
      while (buf[p]!=',' && buf[p]!=':') {
         if (p>=len) return -1L;
         if (buf[p]=='*') {
            if (dat->igatep==0UL && dat->hbitp==0UL) dat->hbitp = iv+1UL;
         }
         else if (i<=8UL) {
            dat->viacalls[iv][i] = buf[p];
            ++i;
         }
         ++p;
      }
      if (dat->igatep==iv) dat->igatelen = p-dat->igaterawp;
      ++iv;
   }
   if (dat->hbitp>0UL || iv>(unsigned long)(dat->igatep!=0UL)
                *2UL && viaghost(dat->viacalls[0UL], 9ul)) {
      /* maybe not direct */
      i = 0UL;
      while ((i<9UL && i<dat->hbitp) && iscall(dat->viacalls[i])) ++i;
      dat->hbitp = i+1UL;
   }
   else dat->hbitp = 0UL;
   /*WrInt(dat.hbitp, 1);WrStrLn("=h"); */
   if (p+1UL>=len || buf[p]!=':') return -1L;
   ++p;
   payload = p;
   dat->typc = buf[payload];
   switch ((unsigned)dat->typc) {
   case '\034':
   case '\035':
   case '\'':
   case '`':
      dat->type = aprsdecode_MICE;
      break;
   case '!':
   case '$':
   case '/':
   case '=':
   case '@':
      dat->type = aprsdecode_POS;
      break;
   case '#':
   case '*':
      dat->type = aprsdecode_WETH;
      break;
   case '_':
      dat->type = aprsdecode_PWETH;
      compos = p; /* positionless wx */
      dat->sym = '_';
      dat->symt = '/';
      break;
   case ':':
      dat->type = aprsdecode_MSG;
      break;
   case ';':
      dat->type = aprsdecode_OBJ;
      break;
   case ')':
      dat->type = aprsdecode_ITEM;
      break;
   case '<':
      dat->type = aprsdecode_SCAP;
      break;
   case '>':
      dat->type = aprsdecode_STAT;
      break;
   case 'T':
      dat->type = aprsdecode_TELE;
      break;
   case '?':
      dat->type = aprsdecode_QUERY;
      break;
   default:;
      dat->type = aprsdecode_UNKNOWN;
      break;
   } /* end switch */
   ++p;
   if (dat->type==aprsdecode_OBJ) {
      /* object */
      if (buf[p+9UL]=='_' || buf[p+9UL]=='*') {
         memcpy(dat->objectfrom,dat->srccall,9u);
         i = 0UL;
         while (i<=8UL) {
            if (p+i>=len) return -1L;
            dat->srccall[i] = buf[p+i];
            ++i;
         }
         if (buf[p+i]=='_') dat->objkill = '1';
         else if (buf[p+i]!='*') dat->type = aprsdecode_UNKNOWN;
         dat->timestamp = buf[p+16UL];
         while (i>0UL && (unsigned char)dat->srccall[i-1UL]<=' ') {
            dat->srccall[i-1UL] = 0;
            --i;
         }
      }
      else dat->type = aprsdecode_UNKNOWN;
   }
   else if (dat->typc==')') {
      /* item */
      memcpy(dat->objectfrom,dat->srccall,9u);
      i = 0UL;
      while (buf[p+i]!='!' && buf[p+i]!='_') {
         if (p+i>=len) return -1L;
         if (i<=8UL) dat->srccall[i] = buf[p+i];
         ++i;
      }
      if (i<=8UL) dat->srccall[i] = 0;
      if (buf[p+i]=='_') dat->objkill = '1';
   }
   else if (dat->typc==':') {
      /* msg, bulletin */
      if (buf[p+9UL]==':') {
         i = 0UL;
         while (i<9UL && p<len) {
            if (i<=8UL && (unsigned char)buf[p]>' ') dat->msgto[i] = buf[p];
            ++i;
            ++p;
         }
         if (X2C_STRCMP(dat->msgto,9u,dat->srccall,9u)==0) {
            dat->type = aprsdecode_TELE; /* msg to itself */
         }
         ++p;
         i = 0UL;
         if ((buf[p]=='r' && buf[p+1UL]=='e') && buf[p+2UL]=='j') {
            dat->ackrej = aprsdecode_MSGREJ; /* reject */
         }
         else if ((buf[p]=='a' && buf[p+1UL]=='c') && buf[p+2UL]=='k') {
            dat->ackrej = aprsdecode_MSGACK; /* ack */
         }
         if (dat->ackrej>aprsdecode_MSGMSG) {
            p += 3UL;
            while ((i<=4UL && (unsigned char)buf[p]>' ') && p<len) {
               dat->acktext[i] = buf[p];
               ++i;
               ++p;
            }
         }
         else {
            while ((i<=66UL && buf[p]!='{') && p<len) {
               /* message text */
               dat->msgtext[i] = buf[p];
               ++i;
               ++p;
            }
            dat->ackrej = aprsdecode_MSGMSG;
            if (buf[p]=='{') {
               /* message ack */
               i = 0UL;
               ++p;
               while ((i<=4UL && (unsigned char)buf[p]>' ') && p<len) {
                  dat->acktext[i] = buf[p];
                  ++i;
                  ++p;
               }
            }
         }
      }
   }
   else if (dat->typc=='?') {
      /* general query */
      i = 0UL;
      while ((i<9UL && (unsigned char)buf[p]>' ') && p<len) {
         if (i<=8UL) dat->msgto[i] = buf[p];
         ++i;
         ++p;
      }
   }
   dat->course = X2C_max_longcard;
   if (X2C_IN((long)dat->type,12,0xCEU)) {
      aprspos_GetPos(&dat->pos, &dat->speed, &dat->course, &dat->altitude,
                &dat->sym, &dat->symt, buf, buf_len, micedest, payload,
                &compos, &dat->postyp);
      aprspos_GetSym(dat->dstcall, 9ul, &dat->sym, &dat->symt);
      if (aprsdecode_checksymb(dat->symt, dat->sym)) {
         dat->symt = 0; /* wrong symbol */
         dat->sym = 0;
      }
      if (compos==0UL) compos = payload;
      GetHRT(&dat->pos, &dat->altitude, &dat->speed, buf, buf_len, &compos,
                dat->hrtposes, 32ul, &dat->hrtlen, &dat->hrttime);
   }
   if (X2C_IN((long)dat->type,12,
                0x4CU) && dat->sym=='_' || dat->type==aprsdecode_PWETH) {
      GetWX(&dat->wx, &dat->course, &dat->speed, buf, buf_len, &compos);
   }
   i = 0UL;
   if (compos) p = compos;
   else p = payload+(unsigned long)(dat->type!=aprsdecode_UNKNOWN);
   while (p<len && i<=255UL) {
      dat->comment0[i] = buf[p];
      ++p;
      ++i;
   }
   return 0L;
} /* end Decode() */


static char getbeaconparm(char s[], unsigned long s_len,
                unsigned long * time0, char * port)
/* time:port:rawframe */
{
   unsigned long i;
   *port = s[0UL];
   if ((unsigned char)*port<' ' || s[1UL]!=':') return 0;
   *time0 = 0UL;
   i = 2UL;
   while ((unsigned char)s[i]>='0' && (unsigned char)s[i]<='9') {
      *time0 = ( *time0*10UL+(unsigned long)(unsigned char)s[i])-48UL;
      ++i;
   }
   if (s[i]!=':') return 0;
   aprsstr_Delstr(s, s_len, 0UL, i+1UL);
   return 1;
} /* end getbeaconparm() */


extern void aprsdecode_getbeaconname(char raw[], unsigned long raw_len,
                char name[], unsigned long name_len, char symb[],
                unsigned long symb_len, char * isobj, char * isdel,
                char * isbad)
/* item object else src name */
{
   struct aprsdecode_DAT dat;
   char port;
   unsigned long time0;
   X2C_PCOPY((void **)&raw,raw_len);
   *isobj = 0;
   *isdel = 0;
   *isbad = 1;
   name[0UL] = 0;
   if (getbeaconparm(raw, raw_len, &time0, &port)) {
      beaconmacros(raw, raw_len, "", 1ul, "", 1ul, 0);
      if (aprsdecode_Decode(raw, raw_len, &dat)==0L) {
         aprsstr_Assign(name, name_len, dat.srccall, 9ul);
         aprsstr_Assign(symb, symb_len, (char *) &dat.symt, 1u/1u);
         aprsstr_Append(symb, symb_len, (char *) &dat.sym, 1u/1u);
         *isobj = dat.type==aprsdecode_OBJ || dat.type==aprsdecode_ITEM;
         *isdel = dat.objkill=='1';
         *isbad = (unsigned char)dat.symt<' ' || (unsigned char)
                name[0UL]<=' ';
      }
      else *isbad = 1;
   }
   X2C_PFREE(raw);
} /* end getbeaconname() */


extern void aprsdecode_extractbeacon(char raw[], unsigned long raw_len,
                char withparm, char macros)
/* monitor line to beacon config */
{
   struct aprsdecode_DAT dat;
   char port;
   unsigned long i;
   unsigned long time0;
   long j;
   char s[1000];
   X2C_PCOPY((void **)&raw,raw_len);
   if (withparm) {
      if (getbeaconparm(raw, raw_len, &time0, &port)) {
         if (macros) beaconmacros(raw, raw_len, "", 1ul, "", 1ul, 0);
         aprsstr_IntToStr((long)time0, 0UL, s, 1000ul);
         useri_AddConfLine(useri_fRBTIME, 1U, s, 1000ul);
         useri_AddConfLine(useri_fRBPORT, 1U, (char *) &port, 1u/1u);
      }
      else useri_encerr("beacon syntax wrong, delete line!", 34ul);
   }
   if (aprsdecode_Decode(raw, raw_len, &dat)==0L) {
      if (dat.type==aprsdecode_OBJ || dat.type==aprsdecode_ITEM) {
         aprsstr_Assign(s, 1000ul, dat.srccall, 9ul);
      }
      else useri_confstr(useri_fMYCALL, s, 1000ul);
      useri_AddConfLine(useri_fRBNAME, 1U, s, 1000ul);
      if (aprspos_posvalid(dat.pos)) {
         aprstext_postostr(dat.pos, '3', s, 1000ul);
         useri_AddConfLine(useri_fRBPOS, 1U, s, 1000ul);
      }
      useri_AddConfLine(useri_fRBCOMMENT, 1U, dat.comment0, 256ul);
      s[0U] = dat.symt;
      s[1U] = dat.sym;
      s[2U] = 0;
      useri_AddConfLine(useri_fRBSYMB, 1U, s, 1000ul);
      useri_AddConfLine(useri_fRBSPEED, 1U, "", 1ul);
      useri_AddConfLine(useri_fRBDIR, 1U, "", 1ul);
      if (dat.speed>0UL && dat.speed<2147483647UL) {
         aprsstr_IntToStr((long)aprsdecode_trunc((float)dat.speed*1.852f),
                1UL, s, 1000ul);
         useri_AddConfLine(useri_fRBSPEED, 1U, s, 1000ul);
         if (dat.course<360UL) {
            aprsstr_IntToStr((long)dat.course, 1UL, s, 1000ul);
            useri_AddConfLine(useri_fRBDIR, 1U, s, 1000ul);
         }
      }
      if (dat.altitude<X2C_max_longint) {
         aprsstr_IntToStr(dat.altitude, 1UL, s, 1000ul);
      }
      else s[0U] = 0;
      useri_AddConfLine(useri_fRBALT, 1U, s, 1000ul);
      s[1U] = 0;
      s[0U] = 'b';
      if (dat.type==aprsdecode_OBJ) {
         if (dat.timestamp=='h') s[0U] = 'H';
         else s[0U] = 'O';
         if (dat.objkill=='1') s[0U] = 'P';
      }
      else if (dat.type==aprsdecode_ITEM) {
         if (dat.objkill=='1') s[0U] = 'J';
         else s[0U] = 'I';
      }
      useri_AddConfLine(useri_fRBTYP, 1U, s, 1000ul);
      useri_AddConfLine(useri_fRBPOSTYP, 1U, (char *) &dat.postyp, 1u/1u);
      if (withparm) {
         s[0U] = 0;
         j = aprsstr_InStr(dat.dstcall, 9ul, "-", 2ul);
                /* move dest ssid to viapath */
         if (j>=0L) {
            aprsstr_Assign(s, 1000ul, dat.dstcall, 9ul);
            aprsstr_Delstr(s, 1000ul, 0UL, (unsigned long)j);
            dat.dstcall[j] = 0;
         }
         if (dat.type==aprsdecode_MICE) strncpy(dat.dstcall,"APLM01",9u);
         useri_AddConfLine(useri_fRBDEST, 1U, dat.dstcall, 9ul);
         useri_AddConfLine(useri_fRBNAME, 1U, dat.srccall, 9ul);
         i = 0UL;
         while (i<=9UL && dat.viacalls[i][0UL]) {
            /* make via path */
            if (s[0U]) aprsstr_Append(s, 1000ul, ",", 2ul);
            aprsstr_Append(s, 1000ul, dat.viacalls[i], 9ul);
            ++i;
         }
         useri_AddConfLine(useri_fRBPATH, 1U, s, 1000ul);
      }
      else {
         useri_AddConfLine(useri_fRBDEST, 1U, "APLM01", 7ul);
      }
   }
   X2C_PFREE(raw);
} /* end extractbeacon() */


extern void aprsdecode_importbeacon(aprsdecode_pOPHIST op)
/* op last pos frame to beacon config */
{
   aprsdecode_pFRAMEHIST pl;
   aprsdecode_pFRAMEHIST pf;
   struct aprsdecode_DAT dat;
   if (useri_beaconediting && op) {
      pl = 0;
      pf = op->frames;
      while (pf) {
         if (aprspos_posvalid(pf->vardat->pos)) pl = pf;
         pf = pf->next;
      }
      if ((pl && pl->vardat) && aprsdecode_Decode(pl->vardat->raw, 500ul,
                &dat)==0L) {
         if (X2C_STRCMP(useri_beaconimported,9u,dat.srccall,9u)==0) {
            aprsdecode_extractbeacon(pl->vardat->raw, 500ul, 0, 0);
            useri_beaconed = 1;
            useri_beaconimported[0UL] = '\012';
         }
         else memcpy(useri_beaconimported,dat.srccall,9u);
      }
   }
} /* end importbeacon() */


static void beepmsg(char isack)
{
   if (useri_configon(useri_fBEEPMSG)) {
      aprsdecode_beeplim(100L, (unsigned long)useri_conf2int(useri_fBEEPMSG,
                (unsigned long)isack*2UL, 20L, 8000L, 1000L),
                (unsigned long)useri_conf2int(useri_fBEEPMSG,
                (unsigned long)isack*2UL+1UL, 0L, 5000L, 250L));
   }
} /* end beepmsg() */


static void popupmessage(const char from[], unsigned long from_len,
                const char to[], unsigned long to_len, const char txt[],
                unsigned long txt_len, const char ack[],
                unsigned long ack_len, unsigned long time0, char port,
                char isquery0)
{
   aprsdecode_pMSGFIFO pl;
   aprsdecode_pMSGFIFO pm;
   long cnt;
   Storage_ALLOCATE((X2C_ADDRESS *) &pm, sizeof(struct aprsdecode_MSGFIFO));
   if (pm==0) {
      osi_WrStrLn("msg out of memory", 18ul);
      return;
   }
   aprsstr_Assign(pm->from, 9ul, from, from_len);
   aprsstr_Assign(pm->to, 9ul, to, to_len);
   aprsstr_Assign(pm->txt, 67ul, txt, txt_len);
   aprsstr_Assign(pm->ack, 5ul, ack, ack_len);
   pm->time0 = time0;
   pm->port = port;
   pm->query = isquery0;
   pm->next = 0;
   cnt = 1L;
   if (aprsdecode_msgfifo0==0) aprsdecode_msgfifo0 = pm;
   else {
      cnt = 2L;
      pl = aprsdecode_msgfifo0;
      while (pl->next) {
         pl = pl->next;
         ++cnt;
      }
      pl->next = pm;
   }
   /*WrInt(nextmsg, 10); WrStrLn("nm"); */
   if (useri_nextmsg==0L) {
      if (useri_configon(useri_fPOPUPMSG)) useri_nextmsg = -cnt;
   }
   else useri_nextmsg = -labs(useri_nextmsg);
} /* end popupmessage() */


extern void aprsdecode_getactack(aprsdecode_MONCALL call, char ack[],
                unsigned long ack_len)
/* get actual message ack */
{
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST pf;
   struct aprsdecode_DAT dat;
   char mid[2];
   char aa[2];
   char ok0;
   aprsdecode_MONCALL my;
   ack[0UL] = 0;
   op = findop(call, 0);
   if (op) {
      useri_confstr(useri_fMYCALL, my, 9ul);
      pf = op->frames;
      while (pf) {
         if (((aprsdecode_Decode(pf->vardat->raw, 500ul,
                &dat)>=0L && dat.type==aprsdecode_MSG)
                && dat.ackrej==aprsdecode_MSGMSG) && X2C_STRCMP(dat.msgto,9u,
                my,9u)==0) {
            ok0 = replyack(aa, 2ul, mid, 2ul, dat.acktext, 5ul);
            if (mid[0U]) aprsstr_Assign(ack, ack_len, mid, 2ul);
         }
         pf = pf->next;
      }
   }
} /* end getactack() */


static char ch(unsigned long n)
{
   if (n<10UL) n += 48UL;
   else if (n<36UL) n += 55UL;
   else n += 61UL;
   return (char)n;
} /* end ch() */


extern void aprsdecode_acknumstr(char aa[], unsigned long aa_len,
                unsigned long n)
{
   aa[0UL] = ch((n/62UL)%62UL);
   aa[1UL] = ch(n%62UL);
   if (aa_len-1>=2UL) aa[2UL] = 0;
} /* end acknumstr() */


static char sendtxmsg(unsigned long acknum, const aprsdecode_MONCALL to,
                const char txt[], unsigned long txt_len, char port,
                char rfonly, unsigned long ackcnt, char errm[],
                unsigned long errm_len)
{
   unsigned long i;
   aprsdecode_FRAMEBUF s;
   aprsdecode_FRAMEBUF rfs;
   char am[51];
   char h1[51];
   char h[51];
   char tmp;
   if (ackcnt==0UL) strncpy(am,"Ack ",51u);
   else strncpy(am,"Msg ",51u);
   errm[0UL] = 0;
   if (to[0UL]==0) return 0;
   useri_confstr(useri_fMYCALL, rfs, 512ul);
   if (aprsstr_Length(rfs, 512ul)<3UL) {
      aprsstr_Assign(errm, errm_len, "Msg not sent (no valid Mycall)", 31ul);
      return 0;
   }
   aprsstr_Append(rfs, 512ul, ">", 2ul);
   if (rfonly) useri_confstr(useri_fMSGRFDEST, h, 51ul);
   else useri_confstr(useri_fMSGNETDEST, h, 51ul);
   aprsstr_Append(rfs, 512ul, h, 51ul);
   if (h[0U]==0) {
      aprsstr_Assign(errm, errm_len, am, 51ul);
      aprsstr_Append(errm, errm_len, "not sent (no Port set)", 23ul);
      return 0;
   }
   if (port=='A' || (unsigned char)port>='1' && (unsigned char)port<='9') {
      /* radio so add via path */
      useri_confstr(useri_fMSGPATH, h, 51ul);
      if ((unsigned char)h[0U]>='1' && (unsigned char)h[0U]<='7') {
         aprsstr_Append(rfs, 512ul, "-", 2ul);
         aprsstr_Append(rfs, 512ul, (char *) &h[0U], 1u/1u);
      }
      else if (h[0U]) {
         aprsstr_Append(rfs, 512ul, ",", 2ul);
         aprsstr_Append(rfs, 512ul, h, 51ul);
      }
   }
   aprsstr_Append(rfs, 512ul, ":", 2ul);
   strncpy(s,":",512u);
   aprsstr_Append(s, 512ul, to, 9ul);
   for (i = aprsstr_Length(to, 9ul); i<=8UL; i++) {
      aprsstr_Append(s, 512ul, " ", 2ul);
   } /* end for */
   aprsstr_Append(s, 512ul, ":", 2ul);
   if (ackcnt==0UL) aprsstr_Append(s, 512ul, "ack", 4ul);
   aprsstr_Append(s, 512ul, txt, txt_len);
   if (acknum>0UL) {
      aprsstr_Append(s, 512ul, "{", 2ul);
      aprsdecode_acknumstr(h, 51ul, acknum);
      aprsstr_Append(s, 512ul, h, 51ul);
      aprsstr_Append(s, 512ul, "}", 2ul);
      aprsdecode_getactack(to, h, 51ul);
      if (h[0U]) aprsstr_Append(s, 512ul, h, 51ul);
   }
   /*WrInt(ORD(rfonly), 10);WrStrLn(port); */
   aprsstr_Append(rfs, 512ul, s, 512ul);
   for (i = 0UL; i<=3UL; i++) {
      /* try the rf ports */
      if ((useri_configon((unsigned char)(35UL+i))
                && (port=='A' || port==(char)(i+49UL))) && Sendudp(rfs,
                512ul, i)) {
         memcpy(h,am,51u);
         if (ackcnt==1UL) aprsstr_Append(h, 51ul, "sent", 5ul);
         else {
            aprsstr_Append(h, 51ul, "resent(", 8ul);
            aprsstr_IntToStr((long)ackcnt, 0UL, h1, 51ul);
            aprsstr_Append(h, 51ul, h1, 51ul);
            aprsstr_Append(h, 51ul, ")", 2ul);
         }
         aprsstr_Append(h, 51ul, " to ", 5ul);
         aprsstr_Append(h, 51ul, to, 9ul);
         aprsstr_Append(h, 51ul, " on Port ", 10ul);
         aprsstr_Append(h, 51ul, (char *)(tmp = (char)(i+49UL),&tmp), 1u/1u);
         useri_textautosize(0L, 0L, 4UL, 2UL, 'b', h, 51ul);
         return 1;
      }
   } /* end for */
   /* make it better */
   if (!rfonly && port=='N') {
      /*
          IF configon(fCONNECT) & configon(fALLOWNETTX) THEN 
            IF SendNet(tcpsocks, s, TRUE) THEN
              errm[0]:=0C;
              h:=am; Append(h, "(re)sent to "); Append(h, to);
                Append(h, " on Net");
              textautosize(0, 0, 4, 2, "b", h);
              RETURN TRUE
      
            ELSE Assign(errm, am); Append(errm, "not sent");
                RETURN FALSE END;
      
          ELSE Assign(errm, am); Append(errm, "not sent (Netport not configured for tx)");
                 RETURN FALSE END;
      */
      SendNet(aprsdecode_tcpsocks, s, 512ul, 1, errm, errm_len);
      if (errm[0UL]) {
         memcpy(h,am,51u);
         aprsstr_Append(h, 51ul, "not sent ", 10ul);
         aprsstr_Append(h, 51ul, errm, errm_len);
         aprsstr_Assign(errm, errm_len, h, 51ul);
      }
      else {
         memcpy(h,am,51u);
         aprsstr_Append(h, 51ul, "(re)sent to ", 13ul);
         aprsstr_Append(h, 51ul, to, 9ul);
         aprsstr_Append(h, 51ul, " on Net", 8ul);
         useri_textautosize(0L, 0L, 4UL, 2UL, 'b', h, 51ul);
      }
      return 1;
   }
   else {
      /*    ELSE (*Assign(errm, am); Append(errm, "not sent");
                *) RETURN FALSE END; */
      aprsstr_Assign(errm, errm_len, am, 51ul);
      aprsstr_Append(errm, errm_len, "not sent (delete Msg or open outgoing P\
ort)", 44ul);
      return 0;
   }
   return 0;
} /* end sendtxmsg() */


static void ackmsg(const char from[], unsigned long from_len,
                const char rep[], unsigned long rep_len, unsigned char rej)
{
   aprsdecode_pTXMESSAGE pm;
   char bb[2];
   char aa[2];
   /*
     WrStr("replyack:");
     WrStr(from);
     WrStr("|");
     WrStr(rep);
     WrStr("|");
     WrLn;
   */
   if (rej==aprsdecode_MSGMSG) {
      if (!replyack(bb, 2ul, aa, 2ul, rep, rep_len)) return;
   }
   else {
      /* message only */
      aprsstr_Assign(bb, 2ul, rep, rep_len);
   }
   pm = aprsdecode_txmessages;
   while (pm) {
      if (pm->acktime==0UL && X2C_STRCMP(from,from_len,pm->to,9u)==0) {
         aprsdecode_acknumstr(aa, 2ul, pm->acknum);
         if (X2C_STRCMP(aa,2u,bb,2u)==0) {
            /* message acked now */
            pm->acktime = aprsdecode_realtime;
            pm->rej = rej==aprsdecode_MSGREJ;
            useri_refresh = 1;
            /*WrInt(ORD(rej), 10); WrStrLn("=rej"); */
            beepmsg(1);
         }
      }
      pm = pm->next;
   }
} /* end ackmsg() */

#define aprsdecode_QUERYRESPONSEWINDOW 30
/* time after sending a msg to display already received query response */


static void showmsg(const char from[], unsigned long from_len,
                const char to[], unsigned long to_len, const char txt[],
                unsigned long txt_len, const char ack[],
                unsigned long ack_len, unsigned char rej, char my,
                char query, char port)
{
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST opf;
   aprsdecode_ACKTEXT aack;
   aprsdecode_ACKTEXT rep;
   aprsdecode_MONCALL call;
   struct aprsdecode_DAT dat;
   char s[81];
   /*
     WrStr(from);
     WrStr(">");
     WrStr(to);
     WrStr("|");
     WrStr(txt);
     WrStr("|");
     WrStr(ack);
     WrLn;
   */
   opf = 0;
   /*WrStr(ack);WrStrLn("<a1"); */
   if (ack[0UL]) {
      /*WrStr(ack);WrStrLn("<a2"); */
      aprsstr_Assign(call, 9ul, from, from_len);
      op = findop(call, 1); /* find sender of msg */
      if (op) {
         opf = op->frames;
         for (;;) {
            if (opf==0) break;
            if ((((aprsdecode_Decode(opf->vardat->raw, 500ul,
                &dat)>=0L && dat.type==aprsdecode_MSG)
                && dat.ackrej==aprsdecode_MSGMSG) && X2C_STRCMP(dat.msgto,9u,
                to,to_len)==0) && X2C_STRCMP(txt,txt_len,dat.msgtext,
                67u)==0) {
               if (ack[0UL]==0) {
                  if (lastmsgtx+30UL<aprsdecode_realtime) {
                     break; /* we do not wait for a query response */
                  }
               }
               else if (X2C_STRCMP(dat.acktext,5u,ack,
                ack_len)==0 || replyack(rep, 5ul, aack, 5ul, dat.acktext,
                5ul) && X2C_STRCMP(aack,5u,ack,ack_len)==0) {
                  /*            OR (opf^.time+dupetime>=realtime)
                THEN EXIT END;               (* dupe of a response *) */
                  break; /* we have this msg got before */
               }
            }
            opf = opf->next;
         }
      }
   }
   if (opf==0) {
      if (my || useri_configon(useri_fMSGALLSSID) && (!aprsstr_StrCmp(from,
                from_len, to, to_len) || useri_configon(useri_fPASSSELFMSG))) {
         if (!useri_configon(useri_fPOPUPMSG)) {
            if (query) strncpy(s,"Query from ",81u);
            else strncpy(s,"Message from ",81u);
            aprsstr_Append(s, 81ul, from, from_len);
            aprsstr_Append(s, 81ul, " to ", 5ul);
            aprsstr_Append(s, 81ul, to, to_len);
            aprsstr_Append(s, 81ul, " (click on M Button)", 21ul);
            useri_textautosize(0L, 5L, 6UL, 0UL, 'y', s, 81ul);
         }
         popupmessage(from, from_len, to, to_len, txt, txt_len, ack, ack_len,
                 aprsdecode_realtime, port, query);
         ++aprsdecode_tracenew.winevent;
         beepmsg(0);
      }
   }
   else if (aprsdecode_verb) {
      InOut_WriteString("seen msg:", 10ul);
      InOut_WriteString(from, from_len);
      InOut_WriteString(">", 2ul);
      InOut_WriteString(to, to_len);
      InOut_WriteString("|", 2ul);
      InOut_WriteString(txt, txt_len);
      InOut_WriteString("|", 2ul);
      InOut_WriteString(ack, ack_len);
      InOut_WriteString("|", 2ul);
      InOut_WriteString((char *) &port, 1u/1u);
      osi_WrLn();
   }
} /* end showmsg() */


static char isquery(const struct aprsdecode_DAT dat, char port)
{
   unsigned long j;
   unsigned long i;
   char err[201];
   char wdata[201];
   char key[201];
   char path[201];
   char s[201];
   /*  IF dat.msgtext[0]<>"?" THEN RETURN FALSE END; */
   aprsstr_Assign(path, 201ul, dat.srccall, 9ul);
   aprsstr_Append(path, 201ul, ">", 2ul);
   aprsstr_Append(path, 201ul, dat.dstcall, 9ul);
   i = 0UL;
   while (dat.viacalls[i][0UL]) {
      aprsstr_Append(path, 201ul, ",", 2ul);
      aprsstr_Append(path, 201ul, dat.viacalls[i], 9ul);
      if (i+2UL==dat.hbitp) aprsstr_Append(path, 201ul, "*", 2ul);
      ++i;
   }
   i = 0UL;
   for (;;) {
      useri_confstrings(useri_fQUERYS, i, 0, s, 201ul);
      if (s[0U]==0) break;
      j = (unsigned long)aprsstr_InStr(s, 201ul, ":", 2ul);
      if (j>0UL) {
         aprsstr_Assign(key, 201ul, s, 201ul);
         key[j] = 0;
         aprsstr_Delstr(s, 201ul, 0UL, j+1UL);
         if (aprsstr_InStr(dat.msgtext, 67ul, key, 201ul)==0L) {
            aprsstr_Assign(wdata, 201ul, dat.msgtext, 67ul);
            aprsstr_Delstr(wdata, 201ul, 0UL, aprsstr_Length(key, 201ul));
            beaconmacros(s, 201ul, path, 201ul, wdata, 201ul, 1);
            if (s[0U]==0) return 1;
            if (!sendtxmsg(0UL, dat.srccall, s, 201ul, port, port!='N', 1UL,
                err, 201ul)) {
               useri_xerrmsg(err, 201ul);
               return 0;
            }
            else {
               useri_xerrmsg(err, 201ul);
               return 1;
            }
         }
      }
      ++i;
   }
   /*WrStrLn(port);WrStrLn(dat.srccall);WrStrLn(dat.msgtext); */
   return 0;
} /* end isquery() */


static void getmessage(aprsdecode_pTCPSOCK cp, unsigned long up,
                const struct aprsdecode_DAT dat)
/* if first: popup, if full match & lastack>10s: send ack */
{
   unsigned long i;
   char void0;
   char query;
   char fit;
   char h[100];
   char port;
   char ch0;
   aprsdecode_MONCALL my;
   useri_confstr(useri_fMYCALL, my, 9ul);
   if (aprsstr_Length(my, 9ul)<3UL) return;
   aprsstr_Assign(h, 100ul, dat.msgto, 9ul);
   fit = 0;
   i = 0UL;
   while (h[i]==my[i] && h[i]) {
      if (h[i]=='-' || my[i]=='-') fit = 1;
      ++i;
   }
   if (i==0UL) return;
   ch0 = my[i];
   if (h[i]==0 && ch0=='-' || h[i]=='-' && ch0==0) fit = 1;
   if (cp) port = 'N';
   else port = (char)(up+48UL);
   if (ch0==0 && h[i]==0) {
      /*    WrStr("my message "); */
      if (dat.ackrej==aprsdecode_MSGMSG) {
         /*FOR i:=0 TO HIGH(dat.acktext) DO WrInt(ORD(dat.acktext[i]), 4);
                END; WrStrLn("=ack"); */
         query = isquery(dat, port);
         showmsg(dat.srccall, 9ul, dat.msgto, 9ul, dat.msgtext, 67ul,
                dat.acktext, 5ul, dat.ackrej, 1, query, port);
         if (!query && dat.acktext[0UL]) {
            void0 = sendtxmsg(0UL, dat.srccall, dat.acktext, 5ul, port,
                port!='N', 0UL, h, 100ul);
            useri_xerrmsg(h, 100ul);
         }
      }
      if (dat.acktext[0UL]) {
         ackmsg(dat.srccall, 9ul, dat.acktext, 5ul, dat.ackrej);
      }
   }
   else if (fit && dat.ackrej==aprsdecode_MSGMSG) {
      /*  WrStr("other ssid message "); */
      showmsg(dat.srccall, 9ul, dat.msgto, 9ul, dat.msgtext, 67ul,
                dat.acktext, 5ul, dat.ackrej, 0, 0, port);
   }
} /* end getmessage() */

#define aprsdecode_SETPORT 2

#define aprsdecode_DEL 6

#define aprsdecode_KICK 5

#define aprsdecode_RESTART 4

#define aprsdecode_HOLD 3

#define aprsdecode_EDIT 1


extern void aprsdecode_deltxmsg(unsigned long cmd, unsigned long n)
/* delete/edit tx message number */
{
   aprsdecode_pTXMESSAGE po;
   aprsdecode_pTXMESSAGE pm;
   pm = aprsdecode_txmessages;
   if (pm) {
      if (cmd==6UL || cmd==1UL) {
         if (n==0UL) aprsdecode_txmessages = aprsdecode_txmessages->next;
         else {
            do {
               po = pm;
               pm = pm->next;
               --n;
            } while (!(n==0UL || pm==0));
            if (pm) po->next = pm->next;
         }
         if (pm) {
            if (cmd==1UL) {
               useri_AddConfLine(useri_fMSGTEXT, 0U, pm->msgtext, 67ul);
               useri_AddConfLine(useri_fMSGTO, 0U, pm->to, 9ul);
               useri_AddConfLine(useri_fMSGPORT, 0U, (char *) &pm->port,
                1u/1u);
            }
            Storage_DEALLOCATE((X2C_ADDRESS *) &pm,
                sizeof(struct aprsdecode_TXMESSAGE));
         }
      }
      else {
         while (n>0UL && pm) {
            pm = pm->next;
            --n;
         }
         if (pm && pm->acktime==0UL) {
            if (cmd==5UL) pm->txtime = 0UL;
            else if (cmd==4UL) {
               pm->txtime = 0UL;
               pm->acktime = 0UL;
               pm->txcnt = 0UL;
               pm->heard = 0;
            }
            else if (cmd==3UL) pm->txtime = X2C_max_longcard;
            else if (cmd==2UL) {
               useri_confstr(useri_fMSGPORT, (char *) &pm->port, 1u/1u);
            }
         }
      }
   }
} /* end deltxmsg() */


extern void aprsdecode_makemsg(char ack)
{
   aprsdecode_pTXMESSAGE pn;
   aprsdecode_pTXMESSAGE pm;
   char po;
   char ht[101];
   char hm[101];
   char mes[101];
   long i;
   char c;
   struct aprsdecode_TXMESSAGE * anonym;
   long tmp;
   mes[0U] = 0;
   useri_confstr(useri_fMSGPORT, (char *) &po, 1u/1u);
   if (po==' ' || X2C_CAP(po)=='A') po = 'A';
   else if (X2C_CAP(po)=='N' || X2C_CAP(po)=='I') po = 'N';
   else if ((unsigned char)po<'0' || (unsigned char)po>'9') {
      strncpy(mes,"need Port where to send",101u);
   }
   useri_confstr(useri_fMSGTO, ht, 101ul); /* no destination call */
   if (ht[0U]==0) strncpy(mes,"need call to send to",101u);
   useri_confstr(useri_fMSGTEXT, hm, 101ul);
   if (hm[0U]==0) {
      /* empty message line */
      if (!sentemptymsg) {
         strncpy(mes,"send empty line? if so, enter again",101u);
         sentemptymsg = 1;
      }
      else sentemptymsg = 0;
   }
   else sentemptymsg = 0;
   pn = aprsdecode_txmessages; /* look for same msg pending */
   while (pn) {
      if (((((ack && pn->acknum>0UL) && pn->acktime==0UL)
                && aprsstr_StrCmp(pn->to, 9ul, ht,
                101ul)) && aprsstr_StrCmp(pn->msgtext, 67ul, hm,
                101ul)) && pn->port==po) {
         strncpy(mes,"same message pending, try [kick] or delete",101u);
      }
      pn = pn->next;
   }
   if (mes[0U]==0) {
      Storage_ALLOCATE((X2C_ADDRESS *) &pm,
                sizeof(struct aprsdecode_TXMESSAGE));
      if (pm==0) Err("out of memory", 14ul);
      memset((X2C_ADDRESS)pm,(char)0,sizeof(struct aprsdecode_TXMESSAGE));
      { /* with */
         struct aprsdecode_TXMESSAGE * anonym = pm;
         anonym->port = po;
         useri_confstr(useri_fMSGTO, anonym->to, 9ul);
         aprsstr_Caps(anonym->to, 9ul);
         useri_confstr(useri_fMSGTEXT, anonym->msgtext, 67ul);
         tmp = (long)aprsstr_Length(anonym->msgtext, 67ul)-1L;
         i = 0L;
         if (i<=tmp) for (;; i++) {
            c = anonym->msgtext[i];
            if ((c=='|' || c=='~') || c=='{') anonym->msgtext[i] = ' ';
            if (i==tmp) break;
         } /* end for */
         if (ack) {
            if (msgmid==0UL) {
               msgmid = (aprsdecode_realtime%68400UL)/30UL;
                /* init msg number with 30s daytime */
            }
            msgmid = (msgmid+1UL)%3843UL;
            anonym->acknum = msgmid+1UL; /* 0 is query with no ack */
         }
      }
      pn = aprsdecode_txmessages;
      if (pn==0) aprsdecode_txmessages = pm;
      else {
         while (pn->next) pn = pn->next;
         pn->next = pm;
      }
      useri_clrmsgtext("", 1ul);
      useri_textautosize(0L, 5L, 6UL, 5UL, 'b', "Msg Stored", 11ul);
   }
   useri_xerrmsg(mes, 101ul);
} /* end makemsg() */

#define aprsdecode_MAXMSGTXTIME 3600

#define aprsdecode_MAXMSGRETRY 20

#define aprsdecode_MAXNOACKRETRY 1

#define aprsdecode_MSGNOTXTIME 10

#define aprsdecode_MSGTXTIME 15

/*    my:MONCALL; */

static void sendmsg(void)
{
   aprsdecode_pTXMESSAGE pn;
   aprsdecode_pTXMESSAGE pm;
   unsigned long t;
   unsigned long maxret;
   unsigned long tc;
   char port;
   char sent;
   char err[101];
   if (lastmsgtx>aprsdecode_realtime) lastmsgtx = aprsdecode_realtime;
   /*  confstr(fMYCALL, my); */
   /*  IF (lastmsgtx+MSGNOTXTIME<realtime) & (Length(my)>=3) THEN */
   if (lastmsgtx+10UL<aprsdecode_realtime) {
      err[0U] = 0;
      pm = aprsdecode_txmessages;
      for (;;) {
         if (pm==0) break;
         if (pm->acknum>0UL) maxret = 20UL;
         else maxret = 1UL;
         if ((pm->acktime==0UL && pm->txtime!=X2C_max_longcard)
                && (pm->txcnt<maxret || pm->txtime==0UL)) {
            tc = pm->txcnt;
            t = 15UL;
            for (;;) {
               if (tc<=1UL) break;
               t += t;
               --tc;
               if (t>3600UL) {
                  t = 3600UL;
                  break;
               }
            }
            if (pm->txtime+t<=aprsdecode_realtime) {
               pn = aprsdecode_txmessages;
               while (pn!=pm && ((((pn->acknum==0UL || pn->acktime>0UL)
                || pn->txcnt>=20UL) || !aprsstr_StrCmp(pn->to, 9ul, pm->to,
                9ul)) || pn->txtime==X2C_max_longcard)) pn = pn->next;
               if (pn==pm) {
                  /* not older unacked msg to this call */
                  port = pm->port;
                  sent = 0;
                  if ((pm->txcnt<=(unsigned long)
                pm->heard || pm->txtime==0UL) && (port=='N' || port=='A')) {
                     if (sendtxmsg(pm->acknum, pm->to, pm->msgtext, 67ul,
                'N', 0, pm->txcnt+1UL, err, 101ul) && err[0U]==0) sent = 1;
                  }
                  if (port=='A' || (unsigned char)
                port>='1' && (unsigned char)port<='9') {
                     if (sendtxmsg(pm->acknum, pm->to, pm->msgtext, 67ul,
                port, sent || port!='A', pm->txcnt+1UL, err,
                101ul) && err[0U]==0) sent = 1;
                     else if (sent || pm->txcnt>0UL) err[0U] = 0;
                  }
                  if (sent) {
                     ++pm->txcnt;
                     pm->txtime = aprsdecode_realtime;
                     lastmsgtx = aprsdecode_realtime;
                     useri_refresh = 1;
                     break; /* send 1 msg a time */
                  }
               }
            }
         }
         pm = pm->next;
      }
      useri_xerrmsg(err, 101ul);
   }
} /* end sendmsg() */


static void setmsgheard(const aprsdecode_MONCALL us)
/* set heard flag in tx msg if user heard now */
{
   aprsdecode_pTXMESSAGE pm;
   pm = aprsdecode_txmessages;
   for (;;) {
      if (pm==0) break;
      if ((pm->acktime==0UL && pm->txcnt==1UL) && aprsstr_StrCmp(pm->to, 9ul,
                 us, 9ul)) pm->heard = 1;
      pm = pm->next;
   }
} /* end setmsgheard() */


extern aprsdecode_pOPHIST aprsdecode_selop(void)
{
   if (aprsdecode_click.entries>0UL) {
      return aprsdecode_click.table[aprsdecode_click.selected].opf;
   }
   else return 0;
   return 0;
} /* end selop() */

/*
PROCEDURE objsender(op:pOPHIST; VAR s:ARRAY OF CHAR);
                (* find (last) sender of an item/object *)
VAR dat:DAT;
    f:pFRAMEHIST;
BEGIN
  s[0]:=0C;
  IF (op<>NIL) & (ISOBJECT IN op^.drawhints) THEN
    f:=op^.frames;
    IF f<>NIL THEN
      WHILE f^.next<>NIL DO f:=f^.next END;
                (* goto last frame *)
      IF (Decode(f^.vardat^.raw, dat)>=0) & ((dat.type=OBJ) OR (dat.type=ITEM))
      THEN Assign(s, dat.objectfrom) END;
    END;
  END;
END objsender;
*/

extern void aprsdecode_objsender(aprsdecode_pOPHIST op, char s[],
                unsigned long s_len)
/* find (last) sender of an item/object */
{
   struct aprsdecode_DAT dat;
   aprsdecode_pFRAMEHIST f;
   aprsdecode_pOPHIST o;
   s[0UL] = 0;
   if (op) {
      if ((0x2U & op->drawhints)) {
         f = op->frames;
         if (f) {
            while (f->next) f = f->next;
            if (aprsdecode_Decode(f->vardat->raw, 500ul,
                &dat)>=0L && (dat.type==aprsdecode_OBJ || dat.type==aprsdecode_ITEM)
                ) aprsstr_Assign(s, s_len, dat.objectfrom, 9ul);
         }
      }
      else {
         o = aprsdecode_ophist0;
         for (;;) {
            if (o==0) break;
            f = o->frames;
            if (((f && aprsdecode_Decode(f->vardat->raw, 500ul,
                &dat)>=0L) && (dat.type==aprsdecode_OBJ || dat.type==aprsdecode_ITEM)
                ) && X2C_STRCMP(dat.objectfrom,9u,op->call,9u)==0) {
               aprsstr_Assign(s, s_len, op->call, 9ul);
               break;
            }
            o = o->next;
         }
      }
   }
} /* end objsender() */

#define aprsdecode_MAXDELAY 30
/* add seconds to time */


static void timespeed(aprsdecode_pFRAMEHIST lastf,
                struct aprspos_POSITION pos1, unsigned long time1,
                unsigned char * err)
{
   float d;
   unsigned long t;
   if (((((*err&0xFU)!=0U || lastf==0) || (lastf->nodraw&0xFU)!=0U)
                || !aprspos_posvalid(lastf->vardat->pos))
                || !aprspos_posvalid(pos1)) return;
   d = aprspos_distance(lastf->vardat->pos, pos1);
   /*
     IF d>50.0 THEN INCL(err, eDIST) END;
   */
   if (lastf->time0>=time1) t = 0UL;
   else t = time1-lastf->time0;
   if (X2C_DIVR(d*3600.0f,(float)(t+30UL))>maxspeed) *err |= 0x10U;
} /* end timespeed() */

#define aprsdecode_MINHOP 20.0

#define aprsdecode_HOPPART 0.5


/* 1 wayout, delete before, 2 wayout join last good with last else delete before */
static void joinchunk(aprsdecode_pOPHIST op)
{
   aprsdecode_pFRAMEHIST lbad;
   aprsdecode_pFRAMEHIST bad;
   aprsdecode_pFRAMEHIST llpf;
   aprsdecode_pFRAMEHIST lpf;
   aprsdecode_pFRAMEHIST pf;
   aprsdecode_pVARDAT v;
   float d;
   float lastkm;
   float km;
   float waylen;
   waylen = (float)op->lastkmh*0.03f+20.0f;
   lastkm = 0.0f;
   llpf = 0;
   lpf = 0;
   pf = op->frames;
   bad = 0;
   lbad = 0;
   while (pf) {
      v = pf->vardat;
      if (aprspos_posvalid(v->pos) && (pf->nodraw&0xCU)==0U) {
         if (lpf) {
            km = aprspos_distance(lpf->vardat->pos, v->pos);
            if (aprsdecode_spikesens>0.0f && llpf) {
               d = aprsdecode_spikesens*aprspos_distance(llpf->vardat->pos,
                v->pos);
               if ((km>d && lastkm>d) && (0x8U & lpf->nodraw)==0) {
                  lpf->nodraw = lpf->nodraw&~0x11U|0x2U;
               }
            }
            /*
                        pf^.nodraw:=pf^.nodraw-ERRSET{eDIST, eSPEED};
            */
            if (aprsdecode_maxhop>0.0f) {
               if ((0x2U & lpf->nodraw)) lbad = 0;
               else if (lbad) bad = lbad;
               if (((0x2U & lpf->nodraw)==0 && km>waylen)
                && !(pf->nodraw&~0x1EU)) {
                  /* & (lastkm<=waylen)*/
                  pf->nodraw |= 0x1U;
                  lbad = pf;
               }
               if (km<waylen && (pf->nodraw&0x1FU)==0U) {
                  waylen = waylen+km*0.5f;
                  if (waylen>aprsdecode_maxhop) waylen = aprsdecode_maxhop;
               }
            }
            lastkm = km;
         }
         llpf = lpf;
         lpf = pf;
      }
      pf = pf->next;
   }
   if (lbad) bad = lbad;
   if (bad) {
      pf = op->frames;
      for (;;) {
         if (pf) pf->nodraw |= 0x20U;
         if (pf==bad) break;
         pf = pf->next;
      }
      if (pf) aprsdecode_posinval(&op->lastpos);
      while (pf) {
         /* maybe we have no pos now */
         if ((!(pf->nodraw&~0x9U) || !aprspos_posvalid(op->lastpos))
                && aprspos_posvalid(pf->vardat->pos)) {
            op->lastpos = pf->vardat->pos;
         }
         pf = pf->next;
      }
   }
} /* end joinchunk() */


extern void aprsdecode_makelogfn(char s[], unsigned long s_len)
{
   aprsdecode_FILENAME fn;
   useri_confstr(useri_fLOGWFN, fn, 1024ul);
   filepath(fn, 1024ul);
   if (fn[0UL]) aprsstr_Append(fn, 1024ul, "/", 2ul);
   aprsstr_Append(fn, 1024ul, s, s_len);
   aprsstr_Assign(s, s_len, fn, 1024ul);
/*WrStr("<"); WrStr(s);WrStrLn(">"); */
} /* end makelogfn() */


static void inwindow(aprsdecode_pOPHIST op, const char rawbuf[],
                unsigned long rawbuf_len)
{
   unsigned long matchlen;
   unsigned long j;
   unsigned long ii;
   unsigned long i;
   char in;
   aprsdecode_pFRAMEHIST pf;
   char wc[100];
   aprsdecode_FILENAME fn;
   aprsdecode_pVARDAT pv;
   unsigned char doset;
   struct aprspos_POSITION * anonym;
   in = 0;
   aprsdecode_tracenew.call[0UL] = 0;
   doset = 0U;
   if (op) {
      setmsgheard(op->call);
      pf = op->frames;
      while (pf && !in) {
         /* find waypoint inside screen */
         pv = pf->vardat;
         if (aprspos_posvalid(pv->pos)) {
            { /* with */
               struct aprspos_POSITION * anonym = &pv->pos;
               if (((anonym->long0>aprsdecode_tracenew.winpos0.long0 && anonym->lat<aprsdecode_tracenew.winpos0.lat)
                 && anonym->long0<aprsdecode_tracenew.winpos1.long0)
                && anonym->lat>aprsdecode_tracenew.winpos1.lat) in = 1;
            }
         }
         pf = pf->next;
      }
      if (in) ++aprsdecode_tracenew.winevent;
      /*find user in watchlist */
      /*  IF posvalid(op^.lastpos) THEN */
      j = 0UL;
      matchlen = 0UL;
      for (;;) {
         useri_confstrings(useri_fWATCH, j, 1, wc, 100ul);
         if (wc[0U]==0) break;
         i = 0UL;
         ii = 0UL;
         for (;;) {
            if (((i>99UL || ii>8UL) || wc[i]==0) || op->call[ii]==0) {
               if ((ii>8UL || op->call[ii]==0) && (wc[i]==0 || wc[i]=='*')) {
                  if (i>=matchlen) {
                     memcpy(aprsdecode_tracenew.call,op->call,9u);
                     doset = useri_confflags(useri_fWATCH, j);
                     matchlen = i;
                  }
               }
               break;
            }
            if (wc[i]=='*') {
               if (wc[i+1UL]) ++i;
            }
            else {
               if (X2C_CAP(wc[i])!=X2C_CAP(op->call[ii])) break;
               ++i;
            }
            ++ii;
         }
         ++j;
      }
      /*  END; */
      aprsdecode_tracenew.beep = (char)(doset&1);
      aprsdecode_tracenew.follow = (char)(doset/2U&1);
      if ((!aprsdecode_tracenew.follow && aprsdecode_click.watchmhop)
                && X2C_STRCMP(op->call,9u,aprsdecode_click.mhop,9u)==0) {
         aprsdecode_tracenew.follow = 1;
         memcpy(aprsdecode_tracenew.call,op->call,9u);
      }
      /*WrStrLn(op^.call); WrStrLn(click.mhop);
                WrInt(ORD(click.watchmhop), 1);
                WrInt(ORD(tracenew.follow), 1); WrStrLn(""); */
      if ((doset/4U&1)) {
         /* append to watchcall file */
         aprsstr_Assign(fn, 1024ul, op->call, 9ul);
         alfanum(fn, 1024ul); /* remove bad chars */
         /*
               confstr(fLOGWFN, fn);
               filepath(fn);
               IF fn[0]<>0C THEN Append(fn, DIRSEP) END;
               Append(fn, wc);
         */
         aprsdecode_makelogfn(fn, 1024ul);
         if (!wrlog(rawbuf, rawbuf_len, aprsdecode_realtime, fn, 1024ul,
                0L)) poplogerr(op->call, 9ul);
      }
   }
} /* end inwindow() */


extern unsigned long aprsdecode_finddup(aprsdecode_pFRAMEHIST pf,
                aprsdecode_pFRAMEHIST frame)
/* return delay time if dupe found */
{
   struct aprspos_POSITION apos;
   unsigned long ret;
   unsigned long dupedelay;
   unsigned long speed;
   unsigned long course;
   long altitude;
   struct aprsdecode_DAT dat;
   /* search from begin to 1 frame before */
   struct aprsdecode_VARDAT * anonym;
   apos = frame->vardat->pos;
   ret = 0UL;
   if ((pf!=frame && aprspos_posvalid(apos))
                && aprsdecode_Decode(frame->vardat->raw, 500ul, &dat)>=0L) {
      speed = dat.speed;
      course = dat.course;
      altitude = dat.altitude;
      dupedelay = frame->time0-aprsdecode_lums.dupemaxtime;
      while (pf->next!=frame) {
         { /* with */
            struct aprsdecode_VARDAT * anonym = pf->vardat;
            if (pf->time0>=dupedelay && (frame->vardat==pf->vardat || ((((apos.lat==anonym->pos.lat && apos.long0==anonym->pos.long0)
                 && aprsdecode_Decode(anonym->raw, 500ul,
                &dat)>=0L) && speed==dat.speed) && course==dat.course)
                && altitude==dat.altitude)) {
               /* dupe pos found */
               ret = frame->time0-pf->time0;
            }
            else if (ret>0UL) return ret;
         }
         /* pos changed so we have real dupe */
         pf = pf->next;
      }
   }
   return 0UL;
} /* end finddup() */

#define aprsdecode_POLY13 0x112D 

#define aprsdecode_TOPBIT 12


static void initcrc12(void)
{
   unsigned short remainder;
   unsigned long bit;
   unsigned long dividend;
   for (dividend = 0UL; dividend<=255UL; dividend++) {
      remainder = X2C_LSH((unsigned short)dividend,16,4);
      for (bit = 8UL;; bit--) {
         if ((0x1000U & remainder)) {
            remainder = X2C_LSH(remainder,16,1)^0x112DU;
         }
         else remainder = X2C_LSH(remainder,16,1);
         if (bit==0UL) break;
      } /* end for */
      crctable12[dividend] = remainder&0x1FFFU;
   } /* end for */
} /* end initcrc12() */

typedef unsigned char * RAW;

#define aprsdecode_TOPBIT0 12


static void crca12(const RAW data, long len, unsigned short * rem)
{
   long i;
   long tmp;
   tmp = len-1L;
   i = 0L;
   if (i<=tmp) for (;; i++) {
      *rem = X2C_LSH(*rem,16,8)^crctable12[(unsigned short)(X2C_LSH(*rem,16,
                -5)&0xFFU)]^(unsigned short)data[i];
      if (i==tmp) break;
   } /* end for */
} /* end crca12() */

struct _0;


struct _0 {
   aprsdecode_pFRAMEHIST wayp;
   unsigned short next;
};


static char lookup(unsigned long * wridx, struct _0 table[4096],
                unsigned short index[8192], aprsdecode_pFRAMEHIST waypoint)
{
   unsigned long sum;
   unsigned short ojp;
   unsigned short jp;
   unsigned short ip;
   struct aprsdecode_DAT dat1;
   struct aprsdecode_DAT dat;
   unsigned short rem;
   struct aprsdecode_FRAMEHIST * anonym;
   if (aprsdecode_Decode(waypoint->vardat->raw, 500ul,
                &dat)>=0L && aprspos_posvalid(dat.pos)) {
      rem = 0U;
      crca12((RAW) &dat.speed, 2L, &rem);
      crca12((RAW) &dat.course, 2L, &rem);
      crca12((RAW) &dat.altitude, 2L, &rem);
      crca12((RAW) &dat.pos.lat, (long)sizeof(float), &rem);
      crca12((RAW) &dat.pos.long0, (long)sizeof(float), &rem);
      sum = (unsigned long)(rem&0x1FFFU);
      ip = index[sum];
      /*WrInt(sum, 10);WrInt(ip, 10);WrStrLn("=ip"); */
      if (ip) {
         /* starts with 1, not found */
         --ip;
         jp = ip;
         for (;;) {
            { /* with */
               struct aprsdecode_FRAMEHIST * anonym = table[jp].wayp;
               if (anonym->vardat==waypoint->vardat || ((((anonym->vardat->pos.lat==dat.pos.lat && anonym->vardat->pos.long0==dat.pos.long0)
                 && aprsdecode_Decode(anonym->vardat->raw, 500ul,
                &dat1)>=0L) && dat1.speed==dat.speed)
                && dat1.course==dat.course) && dat1.altitude==dat.altitude) {
                  if (anonym->time0+aprsdecode_lums.dupemaxtime>waypoint->time0)
                 return 1;
                  else break;
               }
            }
            ojp = jp;
            jp = table[jp].next;
            if (jp>4095U || ojp<jp==((unsigned long)
                ojp<*wridx==(unsigned long)jp<*wridx)) {
               /*WrInt(ORD(jp>HIGH(table)), 2); WrInt(ojp, 6); WrInt(jp, 6);
                WrInt(wridx, 6); WrStr("o"); */
               break;
            }
         }
      }
      else {
         /*INC(collis); */
         ip = 65535U;
      }
      index[sum] = (unsigned short)(*wridx+1UL);
      table[*wridx].wayp = waypoint;
      table[*wridx].next = ip;
      *wridx = *wridx+1UL&4095UL;
   }
   return 0;
} /* end lookup() */


extern void aprsdecode_Checktrack(aprsdecode_pOPHIST op,
                aprsdecode_pFRAMEHIST lastf)
{
   aprsdecode_pFRAMEHIST f;
   aprsdecode_pFRAMEHIST frame;
   struct aprspos_POSITION last2;
   struct aprspos_POSITION last;
   struct aprspos_POSITION apos;
   unsigned long starttime;
   char cmp;
   unsigned long wridx;
   unsigned short index[8192];
   struct _0 table[4096];
   struct aprsdecode_OPHIST * anonym;
   if (lastf) frame = lastf->next;
   else {
      op->margin0.long0 = 3.1415926535f;
      op->margin0.lat = (-1.57079632675f);
      op->margin1.long0 = (-3.1415926535f);
      op->margin1.lat = 1.57079632675f;
      frame = op->frames;
      while (frame) {
         frame->vardat->refcnt = 0U;
         frame = frame->next;
      }
      frame = op->frames;
      while (frame) {
         ++frame->vardat->refcnt;
         frame = frame->next;
      }
      frame = op->frames;
   }
   if (frame==0) return;
   starttime = frame->time0; /* from this waypoint dupes will be set */
   f = op->frames;
   while (f && f->time0+aprsdecode_lums.dupemaxtime<starttime) f = f->next;
   memset((char *)index,(char)0,16384UL);
   wridx = 0UL;
   lastf = 0;
   cmp = 0;
   aprsdecode_posinval(&last);
   aprsdecode_posinval(&last2);
   while (f) {
      if (f==frame) cmp = 1;
      apos = f->vardat->pos;
      if (aprspos_posvalid(apos)) {
         if (cmp) {
            { /* with */
               struct aprsdecode_OPHIST * anonym = op;
               if (apos.long0<anonym->margin0.long0) {
                  anonym->margin0.long0 = apos.long0;
               }
               if (apos.long0>anonym->margin1.long0) {
                  anonym->margin1.long0 = apos.long0;
               }
               if (apos.lat>anonym->margin0.lat) {
                  anonym->margin0.lat = apos.lat;
               }
               if (apos.lat<anonym->margin1.lat) {
                  anonym->margin1.lat = apos.lat;
               }
            }
            /*        IF (op^.margin0.long<>op^.margin1.long)
                OR (op^.margin0.lat<>op^.margin1.lat)
                THEN   (* moving station *) */
            f->nodraw &= ~0x8U;
         }
         if (lookup(&wridx, table, index, f) && cmp) {
            /*WrFixed(apos.lat, 10,5); WrFixed(apos.long, 10,5); WrLn; */
            if (apos.lat!=last.lat || apos.long0!=last.long0) {
               if (apos.lat!=last2.lat || apos.long0!=last2.long0) {
                  f->nodraw |= 0x8U;
               }
            }
         }
         if (maxspeed>0.0f) timespeed(lastf, apos, f->time0, &f->nodraw);
         if (!(f->nodraw&~0x14U) || !aprspos_posvalid(op->lastpos)) {
            op->lastpos = apos;
         }
         last2 = last;
         last = apos;
      }
      else f->nodraw |= 0x40U;
      lastf = f;
      f = f->next;
   }
   joinchunk(op);
/*WrInt(collis, 10);WrStrLn("=collisions"); */
/*
WrStr(op^.call); WrStr(op^.sym.tab);WrFixed(op^.lastpos.lat, 5,10);WrLn;
*/
} /* end Checktrack() */

/*
PROCEDURE Checktrack(op:pOPHIST; lastf:pFRAMEHIST);
VAR frame, f:pFRAMEHIST;
--    v:pVARDAT;
    apos, last, last2:POSITION;

    starttime:TIME;
    cmp:BOOLEAN;
--    sum:CARD16;
--    dat        : DAT;
--    duphash               : HASHSET;
    wridx, new:CARDINAL;
    index:ARRAY[0..16383] OF CARD16;
    table:ARRAY[0..4095] OF RECORD
      wayp:pFRAMEHIST;
      next:CARD16;
    END;


  PROCEDURE lookup(waypoint:pFRAMEHIST):BOOLEAN;

  TYPE pHASH=POINTER TO ARRAY[0..7] OF CHAR;

  VAR sum: CARDINAL;
    ip, jp, ojp:CARD16;
    dat, dat1:DAT;
    phash:pHASH;
 
  BEGIN
    phash:=CAST(pHASH, ADR(waypoint^.vardat^.pos));
    sum:=Hash(phash^, 0, SIZE(phash^)) MOD (HIGH(index)+1);
    ip:=index[sum];
WrInt(sum, 10);WrInt(ip, 10);WrStrLn("=ip");
    IF ip<>0 THEN                                                  (* starts with 1,
                 not found *)
      DEC(ip);
      jp:=ip;
      dat1.srccall[0]:=0C;
      LOOP
WrStr(".");
        WITH table[jp].wayp^ DO 
          IF (vardat=waypoint^.vardat)
          OR (vardat^.pos.lat=waypoint^.vardat^.pos.lat)
          & (vardat^.pos.long=waypoint^.vardat^.pos.long)
          & (Decode(vardat^.raw, dat)>=0)
          & ((dat1.srccall[0]<>0C) OR (Decode(waypoint^.vardat^.raw,
                dat1)>=0))  
          & (dat1.speed=dat.speed) & (dat1.course=dat.course)
                & (dat1.altitude=dat.altitude) THEN
            IF time+lums.dupemaxtime>waypoint^.time THEN RETURN TRUE ELSE EXIT END;

          END;
        END;
        ojp:=jp; 
        jp:=table[jp].next;
        IF (jp>HIGH(table)) OR ((ojp<jp)=((ojp<wridx)=(jp<wridx))) THEN
WrInt(ORD(jp>HIGH(table)), 2); WrInt(ojp, 6); WrInt(jp, 6); WrInt(wridx, 6);
                WrStr("o");
          EXIT

        END;
      END;
WrStrLn("/");
    ELSE ip:=MAX(CARD16) END;
    index[sum]:=wridx+1;
    table[wridx].wayp:=waypoint;
    table[wridx].next:=ip;
    wridx:=(wridx+1) MOD (HIGH(table)+1);
    RETURN FALSE
  END lookup;



BEGIN

WrStrLn(op^.call);

  IF lastf<>NIL THEN frame:=lastf^.next; 
  ELSE 
    op^.margin0.long:=PI2*0.5;
    op^.margin0.lat:=-PI2*0.25;
    op^.margin1.long:=-PI2*0.5;
    op^.margin1.lat:=PI2*0.25;
    frame:=op^.frames;
    WHILE frame<>NIL DO frame^.vardat^.refcnt:=0; frame:=frame^.next END;
    frame:=op^.frames;
    WHILE frame<>NIL DO INC(frame^.vardat^.refcnt); frame:=frame^.next END;
    frame:=op^.frames;
  END;
  IF frame=NIL THEN RETURN END;

  starttime:=frame^.time;                      (* from this waypoint dupes will be set *)
  f:=op^.frames;
  WHILE (f<>NIL) & (f^.time+lums.dupemaxtime<starttime) DO f:=f^.next END;
                (* skip before dupetime *)

  FILL(ADR(index), 0C, SIZE(index));
  wridx:=0;
  lastf:=NIL;
  cmp:=FALSE;
  posinval(last);
  posinval(last2);
  WHILE f<>NIL DO
    apos:=f^.vardat^.pos;
    IF posvalid(apos) THEN
      IF f=frame THEN cmp:=TRUE END;
                (* from now set dupes *)
      IF cmp THEN
        WITH op^ DO
          IF apos.long<margin0.long THEN margin0.long:=apos.long END;
          IF apos.long>margin1.long THEN margin1.long:=apos.long END;
          IF apos.lat >margin0.lat  THEN margin0.lat :=apos.lat  END;
          IF apos.lat <margin1.lat  THEN margin1.lat :=apos.lat  END;
        END;

--        IF (op^.margin0.long<>op^.margin1.long)
                OR (op^.margin0.lat<>op^.margin1.lat)
                THEN   (* moving station *)

        EXCL(f^.nodraw, eDUPE);
      END;
      IF lookup(f) & cmp THEN
--WrFixed(apos.lat, 10,5); WrFixed(apos.long, 10,5); WrLn;
        IF (apos.lat<>last.lat) OR (apos.long<>last.long) THEN
          IF (apos.lat<>last2.lat) OR (apos.long<>last2.long)
                THEN INCL(f^.nodraw, eDUPE) END;
        END;

      END; 
      IF maxspeed>0.0 THEN timespeed(lastf, apos, f^.time, f^.nodraw) END;
      IF (f^.nodraw<=ERRSET{eSYMB,eSPEED}
                ) OR NOT posvalid(op^.lastpos) THEN op^.lastpos:=apos END;

      last2:=last;
      last:=apos;


    ELSE INCL(frame^.nodraw, eNOPOS) END;
    lastf:=f;
    f:=f^.next;

  END;
  joinchunk(op);
(*
WrStr(op^.call); WrStr(op^.sym.tab);WrFixed(op^.lastpos.lat, 5,10);WrLn;
*)
END Checktrack;
*/

static char strcmp0(const char a[], unsigned long a_len, const char b[],
                unsigned long b_len)
/* faster but needs 0 termination */
{
   unsigned long i;
   i = 0UL;
   for (;;) {
      if (a[i]!=b[i]) return 0;
      if (a[i]==0) return 1;
      ++i;
   }
   return 0;
} /* end strcmp() */


static char callfilt(unsigned char v, const char str[],
                unsigned long str_len, char fullmatch)
{
   unsigned long j;
   unsigned long i;
   char s[31];
   if (str[0UL]==0) return 0;
   i = 0UL;
   for (;;) {
      useri_confstrings(v, i, 0, s, 31ul);
      if (s[0U]==0) return 0;
      s[30U] = 0;
      /*  IF fullmatch THEN IF StrCmp(s, str) THEN RETURN TRUE END;  */
      if (fullmatch) {
         j = 0UL;
         for (;;) {
            if (s[j]=='*') {
               if (s[j+1UL]==0 || str[j]==0) return 1;
            }
            else {
               if (s[j]!=str[j]) break;
               if (str[j]==0) return 1;
            }
            ++j;
            if (j>str_len-1) break;
         }
      }
      else if (aprsstr_InStr(str, str_len, s, 31ul)>=0L) return 1;
      /* comment filter */
      ++i;
   }
   return 0;
} /* end callfilt() */


static char isacall(const char s[], unsigned long s_len)
{
   unsigned long p;
   unsigned long lit;
   unsigned long num1;
   char c;
   /*  IF configon(fMUSTBECALL) THEN */
   p = 0UL;
   num1 = 0UL;
   lit = 0UL;
   for (;;) {
      c = s[p];
      if ((unsigned char)c>='0' && (unsigned char)c<='9') ++num1;
      else if ((unsigned char)c>='A' && (unsigned char)c<='Z') ++lit;
      else break;
      ++p;
      if (p>5UL) break;
   }
   if ((lit<2UL || num1==0UL) || num1>2UL) return 0;
   if (s[p]=='-') {
      ++p;
      if (s[p]=='1') {
         ++p;
         if ((unsigned char)s[p]>='0' && (unsigned char)s[p]<='5') ++p;
      }
      else {
         if ((unsigned char)s[p]<'1' || (unsigned char)s[p]>'9') return 0;
         ++p;
      }
   }
   return p>s_len-1 || s[p]==0;
/*  END; */
/*  RETURN TRUE */
} /* end isacall() */

/*
PROCEDURE framestat(c-:MONCALL; isigate:BOOLEAN);
BEGIN
  IF iscall(c) THEN 
--    WrStr(c);
--    WrInt(ORD(isigate), 2);
  END;
END framestat;
*/

static void inserthrt(const struct aprsdecode_DAT dat,
                aprsdecode_pOPHIST * op, unsigned char errs)
{
   aprsdecode_pFRAMEHIST lastf;
   aprsdecode_pFRAMEHIST f;
   aprsdecode_pVARDAT v;
   unsigned long len;
   unsigned long ih;
   unsigned long i;
   unsigned long stime;
   char s[51];
   char rb[51];
   struct aprsdecode_VARDAT * anonym;
   unsigned long tmp;
   f = (*op)->frames;
   lastf = 0;
   tmp = dat.hrtlen-1UL;
   ih = 0UL;
   if (ih<=tmp) for (;; ih++) {
      if (dat.hrtposes[ih].notinterpolated) {
         /* interpolating does vector drawer */
         stime = dat.hrtposes[ih].dtime;
         while (f && f->time0<stime) {
            lastf = f;
            f = f->next;
         }
         if (f==0 || f->time0!=stime) {
            /* not a redundant same time frame */
            Storage_ALLOCATE((X2C_ADDRESS *) &f,
                sizeof(struct aprsdecode_FRAMEHIST));
            useri_debugmem.req = sizeof(struct aprsdecode_FRAMEHIST);
            useri_debugmem.mon += useri_debugmem.req;
            if (f==0) return;
            memset((X2C_ADDRESS)f,(char)0,
                sizeof(struct aprsdecode_FRAMEHIST));
            if ((float)fabs(dat.hrtposes[ih].dalt)<2.147483646E+9f) {
               aprsstr_IntToStr((long)X2C_TRUNCI(dat.hrtposes[ih].dalt,
                X2C_min_longint,X2C_max_longint), 0UL, s, 51ul);
               strncpy(rb,">:$HR[a",51u);
               aprsstr_Append(rb, 51ul, s, 51ul);
               aprsstr_Append(rb, 51ul, "]", 2ul);
            }
            else rb[0U] = 0;
            len = (sizeof(struct aprsdecode_VARDAT)-499UL)+aprsstr_Length(rb,
                 51ul);
            Storage_ALLOCATE((X2C_ADDRESS *) &v, len);
            useri_debugmem.req = len;
            useri_debugmem.mon += len;
            if (v==0) return;
            memset((X2C_ADDRESS)v,(char)0,len);
            { /* with */
               struct aprsdecode_VARDAT * anonym = v;
               anonym->pos.long0 = dat.hrtposes[ih].dlong;
               anonym->pos.lat = dat.hrtposes[ih].dlat;
               i = 0UL;
               do {
                  anonym->raw[i] = rb[i];
                  ++i;
               } while (rb[i]);
            }
            v->lastref = f;
            f->vardat = v;
            f->time0 = stime;
            f->nodraw = errs;
            if (lastf==0) {
               f->next = (*op)->frames;
               (*op)->frames = f; /* new track */
            }
            else {
               f->next = lastf->next; /* not nil if insert older frame */
               lastf->next = f; /* append waypoint */
            }
         }
      }
      if (ih==tmp) break;
   } /* end for */
} /* end inserthrt() */


static void settempspeed(aprsdecode_pOPHIST op,
                const struct aprsdecode_DAT dat)
{
   float rv;
   op->drawhints = op->drawhints&~0x3U;
   if (dat.objectfrom[0UL]) op->drawhints |= 0x2U;
   /*  EXCL(op^.drawhints, MIRRORSYM); */
   if (dat.sym=='_') {
      rv = aprstext_FtoC(dat.wx.temp);
      if (rv>=(-99.0f) && rv<=99.0f) {
         op->lasttempalt = (short)X2C_TRUNCI(rv,-32768,32767);
         op->temptime = aprsdecode_systime;
         if (dat.course<=360UL) {
            op->lastinftyp = (unsigned char)(110UL+dat.course/4UL);
         }
         else op->lastinftyp = 100U;
         if (dat.speed<500UL) {
            op->lastkmh = (short)X2C_TRUNCI((float)dat.speed*1.609f+0.5f,
                -32768,32767);
         }
         else op->lastkmh = 0;
      }
      else op->lastinftyp = 0U;
   }
   else if (dat.speed<30000UL) {
      op->lastkmh = (short)X2C_TRUNCI((float)dat.speed*1.852f+0.5f,-32768,
                32767);
      if (dat.speed>0UL && dat.course>180UL) op->drawhints |= 0x1U;
      if (dat.course<=360UL) {
         op->lastinftyp = (unsigned char)(10UL+dat.course/4UL);
      }
      else op->lastinftyp = 1U;
   }
   else if (op->lastinftyp<100U) op->lastinftyp = 0U;
   if (op->lastinftyp<100U) {
      if (dat.altitude>-10000L && dat.altitude<55535L) {
         op->lasttempalt = (short)(dat.altitude+(-22768L));
      }
      else op->lasttempalt = -32768;
   }
} /* end settempspeed() */

#define aprsdecode_SAMETIME 27
/* max time diff for merged in frames */

static SET256 _cnst0 = {0x00000000UL,0x03FF8000UL,0x17FFFFFEUL,0x00000000UL,
                0x00000000UL,0x00000000UL,0x00000000UL,0x00000000UL};
static CHSET _cnst = {0x30000000UL,0x280086BAUL,0x80000001UL,0x00000001UL};

extern long aprsdecode_Stoframe(aprsdecode_pOPHIST * optab, char rawbuf[],
                unsigned long rawbuf_len, unsigned long stime, char logmode,
                struct aprsdecode_DAT dat)
{
   aprsdecode_pOPHIST opo;
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST lastf;
   aprsdecode_pFRAMEHIST frame;
   aprsdecode_pVARDAT same;
   aprsdecode_MONCALL igate;
   unsigned long len;
   unsigned long i;
   unsigned short ch0;
   unsigned short hash;
   struct aprsdecode_VARDAT * anonym;
   long aprsdecode_Stoframe_ret;
   X2C_PCOPY((void **)&rawbuf,rawbuf_len);
   i = 0UL;
   hash = 0U;
   do {
      ch0 = (unsigned short)(unsigned char)rawbuf[i];
      hash += ch0;
      ++i;
   } while (ch0);
   do {
      --i;
   } while (!(i==0UL || (rawbuf[i]!='\015' && rawbuf[i]!='\012') && rawbuf[i]));
   rawbuf[i+1UL] = 0; /* remove trailing CR LF */
   /*FOR len:=0 TO i DO WrInt(ORD(rawbuf[len]), 4) END; WrLn; */
   /*  IF NOT logmode THEN */
   if (dat.igatep>0UL && dat.igatep<=9UL) {
      memcpy(igate,dat.viacalls[dat.igatep],9u);
   }
   else igate[0UL] = 0;
   if (((((callfilt(useri_fOPFILT, dat.srccall, 9ul,
                1) || callfilt(useri_fIGATEFILT, igate, 9ul,
                1)) || callfilt(useri_fOBJSRCFILT, dat.objectfrom, 9ul,
                1)) || callfilt(useri_fDESTFILT, dat.dstcall, 9ul,
                0)) || callfilt(useri_fSTRFILT, dat.comment0, 256ul,
                0)) || callfilt(useri_fRAWFILT, rawbuf, rawbuf_len, 0)) {
      aprsdecode_Stoframe_ret = -1L;
      goto label;
   }
   if (useri_configon(useri_fMUSTBECALL)) {
      if (dat.type==aprsdecode_OBJ || dat.type==aprsdecode_ITEM) {
         if (!isacall(dat.objectfrom, 9ul)) {
            aprsdecode_Stoframe_ret = -1L;
            goto label;
         }
      }
      else if (!isacall(dat.srccall, 9ul)) {
         aprsdecode_Stoframe_ret = -1L;
         goto label;
      }
   }
   /*  END; */
   /*  IF NOT logmode & (dat.type=MSG) THEN getmessage END; */
   /*  IF dat.hbitp<>0 THEN igate[0]:=0C END; */
   opo = 0;
   op = *optab;
   while (op && X2C_STRCMP(dat.srccall,9u,op->call,9u)) {
      opo = op;
      op = op->next;
   }
   if (op==0) {
      /* new user */
      Storage_ALLOCATE((X2C_ADDRESS *) &op,
                sizeof(struct aprsdecode_OPHIST));
      useri_debugmem.req = sizeof(struct aprsdecode_OPHIST);
      useri_debugmem.mon += useri_debugmem.req;
      if (op==0) {
         aprsdecode_Stoframe_ret = -2L;
         goto label;
      }
      memset((X2C_ADDRESS)op,(char)0,sizeof(struct aprsdecode_OPHIST));
      memcpy(op->call,dat.srccall,9u);
      if (logmode || opo==0) {
         /* store it at begin of list for quick find */
         op->next = *optab;
         *optab = op;
      }
      else opo->next = op;
   }
   else if (logmode && opo) {
      /* move it to begin of list */
      opo->next = op->next;
      op->next = *optab;
      *optab = op;
   }
   if (dat.hrttime>0UL) stime = dat.hrttime;
   if (op->lasttime<=stime) {
      /* new waypoint */
      settempspeed(op, dat);
      /*
          IF dat.objectfrom[0]<>0C THEN INCL(op^.drawhints, ISOBJECT) END;
          EXCL(op^.drawhints, MIRRORSYM);
      
          IF dat.sym="_" THEN
            rv:=FtoC(dat.wx.temp);
            IF (rv>=MINTEMP) & (rv<=MAXTEMP) THEN
              op^.lasttempalt:=VAL(INT16, rv);
              op^.temptime:=systime;
              IF dat.course<=360 THEN op^.lastinftyp:=110+dat.course DIV 4;
              ELSE op^.lastinftyp:=100 END;
                (* wx but no course *)
              IF dat.speed<500 THEN
                op^.lastkmh:=VAL(INT16, FLOAT(dat.speed)*WKNOTS+0.5);
              ELSE op^.lastkmh:=0 END;
            ELSE op^.lastinftyp:=0 END;
      
          ELSIF dat.speed<30000 THEN
            op^.lastkmh:=VAL(INT16, FLOAT(dat.speed)*KNOTS+0.5);
            IF (dat.speed>0) & (dat.course>180) THEN INCL(op^.drawhints,
                MIRRORSYM) END;
            IF dat.course<=360 THEN op^.lastinftyp:=10+dat.course DIV 4
            ELSE op^.lastinftyp:=1 END;
                (* drive speed but no course *)
      
          ELSIF op^.lastinftyp<100 THEN op^.lastinftyp:=0 END;
                (* set no info if not wx *)
      
          IF op^.lastinftyp<100 THEN
            IF (dat.altitude>-10000) & (dat.altitude<65535-10000)
            THEN op^.lasttempalt:=dat.altitude+(10000-32768)
                ELSE op^.lasttempalt:=-32768 END;
          END;
      */
      op->lasttime = stime;
   }
   frame = op->frames;
   lastf = 0;
   same = 0;
   if (logmode) {
      /* refcnt used as raw string hash */
      while (frame && (frame->vardat->refcnt!=hash || !strcmp0(frame->vardat->raw,
                 500ul, rawbuf, rawbuf_len))) {
         lastf = frame;
         frame = frame->next;
      }
   }
   else {
      /* online data find without hash */
      while (frame && !strcmp0(frame->vardat->raw, 500ul, rawbuf,
                rawbuf_len)) {
         lastf = frame;
         frame = frame->next;
      }
   }
   if (frame) {
      /* found same frame */
      same = frame->vardat;
      lastf = same->lastref;
      while (lastf->next) lastf = lastf->next;
   }
   if (lastf && lastf->time0>stime) {
      /* older frame to insert */
      lastf = 0;
      frame = op->frames;
      while (frame && frame->time0<stime) {
         lastf = frame;
         frame = frame->next;
      }
      if (frame && frame->time0-stime<27UL) {
         aprsdecode_Stoframe_ret = -2L;
         goto label;
      }
   }
   /* not insert same frame */
   Storage_ALLOCATE((X2C_ADDRESS *) &frame,
                sizeof(struct aprsdecode_FRAMEHIST));
   useri_debugmem.req = sizeof(struct aprsdecode_FRAMEHIST);
   useri_debugmem.mon += useri_debugmem.req;
   if (frame==0) {
      aprsdecode_Stoframe_ret = -2L;
      goto label;
   }
   memset((X2C_ADDRESS)frame,(char)0,sizeof(struct aprsdecode_FRAMEHIST));
   if (same==0) {
      /* found new data */
      len = (sizeof(struct aprsdecode_VARDAT)-499UL)+aprsstr_Length(rawbuf,
                rawbuf_len);
      Storage_ALLOCATE((X2C_ADDRESS *) &same, len);
      useri_debugmem.req = len;
      useri_debugmem.mon += len;
      if (same==0) {
         aprsdecode_Stoframe_ret = -2L;
         goto label;
      }
      memset((X2C_ADDRESS)same,(char)0,len);
      { /* with */
         struct aprsdecode_VARDAT * anonym = same;
         anonym->pos = dat.pos;
         /*      altitude:=dat.altitude; */
         if (dat.hbitp==0UL && dat.igaterawp<=255UL) {
            /* direct to igate */
            anonym->igatepos = (unsigned char)dat.igaterawp;
            anonym->igatelen = (unsigned char)dat.igatelen;
         }
         i = 0UL;
         do {
            anonym->raw[i] = rawbuf[i];
            ++i;
         } while (rawbuf[i]);
         if (logmode) anonym->refcnt = hash;
      }
   }
   else if (!logmode) ++same->refcnt;
   if (same->lastref==0 || same->lastref->time0<=stime) {
      same->lastref = frame;
   }
   frame->vardat = same;
   frame->time0 = stime;
   if (X2C_INL((long)(unsigned char)dat.typc,128,_cnst)) {
      if ((X2C_INL((unsigned char)dat.symt,256,
                _cnst0) && (unsigned char)dat.sym>' ') && (unsigned char)
                dat.sym<'\177') {
         op->sym.tab = dat.symt;
         op->sym.pic = dat.sym;
      }
      else frame->nodraw |= 0x4U;
      if (dat.objkill=='1') {
         op->sym.tab = '\001';
         op->sym.pic = 0;
      }
   }
   if (lastf==0) {
      frame->next = op->frames;
      op->frames = frame; /* new track */
   }
   else {
      frame->next = lastf->next; /* not nil if insert older frame */
      lastf->next = frame; /* append waypoint */
   }
   if (dat.hrtlen>0UL) inserthrt(dat, &op, frame->nodraw);
   if (!logmode) {
      /* read log check whole track at end */
      aprsdecode_Checktrack(op, lastf);
      inwindow(op, rawbuf, rawbuf_len);
   }
   /*WrInt(dat.igatep, 10); WrInt(dat.hbitp, 5);  WrStrLn(rawbuf); */
   /*IF (dat.igatep>0) & (dat.igatep<=HIGH(dat.viacalls)) */
   /*THEN framestat(dat.viacalls[dat.igatep], TRUE) END; */
   /*FOR i:=1 TO dat.hbitp DO framestat(dat.viacalls[i], FALSE) END; */
   /*WrLn; */
   if (!(frame->nodraw&~0x1U)) aprsdecode_Stoframe_ret = 0L;
   else aprsdecode_Stoframe_ret = -1L;
   label:;
   X2C_PFREE(rawbuf);
   return aprsdecode_Stoframe_ret;
} /* end Stoframe() */

/*
PROCEDURE chkptr(op:pOPHIST);                     (* check database pointer system *)
VAR
    frame, f:pFRAMEHIST;
    v:pVARDAT;
    c, cl:CARDINAL;
BEGIN
  WHILE op<>NIL DO
    frame:=op^.frames;
    WHILE frame<>NIL DO EXCL(frame^.nodraw, eNODRAW); frame:=frame^.next;
                END;
    frame:=op^.frames;

    IF frame=NIL THEN WrStrLn("op with no frames") END; 
    LOOP
      IF frame=NIL THEN EXIT END;

      IF (frame^.next<>NIL) & (frame^.time>frame^.next^.time)
                THEN WrStrLn("time decreases") END;

      IF NOT (eNODRAW IN frame^.nodraw) THEN 
        v:=frame^.vardat;
        f:=frame;
        c:=0;
        cl:=0;
        WHILE f<>NIL DO
          IF f^.vardat=v THEN 
            INC(c);
            IF eNODRAW IN f^.nodraw THEN WrStrLn("double used waypoint") END;
                  
            INCL(f^.nodraw, eNODRAW);
            IF v^.lastref=f THEN cl:=c END;
          END;
          f:=f^.next;
        END;
        IF cl<>c THEN WrStrLn("lastref not last") END;
        IF v^.refcnt<>c THEN WrStrLn("refcnt wrong") END;
      END;
      frame:=frame^.next;

    END;
    op:=op^.next;
  END;
END chkptr;
*/

static char locked(aprsdecode_pOPHIST op)
/* prevent entries in click table from purge */
{
   unsigned long i;
   i = 0UL;
   while (i<aprsdecode_click.entries) {
      if (aprsdecode_click.table[i].opf==op) return 1;
      ++i;
   }
   return 0;
} /* end locked() */


static void freevardat(aprsdecode_pVARDAT v)
{
   unsigned long alen;
   alen = (sizeof(struct aprsdecode_VARDAT)-499UL)+aprsstr_Length(v->raw,
                500ul);
   Storage_DEALLOCATE((X2C_ADDRESS *) &v, alen);
   useri_debugmem.mon -= alen;
} /* end freevardat() */

#define aprsdecode_CHKINTERVALL 120
/* seconds to next check track */


extern void aprsdecode_purge(aprsdecode_pOPHIST * ops, unsigned long oldt,
                unsigned long oldobj)
{
   aprsdecode_pOPHIST opx;
   aprsdecode_pOPHIST lastop;
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST frame;
   aprsdecode_pVARDAT v;
   unsigned long old;
   char chk;
   /*chkptr(ops); */
   /*
   WrInt(ORD(lums.logmode), 1);
   IF ops=ophist THEN WrStrLn("purge oph")
                ELSIF ops=ophist2 THEN WrStrLn("purge oph2")
                ELSE WrStrLn("purge error") END;
   */
   op = *ops;
   lastop = 0;
   while (op) {
      /*WrStr("op:"); WrStr(op^.call); WrStr("         "); */
      if (op->sym.tab=='\001') old = X2C_max_longcard;
      else if ((0x2U & op->drawhints) && op->lastinftyp<100U) {
         old = oldobj; /* object but no wx obj for wx graphs */
      }
      else old = oldt;
      chk = 0;
      for (;;) {
         frame = op->frames;
         if (frame==0) break;
         /*WrCard(systime-frame^.time, 6);
                IF frame^.next<>NIL THEN WrStr("+") END; */
         if (frame->time0>=old || locked(op)) break;
         if (frame->next && frame->time0/120UL!=frame->next->time0/120UL) {
            chk = 1;
         }
         v = frame->vardat;
         if (v->lastref==frame) freevardat(v);
         else --v->refcnt;
         op->frames = frame->next;
         /*WrStr("(purge)"); */
         useri_debugmem.mon -= sizeof(struct aprsdecode_FRAMEHIST);
         Storage_DEALLOCATE((X2C_ADDRESS *) &frame,
                sizeof(struct aprsdecode_FRAMEHIST));
      }
      if (op->frames==0 && !locked(op)) {
         /* empty op */
         opx = op;
         op = op->next;
         if (lastop==0) *ops = op;
         else lastop->next = op;
         /*WrStr(opx^.call); WrStrLn("(purgop) "); */
         useri_debugmem.mon -= sizeof(struct aprsdecode_OPHIST);
         Storage_DEALLOCATE((X2C_ADDRESS *) &opx,
                sizeof(struct aprsdecode_OPHIST));
      }
      else {
         if (chk) {
            aprsdecode_Checktrack(op, 0); /*WrStr(op^.call); WrStr(" ")*/
         }
         lastop = op;
         op = op->next;
      }
   }
/*WrLn; */
/*WrStrLn(" --purge"); */
} /* end purge() */


extern void aprsdecode_delwaypoint(aprsdecode_pOPHIST op,
                aprsdecode_pFRAMEHIST * frame)
{
   aprsdecode_pFRAMEHIST lastf;
   aprsdecode_pFRAMEHIST lf;
   aprsdecode_pFRAMEHIST f;
   long mi;
   long di;
   long i;
   struct aprsdecode_DAT dat;
   if (op==0 || *frame==0) return;
   i = 0L;
   di = 0L;
   mi = 0L;
   lf = 0;
   f = op->frames;
   while (f) {
      if (f->next==*frame) {
         lf = f; /* next frame is to delete */
         di = mi;
      }
      if (f->vardat==(*frame)->vardat) ++i;
      f->vardat->refcnt = 0U;
      ++mi;
      f = f->next;
   }
   if (i==1L) freevardat((*frame)->vardat);
   if (lf==0) op->frames = (*frame)->next;
   else lf->next = (*frame)->next;
   useri_debugmem.mon -= sizeof(struct aprsdecode_FRAMEHIST);
   Storage_DEALLOCATE((X2C_ADDRESS *)frame,
                sizeof(struct aprsdecode_FRAMEHIST));
   f = op->frames;
   while (f) {
      if (f->vardat->refcnt<65535U) ++f->vardat->refcnt;
      f->vardat->lastref = f;
      f->nodraw = 0U; /* mark errors new later */
      f = f->next;
   }
   aprsdecode_Checktrack(op, 0);
   i = 0L; /* set new marker to the nearest from deleted with error */
   mi = X2C_max_longint;
   *frame = 0;
   lf = 0;
   lastf = 0;
   f = op->frames;
   while (f) {
      if ((!aprsdecode_lums.errorstep || (f->nodraw&~0x40U)!=0U)
                && labs(i-di)<mi) {
         mi = labs(i-di);
         *frame = f;
      }
      if (i==di) lf = f;
      ++i;
      lastf = f;
      f = f->next;
   }
   if (*frame==0) *frame = lf;
   if (lastf) {
      if (aprsdecode_Decode(lastf->vardat->raw, 500ul, &dat)>=0L) {
         settempspeed(op, dat);
      }
      op->lasttime = lastf->time0;
                /* deleted last frame so correct op time */
   }
} /* end delwaypoint() */


static char cmpfrom(const char a[], unsigned long a_len, unsigned long from,
                const char b[], unsigned long b_len)
{
   unsigned long i;
   i = 0UL;
   while (from<=a_len-1 && b[i]) {
      if (a[from]!=b[i]) return 0;
      ++i;
      ++from;
   }
   return 1;
} /* end cmpfrom() */

/*
PROCEDURE AppQ(VAR h:ARRAY OF CHAR; q-:ARRAY OF CHAR; appcall:BOOLEAN);
BEGIN
  Append(h, ">"+TOCALL+",");
  Append(h, q);
  IF appcall THEN Append(h, servercall) END;
  Append(h, ":");
END AppQ;
*/

static char callchk(unsigned long * qpos, char * tablechk, char buf[],
                unsigned long buf_len, unsigned long * p,
                unsigned long tablen, unsigned long * pssid, char withstar,
                char iscall0)
{
   unsigned long lit;
   unsigned long num1;
   unsigned long u;
   unsigned long j;
   unsigned long i;
   char c;
   *pssid = 0UL;
   u = 0UL;
   for (;;) {
      if (u>=tablen) break;
      j = 0UL;
      i = *p;
      while (ungate[u][j]==buf[i]) {
         ++i;
         ++j;
         if (ungate[u][j]==0) {
            *tablechk = 1;
            goto loop_exit;
         }
      }
      ++u;
   }
   loop_exit:;
   if (iscall0) {
      /* we want a callsign */
      num1 = 0UL;
      lit = 0UL;
      for (;;) {
         c = buf[*p];
         if ((unsigned char)c>='0' && (unsigned char)c<='9') ++num1;
         else if ((unsigned char)c>='A' && (unsigned char)c<='Z') ++lit;
         else break;
         ++*p;
      }
      if ((lit<2UL || num1==0UL) || num1>2UL) return 0;
      *pssid = *p;
      if (buf[*p]=='-') {
         ++*p;
         if (buf[*p]=='1') {
            ++*p;
            if ((unsigned char)buf[*p]>='0' && (unsigned char)buf[*p]<='5') {
               ++*p;
            }
         }
         else {
            if ((unsigned char)buf[*p]<'1' || (unsigned char)buf[*p]>'9') {
               return 0;
            }
            ++*p;
         }
      }
   }
   else if ((buf[*p]=='q' && buf[*p+1UL]=='A') && (unsigned char)
                buf[*p+2UL]>='A') {
      /* qAx */
      *qpos = *p+2UL;
      *p += 3UL;
   }
   else {
      /* we skip any text */
      for (;;) {
         c = buf[*p];
         if (c=='-') *pssid = *p;
         if (((((unsigned char)c<=' ' || c=='>') || c=='*') || c==',')
                || c==':') break;
         ++*p;
      }
   }
   if (*pssid==0UL) *pssid = *p;
   if (withstar && buf[*p]=='*') ++*p;
   return 1;
} /* end callchk() */


static void Iconstruct(char qtext[32], unsigned long * pins, char logcall[],
                unsigned long logcall_len, char buf[], unsigned long buf_len,
                 unsigned long * p)
{
   unsigned long k;
   unsigned long j;
   unsigned long i;
   if (*p<=4UL || buf[*p-2UL]!='I') return;
   /* ,CALL,I: */
   i = *p-4UL;
   while (buf[i]!=',') {
      /* hop back call */
      if (i==0UL) return;
      --i;
   }
   k = i; /* "," before call */
   ++i;
   j = 0UL;
   while (j<=logcall_len-1 && logcall[j]) {
      if (logcall[j]!=buf[i]) return;
      ++i;
      ++j;
   }
   /* logcall = call,I */
   *pins = k;
   i = *p-1UL;
   *p = k+1UL; /* payload */
   for (;;) {
      buf[k] = buf[i];
      if (i>=buf_len-1 || buf[i]==0) break;
      ++k;
      ++i;
   }
   /* delete ,call,I */
   strncpy(qtext,",qAR,",32u); /* insert ,qAR,call */
   aprsstr_Append(qtext, 32ul, logcall, logcall_len);
/*
OE0AAA-9>T8SV40,WIDE1-1,DB0WGS,I:test
OE0AAA-9>T8SV40,WIDE1-1:test
OE0AAA-9>T8SV40,WIDE1-1,qAR,DB0WGS:test 
*/
} /* end Iconstruct() */


static long AprsIs(char buf[], unsigned long buf_len, unsigned long tablen,
                const char logcall[], unsigned long logcall_len,
                unsigned long udpchan, char valid, char * tablechk)
{
   unsigned long qpos;
   unsigned long ha;
   unsigned long ilen;
   unsigned long psum1;
   unsigned long psum0;
   unsigned long pssid;
   unsigned long pins;
   unsigned long len;
   unsigned long p;
   unsigned long i;
   unsigned char hashh;
   unsigned char hashl;
   char qtext[32];
   unsigned long tmp;
   hashl = 0U;
   hashh = 0U;
   p = 0UL;
   pins = 0UL;
   pssid = 0UL; /* for savety */
   len = 0UL;
   while ((len<buf_len-1 && buf[len]) && buf[len]!='\015') ++len;
   if (len==0UL || len+35UL>buf_len-1) return -2L;
   qpos = 0UL;
   *tablechk = 0;
   for (;;) {
      psum0 = p;
      if (!callchk(&qpos, tablechk, buf, buf_len, &p, tablen, &pssid, 0,
                udpchan || callsrc)) return -3L;
      if (buf[p]!='>') return -1L;
      ++p;
      if (!callchk(&qpos, tablechk, buf, buf_len, &p, tablen, &pssid, 0, 0)) {
         return -3L;
      }
      psum1 = pssid;
      while (buf[p]==',') {
         ++p;
         if (!callchk(&qpos, tablechk, buf, buf_len, &p, tablen, &pssid, 1,
                0)) return -3L;
      }
      if (buf[p]!=':') return -1L;
      if (pins==0UL) pins = p;
      ++p;
      if (buf[p]!='}') break;
      ++p;
   }
   if (p>=len) return -2L;
   qtext[0U] = 0;
   if (qpos>0UL) {
      /* qA */
      if (X2C_CAP(buf[qpos])=='Z') return -3L;
      if (X2C_CAP(buf[qpos])=='I') {
         strncpy(qtext,",",32u); /* qAI */
         useri_confappend(useri_fMYCALL, qtext, 32ul);
      }
   }
   else if (udpchan) {
      strncpy(qtext,",qAU,",32u);
      useri_confappend(useri_fMYCALL, qtext, 32ul);
   }
   else if (logcall[0UL]) {
      if (cmpfrom(buf, buf_len, 0UL, logcall, logcall_len)) {
         strncpy(qtext,",qAC,",32u);
      }
      else strncpy(qtext,",qAS,",32u);
      useri_confappend(useri_fMYCALL, qtext, 32ul);
      Iconstruct(qtext, &pins, logcall, logcall_len, buf, buf_len, &p);
   }
   if (buf[p]=='?') return -4L;
   tmp = psum1-1UL;
   i = psum0;
   if (i<=tmp) for (;; i++) {
      aprsstr_HashCh(buf[i], &hashl, &hashh);
      if (i==tmp) break;
   } /* end for */
   while ((p<len && buf[p]!='\015') && buf[p]!='\012') {
      aprsstr_HashCh(buf[p], &hashl, &hashh);
      ++p;
   }
   ha = (unsigned long)(unsigned char)(char)hashl+(unsigned long)
                (unsigned char)(char)hashh*256UL&65535UL;
   if (timehash[ha]+dupetime>aprsdecode_realtime) return -5L;
   /* a duplicate */
   if (udpchan || valid) timehash[ha] = aprsdecode_realtime;
   if (udpchan==0UL && !valid) return -1L;
   buf[p] = '\015';
   ++p;
   buf[p] = '\012';
   ++p;
   buf[p] = 0;
   ++p;
   ilen = aprsstr_Length(qtext, 32ul);
   if (ilen>0UL) {
      while (p>pins) {
         --p;
         buf[p+ilen] = buf[p];
      }
      i = 0UL;
      while (i<ilen) {
         buf[p] = qtext[i];
         ++p;
         ++i;
      }
   }
   return 0L;
} /* end AprsIs() */


static void connstr(aprsdecode_pTCPSOCK cp, char s[], unsigned long s_len)
{
   struct aprsdecode_TCPSOCK * anonym;
   { /* with */
      struct aprsdecode_TCPSOCK * anonym = cp;
      useri_confstr(useri_fMYCALL, s, s_len);
      aprsstr_Append(s, s_len, " connected to ", 15ul);
      aprsstr_Append(s, s_len, anonym->ipnum, 64ul);
      aprsstr_Append(s, s_len, ":", 2ul);
      aprsstr_Append(s, s_len, anonym->port, 6ul);
   }
} /* end connstr() */


static void wrconnected(aprsdecode_pTCPSOCK cp)
{
   char h[301];
   connstr(cp, h, 301ul);
   useri_textautosize(0L, 5L, 6UL, 5UL, 'b', h, 301ul);
} /* end wrconnected() */


extern void aprsdecode_tcpconnstat(char s[], unsigned long s_len)
{
   aprsdecode_pTCPSOCK tcp0;
   char h[100];
   struct aprsdecode_TCPSOCK * anonym;
   tcp0 = aprsdecode_tcpsocks;
   while (tcp0) {
      { /* with */
         struct aprsdecode_TCPSOCK * anonym = tcp0;
         if (anonym->connt>0UL && anonym->connt<aprsdecode_realtime) {
            if (s[0UL]) aprsstr_Append(s, s_len, "\012", 2ul);
            aprsstr_Append(s, s_len, "TCP:", 5ul);
            aprsstr_Append(s, s_len, anonym->ipnum, 64ul);
            aprsstr_Append(s, s_len, ":", 2ul);
            aprsstr_Append(s, s_len, anonym->port, 6ul);
            aprsstr_Append(s, s_len, "\012  Connecttime:", 16ul);
            aprsstr_TimeToStr(aprsdecode_realtime-anonym->connt, h, 100ul);
            aprsstr_Append(s, s_len, h, 100ul);
            if (aprsdecode_lasttcprx<=aprsdecode_realtime && aprsdecode_realtime-aprsdecode_lasttcprx<3600UL)
                 {
               aprsstr_Append(s, s_len, "\012  No rx since:", 16ul);
               aprsstr_TimeToStr(aprsdecode_realtime-aprsdecode_lasttcprx, h,
                 100ul);
               aprsstr_Append(s, s_len, h, 100ul);
            }
            aprsstr_Append(s, s_len, "\012  Rx Frames:", 14ul);
            aprsstr_IntToStr((long)anonym->rxframes, 1UL, h, 100ul);
            aprsstr_Append(s, s_len, h, 100ul);
            aprsstr_Append(s, s_len, "\012  Rx Bytes:", 13ul);
            aprsstr_IntToStr((long)anonym->rxbytes, 1UL, h, 100ul);
            aprsstr_Append(s, s_len, h, 100ul);
            aprsstr_Append(s, s_len, "\012  Tx Frames:", 14ul);
            aprsstr_IntToStr((long)anonym->txframes, 1UL, h, 100ul);
            aprsstr_Append(s, s_len, h, 100ul);
            aprsstr_Append(s, s_len, "\012  Tx Bytes:", 13ul);
            aprsstr_IntToStr((long)anonym->txbytes, 1UL, h, 100ul);
            aprsstr_Append(s, s_len, h, 100ul);
         }
         tcp0 = anonym->next;
      }
   }
} /* end tcpconnstat() */


extern void aprsdecode_udpconnstat(unsigned long port, char s[],
                unsigned long s_len)
{
   char h[51];
   unsigned long i;
   struct aprsdecode_UDPSOCK * anonym;
   if (port<=3UL) {
      { /* with */
         struct aprsdecode_UDPSOCK * anonym = &aprsdecode_udpsocks0[port];
         if (anonym->starttime>0UL) {
            if (aprsdecode_realtime<anonym->starttime) {
               anonym->starttime = aprsdecode_realtime;
            }
            if (s[0UL]) aprsstr_Append(s, s_len, "\012", 2ul);
            aprsstr_Append(s, s_len, "Rf ", 4ul);
            aprsstr_IntToStr((long)(port+1UL), 1UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            aprsstr_Append(s, s_len, ":", 2ul);
            i = 1UL;
            for (;;) {
               useri_conf2str((unsigned char)(35UL+port), i, h, 51ul);
               if (h[0U]==0) break;
               if (i>1UL) aprsstr_Append(s, s_len, " ", 2ul);
               aprsstr_Append(s, s_len, h, 51ul);
               ++i;
            }
            aprsstr_Append(s, s_len, "\012  ", 4ul);
            aprsstr_ipv4tostr(anonym->ip, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            aprsstr_Append(s, s_len, ":", 2ul);
            aprsstr_IntToStr((long)anonym->dport, 0UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            if (anonym->checkip) aprsstr_Append(s, s_len, "/", 2ul);
            else aprsstr_Append(s, s_len, ":", 2ul);
            aprsstr_IntToStr((long)anonym->bindport, 0UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            aprsstr_Append(s, s_len, "\012  Uptime :", 12ul);
            aprsstr_TimeToStr(aprsdecode_realtime-anonym->starttime, h,
                51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            if (anonym->lastudprx>0UL && anonym->lastudprx<=aprsdecode_realtime)
                 {
               aprsstr_Append(s, s_len, "\012  Last rx:", 12ul);
               aprsstr_TimeToStr(aprsdecode_realtime-anonym->lastudprx, h,
                51ul);
               aprsstr_Append(s, s_len, h, 51ul);
            }
            if (anonym->lastudptx>0UL && anonym->lastudptx<=aprsdecode_realtime)
                 {
               aprsstr_Append(s, s_len, "\012  Last tx:", 12ul);
               aprsstr_TimeToStr(aprsdecode_realtime-anonym->lastudptx, h,
                51ul);
               aprsstr_Append(s, s_len, h, 51ul);
            }
            aprsstr_Append(s, s_len, "\012  Rx Frames:", 14ul);
            aprsstr_IntToStr((long)anonym->rxframes, 1UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            aprsstr_Append(s, s_len, "\012  Rx Bytes:", 13ul);
            aprsstr_IntToStr((long)anonym->rxbytes, 1UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            aprsstr_Append(s, s_len, "\012  Tx Frames:", 14ul);
            aprsstr_IntToStr((long)anonym->txframes, 1UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
            aprsstr_Append(s, s_len, "\012  Tx Bytes:", 13ul);
            aprsstr_IntToStr((long)anonym->txbytes, 1UL, h, 51ul);
            aprsstr_Append(s, s_len, h, 51ul);
         }
      }
   }
} /* end udpconnstat() */


extern void aprsdecode_tcpclose(aprsdecode_pTCPSOCK w, char fin)
{
   char h[201];
   struct aprsdecode_TCPSOCK * anonym;
   /*
       WHILE txbuf<>NIL DO
         pb:=txbuf;
         txbuf:=txbuf^.next;
   (*
   WrStrLn("deallocw");
   *)
         DEC(debugmem.mon, SIZE(pb^)); DEALLOCATE(pb, SIZE(pb^));
         IF NOT fin & (txbuf<>NIL) THEN RETURN END;
   
       END;
   */
   { /* with */
      struct aprsdecode_TCPSOCK * anonym = w;
      if ((long)anonym->fd>=0L) osi_CloseSock(anonym->fd);
      anonym->fd = -1L;
      aprsdecode_lasttcprx = 0UL;
      strncpy(h,"TCP Close ",201u);
      aprsstr_Append(h, 201ul, anonym->ipnum, 64ul);
      aprsstr_Append(h, 201ul, ":", 2ul);
      aprsstr_Append(h, 201ul, anonym->port, 6ul);
      useri_textautosize(0L, 5L, 6UL, 4UL, 'e', h, 201ul);
   }
} /* end tcpclose() */


static void Watchbeacon(aprsdecode_pTCPSOCK cp)
{
   char ok0;
   aprsdecode_FRAMEBUF tmp;
   ok0 = Sendtcp(cp, *(aprsdecode_FRAMEBUF *)memcpy(&tmp,"# watchbeacon\015\01\
2",16u));
   cp->watchtime = aprsdecode_realtime;
} /* end Watchbeacon() */


static void appfilter(char s[], unsigned long s_len)
/* build string "filter m/300" */
{
   char h[1000];
   useri_confstr(useri_fSERVERFILT, h, 1000ul);
   if (h[0U]) {
      aprsstr_Append(s, s_len, " filter ", 9ul);
      aprsstr_Append(s, s_len, h, 1000ul);
   }
} /* end appfilter() */


static void updatefilters(aprsdecode_pTCPSOCK cp)
{
   char ok0;
   aprsdecode_FRAMEBUF s;
   strncpy(s,"#",512u);
   appfilter(s, 512ul);
   aprsstr_Append(s, 512ul, "\015\012", 3ul);
   ok0 = Sendtcp(cp, s);
   strncpy(s,"Sent to Server",512u);
   appfilter(s, 512ul);
   useri_textautosize(0L, 5L, 6UL, 4UL, 'b', s, 512ul);
} /* end updatefilters() */


static char tcpconn(aprsdecode_pTCPSOCK * sockchain, long f)
{
   aprsdecode_pTCPSOCK cp;
   aprsdecode_FRAMEBUF h;
   long res;
   char ok0;
   char s[100];
   struct aprsdecode_TCPSOCK * anonym;
   if ((long)f<0L) return 0;
   Storage_ALLOCATE((X2C_ADDRESS *) &cp, sizeof(struct aprsdecode_TCPSOCK));
   useri_debugmem.req = sizeof(struct aprsdecode_TCPSOCK);
   useri_debugmem.mon += useri_debugmem.req;
   if (cp==0) {
      osi_CloseSock(f);
      if (aprsdecode_verb) osi_WrStrLn("tcp conn out of memory", 23ul);
      return 0;
   }
   memset((X2C_ADDRESS)cp,(char)0,sizeof(struct aprsdecode_TCPSOCK));
   { /* with */
      struct aprsdecode_TCPSOCK * anonym = cp;
      anonym->fd = f;
      res = socknonblock(f);
      /*    txbuf:=NIL; */
      anonym->watchtime = aprsdecode_realtime;
   }
   cp->next = *sockchain;
   *sockchain = cp;
   useri_confstr(useri_fMYCALL, s, 100ul);
   if (s[0U]) {
      aprsstr_Assign(h, 512ul, "user ", 6ul);
      aprsstr_Append(h, 512ul, s, 100ul);
      useri_confstr(useri_fPW, s, 100ul);
      if (s[0U]) {
         /* & configon(fPW)*/
         aprsstr_Append(h, 512ul, " pass ", 7ul);
         aprsstr_Append(h, 512ul, s, 100ul);
      }
      aprsstr_Append(h, 512ul, " vers ", 7ul);
      aprsstr_Append(h, 512ul, "aprsmap(cu) 0.46", 17ul);
      appfilter(h, 512ul);
      /*    IF filter[0]<>0C THEN Append(h, " filter ");
                Append(h, filter) END; */
      aprsstr_Append(h, 512ul, "\015\012", 3ul);
      ok0 = Sendtcp(cp, h);
   }
   cp->txframes = 0UL;
   cp->txbytes = 0UL;
   return 1;
} /* end tcpconn() */

/*
PROCEDURE Gateconn(VAR cp:pTCPSOCK);
VAR fd:SOCKET;
    p, act, try, kill:pTCPSOCK;
    max:CARDINAL;
BEGIN
--WrStrLn(" gateconn");
(*
  IF cp<>NIL THEN
    IF (cp^.fd<>CAST(SOCKET,-1)) & NOT configon(fSERVERURL) THEN
      saybusy(kill^.fd, "");  
    END;
  ELSIF 
*)
  act:=NIL;
  try:=NIL;
  p:=cp;
  WHILE p<>NIL DO
    IF p^.connt>0 THEN                                            (* we have gate connect *)
      IF act<>NIL THEN                                            (* we have 2 connects *)
        IF act^.gatepri>p^.gatepri THEN (* stop second best *)
          kill:=act; 
          act:=p;
        ELSE kill:=p END;                                         (* stop second best *)
        saybusy(kill^.fd, "# remove double connect"+CR+LF)  
      ELSE act:=p END;
    ELSE try:=p END;                                              (* trying to connect *)
    p:=p^.next;
  END;
  IF (try=NIL) & Watchclock(connecttime,
                GATECONNDELAY) THEN                                                   (* no trials *)
    IF act=NIL THEN max:=HIGH(gateways) ELSE max:=act^.gatepri END;
                (* try only better *)
    IF (trygate>=max) OR (trygate>HIGH(gateways)) 
    OR (gateways[trygate].url[0]=0C) THEN trygate:=0 END;
                (* loop thru table *) 

    IF trygate<max THEN
      WITH gateways[trygate] DO
        IF url[0]<>0C THEN
          fd:=connectto(url, port);
--WrInt(fd, 1);WrStrLn(" connfd");

          IF CAST(INTEGER,fd)>=0 THEN
            IF tcpconn(cp, fd, filterst) THEN
              Assign(cp^.ipnum, url);
              Assign(cp^.port, port);
              cp^.gatepri:=trygate;
--            Assign(h, servercall);
--            Append(h, " connected to "); Append(h, url); Append(h, ":");
                Append(h, port);
--            xerrmsg(h);
            END;
          END; 
        END;
      END;
    END;
    INC(trygate);

  END;
END Gateconn;
*/

static void urlport(char s[], unsigned long s_len, char url[],
                unsigned long url_len, char port[], unsigned long port_len)
/* url:port to url port */
{
   long i;
   X2C_PCOPY((void **)&s,s_len);
   aprsstr_Assign(url, url_len, s, s_len);
   i = aprsstr_InStr(url, url_len, ":", 2ul);
   if (i>0L) {
      url[i] = 0;
      aprsstr_Delstr(s, s_len, 0UL, (unsigned long)(i+1L));
      aprsstr_Assign(port, port_len, s, s_len);
   }
   else port[0UL] = 0;
   X2C_PFREE(s);
} /* end urlport() */

#define aprsdecode_DEFAULTPORT "14580"


static void Gateconn(aprsdecode_pTCPSOCK * cp)
{
   long fd;
   char port[1000];
   char ip[1000];
   char s[1000];
   /*WrInt(cycleservers, 5); WrStrLn(" gateconn"); */
   if (*cp) {
      if ((long)(*cp)->fd>=0L && !useri_configon(useri_fCONNECT)) {
         saybusy(&(*cp)->fd, "", 1ul);
      }
   }
   else if (useri_configon(useri_fCONNECT) && Watchclock(&connecttime,
                30UL)) {
      for (;;) {
         useri_confstrings(useri_fSERVERURL, cycleservers, 0, s, 1000ul);
         if (s[0U] || cycleservers==0UL) break;
         cycleservers = 0UL;
      }
      if (s[0U]) {
         ++cycleservers;
         urlport(s, 1000ul, ip, 1000ul, port, 1000ul);
         if (port[0U]==0) strncpy(port,"14580",1000u);
         /*WrInt(cycleservers, 5);WrStr(" <"); WrStr(ip);WrStr("><");
                WrStr(port);WrStrLn(">"); */
         fd = connectto(ip, port);
         if ((long)fd>=0L) {
            if (tcpconn(cp, fd)) {
               aprsstr_Assign((*cp)->ipnum, 64ul, ip, 1000ul);
               aprsstr_Assign((*cp)->port, 6ul, port, 1000ul);
               (*cp)->beacont = 0UL; /* send next beacon now */
            }
         }
      }
      else useri_xerrmsg("Net: No Online/Server URL configured", 37ul);
   }
} /* end Gateconn() */


static void porttext(unsigned long p, char s[], unsigned long s_len)
{
   char h[31];
   aprsstr_IntToStr((long)(p+1UL), 1UL, h, 31ul);
   aprsstr_Assign(s, s_len, "UDP ", 5ul);
   aprsstr_Append(s, s_len, h, 31ul);
} /* end porttext() */


static void rfbeacons(void)
{
   unsigned long i;
   unsigned long nb;
   long j;
   unsigned long shift;
   unsigned long bshift;
   unsigned long bn;
   unsigned long bt;
   aprsdecode_FRAMEBUF s;
   char h[101];
   char say[101];
   char port;
   struct aprsdecode_DAT dat;
   char tmp;
   nb = 0UL;
   shift = beaconrandomstart;
   bshift = (unsigned long)useri_conf2int(useri_fRFBTSHIFT, 0UL, 0L, 86400L,
                120L);
   if (rfbecondone==0UL || rfbecondone>aprsdecode_realtime) {
      rfbecondone = aprsdecode_realtime;
   }
   for (;;) {
      if (aprsdecode_testbeaconbuf[0UL]) {
         /* manual sent beacon */
         aprsstr_Assign(s, 512ul, aprsdecode_testbeaconbuf, 512ul);
         aprsdecode_testbeaconbuf[0UL] = 0;
         strncpy(say,"Sent: ",101u);
         aprsstr_Append(say, 101ul, s, 512ul);
         if (getbeaconparm(s, 512ul, &bt, &port)) bt = 1UL;
         else bt = 0UL;
      }
      else {
         useri_confstrings(useri_fRBTEXT, nb, 0, s, 512ul);
         if (s[0UL]==0) break;
         if (!getbeaconparm(s, 512ul, &bt, &port)) bt = 0UL;
         say[0U] = 0;
      }
      if (bt>0UL) {
         /* valid time */
         bn = (aprsdecode_realtime+shift)%bt;
         /*WrInt(realtime MOD 86400, 10); WrInt(realtime-rfbecondone, 4);
                WrInt(bshift, 4); WrInt(shift, 4); WrInt(n, 4);
                WrStrLn(" b"); */
         if (bn<aprsdecode_realtime-rfbecondone) {
            beaconmacros(s, 512ul, "", 1ul, "", 1ul, 1);
            if (s[0UL]) {
               if (aprsdecode_Decode(s, 512ul, &dat)>=0L) {
                  if ((unsigned char)dat.symt<' ') {
                     strncpy(h,"beacon warning: no symbol: ",101u);
                     aprsstr_Append(h, 101ul, s, 512ul);
                     useri_xerrmsg(h, 101ul);
                  }
                  if (X2C_CAP(port)=='N') {
                     if (dat.type!=aprsdecode_MICE) {
                        j = aprsstr_InStr(s, 512ul, ":", 2ul);
                        if (j>=0L) {
                           aprsstr_Delstr(s, 512ul, 0UL,
                (unsigned long)(j+1L)); /* remove rf adress */
                           SendNet(aprsdecode_tcpsocks, s, 512ul, 1, h,
                101ul);
                           if (h[0U]) {
                              strncpy(say,"beacon: not sent ",101u);
                              aprsstr_Append(say, 101ul, h, 101ul);
                           }
                        }
                     }
                     else strncpy(say,"beacon: not sent mic-e to Net",101u);
                  }
                  else if ((unsigned char)port>='1') {
                     if (s[0UL]) {
                        i = (unsigned long)(unsigned char)port-49UL;
                        if (i<4UL && useri_configon((unsigned char)(35UL+i)))
                 {
                           if (!Sendudp(s, 512ul, i)) {
                              strncpy(say,"beacon: Rfport ",101u);
                              aprsstr_Append(say, 101ul,
                (char *)(tmp = (char)(i+49UL),&tmp), 1u/1u);
                              aprsstr_Append(say, 101ul, " not configured to \
send", 24ul);
                           }
                        }
                        else {
                           strncpy(say,"beacon: Rfport ",101u);
                           aprsstr_Append(say, 101ul,
                (char *)(tmp = (char)(i+49UL),&tmp), 1u/1u);
                           aprsstr_Append(say, 101ul, " not enabled", 13ul);
                        }
                     }
                  }
                  if (say[0U]) {
                     useri_textautosize(0L, 0L, 4UL, 2UL, 'b', say, 101ul);
                  }
               }
               else {
                  strncpy(h,"rfbeacon not decodeable: ",101u);
                  aprsstr_Append(h, 101ul, s, 512ul);
                  useri_xerrmsg(h, 101ul);
               }
            }
         }
         shift += bshift; /* delay next beacon by shift time to before beacon */
      }
      ++nb;
   }
   rfbecondone = aprsdecode_realtime;
} /* end rfbeacons() */


static void startserial(struct xosi_PROCESSHANDLE * pid, unsigned char cfg)
{
   char h[1001];
   char s[1001];
   if (useri_configon(cfg)) {
      if (!(pid->runs || pid->started)) {
         useri_confstr(cfg, s, 1001ul);
         if (s[0U]) {
            xosi_StartProg(s, 1001ul, pid);
            pid->started = 1;
            if (pid->runs) {
               useri_textautosize(0L, 0L, 4UL, 3UL, 'g', "Start Serial Interf\
ace", 23ul);
            }
            else {
               strncpy(h,"can not start ",1001u);
               aprsstr_Append(h, 1001ul, s, 1001ul);
               useri_xerrmsg(h, 1001ul);
               useri_configbool(cfg, 0);
               useri_refresh = 1;
            }
         }
      }
   }
   else pid->started = 0;
} /* end startserial() */


extern void aprsdecode_tcpjobs(void)
{
   aprsdecode_pTCPSOCK acttcp;
   long i;
   long len;
   char ok0;
   char s[1000];
   struct aprsdecode_TCPSOCK * anonym;
   /*        IF beacont<realtime THEN Timebeacon(acttcp) END; */
   struct aprsdecode_UDPSOCK * anonym0;
   if (qwatchtime!=aprsdecode_realtime) {
      acttcp = aprsdecode_tcpsocks;
      while (acttcp) {
         { /* with */
            struct aprsdecode_TCPSOCK * anonym = acttcp;
            Timebeacon(acttcp);
            if (anonym->watchtime+180UL<aprsdecode_realtime) {
               Watchbeacon(acttcp);
            }
            if (useri_configon(useri_fALLOWGATE)) WatchTXQ(acttcp);
            if (useri_isupdated(useri_fSERVERFILT)) updatefilters(acttcp);
            acttcp = anonym->next;
         }
      }
      qwatchtime = aprsdecode_realtime;
      Gateconn(&aprsdecode_tcpsocks);
      /*  IF configon(fRFBTSHIFT) THEN rfbeacons END; */
      rfbeacons();
   }
   /*UDP */
   for (i = 0L; i<=3L; i++) {
      { /* with */
         struct aprsdecode_UDPSOCK * anonym0 = &aprsdecode_udpsocks0[i];
         if (useri_configon((unsigned char)(35UL+(unsigned long)i))
                && !useri_isupdated((unsigned char)(35UL+(unsigned long)i))) {
            if ((long)anonym0->fd<0L) {
               ok0 = 0;
               useri_confstr((unsigned char)(35UL+(unsigned long)i), s,
                1000ul);
               if (s[0U]) {
                  memset((char *) &aprsdecode_udpsocks0[i],(char)0,
                sizeof(struct aprsdecode_UDPSOCK));
                  anonym0->fd = -1L;
                  if (aprsstr_GetIp2(s, 1000ul, &anonym0->ip,
                &anonym0->dport, &anonym0->bindport, &anonym0->checkip)<0L) {
                     anonym0->rawread = 1;
                     porttext((unsigned long)i, s, 1000ul);
                     aprsstr_Append(s, 1000ul, " Config Error <IPNumber:TxPor\
t:RxPort>", 39ul);
                     useri_textautosize(0L, 5L, 6UL, 10UL, 'e', s, 1000ul);
                  }
                  else {
                     anonym0->fd = openudp();
                     anonym0->rawread = 1;
                     porttext((unsigned long)i, s, 1000ul);
                     if ((long)anonym0->fd<0L || bindudp(anonym0->fd,
                anonym0->bindport)<0L) {
                        aprsstr_Append(s, 1000ul, " Bindport Error, Illegal (\
Rx)Port or in Use", 44ul);
                        useri_textautosize(0L, 5L, 6UL, 10UL, 'e', s,
                1000ul);
                        osi_CloseSock(anonym0->fd);
                        anonym0->fd = -1L;
                     }
                     else {
                        aprsstr_Append(s, 1000ul, " Open", 6ul);
                        len = socknonblock(anonym0->fd);
                        useri_textautosize(0L, 5L, 6UL, 5UL, 'b', s, 1000ul);
                        ok0 = 1;
                     }
                  }
               }
               else {
                  porttext((unsigned long)i, s, 1000ul);
                  aprsstr_Append(s, 1000ul, " Not Configured", 16ul);
                  useri_textautosize(0L, 5L, 6UL, 10UL, 'e', s, 1000ul);
               }
               if (!ok0) {
                  useri_configbool((unsigned char)(35UL+(unsigned long)i),
                0);
               }
            }
            startserial(&aprsdecode_serialpid, useri_fSERIALTASK);
            startserial(&aprsdecode_serialpid2, useri_fSERIALTASK2);
         }
         else if ((long)anonym0->fd>=0L) {
            osi_CloseSock(anonym0->fd);
            anonym0->fd = -1L;
            anonym0->starttime = 0UL;
            porttext((unsigned long)i, s, 1000ul);
            aprsstr_Append(s, 1000ul, " Close", 7ul);
            useri_textautosize(0L, 5L, 6UL, 4UL, 'e', s, 1000ul);
         }
      }
   } /* end for */
   sendmsg();
   /*WrInt(tickertime, 15); WrInt(realtime, 15); WrLn; */
   if (tickertime+60UL<aprsdecode_realtime) Stopticker();
} /* end tcpjobs() */


static void beepprox(float d, float km)
{
   if ((useri_configon(useri_fBEEPPROX) && km!=0.0f) && d<km) {
      d = 1.0f-X2C_DIVR(d,km);
      d = d*d;
      d = d*d;
      aprsdecode_beeplim(100L, aprsdecode_trunc((1.0f-d)*(float)useri_conf2int(useri_fBEEPPROX,
                 0UL, 20L, 8000L,
                400L)+d*(float)useri_conf2int(useri_fBEEPPROX, 2UL, 20L,
                8000L, 400L)), aprsdecode_trunc((1.0f-d)*(float)useri_conf2int(useri_fBEEPPROX,
                 1UL, 0L, 5000L, 40L)+d*(float)useri_conf2int(useri_fBEEPPROX,
                 3UL, 0L, 5000L, 1000L)));
   }
} /* end beepprox() */

#define aprsdecode_LOOKBACK 180
/* last s look if nearest pos is last */


static void approxywarn(struct aprspos_POSITION pos, const char call[],
                unsigned long call_len)
{
   char s[31];
   char h[101];
   float d;
   float mino;
   float mind;
   float km;
   struct aprspos_POSITION mypos;
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST pf;
   long fd;
   km = useri_conf2real(useri_fAPPROXY, 0UL, 0.0f, 30000.0f, 0.0f);
   if (km!=0.0f) {
      if (aprsdecode_getmypos(&mypos)) {
         d = aprspos_distance(pos, mypos);
         if (d<=km) {
            /* quick check if it is in radius */
            op = aprsdecode_ophist0;
            while (op && X2C_STRCMP(op->call,9u,call,call_len)) {
               op = op->next;
            }
            if (op) {
               mino = 30000.0f;
               mind = 30000.0f;
               pf = op->frames;
               while (pf) {
                  if (pf->time0+180UL>aprsdecode_realtime && (pf->nodraw&0x4FU)
                ==0U) {
                     d = aprspos_distance(pf->vardat->pos, mypos);
                     /*
                     FixToStr(distance(pf^.vardat^.pos, mypos), 4, s);
                WrStr(s); WrStr(" ");
                     FixToStr(distance(mypos, pf^.vardat^.pos), 4, s);
                WrStr(s); WrStr(" ");
                     postostr(pf^.vardat^.pos, s); WrStr(s); WrStr(" ");
                postostr(mypos, s); WrStrLn(s);
                     */
                     mino = mind;
                     if (d<mind) mind = d;
                  }
                  pf = pf->next;
               }
               if ((mino<29000.0f && mino!=d) && mino>d) {
                  beepprox(d, km);
                  strncpy(h,"approach ",101u);
                  aprsstr_FixToStr(d, 3UL, s, 31ul);
                  aprsstr_Append(h, 101ul, s, 31ul);
                  aprsstr_Append(h, 101ul, "km ", 4ul);
                  aprsstr_Append(h, 101ul, call, call_len);
                  useri_textautosize(0L, 5L, 6UL, 120UL, 'r', h, 101ul);
                  fd = osi_OpenWrite("proxwarn", 9ul);
                  if (osi_FdValid(fd)) {
                     aprsstr_Assign(h, 101ul, call, call_len);
                     aprsstr_IntToStr((long)aprsdecode_trunc(d*1000.0f), 0UL,
                 s, 31ul);
                     aprsstr_Append(h, 101ul, " p ", 4ul);
                     aprsstr_Append(h, 101ul, s, 31ul);
                     osi_WrBin(fd, (char *)h, 101u/1u, aprsstr_Length(h,
                101ul));
                     osi_Close(fd);
                  }
               }
            }
         }
      }
   }
} /* end approxywarn() */


extern char aprsdecode_IsBulletin(struct aprsdecode_DAT dat)
{
   return ((dat.type==aprsdecode_MSG && dat.msgto[0UL]=='B')
                && dat.msgto[1UL]=='L') && dat.msgto[2UL]=='N';
} /* end IsBulletin() */

#define aprsdecode_DUPTIME 27

#define aprsdecode_NN "n"

#define aprsdecode_BAD "x"

#define aprsdecode_VIA "v"


static void app(unsigned long * viac, aprsdecode_FRAMEBUF tb,
                unsigned long * tp, char c)
{
   if (*tp<511UL) {
      tb[*tp] = c;
      ++*tp;
   }
   if (c==',') ++*viac;
} /* end app() */


static char via(char words[], unsigned long words_len, aprsdecode_FRAMEBUF b,
                 char typ, unsigned long from, unsigned long to)
{
   unsigned long i;
   char h[31];
   if (from>=to) return 0;
   h[0U] = typ;
   i = 1UL;
   while (i<28UL && from<to) {
      h[i] = b[from];
      ++from;
      ++i;
   }
   h[i] = ' ';
   h[i+1UL] = 0;
   return aprsstr_InStr(words, words_len, h, 31ul)>=0L;
} /* end via() */


static char num0(aprsdecode_FRAMEBUF b, unsigned long i, unsigned long * n)
{
   if ((i<511UL && (unsigned char)b[i]>='0') && (unsigned char)b[i]<='9') {
      *n = (unsigned long)(unsigned char)b[i]-48UL;
      return 1;
   }
   return 0;
} /* end num() */


static char scanpath(char words[], unsigned long words_len,
                unsigned long * tp, aprsdecode_FRAMEBUF tb,
                unsigned long * viac, aprsdecode_FRAMEBUF b,
                unsigned long * p, unsigned long to, char store)
{
   unsigned long hp;
   hp = *p;
   while (*p<to) {
      if (store) app(viac, tb, tp, b[*p]);
      if (*p+1UL>=to || b[*p]==',') {
         if (via(words, words_len, b, 'x', hp+1UL,
                *p+(unsigned long)(*p+1UL>=to))) return 0;
         /* bad word */
         hp = *p;
      }
      ++*p;
   }
   return 1;
} /* end scanpath() */


static void digi(const aprsdecode_FRAMEBUF b, unsigned long udpch,
                unsigned long outport, char directonly, char appendrest,
                unsigned long nomovetime, unsigned long radius, char words[],
                 unsigned long words_len)
{
   struct aprsdecode_DAT dat;
   unsigned long pn;
   unsigned long viac;
   unsigned long n2;
   unsigned long n1;
   unsigned long n;
   unsigned long ssid;
   unsigned long len;
   unsigned long tp;
   unsigned long p;
   aprsdecode_FRAMEBUF tb;
   unsigned char hashh;
   unsigned char hashl;
   unsigned long dt;
   char norout;
   struct aprspos_POSITION mypos;
   char s2[31];
   char s1[31];
   char mycall[31];
   struct aprsdecode_UDPSOCK * anonym;
   if (nomovetime<27UL) nomovetime = 27UL;
   if (udpch==0UL) return;
   --udpch;
   if (udpch>3UL) return;
   tp = 0UL;
   p = 0UL;
   len = 0UL;
   ssid = 0UL;
   viac = 0UL;
   useri_confstr(useri_fMYCALL, mycall, 31ul);
   if (mycall[0U]==0) {
      useri_xerrmsg("Digi needs \'My Call\'", 21ul); /* no mycall */
      return;
   }
   norout = aprsstr_InStr(words, words_len, " x-", 4ul)>=0L;
                /* ssid routing off */
   if (aprsstr_InStr(words, words_len, " xi", 4ul)<0L) {
      aprsstr_Append(words, words_len, " v", 3ul);
                /* if not diabled append mycall for via */
      aprsstr_Append(words, words_len, mycall, 31ul);
   }
   aprsstr_Append(words, words_len, " x", 3ul); /* mycall as badword */
   aprsstr_Append(words, words_len, mycall, 31ul);
   aprsstr_Append(words, words_len, " ", 2ul);
   while ((len<511UL && b[len]) && b[len]!=':') ++len;
   while (p<len && b[p]!='>') {
      app(&viac, tb, &tp, b[p]); /* end of src call */
      ++p;
   }
   if (via(words, words_len, b, 'x', 0UL, p)) return;
   /* bad src call */
   if (useri_configon(useri_fMUSTBECALL)) {
      tb[tp] = 0;
      if (!isacall(tb, 512ul)) return;
   }
   n = p+1UL;
   while ((p<len && b[p]!='-') && b[p]!=',') {
      app(&viac, tb, &tp, b[p]); /* ssid or end of dst call */
      ++p;
   }
   if (via(words, words_len, b, 'x', n, p)) return;
   /* bad dest call */
   if (b[p]=='-') {
      /* we have a ssid */
      ++p;
      if (num0(b, p, &n)) {
         ssid = n;
         ++p;
         if (num0(b, p, &n)) {
            ssid = ssid*10UL+n;
            ++p;
         }
      }
   }
   if (ssid==0UL || ssid>7UL) norout = 1;
   if (!norout) --ssid;
   if (ssid>0UL) {
      /* write old or new ssid */
      app(&viac, tb, &tp, '-');
      if (ssid>=10UL) app(&viac, tb, &tp, '1');
      app(&viac, tb, &tp, (char)(ssid%10UL+48UL));
   }
   n = p;
   while (n<len && b[n]!='*') ++n;
   if (n<len) {
      if (directonly) return;
      /* not direct heard */
      if (!scanpath(words, words_len, &tp, tb, &viac, b, &p, n, 1)) return;
      /* append path till * and find badwords */
      ++p;
   }
   app(&viac, tb, &tp, ',');
   n = 0UL;
   while (mycall[n]) {
      app(&viac, tb, &tp, mycall[n]); /* append mycall */
      ++n;
   }
   app(&viac, tb, &tp, '*');
   if (p<len) {
      /* look for via routing */
      norout = 1;
      if (b[p]==',') ++p;
      n = p;
      while (p<len && b[p]!=',') ++p;
      if (((((((via(words, words_len, b, 'n', n, p-3UL) && num0(b, p-3UL,
                &n1)) && n1>0UL) && n1<=7UL) && b[p-2UL]=='-') && num0(b,
                p-1UL, &n2)) && n2>0UL) && n2<=n1) {
         if (directonly && n1!=n2) return;
         /* not direct heard */
         if (appendrest && n2>1UL) {
            /* append not n-0 */
            app(&viac, tb, &tp, ',');
            pn = n;
            while (pn+3UL<p) {
               app(&viac, tb, &tp, b[pn]); /* append via word */
               ++pn;
            }
            app(&viac, tb, &tp, (char)(n1+48UL)); /* append n-n */
            app(&viac, tb, &tp, '-');
            app(&viac, tb, &tp, (char)(n2+47UL));
         }
         norout = 0; /* append rest path later*/
      }
      else if (via(words, words_len, b, 'v', n, p)) norout = 0;
   }
   if (norout) return;
   /* nothing to digipeat */
   if (!scanpath(words, words_len, &tp, tb, &viac, b, &p, len, appendrest)) {
      return;
   }
   /* append rest path and find badwords */
   if (viac>7UL) return;
   /* too much via */
   p = len;
   while (p<511UL && b[p]) {
      app(&viac, tb, &tp, b[p]);
      ++p;
   }
   app(&viac, tb, &tp, 0);
   if (aprsdecode_Decode(tb, 512ul, &dat)<0L) return;
   /*radius */
   if (dat.type!=aprsdecode_MSG && radius>0UL) {
      if (!aprsdecode_getmypos(&mypos)) {
         useri_xerrmsg("Digi with Radius needs MY Position", 35ul);
         return;
      }
      /*IF posvalid(dat.pos) THEN WrFixed(distance(mypos, dat.pos), 1, 6);
                WrStrLn("=radius"); END; */
      if (!aprspos_posvalid(dat.pos) || aprspos_distance(mypos,
                dat.pos)>(float)radius) return;
   }
   /* dupe */
   hashl = 0U;
   hashh = 0U;
   p = 0UL;
   n1 = 0UL;
   n2 = 0UL;
   for (;;) {
      while (tb[p] && tb[p]!=':') ++p;
      if (tb[p]!=':') break;
      n1 = p;
      ++p;
      if (tb[p]!='}') break;
      ++p;
      n2 = p;
   }
   p = n2;
   while (p<n1 && tb[p]!='>') {
      aprsstr_HashCh(tb[p], &hashl, &hashh);
      ++p;
   }
   while ((p<n1 && tb[p]!='-') && tb[p]!=',') {
      aprsstr_HashCh(tb[p], &hashl, &hashh);
      ++p;
   }
   p = n1;
   while (tb[p]) {
      aprsstr_HashCh(tb[p], &hashl, &hashh);
      ++p;
   }
   n = (unsigned long)(unsigned char)(char)hashl+(unsigned long)
                (unsigned char)(char)hashh*256UL&65535UL;
   if (dat.type==aprsdecode_MSG && !aprsdecode_IsBulletin(dat)) dt = 27UL;
   else dt = nomovetime;
   if (outport>0UL && outport-1UL<=3UL) {
      { /* with */
         struct aprsdecode_UDPSOCK * anonym = &aprsdecode_udpsocks0[outport-1UL]
                ;
         if (anonym->pdupetimes==0) {
            Storage_ALLOCATE((X2C_ADDRESS *) &anonym->pdupetimes, 262144UL);
            useri_debugmem.req = 262144UL;
            useri_debugmem.mon += 262144UL;
            if (anonym->pdupetimes==0) {
               osi_WrStrLn("duptime out of memory", 22ul);
               useri_wrheap();
               return;
            }
            memset((X2C_ADDRESS)anonym->pdupetimes,(char)0,262144UL);
         }
         if (anonym->pdupetimes[n]+dt>aprsdecode_realtime) {
            /*WrInt(pdupetimes^[n]+dt-realtime, 5); WrStrLn("=age");*/
            return;
         }
         anonym->pdupetimes[n] = aprsdecode_realtime;
      }
   }
   if ((outport==0UL || outport-1UL>3UL) || !Sendudp(tb, 512ul,
                outport-1UL)) {
      strncpy(s1,"aprsdigi:can not send to port ",31u);
      aprsstr_IntToStr((long)outport, 0UL, s2, 31ul);
      aprsstr_Append(s1, 31ul, s2, 31ul);
      useri_xerrmsg(s1, 31ul);
   }
/*WrInt(udpch, 10); WrInt(dt, 10); WrInt(n, 10); WrStrLn("=h"); WrStrLn(tb);
                */
} /* end digi() */


static void digipeat(const aprsdecode_FRAMEBUF b, unsigned long udpch)
{
   unsigned long line;
   unsigned long outport;
   unsigned long radius;
   unsigned long nomovetime;
   char directonly;
   char appendrest;
   char words[201];
   char word[11];
   char c;
   line = 0UL;
   for (;;) {
      useri_confstrings(useri_fDIGI, line, 0, words, 201ul);
      if (words[0U]==0) break;
      /*words:="*42 890 300 TCPIP* WIDEn TRACEn RELAY ECHO GATE ";  */
      c = words[0U]; /* input port */
      if (c=='*' || c==(char)(udpch+48UL)) {
         /* our input port */
         c = words[1U]; /* output port */
         if ((unsigned char)c>='1') {
            outport = (unsigned long)(unsigned char)c-48UL;
         }
         else outport = 0UL;
         directonly = 1;
         appendrest = 0;
         switch ((unsigned)words[2U]) { /* direct/delpath */
         case '1':
            appendrest = 1;
            break;
         case '2':
            directonly = 0;
            break;
         case '3':
            directonly = 0;
            appendrest = 1;
            break;
         } /* end switch */
         aprsstr_Extractword(words, 201ul, word, 11ul);
         aprsstr_Extractword(words, 201ul, word, 11ul);
         if (aprsstr_StrToCard(word, 11ul, &nomovetime)) {
            aprsstr_Extractword(words, 201ul, word, 11ul);
            if (aprsstr_StrToCard(word, 11ul, &radius)) {
               digi(b, udpch, outport, directonly, appendrest, nomovetime,
                radius, words, 201ul);
            }
         }
      }
      ++line;
   }
} /* end digipeat() */

static char rfhered;

#define aprsdecode_PURGEINTERVALL 10
/* seconds+1 with no purge */


static void storedata(aprsdecode_FRAMEBUF mb, aprsdecode_pTCPSOCK cp,
                unsigned long udpch, char valid, char local)
{
   long res;
   char fn[1000];
   char s[21];
   aprsdecode_MONCALL server;
   char tabchk;
   char ok0;
   aprsdecode_pTCPSOCK tp;
   struct aprsdecode_DAT dat;
   char dir;
   char tmp;
   if (aprsdecode_lastpurge!=aprsdecode_realtime/10UL) {
      if (aprsdecode_lums.logmode) {
         aprsdecode_purge(&aprsdecode_ophist2,
                aprsdecode_realtime-aprsdecode_lums.purgetime,
                aprsdecode_realtime-aprsdecode_lums.purgetimeobj);
      }
      else {
         aprsdecode_purge(&aprsdecode_ophist0,
                aprsdecode_realtime-aprsdecode_lums.purgetime,
                aprsdecode_realtime-aprsdecode_lums.purgetimeobj);
      }
      aprsdecode_lastpurge = aprsdecode_realtime/10UL;
   }
   if (cp) memcpy(server,cp->user.call,9u);
   else server[0UL] = 0;
   /*WrInt(rxidle, 15); WrInt(lastlooped+IGATEMAXDELAY, 15);
                WrInt(time(), 15); WrStrLn(" ig"); */
   if (local) res = 0L;
   else if (aprsdecode_rxidle>=3UL && aprsdecode_lastlooped+5UL>=TimeConv_time()
                ) {
      /* not too long delayd data */
      digipeat(mb, udpch);
      res = AprsIs(mb, 512ul, 6UL, server, 9ul, udpch, valid, &tabchk);
      if (((res>=0L && useri_configon(useri_fALLOWGATE)) && !tabchk)
                && udpch>0UL) {
         useri_confstr(useri_fALLOWGATE, s, 21ul);
         if (aprsstr_InStr(s, 21ul, (char *)(tmp = (char)(udpch+48UL),&tmp),
                1u/1u)>=0L) {
            tp = aprsdecode_tcpsocks;
            while (tp) {
               ok0 = Sendtcp(tp, mb);
               tp = tp->next;
            }
         }
      }
   }
   else {
      /* delayed */
      res = 0L;
      if (aprsdecode_verb) {
         osi_WrStrLn("---discarded delayed digpeat-frame", 35ul);
      }
   }
   if (aprsdecode_verb && udpch>0UL) {
      InOut_WriteInt(res, 3UL);
      InOut_WriteString(" udp ", 6ul);
      osi_WrStrLn(mb, 512ul);
   }
   if (res>=0L && aprsdecode_Decode(mb, 512ul, &dat)>=0L) {
      if (dat.type==aprsdecode_MSG) {
         if (udpch>0UL) getmessage(0, udpch, dat);
         else getmessage(cp, 0UL, dat);
      }
      if (aprsdecode_lums.logmode) {
         res = aprsdecode_Stoframe(&aprsdecode_ophist2, mb, 512ul,
                aprsdecode_realtime, 0, dat);
      }
      else {
         res = aprsdecode_Stoframe(&aprsdecode_ophist0, mb, 512ul,
                aprsdecode_realtime, 0, dat);
      }
      if (!local) {
         if (useri_configon(useri_fLOGWFN)) {
            useri_confstr(useri_fLOGWFN, fn, 1000ul);
            if (!wrlog(mb, 512ul, aprsdecode_realtime, fn, 1000ul, res)) {
               poplogerr(fn, 1000ul);
            }
         }
         if (aprspos_posvalid(dat.pos) && useri_configon(useri_fAPPROXY)) {
            approxywarn(dat.pos, dat.srccall, 9ul);
         }
      }
   }
   if (res==-2L) {
      dir = '#';
   }
   else if (res<0L) dir = '-';
   else dir = '>';
   if (udpch>0UL) wrmon(udpch, dir, mb, 512ul);
   else wrmon(0UL, dir, mb, 512ul);
} /* end storedata() */


extern void aprsdecode_drawbeacon(char raw[], unsigned long raw_len)
{
   aprsdecode_FRAMEBUF b;
   char port;
   unsigned long time0;
   aprsstr_Assign(b, 512ul, raw, raw_len);
   if (getbeaconparm(b, 512ul, &time0, &port)) {
      beaconmacros(b, 512ul, "", 1ul, "", 1ul, 1);
      storedata(b, 0, 0UL, 0, 1);
      if (aprsdecode_click.mhop[0UL]) {
         useri_textautosize(0L, 0L, 4UL, 5UL, 'r', "switch to \"show all\" to\
 see new object", 39ul);
      }
   }
   else useri_xerrmsg("decode error", 13ul);
} /* end drawbeacon() */


extern void aprsdecode_udpin(unsigned long port)
{
   aprsdecode_FRAMEBUF mbuf;
   struct aprsdecode_UDPSOCK * anonym;
   while (getudp(port, mbuf)) {
      if (mbuf[0UL]) {
         { /* with */
            struct aprsdecode_UDPSOCK * anonym = &aprsdecode_udpsocks0[port];
                
            anonym->lastudprx = aprsdecode_realtime;
            ++anonym->rxframes;
            anonym->rxbytes += aprsstr_Length(mbuf, 512ul);
            if (anonym->starttime==0UL) {
               anonym->starttime = aprsdecode_realtime;
            }
         }
         aprsdecode_lastanyudprx = aprsdecode_realtime;
         storedata(mbuf, 0, port+1UL, 0, 0);
      }
      else if (aprsdecode_verb) osi_WrStrLn("axudp decode error", 19ul);
   }
} /* end udpin() */


extern long aprsdecode_tcpout(aprsdecode_pTCPSOCK acttcp)
{
   long i;
   long res;
   struct aprsdecode_TCPSOCK * anonym;
   long tmp;
   if ((long)acttcp->fd>=0L) {
      { /* with */
         struct aprsdecode_TCPSOCK * anonym = acttcp;
         res = sendsock(anonym->fd, anonym->tbuf, anonym->tlen);
         if (res>0L) {
            tmp = anonym->tlen-1L;
            i = res;
            if (i<=tmp) for (;; i++) {
               anonym->tbuf[i-res] = anonym->tbuf[i];
               if (i==tmp) break;
            } /* end for */
            anonym->tlen -= res;
            if (anonym->connt==0UL) {
               anonym->connt = aprsdecode_realtime;
               wrconnected(acttcp);
            }
            anonym->watchtime = aprsdecode_realtime;
         }
      }
   }
   else {
      /*    ELSE tlen:=0;                        (* should not happen *) */
      res = -1L;
   }
   return res;
} /* end tcpout() */


extern void aprsdecode_tcpin(aprsdecode_pTCPSOCK acttcp)
{
   long res;
   aprsdecode_FRAMEBUF mbuf;
   if ((long)acttcp->fd>=0L) {
      for (;;) {
         res = Gettcp(acttcp->fd, mbuf, acttcp->rbuf, &acttcp->rpos);
         /*WrInt(res, 1);WrStrLn(" gettcp"); */
         if (res<0L) {
            /*WrStrLn("closesock"); */
            osi_CloseSock(acttcp->fd);
            acttcp->fd = -1L;
            break;
         }
         if (res<=0L) break;
         ++acttcp->rxframes;
         acttcp->rxbytes += aprsstr_Length(mbuf, 512ul);
         acttcp->watchtime = aprsdecode_realtime;
         /*    WrMon(mbuf); */
         storedata(mbuf, acttcp, 0UL, 1, 0);
         aprsdecode_lasttcprx = aprsdecode_realtime;
      }
   }
} /* end tcpin() */


extern void aprsdecode_initparms(void)
{
   unsigned long i;
   struct aprsdecode__D2 * anonym;
   initcrc12();
   memset((char *)aprsdecode_gateways,(char)0,
                sizeof(struct aprsdecode__D4 [10]));
   memset((char *)aprsdecode_udpsocks0,(char)0,
                sizeof(struct aprsdecode_UDPSOCK [4]));
   for (i = 0UL; i<=3UL; i++) {
      aprsdecode_udpsocks0[i].fd = -1L;
   } /* end for */
   connecttime = 0UL;
   cycleservers = 0UL;
   logframename[0UL] = 0;
   logframes = 6L;
   aprsdecode_testbeaconbuf[0UL] = 0;
   /*  servercall[0]:=0C;  */
   /*  passwd[0]:=0C; */
   rfhered = 0;
   callsrc = 0;
   /*  igatedelay:=TRUE; */
   rfbeacont = 0UL;
   qas = 0UL;
   qasc = 0UL;
   aprsdecode_initxsize = 0L;
   aprsdecode_initysize = 0L;
   aprsdecode_initzoom = 0L;
   aprsdecode_finezoom = 1.0f;
   aprsdecode_posinval(&aprsdecode_mappos);
   aprsdecode_spikesens = 2.0f;
   aprsdecode_maxhop = 100.0f;
   maxspeed = 1000.0f;
   nextbeep = 0UL;
   { /* with */
      struct aprsdecode__D2 * anonym = &aprsdecode_lums;
      anonym->map = 350L;
      anonym->rfbri = 350L;
      anonym->track = 1000L;
      anonym->waypoint = 800L;
      anonym->sym = 1000L;
      anonym->obj = 1000L;
      anonym->text = 750L;
      anonym->nomov = 90L;
      anonym->maxdim = 3600UL;
      anonym->firstdim = 82800UL;
      anonym->purgetime = 90000UL;
      anonym->purgetimeobj = 90000UL;
      anonym->dupemaxtime = 10800UL;
      anonym->fps = 0L;
      /*  mapmode:=REFLATER; */
      strncpy(anonym->mapname,"tiles",41u);
      anonym->maplumcorr = -128L;
      anonym->gamma = 2.2f;
      anonym->centering = 40L;
      anonym->movestep = 10UL;
      strncpy(anonym->configfn,"aprsmap.cfg",257u);
      anonym->wxcol = 0;
   }
   aprsdecode_lastpurge = 0UL;
   aprsdecode_lastanyudprx = 0UL;
   aprsdecode_lasttcprx = 0UL;
   strncpy(aprsdecode_mapdir,"osm",1025u);
   parms();
   aprsdecode_ophist0 = 0;
   aprsdecode_ophist2 = 0;
   aprsdecode_txmessages = 0;
   aprsdecode_msgfifo0 = 0;
   msgmid = 0UL;
   aprsdecode_realtime = TimeConv_time();
   uptime = aprsdecode_realtime;
   trygate = 0UL;
   dupetime = 60UL;
   ungate[0U][0] = 0;
   strncpy(ungate[1U],"NOGATE",9u);
   strncpy(ungate[2U],"RFONLY",9u);
   strncpy(ungate[3U],"TCPIP",9u);
   strncpy(ungate[4U],"TCPXX",9u);
   strncpy(ungate[5U],"q",9u);
   ungate[6U][0] = 0;
   aprsdecode_tcpsocks = 0;
   Stopticker();
   tickertime = 0UL;
   sentemptymsg = 0;
   beaconrandomstart = TimeConv_time()%60UL;
                /* set a beacon scheduler start second in minute */
} /* end initparms() */


extern void aprsdecode_BEGIN(void)
{
   static int aprsdecode_init = 0;
   if (aprsdecode_init) return;
   aprsdecode_init = 1;
   if (sizeof(aprsdecode_MONCALL)!=9) X2C_ASSERT(0);
   if (sizeof(aprsdecode_FRAMEBUF)!=512) X2C_ASSERT(0);
   if (sizeof(aprsdecode_WWWB)!=1401) X2C_ASSERT(0);
   if (sizeof(aprsdecode_FILENAME)!=1024) X2C_ASSERT(0);
   if (sizeof(aprsdecode_DUPETIMES)!=262144) X2C_ASSERT(0);
   if (sizeof(aprsdecode_MSGTEXT)!=67) X2C_ASSERT(0);
   if (sizeof(aprsdecode_ACKTEXT)!=5) X2C_ASSERT(0);
   if (sizeof(aprsdecode_ACKREPTEXT)!=2) X2C_ASSERT(0);
   if (sizeof(unsigned char)!=1) X2C_ASSERT(0);
   if (sizeof(unsigned char)!=1) X2C_ASSERT(0);
   if (sizeof(unsigned short)!=2) X2C_ASSERT(0);
   if (sizeof(unsigned short)!=2) X2C_ASSERT(0);
   if (sizeof(SET256)!=32) X2C_ASSERT(0);
   if (sizeof(CHSET)!=16) X2C_ASSERT(0);
   if (sizeof(TICKERCALL)!=31) X2C_ASSERT(0);
   aprstext_BEGIN();
   useri_BEGIN();
   TimeConv_BEGIN();
   Storage_BEGIN();
   aprspos_BEGIN();
   aprsstr_BEGIN();
   osi_BEGIN();
   xosi_BEGIN();
}
