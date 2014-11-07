//=============================================================================
// Wildcard.c
// Программа проверки значения Wildcard mask для маршрутизаторов Cisco
//=============================================================================
#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Wildcard.h"

//-----------------------------------------------------------------------------
// Dialog Window procedure prototype
//-----------------------------------------------------------------------------
MRESULT EXPENTRY DlgMenu (HWND, ULONG ,MPARAM, MPARAM);
void SendErrMsg(HWND, char *);
void CalcAddr(HWND, unsigned int);

//-----------------------------------------------------------------------------
// Global Variablies
//-----------------------------------------------------------------------------
unsigned char ErrMsg[96], addr[4], mask[4];

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
  unsigned int i, j, num;
  char InField[4];
  static char ErrAddr[] = "Wrong IP address";
  static char ErrMask[] = "Wrong wildcard mask";

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
      for ( i=0; i<8; i++ )
        WinSendDlgItemMsg(hwndDlg, EF_A1+i, EM_SETTEXTLIMIT, (MPARAM)3, 0);
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
          for ( i=0; i<4; i++ )
            {
            WinSendDlgItemMsg( hwndDlg,
                               MLE1+i,
                               MLM_SETSEL,
                               MPFROMSHORT(0),
                               (MPARAM)WinSendDlgItemMsg(hwndDlg,
                                                         MLE1+i,
                                                         MLM_QUERYTEXTLENGTH,
                                                         MPVOID,
                                                         MPVOID) );
            WinSendDlgItemMsg(hwndDlg, MLE1+i, MLM_CLEAR, MPVOID, MPVOID);
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
//-----------------------------------------------------------------------------
// Разберем поля маски
//-----------------------------------------------------------------------------
            if ( (num=WinQueryDlgItemTextLength(hwndDlg, EF_M1+i)) == 0 )
              {
              SendErrMsg(hwndDlg, ErrMask);
              return (0);
              }

            WinQueryDlgItemText(hwndDlg, EF_M1+i, sizeof(InField), InField);
            for ( j=0; j<num; j++)
              if ( isdigit(InField[j]) == 0 )
                {
                SendErrMsg(hwndDlg, ErrMask);
                return (0);
                }

            if ( (num=atoi(InField)) > 255 )
              {
              SendErrMsg(hwndDlg, ErrMask);
              return (0);
              }

            mask[i] = num;

            CalcAddr(hwndDlg, i);
            }
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

//=============================================================================
// CalcAddr - подпрограмма определения законного диапазона адресов
//=============================================================================
void CalcAddr(HWND hwnd, unsigned int Ind)
{
  unsigned char InvMask, SelAddr, i, start, stop, Msg[16];
  unsigned int Index;
  BOOL flag = FALSE;

  InvMask = ~mask[Ind];
  SelAddr = addr[Ind] & InvMask;

  for ( Index=0; Index<=255; Index++ )
    {
    i = Index;
    if ( (i & InvMask) == SelAddr )
      {
      if ( flag ) stop = i;
      else
        {
        flag = TRUE;
        start = i;
        stop = i;
        }
      }
      else
        {
        if ( !flag ) continue;
        flag = FALSE;
        if ( start == stop ) sprintf(Msg, "%d\n", start);
        else sprintf(Msg, "%d-%d\n", start, stop);
        WinSendDlgItemMsg(hwnd, MLE1+Ind, MLM_INSERT, Msg, 0l);
        }
    }
  if ( !flag ) return;
  if ( start == stop ) sprintf(Msg, "%d\n", start);
  else sprintf(Msg, "%d-%d\n", start, stop);
  WinSendDlgItemMsg(hwnd, MLE1+Ind, MLM_INSERT, Msg, 0l);
}