//=============================================================================
// SubNets.c
// Программа - калькулятор подсетей
//=============================================================================
#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SubNets.h"

//-----------------------------------------------------------------------------
// Dialog Window procedure prototype
//-----------------------------------------------------------------------------
MRESULT EXPENTRY DlgMenu (HWND, ULONG ,MPARAM, MPARAM);
void SendErrMsg(HWND, char *);

//-----------------------------------------------------------------------------
// Global Variablies
//-----------------------------------------------------------------------------
unsigned char ErrMsg[96], addr[4], mask[4], network[4];

//=============================================================================
// Main procedure
//=============================================================================
void main()
     {
     HAB   hab = WinInitialize (0);          // Anchor
     HMQ   hmq = WinCreateMsgQueue(hab, 0);  // Message queue handle

     WinDlgBox( HWND_DESKTOP,
                HWND_DESKTOP,
                DlgMenu,
                NULLHANDLE,
                DIALOGWIN,
                0 );

     WinDestroyMsgQueue(hmq);
     WinTerminate(hab);
     }

//=============================================================================
// Dialog procedure
//=============================================================================
MRESULT EXPENTRY DlgMenu (HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
  {
  unsigned int i, j, k, num, slider, masklen, subnetbit, hostbit;
  unsigned int numsubnet, numhost, numtotal;
  unsigned char InField[4], OutScale[3], class[2] = "\0", OutMsg[48];
  unsigned char currnet[4], netstart[4], netstop[4], broadcast[4];
  static char ErrAddr[] = "Wrong IP address";
  static char ErrMask[] = "Wrong network mask";

  switch (msg)
    {
//-----------------------------------------------------------------------------
// Handle the initialization of the dialog
//-----------------------------------------------------------------------------
    case WM_INITDLG:
      {
//-----------------------------------------------------------------------------
// Set icon
//-----------------------------------------------------------------------------
      HPOINTER hptr = (HPOINTER)WinLoadPointer(HWND_DESKTOP, NULLHANDLE, 1);
      WinSendMsg(hwndDlg, WM_SETICON, (MPARAM) hptr, 0l);
      for ( i=0; i<4; i++ )
        WinSendDlgItemMsg(hwndDlg, EF_A1+i, EM_SETTEXTLIMIT, (MPARAM)3, 0);

      WinSendDlgItemMsg(hwndDlg,
                        SBH,
                        SLM_SETTICKSIZE,
                        MPFROM2SHORT(SMA_SETALLTICKS, 3),
                        NULL);
      for ( i=0; i<=32; i+=8 )
        {
        itoa(i, OutScale, 10);
        WinSendDlgItemMsg(hwndDlg,
                          SBH,
                          SLM_SETSCALETEXT,
                          MPFROMLONG(i),
                          MPFROMP(OutScale));
        }
      break;
      }
//-----------------------------------------------------------------------------
// Handle WM_COMMAND
//-----------------------------------------------------------------------------
    case WM_COMMAND:
      {
      switch(SHORT1FROMMP(mp1))
        {
//-----------------------------------------------------------------------------
// Close the dialog
//-----------------------------------------------------------------------------
        case PB_EXIT:
          {
          WinSendMsg(hwndDlg, WM_CLOSE, 0L, 0L);
          break;
          }
//-----------------------------------------------------------------------------
// Calculate
//-----------------------------------------------------------------------------
        case PB_CALCULATE:
          {
          WinSetDlgItemText(hwndDlg, LT_NET,  "");
          WinSetDlgItemText(hwndDlg, TXT_MB,  "");
          WinSetDlgItemText(hwndDlg, TXT_M,   "");
          WinSetDlgItemText(hwndDlg, TXT_SB,  "");
          WinSetDlgItemText(hwndDlg, TXT_HB,  "");
          WinSetDlgItemText(hwndDlg, TXT_NS,  "");
          WinSetDlgItemText(hwndDlg, TXT_NH,  "");
          WinSetDlgItemText(hwndDlg, TXT_T,   "");
          WinSetDlgItemText(hwndDlg, TXT_CN,  "");
          WinSetDlgItemText(hwndDlg, TXT_CB,  "");
          WinSetDlgItemText(hwndDlg, TXT_CHR, "");

          for ( i=0; i<4; i++ )
            {
//-----------------------------------------------------------------------------
// Разберем поля IP-адреса
//-----------------------------------------------------------------------------
            if ( (num=WinQueryDlgItemTextLength(hwndDlg, EF_A1+i)) == 0 )
              {
              SendErrMsg(hwndDlg, ErrAddr);
              return (0);
              }

            WinQueryDlgItemText(hwndDlg, EF_A1+i, sizeof(InField), InField);
            for ( j=0; j<num; j++)
              if ( isdigit(InField[j]) == 0 )
                {
                SendErrMsg(hwndDlg, ErrAddr);
                return (0);
                }

            if ( (num=atoi(InField)) > 255 )
              {
              SendErrMsg(hwndDlg, ErrAddr);
              return (0);
              }

            addr[i] = num;
            }
//-----------------------------------------------------------------------------
// Разберем адрес IP
//-----------------------------------------------------------------------------
          memset(network, 0, sizeof(network));
          if ( (addr[0] & '\x80') == '\x00' )
            {
            class[0] = 'A';
            i = 1;
            }
          else
            if ( (addr[0] & '\xc0') == '\x80' )
              {
              class[0] = 'B';
              i = 2;
              }
            else
              if ( (addr[0] & '\xe0') == '\xc0' )
                {
                class[0] = 'C';
                i = 3;
                }
              else
                {
                SendErrMsg(hwndDlg, ErrAddr);
                return (0);
                }
          masklen = 8 * i;

          for ( j=0; j<i; j++ ) network[j] = addr[j];
          sprintf(OutMsg, "Class %s network %d.%d.%d.%d", class,
                  network[0], network[1], network[2], network[3]);
          WinSetDlgItemText(hwndDlg, LT_NET, OutMsg);
//-----------------------------------------------------------------------------
// Разберем поля маски
//-----------------------------------------------------------------------------
          slider =
              (ULONG)WinSendDlgItemMsg(hwndDlg,
                                       SBH,
                                       SLM_QUERYSLIDERINFO,
                                       MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                                    SMA_INCREMENTVALUE),
                                       NULL);

          if ( (slider<masklen) || (slider==(masklen+1)) || (slider>=31) )
            {
            SendErrMsg(hwndDlg, ErrMask);
            return (0);
            }

          sprintf(OutMsg, "%d", slider);
          WinSetDlgItemText(hwndDlg, TXT_MB, OutMsg);

          memset(mask, 0, sizeof(mask));
          for ( i=0, k=0; i<4; i++)
            for ( j=0; (j<8)&&(k<slider); j++, k++ ) mask[i] |= ('\x80'>>j);

          sprintf(OutMsg, "%d.%d.%d.%d", mask[0], mask[1], mask[2], mask[3]);
          WinSetDlgItemText(hwndDlg, TXT_M, OutMsg);

          subnetbit = slider-masklen;
          sprintf(OutMsg, "%d", subnetbit);
          WinSetDlgItemText(hwndDlg, TXT_SB, OutMsg);
          hostbit = 32-masklen-subnetbit;
          sprintf(OutMsg, "%d", hostbit);
          WinSetDlgItemText(hwndDlg, TXT_HB, OutMsg);

          if ( subnetbit != 0 ) numsubnet = (1L<<subnetbit)-2;
          else numsubnet = 0;
          sprintf(OutMsg, "%d", numsubnet);
          WinSetDlgItemText(hwndDlg, TXT_NS, OutMsg);
          numhost = (1L<<hostbit)-2;
          sprintf(OutMsg, "%d", numhost);
          WinSetDlgItemText(hwndDlg, TXT_NH, OutMsg);
          if ( subnetbit != 0 ) numtotal = numsubnet*numhost;
          else numtotal = numhost;
          sprintf(OutMsg, "%d", numtotal);
          WinSetDlgItemText(hwndDlg, TXT_T, OutMsg);

          for (i=0; i<4; i++) currnet[i] = addr[i] & mask[i];
          sprintf(OutMsg, "%d.%d.%d.%d",
                  currnet[0], currnet[1], currnet[2], currnet[3]);
          WinSetDlgItemText(hwndDlg, TXT_CN, OutMsg);

          for (i=0; i<4; i++) broadcast[i] = currnet[i] | (~mask[i]);
          sprintf(OutMsg, "%d.%d.%d.%d",
                  broadcast[0], broadcast[1], broadcast[2], broadcast[3]);
          WinSetDlgItemText(hwndDlg, TXT_CB, OutMsg);

          for (i=0; i<4; i++)
            {
            netstart[i] = currnet[i];
            netstop[i] = broadcast[i];
            }
          netstart[3]++;
          netstop[3]-=1;
          sprintf(OutMsg, "%d.%d.%d.%d-%d.%d.%d.%d",
                  netstart[0], netstart[1], netstart[2], netstart[3],
                  netstop[0],  netstop[1],  netstop[2],  netstop[3]);
          WinSetDlgItemText(hwndDlg, TXT_CHR, OutMsg);

          return (0);
          }
//-----------------------------------------------------------------------------
// Расскажем о себе
//-----------------------------------------------------------------------------
        case PB_ABOUT:
          {
          WinDlgBox( HWND_DESKTOP,
                     hwndDlg,
                     WinDefDlgProc,
                     NULLHANDLE,
                     200,
                     0 );
          return (0);
          }
        }
      break;
      }
    }

  return WinDefDlgProc(hwndDlg, msg, mp1, mp2);
  }

//=============================================================================
// SendErrMsg - подпрограмма выдачи сообщений об ошибках
//=============================================================================
void SendErrMsg(HWND hwnd, char *ptr)
{
   WinMessageBox( HWND_DESKTOP,
                  hwnd,
                  ptr,
                  "Wildcard Message",
                  0,
                  MB_OK | MB_APPLMODAL | MB_ERROR );
}
