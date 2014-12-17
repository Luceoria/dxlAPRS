/*
 * dxlAPRS toolchain
 *
 * Copyright (C) Christian Rabler <oe5dxl@oevsv.at>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#define X2C_int32
#define X2C_index32
#ifndef aprstext_H_
#include "aprstext.h"
#endif
#define aprstext_C_
#ifndef aprsdecode_H_
#include "aprsdecode.h"
#endif
#ifndef aprspos_H_
#include "aprspos.h"
#endif
#ifndef aprsstr_H_
#include "aprsstr.h"
#endif
#ifndef osi_H_
#include "osi.h"
#endif
#ifndef RealMath_H_
#include "RealMath.h"
#endif
#ifndef maptool_H_
#include "maptool.h"
#endif
#ifndef useri_H_
#include "useri.h"
#endif

/* aprs tracks on osm map by oe5dxl */
#define aprstext_PI 3.1415926535898


extern void aprstext_strcp(char from[], unsigned long from_len,
                unsigned long p, unsigned long l, char to[],
                unsigned long to_len)
{
   unsigned long i;
   i = 0UL;
   while (l>0UL && i<=to_len-1) {
      to[i] = from[p];
      ++i;
      ++p;
      --l;
   }
   if (i<=to_len-1) to[i] = 0;
} /* end strcp() */

#define aprstext_DAY 86400


extern void aprstext_DateLocToStr(unsigned long time0, char s[],
                unsigned long s_len)
/* append (+localtime) to time */
{
   unsigned long lo;
   char h[10];
   lo = useri_localtime();
   if (time0+86400UL<aprsdecode_realtime) {
      aprsstr_DateToStr(time0+lo, s, s_len);
   }
   else aprsstr_TimeToStr((time0+lo)%86400UL, s, s_len);
   if (lo) {
      aprsstr_IntToStr(X2C_DIV((long)lo,3600L), 0UL, h, 10ul);
      aprsstr_Append(s, s_len, "(", 2ul);
      aprsstr_Append(s, s_len, h, 10ul);
      aprsstr_Append(s, s_len, ")", 2ul);
   }
} /* end DateLocToStr() */


extern float aprstext_FtoC(float tempf)
{
   return X2C_DIVR(tempf-32.0f,1.8f);
} /* end FtoC() */


static void Errtxt(char s[], unsigned long s_len, aprsdecode_pFRAMEHIST pf,
                aprsdecode_pFRAMEHIST frame)
{
   char hh[100];
   char h[100];
   unsigned long l;
   unsigned char e;
   if (frame) {
      e = frame->nodraw;
      if ((frame->vardat && frame->vardat->lastref)
                && !aprspos_posvalid(frame->vardat->pos)) e |= 0x40U;
      h[0U] = 0;
      if ((0x1U & e)) aprsstr_Append(h, 100ul, "DIST,", 6ul);
      if ((0x2U & e)) aprsstr_Append(h, 100ul, "SPIKE,", 7ul);
      if ((0x4U & e)) aprsstr_Append(h, 100ul, "SYMBOL,", 8ul);
      if ((0x10U & e)) aprsstr_Append(h, 100ul, "SPEED,", 7ul);
      if ((0x40U & e)) aprsstr_Append(h, 100ul, "NOPOS,", 7ul);
      if ((0x8U & e)) {
         aprsstr_Append(h, 100ul, "DUPE,", 6ul);
         if (pf) {
            aprsstr_IntToStr((long)aprsdecode_finddup(pf, frame), 0UL, hh,
                100ul);
            aprsstr_Append(h, 100ul, hh, 100ul);
            aprsstr_Append(h, 100ul, "s,", 3ul);
         }
      }
      if ((0x20U & e)) aprsstr_Append(h, 100ul, "SEG,", 5ul);
      l = aprsstr_Length(h, 100ul);
      if (l>0UL) {
         h[l-1UL] = 0;
         aprsstr_Append(h, 100ul, "]\376", 3ul);
         aprsstr_Append(s, s_len, "\370[", 3ul);
         aprsstr_Append(s, s_len, h, 100ul);
      }
   }
} /* end Errtxt() */


static char Hex(unsigned long d)
{
   d = d&15UL;
   if (d>9UL) d += 7UL;
   return (char)(d+48UL);
} /* end Hex() */


extern void aprstext_Apphex(char s[], unsigned long s_len, char h[],
                unsigned long h_len)
{
   unsigned long j;
   unsigned long i;
   i = 0UL;
   j = aprsstr_Length(s, s_len);
   while ((i<=h_len-1 && h[i]) && j+10UL<s_len-1) {
      if (h[i]!='\015') {
         if ((unsigned char)h[i]<' ' || (unsigned char)h[i]>='\177') {
            s[j] = '\371';
            ++j;
            s[j] = '<';
            ++j;
            s[j] = Hex((unsigned long)(unsigned char)h[i]/16UL);
            ++j;
            s[j] = Hex((unsigned long)(unsigned char)h[i]);
            ++j;
            s[j] = '>';
            ++j;
            s[j] = '\376';
         }
         else s[j] = h[i];
         ++j;
      }
      ++i;
   }
   s[j] = 0;
} /* end Apphex() */

/*
  PROCEDURE radtostr(r:REAL; VAR s:ARRAY OF CHAR);
  BEGIN FixToStr(r*180.0/PI+0.000005, 6, s); END radtostr;
*/

static void rfdist(aprsdecode_pVARDAT v, char h[], unsigned long h_len)
{
   aprsdecode_MONCALL digi;
   aprsdecode_pOPHIST ig;
   char s[32];
   struct aprsdecode_VARDAT * anonym;
   { /* with */
      struct aprsdecode_VARDAT * anonym = v;
      if (anonym->igatelen==0U || !aprspos_posvalid(anonym->pos)) return;
      aprstext_strcp(anonym->raw, 500ul, (unsigned long)anonym->igatepos,
                (unsigned long)anonym->igatelen, digi, 9ul);
      ig = aprsdecode_ophist0;
      while (ig && X2C_STRCMP(ig->call,9u,digi,9u)) ig = ig->next;
      if (ig==0 || !aprspos_posvalid(ig->lastpos)) return;
      aprsstr_Append(h, h_len, " Igate:", 8ul);
      aprsstr_Append(h, h_len, digi, 9ul);
      aprsstr_Append(h, h_len, "(", 2ul);
      aprsstr_FixToStr(aprspos_distance(ig->lastpos, anonym->pos), 4UL, s,
                32ul);
      aprsstr_Append(h, h_len, s, 32ul);
      aprsstr_Append(h, h_len, "km)", 4ul);
   }
/*
FixToStr(distance(ig^.lastpos, pos), 4, s); WrStr(s); WrStr("|");
FixToStr(distance(pos, ig^.lastpos), 4, s); WrStr(s); WrStr(" ");
postostr(ig^.lastpos, s); WrStr(s); WrStr(" "); postostr(pos, s); WrStrLn(s);
*/
} /* end rfdist() */

#define aprstext_TAB "\012 "


extern void aprstext_decode(char s[], unsigned long s_len,
                aprsdecode_pFRAMEHIST pf0, aprsdecode_pFRAMEHIST pf,
                aprsdecode_pVARDAT oldvar, unsigned long odate, char decoded,
                 struct aprsdecode_DAT * dat)
{
   char h[512];
   char colalt;
   char nl;
   long ret;
   long og;
   float resol;
   if (pf->time0>0UL) {
      s[0UL] = '\367';
      s[1UL] = 0;
      aprstext_DateLocToStr(pf->time0, h, 512ul);
      aprsstr_Append(s, s_len, h, 512ul);
      aprsstr_Append(s, s_len, ":\376", 3ul);
   }
   else s[0UL] = 0;
   ret = aprsdecode_Decode(pf->vardat->raw, 500ul, dat);
   if (decoded) {
      aprstext_Apphex(s, s_len, dat->srccall, 9ul);
      og = X2C_max_longint;
      if (!aprspos_posvalid(dat->pos)) dat->pos = pf->vardat->pos;
      if (aprspos_posvalid(dat->pos)) {
         aprstext_postostr(dat->pos, '3', h, 512ul);
         aprsstr_Append(s, s_len, " \367", 3ul);
         aprsstr_Append(s, s_len, h, 512ul);
         aprsstr_Append(s, s_len, "\376 (", 4ul);
         maptool_postoloc(h, 512ul, dat->pos);
         aprsstr_Append(s, s_len, h, 512ul);
         aprsstr_Append(s, s_len, ") ", 3ul);
         og = (long)X2C_TRUNCI(maptool_getsrtm(dat->pos, 0UL, &resol),
                X2C_min_longint,X2C_max_longint);
      }
      nl = 1;
      if ((unsigned char)dat->symt>' ' && (unsigned char)dat->sym>' ') {
         aprsstr_Append(s, s_len, "\012 ", 3ul);
         nl = 0;
         aprsstr_Append(s, s_len, "\375", 2ul);
         /*      Append(s, "Sym:"); */
         aprstext_Apphex(s, s_len, (char *) &dat->symt, 1u/1u);
         aprstext_Apphex(s, s_len, (char *) &dat->sym, 1u/1u);
      }
      if (dat->speed<X2C_max_longcard) {
         if (nl) {
            aprsstr_Append(s, s_len, "\012 ", 3ul);
            nl = 0;
         }
         else aprsstr_Append(s, s_len, " \367", 3ul);
         if (dat->sym=='_') {
            aprsstr_FixToStr((float)dat->speed*1.609f, 2UL, h, 512ul);
         }
         else aprsstr_FixToStr((float)dat->speed*1.852f, 0UL, h, 512ul);
         aprsstr_Append(s, s_len, h, 512ul);
         aprsstr_Append(s, s_len, "km/h\376", 6ul);
         if (dat->course<360UL) {
            aprsstr_Append(s, s_len, " dir:", 6ul);
            aprsstr_IntToStr((long)dat->course, 1UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "deg", 4ul);
         }
      }
      if (dat->altitude<X2C_max_longint) {
         if (nl) {
            aprsstr_Append(s, s_len, "\012 ", 3ul);
            nl = 0;
         }
         else aprsstr_Append(s, s_len, " ", 2ul);
         colalt = useri_conf2int(useri_fALTMIN, 0UL, -10000L, 100000L,
                -10000L)<=dat->altitude;
         if (colalt) aprsstr_Append(s, s_len, "\367", 2ul);
         aprsstr_Append(s, s_len, "NN:", 4ul);
         aprsstr_IntToStr(dat->altitude, 1UL, h, 512ul);
         aprsstr_Append(s, s_len, h, 512ul);
         aprsstr_Append(s, s_len, "m", 2ul);
         if (colalt) aprsstr_Append(s, s_len, "\376", 2ul);
         if (og<30000L) {
            aprsstr_Append(s, s_len, " OG:", 5ul);
            aprsstr_IntToStr(dat->altitude-og, 1UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "m", 2ul);
         }
      }
      rfdist(pf->vardat, s, s_len);
      if ((oldvar && aprspos_posvalid(oldvar->pos))
                && aprspos_posvalid(dat->pos)) {
         if (nl) {
            aprsstr_Append(s, s_len, "\012 ", 3ul);
            nl = 0;
         }
         else aprsstr_Append(s, s_len, " ", 2ul);
         aprsstr_Append(s, s_len, "moved:", 7ul);
         aprsstr_FixToStr(aprspos_distance(oldvar->pos, dat->pos), 4UL, h,
                512ul);
         aprsstr_Append(s, s_len, h, 512ul);
         aprsstr_Append(s, s_len, "km", 3ul);
      }
      if (odate>0UL && pf->time0>=odate) {
         if (nl) {
            aprsstr_Append(s, s_len, "\012 ", 3ul);
            nl = 0;
         }
         else aprsstr_Append(s, s_len, " ", 2ul);
         aprsstr_Append(s, s_len, "since ", 7ul);
         aprsstr_IntToStr((long)(pf->time0-odate), 1UL, h, 512ul);
         aprsstr_Append(s, s_len, h, 512ul);
         aprsstr_Append(s, s_len, "s", 2ul);
      }
      if (dat->type!=aprsdecode_MSG) Errtxt(s, s_len, pf0, pf);
      nl = 1;
      if (dat->sym=='_') {
         if (dat->wx.gust!=1.E+6f) {
            aprsstr_Append(s, s_len, "\012 ", 3ul);
            nl = 0;
            aprsstr_Append(s, s_len, "Gust:", 6ul);
            aprsstr_FixToStr(dat->wx.gust*1.609f, 0UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "km/h", 5ul);
         }
         if (dat->wx.temp!=1.E+6f) {
            if (nl) {
               aprsstr_Append(s, s_len, "\012 ", 3ul);
               nl = 0;
            }
            aprsstr_Append(s, s_len, " Temp:", 7ul);
            aprsstr_FixToStr(aprstext_FtoC(dat->wx.temp), 2UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "C", 2ul);
         }
         if (dat->wx.hygro!=1.E+6f) {
            if (nl) {
               aprsstr_Append(s, s_len, "\012 ", 3ul);
               nl = 0;
            }
            aprsstr_Append(s, s_len, " Hum:", 6ul);
            aprsstr_IntToStr((long)X2C_TRUNCI(dat->wx.hygro+0.5f,
                X2C_min_longint,X2C_max_longint), 1UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "%", 2ul);
         }
         if (dat->wx.baro!=1.E+6f) {
            if (nl) {
               aprsstr_Append(s, s_len, "\012 ", 3ul);
               nl = 0;
            }
            aprsstr_Append(s, s_len, " Baro:", 7ul);
            aprsstr_FixToStr(dat->wx.baro*0.1f, 2UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "hPa", 4ul);
         }
         if (dat->wx.rain1!=1.E+6f) {
            if (nl) {
               aprsstr_Append(s, s_len, "\012 ", 3ul);
               nl = 0;
            }
            aprsstr_Append(s, s_len, " Rain1h:", 9ul);
            aprsstr_FixToStr(dat->wx.rain1*0.254f, 2UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "mm", 3ul);
         }
         if (dat->wx.rain24!=1.E+6f) {
            if (nl) {
               aprsstr_Append(s, s_len, "\012 ", 3ul);
               nl = 0;
            }
            aprsstr_Append(s, s_len, " Rain24h:", 10ul);
            aprsstr_FixToStr(dat->wx.rain24*0.254f, 2UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "mm", 3ul);
         }
         if (dat->wx.raintoday!=1.E+6f) {
            if (nl) {
               aprsstr_Append(s, s_len, "\012 ", 3ul);
               nl = 0;
            }
            aprsstr_Append(s, s_len, " Rain00:", 9ul);
            aprsstr_FixToStr(dat->wx.raintoday*0.254f, 2UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "mm", 3ul);
         }
         if (dat->wx.lum!=1.E+6f) {
            if (nl) aprsstr_Append(s, s_len, "\012 ", 3ul);
            aprsstr_Append(s, s_len, " Luminosity:", 13ul);
            aprsstr_FixToStr(dat->wx.lum, 0UL, h, 512ul);
            aprsstr_Append(s, s_len, h, 512ul);
            aprsstr_Append(s, s_len, "W", 2ul);
         }
      }
      else if (dat->type==aprsdecode_MSG) {
         aprsstr_Append(s, s_len, "\012 ", 3ul);
         aprsstr_Append(s, s_len, " Msg To:", 9ul);
         aprstext_Apphex(s, s_len, dat->msgto, 9ul);
         if (dat->msgtext[0UL]) {
            aprsstr_Append(s, s_len, " Text:[", 8ul);
            aprstext_Apphex(s, s_len, dat->msgtext, 67ul);
            aprsstr_Append(s, s_len, "]", 2ul);
         }
         if (dat->acktext[0UL]) {
            aprsstr_Append(s, s_len, " Ack:[", 7ul);
            aprstext_Apphex(s, s_len, dat->acktext, 5ul);
            aprsstr_Append(s, s_len, "]", 2ul);
         }
         if (dat->ackrej==aprsdecode_MSGREJ) {
            aprsstr_Append(s, s_len, " Reject", 8ul);
         }
      }
      else if (dat->type==aprsdecode_OBJ) {
         aprsstr_Append(s, s_len, "\012 ", 3ul);
         if (dat->objkill=='1') aprsstr_Append(s, s_len, "Killed ", 8ul);
         aprsstr_Append(s, s_len, "Object from:", 13ul);
         aprstext_Apphex(s, s_len, dat->objectfrom, 9ul);
      }
      else if (dat->type==aprsdecode_ITEM) {
         aprsstr_Append(s, s_len, "\012 ", 3ul);
         if (dat->objkill=='1') aprsstr_Append(s, s_len, "Killed ", 8ul);
         aprsstr_Append(s, s_len, "Item from:", 11ul);
         aprstext_Apphex(s, s_len, dat->objectfrom, 9ul);
      }
      if (dat->type!=aprsdecode_MSG && dat->comment0[0UL]) {
         if (dat->type==aprsdecode_TELE) {
            aprsstr_Append(s, s_len, "\012 Telemetry: [", 15ul);
         }
         else aprsstr_Append(s, s_len, "\012 Comment: [", 13ul);
         aprstext_Apphex(s, s_len, dat->comment0, 256ul);
         aprsstr_Append(s, s_len, "]", 2ul);
      }
      aprsstr_Append(s, s_len, "\012 ", 3ul);
   }
   aprsstr_Append(s, s_len, "[", 2ul);
   aprstext_Apphex(s, s_len, pf->vardat->raw, 500ul);
   aprsstr_Append(s, s_len, "]", 2ul);
   if (!decoded) Errtxt(s, s_len, pf0, pf);
} /* end decode() */


extern void aprstext_setmark1(struct aprspos_POSITION pos, char overwrite,
                long alt, unsigned long timestamp)
{
   if ((overwrite || !aprspos_posvalid(aprsdecode_click.markpos))
                || aprsdecode_click.marktime) {
      aprsdecode_click.markpos = pos;
      if (overwrite) aprsdecode_click.marktime = 0UL;
      else aprsdecode_click.marktime = aprsdecode_realtime;
      aprsdecode_click.markalti = alt;
      aprsdecode_click.markpost = timestamp;
   }
/*WrFixed(pos.lat, 5, 10); WrFixed(pos.long, 5, 10); */
/*WrFixed(click.markpos.lat, 5, 10); WrFixed(click.markpos.long, 5, 10);
                WrStrLn(" mp"); */
} /* end setmark1() */


extern void aprstext_setmarkalti(aprsdecode_pFRAMEHIST pf,
                aprsdecode_pOPHIST op, char overwrite)
{
   struct aprsdecode_DAT dat;
   long alt;
   struct aprspos_POSITION pos;
   unsigned long t;
   aprsdecode_posinval(&pos);
   alt = X2C_max_longint;
   t = aprsdecode_realtime;
   if (pf && aprsdecode_Decode(pf->vardat->raw, 500ul, &dat)==0L) {
      if (dat.altitude>-10000L) alt = dat.altitude;
      pos = dat.pos;
      t = pf->time0;
   }
   else if (op && op->lastinftyp<100U) alt = (long)op->lasttempalt+22768L;
   if (!aprspos_posvalid(pos) && op) pos = op->lastpos;
   aprstext_setmark1(pos, overwrite, alt, t);
} /* end setmarkalti() */


extern void aprstext_optext(unsigned long typ, char * last, char s[],
                unsigned long s_len)
{
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST pfe;
   aprsdecode_pFRAMEHIST pf1;
   aprsdecode_pFRAMEHIST pf;
   unsigned long cn;
   unsigned long cx;
   char ss[1000];
   struct aprsdecode_DAT dat;
   /*    islast:BOOLEAN; */
   op = aprsdecode_click.table[aprsdecode_click.selected].opf;
   pf = aprsdecode_click.table[aprsdecode_click.selected].pff0;
   if (pf==0 && op) pf = op->frames;
   s[0UL] = 0;
   /*  islast:=last; */
   *last = 0;
   if (op && pf) {
      if (typ==3UL) aprstext_setmarkalti(pf, op, 0);
      else if (typ==2UL) {
         /*      click.marktime:=realtime; */
         /* find last raw frame */
         pf = op->frames;
         if (pf) {
            while (pf->next) pf = pf->next;
            aprsdecode_click.table[aprsdecode_click.selected].pff0 = pf;
            /*      IF islast THEN   */
            aprstext_setmarkalti(0, op, 0);
                /* not use last frame for marker because marker should be at symbol position but sympol is on last ERROR-FREE position */
         }
      }
      else if (typ==1UL) {
         /*WrStrLn("mt2"); */
         /*      click.marktime:=realtime; */
         /*      END; */
         /* next frame */
         pf1 = pf;
         do {
            if (pf->next==0) pf = op->frames;
            else pf = pf->next;
         } while (!((!aprsdecode_lums.errorstep || pf1==pf) || (pf->nodraw&~0x40U)!=0U));
         /*      IF pf^.next<>NIL THEN pf:=pf^.next ELSE pf:=op^.frames END;
                */
         if (pf) {
            aprsdecode_click.table[aprsdecode_click.selected].pff0 = pf;
            aprsdecode_click.table[aprsdecode_click.selected].pff = pf;
            aprsdecode_click.table[aprsdecode_click.selected]
                .typf = aprsdecode_tTRACK; /* set "track found" */
            aprstext_setmarkalti(pf, op, 0);
         }
      }
      else if (typ==0UL) {
         /*        click.marktime:=realtime; */
         /* back to last frame */
         pf1 = pf;
         pfe = pf;
         do {
            if (!aprsdecode_lums.errorstep || (pfe->nodraw&~0x40U)!=0U) {
               pf = pfe;
            }
            if (pfe->next==0) pfe = op->frames;
            else pfe = pfe->next;
         } while (pfe!=pf1);
         /*
               pf:=op^.frames;
               IF pf=pf1 THEN pf1:=NIL END; 
               WHILE (pf<>NIL) & (pf^.next<>pf1) DO pf:=pf^.next END;
         */
         aprsdecode_click.table[aprsdecode_click.selected].pff0 = pf;
         aprsdecode_click.table[aprsdecode_click.selected].pff = pf;
         aprsdecode_click.table[aprsdecode_click.selected]
                .typf = aprsdecode_tTRACK; /* set "track found" */
         aprstext_setmarkalti(pf, op, 0);
      }
      /*      click.marktime:=realtime;  */
      /*    IF ((typ=1) OR (typ=0)) & lums.errorstep & (pf=pf1)
                THEN Assign(s, "no more errors found"); */
      if ((aprsdecode_lums.errorstep && pf) && (pf->nodraw&~0x40U)==0U) {
         aprsstr_Assign(s, s_len, "Show errors mode: no (more) errors found",
                 41ul);
      }
      else if (pf) {
         cn = 0UL;
         cx = 0UL;
         pfe = 0;
         pf1 = op->frames;
         while (pf1) {
            ++cn;
            if (pf1==pf) cx = cn;
            if (pf1->next==pf) {
               pfe = pf1;
            }
            pf1 = pf1->next;
         }
         aprsstr_IntToStr((long)cx, 0UL, s, s_len);
         aprsstr_Append(s, s_len, "/", 2ul);
         aprsstr_IntToStr((long)cn, 0UL, ss, 1000ul);
         aprsstr_Append(s, s_len, ss, 1000ul);
         aprsstr_Append(s, s_len, " ", 2ul);
         if (pfe==0) {
            aprstext_decode(ss, 1000ul, op->frames, pf, 0, 0UL, 1, &dat);
         }
         else {
            aprstext_decode(ss, 1000ul, op->frames, pf, pfe->vardat,
                pfe->time0, 1, &dat);
         }
         aprsstr_Append(s, s_len, ss, 1000ul);
         *last = pf->next==0;
      }
   }
} /* end optext() */


extern aprsdecode_pOPHIST aprstext_oppo(aprsdecode_MONCALL opcall)
/* find pointer to call */
{
   aprsdecode_pOPHIST op;
   if (opcall[0UL]==0) return 0;
   op = aprsdecode_ophist0;
   while (op && X2C_STRCMP(op->call,9u,opcall,9u)) op = op->next;
   return op;
} /* end oppo() */


extern void aprstext_listop(char decoded)
{
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST pf;
   aprsdecode_pVARDAT oldv;
   unsigned long oldt;
   char s[1000];
   struct aprsdecode_DAT dat;
   op = aprsdecode_click.table[aprsdecode_click.selected].opf;
   if (op) {
      pf = op->frames;
      oldv = 0;
      oldt = 0UL;
      while (pf) {
         aprstext_decode(s, 1000ul, op->frames, pf, oldv, oldt, decoded,
                &dat);
         if (aprspos_posvalid(pf->vardat->pos)) oldv = pf->vardat;
         oldt = pf->time0;
         aprsstr_Append(s, 1000ul, "\012", 2ul);
         if (decoded) aprsstr_Append(s, 1000ul, "\012", 2ul);
         useri_wrstrlist(s, 1000ul, dat.srccall, pf->vardat->pos, pf->time0);
         pf = pf->next;
      }
   }
} /* end listop() */


extern void aprstext_listin(char r[], unsigned long r_len, char port,
                char dir, char decoded)
{
   char s1[1000];
   char s[1000];
   struct aprsdecode_VARDAT vard;
   struct aprsdecode_FRAMEHIST pf;
   unsigned long j;
   unsigned long i;
   struct aprsdecode_DAT dat;
   memset((char *) &vard,(char)0,sizeof(struct aprsdecode_VARDAT));
   i = 0UL;
   j = 0UL;
   while ((i<499UL && i<=r_len-1) && r[i]) {
      if (r[i]!='\015' && r[i]!='\012') j = i;
      vard.raw[i] = r[i];
      ++i;
   }
   vard.raw[j+1UL] = 0;
   memset((char *) &pf,(char)0,sizeof(struct aprsdecode_FRAMEHIST));
   pf.vardat = &vard;
   aprstext_decode(s, 1000ul, 0, &pf, 0, 0UL, decoded, &dat);
   if (decoded) {
      s1[0U] = '\012';
      s1[1U] = port;
      s1[2U] = 0;
   }
   else {
      s1[0U] = port;
      s1[1U] = 0;
   }
   if (dir=='<') aprsstr_Append(s1, 1000ul, "\370<\376", 4ul);
   else aprsstr_Append(s1, 1000ul, (char *) &dir, 1u/1u);
   aprsstr_Append(s1, 1000ul, s, 1000ul);
   useri_wrstrmon(s1, 1000ul, dat.pos);
} /* end listin() */


extern void aprstext_listtyps(char typ, char decod, char oneop[],
                unsigned long oneop_len)
{
   aprsdecode_pOPHIST op;
   aprsdecode_pFRAMEHIST pf;
   char s[1000];
   char lasttext[101];
   char lastto[101];
   struct aprsdecode_DAT dat;
   X2C_PCOPY((void **)&oneop,oneop_len);
   op = aprsdecode_ophist0;
   while (op) {
      pf = op->frames;
      if (typ=='N') {
         /* no pos */
         if (pf && !aprspos_posvalid(op->lastpos)) {
            while (pf->next) pf = pf->next;
            aprstext_decode(s, 1000ul, 0, pf, 0, 0UL, decod, &dat);
            if (decod) aprsstr_Append(s, 1000ul, "\012\012", 3ul);
            else aprsstr_Append(s, 1000ul, "\012", 2ul);
            useri_wrstrlist(s, 1000ul, dat.srccall, pf->vardat->pos,
                pf->time0);
         }
      }
      else if (typ=='D') {
         /* moveing stations */
         if (pf && (op->margin0.lat>op->margin1.lat || op->margin0.long0<op->margin1.long0)
                ) {
            while (pf->next) pf = pf->next;
            aprstext_decode(s, 1000ul, 0, pf, 0, 0UL, decod, &dat);
            if (decod) aprsstr_Append(s, 1000ul, "\012\012", 3ul);
            else aprsstr_Append(s, 1000ul, "\012", 2ul);
            useri_wrstrlist(s, 1000ul, dat.srccall, pf->vardat->pos,
                pf->time0);
         }
      }
      else if (typ=='W') {
         if (pf) {
            while (pf->next) pf = pf->next;
            aprstext_decode(s, 1000ul, 0, pf, 0, 0UL, decod, &dat);
            /*        IF (dat.symt="/") & (dat.sym="_") */
            /*        OR (op^.temptime+SHOWTEMPWIND>systime)
                & (op^.lastinftyp>=100) */
            /*        & (op^.lasttempalt>=-99) & (op^.lasttempalt<=99)
                THEN */
            if (dat.symt=='/' && dat.sym=='_') {
               if (decod) aprsstr_Append(s, 1000ul, "\012\012", 3ul);
               else aprsstr_Append(s, 1000ul, "\012", 2ul);
               useri_wrstrlist(s, 1000ul, dat.srccall, pf->vardat->pos,
                pf->time0);
            }
         }
      }
      else if ((typ=='M' || typ=='B') && (oneop[0UL]==0 || X2C_STRCMP(oneop,
                oneop_len,op->call,9u)==0)) {
         /* bulletins messages */
         lastto[0U] = 0;
         lasttext[0U] = 0;
         while (pf) {
            if (pf->vardat->lastref==pf && aprsdecode_Decode(pf->vardat->raw,
                 500ul, &dat)>=0L) {
               if (typ=='M' || typ=='B') {
                  if ((dat.type==aprsdecode_MSG && X2C_STRCMP(dat.srccall,9u,
                dat.msgto,9u)) && dat.msgtext[0UL]) {
                     if ((typ=='B')==aprsdecode_IsBulletin(dat)) {
                        if (!(aprsstr_StrCmp(lastto, 101ul, dat.msgto,
                9ul) && aprsstr_StrCmp(lasttext, 101ul, dat.msgtext, 67ul))) {
                           aprstext_DateLocToStr(pf->time0, s, 1000ul);
                           aprsstr_Append(s, 1000ul, " ", 2ul);
                           aprsstr_Append(s, 1000ul, dat.srccall, 9ul);
                           aprsstr_Append(s, 1000ul, ">", 2ul);
                           aprsstr_Append(s, 1000ul, dat.msgto, 9ul);
                           aprsstr_Append(s, 1000ul, ":[", 3ul);
                           aprstext_Apphex(s, 1000ul, dat.msgtext, 67ul);
                           aprsstr_Append(s, 1000ul, "]", 2ul);
                           if (dat.acktext[0UL]) {
                              aprstext_Apphex(s, 1000ul, " Ack[", 6ul);
                              aprsstr_Append(s, 1000ul, dat.acktext, 5ul);
                              aprsstr_Append(s, 1000ul, "]", 2ul);
                           }
                           aprsstr_Append(s, 1000ul, "\012", 2ul);
                           useri_wrstrlist(s, 1000ul, dat.srccall,
                pf->vardat->pos, pf->time0);
                           aprsstr_Assign(lastto, 101ul, dat.msgto, 9ul);
                           aprsstr_Assign(lasttext, 101ul, dat.msgtext,
                67ul);
                        }
                     }
                  }
               }
            }
            pf = pf->next;
         }
      }
      else if (typ=='O') {
         if (pf) {
            while (pf->next) pf = pf->next;
            aprstext_decode(s, 1000ul, 0, pf, 0, 0UL, decod, &dat);
            if ((dat.type==aprsdecode_OBJ || dat.type==aprsdecode_ITEM)
                && (oneop[0UL]==0 || X2C_STRCMP(oneop,oneop_len,
                dat.objectfrom,9u)==0)) {
               if (decod) {
                  aprsstr_Append(s, 1000ul, "\012\012", 3ul);
                  useri_wrstrlist(s, 1000ul, dat.srccall, pf->vardat->pos,
                pf->time0);
               }
               else {
                  aprsstr_Append(s, 1000ul, "\012", 2ul);
                  useri_wrstrlist(s, 1000ul, dat.objectfrom, pf->vardat->pos,
                 pf->time0);
               }
            }
         }
      }
      op = op->next;
   }
   X2C_PFREE(oneop);
} /* end listtyps() */

#define aprstext_Z 48


static void degtostr(float d, char lat, char form, char s[],
                unsigned long s_len)
{
   unsigned long i;
   unsigned long n;
   if (s_len-1<11UL) {
      s[0UL] = 0;
      return;
   }
   if (form=='2') i = 7UL;
   else if (form=='3') i = 8UL;
   else i = 9UL;
   if (d<0.0f) {
      d = -d;
      if (lat) s[i] = 'S';
      else s[i+1UL] = 'W';
   }
   else if (lat) s[i] = 'N';
   else s[i+1UL] = 'E';
   if (form=='2') {
      /* DDMM.MMNDDMM.MME */
      n = aprsdecode_trunc(d*3.4377467707849E+5f+0.5f);
      s[0UL] = (char)((n/600000UL)%10UL+48UL);
      i = (unsigned long)!lat;
      s[i] = (char)((n/60000UL)%10UL+48UL);
      ++i;
      s[i] = (char)((n/6000UL)%10UL+48UL);
      ++i;
      s[i] = (char)((n/1000UL)%6UL+48UL);
      ++i;
      s[i] = (char)((n/100UL)%10UL+48UL);
      ++i;
      s[i] = '.';
      ++i;
      s[i] = (char)((n/10UL)%10UL+48UL);
      ++i;
      s[i] = (char)(n%10UL+48UL);
      ++i;
   }
   else if (form=='3') {
      /* DDMM.MMMNDDMM.MMME */
      n = aprsdecode_trunc(d*3.4377467707849E+6f+0.5f);
      s[0UL] = (char)((n/6000000UL)%10UL+48UL);
      i = (unsigned long)!lat;
      s[i] = (char)((n/600000UL)%10UL+48UL);
      ++i;
      s[i] = (char)((n/60000UL)%10UL+48UL);
      ++i;
      s[i] = (char)((n/10000UL)%6UL+48UL);
      ++i;
      s[i] = (char)((n/1000UL)%10UL+48UL);
      ++i;
      s[i] = '.';
      ++i;
      s[i] = (char)((n/100UL)%10UL+48UL);
      ++i;
      s[i] = (char)((n/10UL)%10UL+48UL);
      ++i;
      s[i] = (char)(n%10UL+48UL);
      ++i;
   }
   else {
      /* DDMMSS */
      n = aprsdecode_trunc(d*2.062648062471E+5f+0.5f);
      s[0UL] = (char)((n/360000UL)%10UL+48UL);
      i = (unsigned long)!lat;
      s[i] = (char)((n/36000UL)%10UL+48UL);
      ++i;
      s[i] = (char)((n/3600UL)%10UL+48UL);
      ++i;
      s[i] = '\177';
      ++i;
      s[i] = (char)((n/600UL)%6UL+48UL);
      ++i;
      s[i] = (char)((n/60UL)%10UL+48UL);
      ++i;
      s[i] = '\'';
      ++i;
      s[i] = (char)((n/10UL)%6UL+48UL);
      ++i;
      s[i] = (char)(n%10UL+48UL);
      ++i;
      s[i] = '\"';
      ++i;
   }
   ++i;
   s[i] = 0;
} /* end degtostr() */


extern void aprstext_postostr(struct aprspos_POSITION pos, char form,
                char s[], unsigned long s_len)
{
   char h[32];
   degtostr(pos.lat, 1, form, s, s_len);
   aprsstr_Append(s, s_len, "/", 2ul);
   degtostr(pos.long0, 0, form, h, 32ul);
   aprsstr_Append(s, s_len, h, 32ul);
} /* end postostr() */


static long myround(float x)
{
   if (x>=0.0f) x = x+0.5f;
   else x = x-0.5f;
   return (long)X2C_TRUNCI(x,X2C_min_longint,X2C_max_longint);
} /* end myround() */


extern void aprstext_measure(struct aprspos_POSITION pos0,
                struct aprspos_POSITION pos1, char s[], unsigned long s_len,
                char sum)
{
   char h[32];
   if (aprspos_posvalid(pos0) && aprspos_posvalid(pos1)) {
      aprstext_postostr(pos1, '3', s, s_len);
      aprsstr_Append(s, s_len, " [", 3ul);
      maptool_postoloc(h, 32ul, pos1);
      aprsstr_Append(s, s_len, h, 32ul);
      aprsstr_Append(s, s_len, "] ", 3ul);
      aprsstr_FixToStr(aprspos_distance(pos0, pos1), 4UL, h, 32ul);
      aprsstr_Append(s, s_len, h, 32ul);
      aprsstr_Append(s, s_len, "km,", 4ul);
      aprsstr_IntToStr(myround(aprspos_azimuth(pos0, pos1)), 1UL, h, 32ul);
      aprsstr_Append(s, s_len, h, 32ul);
      aprsstr_Append(s, s_len, "deg to Marker", 14ul);
      if (sum) {
         if (aprsdecode_click.waysum==0.0f) aprsdecode_click.sumpos = pos0;
         aprsdecode_click.waysum = aprsdecode_click.waysum+aprspos_distance(aprsdecode_click.sumpos,
                 pos1);
         aprsdecode_click.sumpos = pos1;
         aprsstr_Append(s, s_len, " sum:", 6ul);
         aprsstr_FixToStr(aprsdecode_click.waysum, 4UL, h, 32ul);
         aprsstr_Append(s, s_len, h, 32ul);
         aprsstr_Append(s, s_len, "km", 3ul);
      }
   }
   else s[0UL] = 0;
} /* end measure() */


static unsigned long c(char * err, char ch)
{
   unsigned long n;
   if ((unsigned char)ch>='0') n = (unsigned long)(unsigned char)ch-48UL;
   else {
      n = 0UL;
      *err = 1;
   }
   if (n>9UL) *err = 1;
   return n;
} /* end c() */


extern void aprstext_degtopos(char s[], unsigned long s_len,
                struct aprspos_POSITION * pos)
/* DDMM.MMNDDDMM.MME */
{
   char err;
   unsigned long d;
   X2C_PCOPY((void **)&s,s_len);
   err = 0;
   d = c(&err, s[0UL])*60000UL+c(&err, s[1UL])*6000UL+c(&err,
                s[2UL])*1000UL+c(&err, s[3UL])*100UL+c(&err,
                s[5UL])*10UL+c(&err, s[6UL]);
   pos->lat = (float)d*2.9088820866572E-6f;
   if (pos->lat>=1.5707963267949f) err = 1;
   if (X2C_CAP(s[7UL])=='S') pos->lat = -pos->lat;
   else if (X2C_CAP(s[7UL])!='N') err = 1;
   d = c(&err, s[9UL])*600000UL+c(&err, s[10UL])*60000UL+c(&err,
                s[11UL])*6000UL+c(&err, s[12UL])*1000UL+c(&err,
                s[13UL])*100UL+c(&err, s[15UL])*10UL+c(&err, s[16UL]);
   pos->long0 = (float)d*2.9088820866572E-6f;
   if (pos->long0>=3.1415926535898f) err = 1;
   if (X2C_CAP(s[17UL])=='W') pos->long0 = -pos->long0;
   else if (X2C_CAP(s[17UL])!='E') err = 1;
   if (err) aprsdecode_posinval(pos);
   X2C_PFREE(s);
} /* end degtopos() */


static char c0(unsigned long * d, unsigned long * i, char s[],
                unsigned long s_len, unsigned long mul)
{
   unsigned long n;
   char ch;
   ch = s[*i];
   if (mul>0UL) {
      if ((unsigned char)ch>='0') n = (unsigned long)(unsigned char)ch-48UL;
      else return 0;
      if (n>9UL) return 0;
      *d += n*mul;
   }
   else if (ch!='.') return 0;
   ++*i;
   return 1;
} /* end c() */


extern void aprstext_deghtopos(char s[], unsigned long s_len,
                struct aprspos_POSITION * pos)
/* DDMM.MMMNDDMM.MMME */
{
   char e;
   char err;
   unsigned long i;
   unsigned long d;
   X2C_PCOPY((void **)&s,s_len);
   err = 0;
   d = 0UL;
   i = 0UL;
   e = ((((((c0(&d, &i, s, s_len, 600000UL) && c0(&d, &i, s, s_len,
                60000UL)) && c0(&d, &i, s, s_len, 10000UL)) && c0(&d, &i, s,
                s_len, 1000UL)) && c0(&d, &i, s, s_len, 0UL)) && c0(&d, &i,
                s, s_len, 100UL)) && c0(&d, &i, s, s_len, 10UL)) && c0(&d,
                &i, s, s_len, 1UL);
   pos->lat = (float)d*2.9088820866572E-7f;
   if (pos->lat>=1.5707963267949f) err = 1;
   if (s[i]=='S') pos->lat = -pos->lat;
   else if (s[i]!='N') err = 1;
   i += 2UL;
   d = 0UL;
   e = (((((((c0(&d, &i, s, s_len, 6000000UL) && c0(&d, &i, s, s_len,
                600000UL)) && c0(&d, &i, s, s_len, 60000UL)) && c0(&d, &i, s,
                 s_len, 10000UL)) && c0(&d, &i, s, s_len, 1000UL)) && c0(&d,
                &i, s, s_len, 0UL)) && c0(&d, &i, s, s_len, 100UL)) && c0(&d,
                 &i, s, s_len, 10UL)) && c0(&d, &i, s, s_len, 1UL);
   pos->long0 = (float)d*2.9088820866572E-7f;
   if (pos->long0>=3.1415926535898f) err = 1;
   if (s[i]=='W') pos->long0 = -pos->long0;
   else if (s[i]!='E') err = 1;
   if (err) aprsdecode_posinval(pos);
   X2C_PFREE(s);
} /* end deghtopos() */


static void cleanposstr(char s[], unsigned long s_len)
{
   unsigned long i;
   i = 0UL;
   while (i<=s_len-1 && s[i]) {
      if ((s[i]==',' || s[i]=='/') || s[i]=='\\') s[i] = ' ';
      ++i;
   }
} /* end cleanposstr() */


extern void aprstext_degdeztopos(char s[], unsigned long s_len,
                struct aprspos_POSITION * pos)
/* lat long in float deg */
{
   float d;
   char h[31];
   X2C_PCOPY((void **)&s,s_len);
   cleanposstr(s, s_len);
   aprsstr_Extractword(s, s_len, h, 31ul);
   if (aprsstr_StrToFix(&d, h, 31ul)) {
      pos->lat = d*1.7453292519943E-2f;
      aprsstr_Extractword(s, s_len, h, 31ul);
      if (aprsstr_StrToFix(&d, h, 31ul)) {
         pos->long0 = d*1.7453292519943E-2f;
         if ((float)fabs(pos->long0)<3.1415926535898f && (float)
                fabs(pos->lat)<=1.484f) goto label;
      }
   }
   aprsdecode_posinval(pos);
   label:;
   X2C_PFREE(s);
} /* end degdeztopos() */


static char num(long n)
{
   return (char)(X2C_MOD(labs(n),10L)+48L);
} /* end num() */


static unsigned long dao91(unsigned long x)
/* 33 + radix91(xx/1.1) of dddmm.mmxx */
{
   return 33UL+(x*20UL+11UL)/22UL;
} /* end dao91() */


static void micedest(long lat, long long0, char s[], unsigned long s_len)
{
   unsigned long nl;
   unsigned long nb;
   nl = (unsigned long)X2C_DIV(labs(long0),6000L);
   nb = (unsigned long)X2C_DIV(labs(lat),6000L);
   s[0UL] = (char)(80UL+nb/10UL);
   s[1UL] = (char)(80UL+nb%10UL);
   nb = (unsigned long)labs(lat)-nb*6000UL;
   s[2UL] = (char)(80UL+nb/1000UL);
   s[3UL] = (char)(48UL+32UL*(unsigned long)(lat>=0L)+(nb/100UL)%10UL);
   s[4UL] = (char)(48UL+32UL*(unsigned long)(nl<10UL || nl>=100UL)+(nb/10UL)
                %10UL);
   s[5UL] = (char)(48UL+32UL*(unsigned long)(long0<0L)+nb%10UL);
   s[6UL] = 0;
} /* end micedest() */


static void micedata(long lat, long long0, unsigned long knots,
                unsigned long dir, long alt, char sym[],
                unsigned long sym_len, char s[], unsigned long s_len)
{
   unsigned long n;
   unsigned long nl;
   X2C_PCOPY((void **)&sym,sym_len);
   if (dir>=360UL) dir = 0UL;
   if (knots>799UL) knots = 0UL;
   nl = (unsigned long)X2C_DIV(labs(long0),6000L);
   if (nl<10UL) s[0UL] = (char)(nl+118UL);
   else if (nl>=100UL) {
      if (nl<110UL) s[0UL] = (char)(nl+8UL);
      else s[0UL] = (char)(nl-72UL);
   }
   else s[0UL] = (char)(nl+28UL);
   nl = (unsigned long)labs(long0)-nl*6000UL; /* long min*100 */
   n = nl/100UL;
   if (n<10UL) n += 60UL;
   s[1UL] = (char)(n+28UL);
   s[2UL] = (char)(nl%100UL+28UL);
   s[3UL] = (char)(knots/10UL+28UL);
   s[4UL] = (char)(32UL+(knots%10UL)*10UL+dir/100UL);
   s[5UL] = (char)(28UL+dir%100UL);
   s[6UL] = sym[1UL];
   s[7UL] = sym[0UL];
   if (alt>-10000L) {
      n = (unsigned long)(alt+10000L);
      s[8UL] = (char)(33UL+(n/8281UL)%91UL);
      s[9UL] = (char)(33UL+(n/91UL)%91UL);
      s[10UL] = (char)(33UL+n%91UL);
      s[11UL] = '}';
      s[12UL] = 0;
   }
   else s[8UL] = 0;
   X2C_PFREE(sym);
} /* end micedata() */


static void alt2str(long feet, char s[], unsigned long s_len)
{
   if (feet>-100000L) {
      s[0UL] = '/';
      s[1UL] = 'A';
      s[2UL] = '=';
      if (feet<0L) {
         feet = -feet;
         s[3UL] = '-';
      }
      else s[3UL] = num(X2C_DIV(feet,100000L));
      s[4UL] = num(X2C_DIV(feet,10000L));
      s[5UL] = num(X2C_DIV(feet,1000L));
      s[6UL] = num(X2C_DIV(feet,100L));
      s[7UL] = num(X2C_DIV(feet,10L));
      s[8UL] = num(feet);
      s[9UL] = 0;
   }
   else s[0UL] = 0;
} /* end alt2str() */


static void speeddir2str(long knots, long dir, char s[],
                unsigned long s_len)
{
   if (dir>=0L && dir<360L) {
      if (dir==0L) dir = 360L;
      s[0UL] = num(X2C_DIV(dir,100L));
      s[1UL] = num(X2C_DIV(dir,10L));
      s[2UL] = num(dir);
   }
   else {
      s[0UL] = '.';
      s[1UL] = '.';
      s[2UL] = '.';
   }
   s[3UL] = '/';
   s[4UL] = num(X2C_DIV(knots,100L));
   s[5UL] = num(X2C_DIV(knots,10L));
   s[6UL] = num(knots);
   s[7UL] = 0;
} /* end speeddir2str() */


static void compressdata(struct aprspos_POSITION pos, unsigned long knots,
                unsigned long dir, long feet, char sym[],
                unsigned long sym_len, char s[], unsigned long s_len)
{
   long n;
   char h[201];
   X2C_PCOPY((void **)&sym,sym_len);
   pos.lat = pos.lat*5.7295779513082E+1f;
   pos.long0 = pos.long0*5.7295779513082E+1f;
   s[0UL] = sym[0UL];
   if (pos.lat<90.0f) {
      n = (long)aprsdecode_trunc((90.0f-pos.lat)*3.80926E+5f);
   }
   else n = 0L;
   s[1UL] = (char)(33L+X2C_DIV(n,753571L));
   s[2UL] = (char)(33L+X2C_MOD(X2C_DIV(n,8281L),91L));
   s[3UL] = (char)(33L+X2C_MOD(X2C_DIV(n,91L),91L));
   s[4UL] = (char)(33L+X2C_MOD(n,91L));
   if (pos.long0>(-180.0f)) {
      n = (long)aprsdecode_trunc((180.0f+pos.long0)*1.90463E+5f);
   }
   else n = 0L;
   s[5UL] = (char)(33L+X2C_DIV(n,753571L));
   s[6UL] = (char)(33L+X2C_MOD(X2C_DIV(n,8281L),91L));
   s[7UL] = (char)(33L+X2C_MOD(X2C_DIV(n,91L),91L));
   s[8UL] = (char)(33L+X2C_MOD(n,91L));
   s[9UL] = sym[1UL];
   if (knots>0UL) {
      if (dir>=360UL) dir = 0UL;
      s[10UL] = (char)(33UL+dir/4UL);
      s[11UL] = (char)(33UL+aprsdecode_trunc(RealMath_ln((float)(knots+1UL))
                *1.29935872129E+1f));
      s[12UL] = '_';
   }
   else if (feet>0L) {
      n = (long)aprsdecode_trunc(RealMath_ln((float)feet)*500.5f+0.5f);
      if (n>=8281L) n = 8280L;
      s[10UL] = (char)(33L+X2C_DIV(n,91L));
      s[11UL] = (char)(33L+X2C_MOD(n,91L));
      s[12UL] = 'W';
   }
   else {
      s[10UL] = ' ';
      s[11UL] = ' ';
      s[12UL] = '_';
   }
   s[13UL] = 0;
   if (knots>0UL && feet>0L) {
      alt2str(feet, h, 201ul);
      aprsstr_Append(s, s_len, h, 201ul);
   }
   X2C_PFREE(sym);
} /* end compressdata() */

#define aprstext_Z0 48


static void deg2str(long lat, long long0, char s[], unsigned long s_len)
/* DDMM.MMNDDDMM.MME */
{
   if (lat<0L) {
      lat = -lat;
      s[7UL] = 'S';
   }
   else s[7UL] = 'N';
   if (long0<0L) {
      long0 = -long0;
      s[17UL] = 'W';
   }
   else s[17UL] = 'E';
   s[0UL] = (char)(X2C_DIV(lat,60000L)+48L);
   s[1UL] = (char)(X2C_MOD(X2C_DIV(lat,6000L),10L)+48L);
   s[2UL] = (char)(X2C_MOD(X2C_DIV(lat,1000L),6L)+48L);
   s[3UL] = (char)(X2C_MOD(X2C_DIV(lat,100L),10L)+48L);
   s[4UL] = '.';
   s[5UL] = (char)(X2C_MOD(X2C_DIV(lat,10L),10L)+48L);
   s[6UL] = (char)(X2C_MOD(lat,10L)+48L);
   s[9UL] = (char)(X2C_DIV(long0,600000L)+48L);
   s[10UL] = (char)(X2C_MOD(X2C_DIV(long0,60000L),10L)+48L);
   s[11UL] = (char)(X2C_MOD(X2C_DIV(long0,6000L),10L)+48L);
   s[12UL] = (char)(X2C_MOD(X2C_DIV(long0,1000L),6L)+48L);
   s[13UL] = (char)(X2C_MOD(X2C_DIV(long0,100L),10L)+48L);
   s[14UL] = '.';
   s[15UL] = (char)(X2C_MOD(X2C_DIV(long0,10L),10L)+48L);
   s[16UL] = (char)(X2C_MOD(long0,10L)+48L);
   s[18UL] = 0;
} /* end deg2str() */

#define aprstext_RAD 3.4377467707849E+7


extern void aprstext_encbeacon(char s[], unsigned long s_len,
                unsigned long * len)
/* assemble beacon string */
{
   char h[201];
   char symb[2];
   char postyp;
   char typ;
   char err;
   char dao;
   char bkn;
   struct aprspos_POSITION pos;
   long i;
   long longd;
   long latd;
   long long0;
   long lat;
   long alt;
   long feet;
   long knots;
   long dir;
   unsigned long datastart;
   err = 0;
   *len = 16UL; /* ax.25 address + UI + PID */
   useri_confstr(useri_fRBTYP, s, s_len);
   typ = X2C_CAP(s[0UL]);
   bkn = (((typ!='O' && typ!='H') && typ!='P') && typ!='I') && typ!='J';
   useri_confstr(useri_fRBPOSTYP, (char *) &postyp, 1u/1u);
   useri_confstr(useri_fRBSYMB, symb, 2ul);
   if (aprsstr_Length(symb, 2ul)!=2UL) {
      useri_encerr("no symbol", 10ul);
      err = 1;
   }
   dao = postyp=='G' || postyp=='M';
   knots = (long)aprsdecode_trunc((float)useri_conf2int(useri_fRBSPEED, 0UL,
                0L, 32767L, 0L)*5.3995680345572E-1f+0.5f);
   dir = useri_conf2int(useri_fRBDIR, 0UL, 0L, 360L, 360L);
   if (dir>360L) useri_encerr("direction <=360", 16ul);
   else if (knots==0L && dir!=360L) {
      useri_encerr("direction needs speed>0", 24ul);
   }
   alt = useri_conf2int(useri_fRBALT, 0UL, -10000L, 1000000L, -32768L);
   feet = (long)aprsdecode_trunc((float)fabs(X2C_DIVR((float)alt,
                0.3048f)+0.5f));
   if (alt<0L) feet = -feet;
   useri_confstr(useri_fRBPORT, s, s_len); /* port */
   aprsstr_Append(s, s_len, ":", 2ul);
   aprsstr_IntToStr(useri_conf2int(useri_fRBTIME, 0UL, 0L, 32767L, 3600L),
                1UL, h, 201ul);
   aprsstr_Append(s, s_len, h, 201ul); /* repeat time */
   aprsstr_Append(s, s_len, ":", 2ul);
   if (bkn) {
      useri_confstr(useri_fRBNAME, h, 201ul);
      if ((unsigned char)h[0U]<=' ') useri_encerr("no beacon call?", 16ul);
      aprsstr_Append(s, s_len, h, 201ul);
      useri_confstr(useri_fMYPOS, h, 201ul);
      aprstext_deghtopos(h, 201ul, &pos);
      if (!aprspos_posvalid(pos)) useri_confstr(useri_fRBPOS, h, 201ul);
   }
   else {
      useri_confstr(useri_fMYCALL, h, 201ul);
      if ((unsigned char)h[0U]<=' ') {
         useri_encerr("object/item needs a Config/Online/My Call", 42ul);
      }
      aprsstr_Append(s, s_len, h, 201ul);
      useri_confstr(useri_fRBPOS, h, 201ul);
   }
   aprstext_deghtopos(h, 201ul, &pos);
   if (!aprspos_posvalid(pos)) {
      useri_encerr("position wrong", 15ul);
      err = 1;
   }
   lat = (long)aprsdecode_trunc((float)fabs(pos.lat)*3.4377467707849E+7f);
   long0 = (long)aprsdecode_trunc((float)fabs(pos.long0)*3.4377467707849E+7f)
                ;
   latd = (long)((unsigned long)lat%100UL);
   longd = (long)((unsigned long)long0%100UL);
   if (dao) {
      lat = X2C_DIV(lat-latd,100L);
      long0 = X2C_DIV(long0-longd,100L);
   }
   else {
      /*    lat :=(lat +50) DIV 100; */
      /*    long:=(long+50) DIV 100; */
      lat = X2C_DIV(lat,100L);
      long0 = X2C_DIV(long0,100L);
   }
   if (pos.lat<0.0f) lat = -lat;
   if (pos.long0<0.0f) long0 = -long0;
   aprsstr_Append(s, s_len, ">", 2ul);
   if (X2C_CAP(postyp)=='M') micedest(lat, long0, h, 201ul);
   else {
      useri_confstr(useri_fRBDEST, h, 201ul);
      if (h[0U]==0) strncpy(h,"APLM01",201u);
      i = aprsstr_InStr(h, 201ul, "-", 2ul);
      if (i>0L) aprsstr_Delstr(h, 201ul, (unsigned long)i, 201UL);
   }
   aprsstr_Append(s, s_len, h, 201ul);
   useri_confstr(useri_fRBPATH, h, 201ul);
   if ((unsigned char)h[0U]>' ') {
      if (h[0U]!='-') {
         aprsstr_Append(s, s_len, ",", 2ul);
      }
      aprsstr_Append(s, s_len, h, 201ul); /* dest ssid + via path */
      i = 0L;
      while (s[i]) {
         if (s[i]==',') *len += 7UL;
         ++i;
      }
   }
   aprsstr_Append(s, s_len, ":", 2ul); /* end of address */
   datastart = aprsstr_Length(s, s_len); /* rest of line for byte count */
   if (!bkn) {
      useri_confstr(useri_fRBNAME, h, 201ul);
      if ((unsigned char)h[0U]<=' ') {
         useri_encerr("no object/item name?", 21ul);
      }
      aprsstr_Append(h, 201ul, "         ", 10ul); /* fix size */
      h[9U] = 0;
      if ((typ=='O' || typ=='H') || typ=='P') {
         aprsstr_Append(s, s_len, ";", 2ul); /* object */
         aprsstr_Append(s, s_len, h, 201ul);
         if (typ=='P') aprsstr_Append(s, s_len, "_", 2ul);
         else aprsstr_Append(s, s_len, "*", 2ul);
         if (typ=='O') aprsstr_Append(s, s_len, "\\\\Zz", 5ul);
         else aprsstr_Append(s, s_len, "\\\\Hh", 5ul);
      }
      else {
         aprsstr_Append(s, s_len, ")", 2ul); /* item */
         i = 3L;
         while ((unsigned char)h[i]>' ') ++i;
         h[i] = 0; /* item size 3..9 */
         aprsstr_Append(s, s_len, h, 201ul);
         if (typ=='J') aprsstr_Append(s, s_len, "_", 2ul);
         else aprsstr_Append(s, s_len, "!", 2ul);
      }
   }
   else {
      if (X2C_CAP(postyp)=='M') strncpy(h,"`",201u);
      else strncpy(h,"=",201u);
      aprsstr_Append(s, s_len, h, 201ul); /* beacon protocol */
   }
   switch ((unsigned)X2C_CAP(postyp)) {
   case 'M':
      micedata(lat, long0, (unsigned long)knots, (unsigned long)dir, alt,
                symb, 2ul, h, 201ul);
      aprsstr_Append(s, s_len, h, 201ul);
      break;
   case 'C':
      if (feet<0L && feet>-10000L) {
         useri_encerr("no negative altitude in compressed mode", 40ul);
         err = 1;
      }
      compressdata(pos, (unsigned long)knots, (unsigned long)dir, feet, symb,
                 2ul, h, 201ul);
      aprsstr_Append(s, s_len, h, 201ul);
      break;
   default:;
      deg2str(lat, long0, h, 201ul);
      h[8U] = symb[0U]; /* symbol */
      h[18U] = symb[1U];
      h[19U] = 0;
      aprsstr_Append(s, s_len, h, 201ul);
      if (knots>0L) {
         /* dir, speed */
         speeddir2str(knots, dir, h, 201ul);
         aprsstr_Append(s, s_len, h, 201ul);
      }
      if (alt>-10000L) {
         alt2str(feet, h, 201ul);
         aprsstr_Append(s, s_len, h, 201ul);
      }
      break;
   } /* end switch */
   if (X2C_CAP(postyp)!='C' && dao) {
      /* DAO */
      h[0U] = '!';
      h[1U] = 'w';
      h[2U] = (char)dao91((unsigned long)latd);
      h[3U] = (char)dao91((unsigned long)longd);
      h[4U] = '!';
      h[5U] = 0;
      aprsstr_Append(s, s_len, h, 201ul);
   }
   useri_confappend(useri_fRBCOMMENT, s, s_len);
   *len += aprsstr_Length(s, s_len)-datastart;
   if (err) s[0UL] = 0;
} /* end encbeacon() */


extern void aprstext_BEGIN(void)
{
   static int aprstext_init = 0;
   if (aprstext_init) return;
   aprstext_init = 1;
   useri_BEGIN();
   osi_BEGIN();
   maptool_BEGIN();
   aprsdecode_BEGIN();
   aprsstr_BEGIN();
   aprspos_BEGIN();
}
