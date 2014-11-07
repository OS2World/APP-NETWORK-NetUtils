//=============================================================================
// ConfReg2.c
// Программа определения значения Configuration Register
// для маршрутизаторов Cisco
//=============================================================================
#define INCL_WIN

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ConfReg2.h"

//-----------------------------------------------------------------------------
// Dialog Window procedure prototype
//-----------------------------------------------------------------------------
MRESULT EXPENTRY DlgMenu (HWND, ULONG ,MPARAM, MPARAM);
void SendErrMsg(HWND, char *);
void FormFields(HWND);

//-----------------------------------------------------------------------------
// Global Variablies
//-----------------------------------------------------------------------------
unsigned char ErrMsg[96], Byte_Hi = '\x00', Byte_Lo = '\x00';

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
  int i;
  char InReg[5];
  unsigned short ChkState;

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
      WinSendDlgItemMsg(hwndDlg, EF_VALUE, EM_SETTEXTLIMIT, (MPARAM)4, 0);
      FormFields(hwndDlg);
      break;
      }
//-----------------------------------------------------------------------------
// Handle WM_CONTROL
//-----------------------------------------------------------------------------
    case WM_CONTROL:
      {
      switch(SHORT1FROMMP(mp1))
        {
//-----------------------------------------------------------------------------
// Boot Field
//-----------------------------------------------------------------------------
        case RB_0:
          {
          Byte_Lo &= '\xf0';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_1:
          {
          Byte_Lo &= '\xf0';
          Byte_Lo |= '\x01';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_2:
          {
          Byte_Lo &= '\xf0';
          Byte_Lo |= '\x02';
          FormFields(hwndDlg);
          return (0);
          }
//-----------------------------------------------------------------------------
// Console Rate Field
//-----------------------------------------------------------------------------
        case RB_1200:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Hi |= '\x10';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_2400:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Hi |= '\x18';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_4800:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Hi |= '\x08';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_9600:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_19200:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Lo |= '\x20';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_38400:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Lo |= '\x20';
          Byte_Hi |= '\x08';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_57600:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Lo |= '\x20';
          Byte_Hi |= '\x10';
          FormFields(hwndDlg);
          return (0);
          }

        case RB_115200:
          {
          Byte_Hi &= '\xe7';
          Byte_Lo &= '\xdf';
          Byte_Lo |= '\x20';
          Byte_Hi |= '\x18';
          FormFields(hwndDlg);
          return (0);
          }
//-----------------------------------------------------------------------------
// Other Fields
//-----------------------------------------------------------------------------
        case CB_BIT4:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT4);
          if ( ChkState ) Byte_Lo |= '\x10';
          else Byte_Lo &= '\xef';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT6:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT6);
          if ( ChkState ) Byte_Lo |= '\x40';
          else Byte_Lo &= '\xbf';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT7:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT7);
          if ( ChkState ) Byte_Lo |= '\x80';
          else Byte_Lo &= '\x7f';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT8:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT8);
          if ( ChkState ) Byte_Hi |= '\x01';
          else Byte_Hi &= '\xfe';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT9:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT9);
          if ( ChkState ) Byte_Hi |= '\x02';
          else Byte_Hi &= '\xfd';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT10:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT10);
          if ( ChkState ) Byte_Hi |= '\x04';
          else Byte_Hi &= '\xfb';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT13:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT13);
          if ( ChkState ) Byte_Hi |= '\x20';
          else Byte_Hi &= '\xdf';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT14:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT14);
          if ( ChkState ) Byte_Hi |= '\x40';
          else Byte_Hi &= '\xbf';
          FormFields(hwndDlg);
          return (0);
          }

        case CB_BIT15:
          {
          ChkState = WinQueryButtonCheckstate(hwndDlg, CB_BIT15);
          if ( ChkState ) Byte_Hi |= '\x80';
          else Byte_Hi &= '\x7f';
          FormFields(hwndDlg);
          return (0);
          }
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
          if ( WinQueryDlgItemTextLength(hwndDlg, EF_VALUE) != 4 )
            {
            SendErrMsg(hwndDlg, "Wrong Configuration Register lenght");
            return (0);
            }
          WinQueryDlgItemText(hwndDlg, EF_VALUE, sizeof(InReg), InReg);
          for (i=0; i<4; i++)
            if ( isxdigit(InReg[i]) == 0 )
              {
              SendErrMsg(hwndDlg, "Wrong Configuration Register value");
              return (0);
              }

          if ( isdigit(InReg[0]) ) Byte_Hi = (InReg[0] - '0') << 4;
          else Byte_Hi = (toupper(InReg[0]) - 'A' + '\x0a') << 4;
          if ( isdigit(InReg[1]) ) Byte_Hi |= (InReg[1] - '0');
          else Byte_Hi |= (toupper(InReg[1]) - 'A' + '\x0a');

          if ( isdigit(InReg[2]) ) Byte_Lo = (InReg[2] - '0') << 4;
          else Byte_Lo = (toupper(InReg[2]) - 'A' + '\x0a') << 4;
          if ( isdigit(InReg[3]) ) Byte_Lo |= (InReg[3] - '0');
          else Byte_Lo |= (toupper(InReg[3]) - 'A' + '\x0a');

          FormFields(hwndDlg);
          return (0);
          }
//-----------------------------------------------------------------------------
// Reset Configuration Register to ZERO
//-----------------------------------------------------------------------------
        case PB_RESET:
          {
          Byte_Hi = '\x00';
          Byte_Lo = '\x00';
          FormFields(hwndDlg);
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
                  "ConfReg2 Message",
                  0,
                  MB_OK | MB_APPLMODAL | MB_ERROR );
}

//=============================================================================
// FormFields - подпрограмма формирования значения переключателей
//=============================================================================
void FormFields(HWND hwnd)
{
  unsigned char Temp, OutReg[5];

  sprintf(OutReg,"%2.2x%2.2x", Byte_Hi, Byte_Lo);
  WinSetDlgItemText(hwnd, EF_VALUE, OutReg);

  if ( Byte_Hi & '\x80' ) WinSetDlgItemText(hwnd, EF_BIT_15, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_15, "0");
  if ( Byte_Hi & '\x40' ) WinSetDlgItemText(hwnd, EF_BIT_14, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_14, "0");
  if ( Byte_Hi & '\x20' ) WinSetDlgItemText(hwnd, EF_BIT_13, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_13, "0");
  if ( Byte_Hi & '\x10' ) WinSetDlgItemText(hwnd, EF_BIT_12, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_12, "0");
  if ( Byte_Hi & '\x08' ) WinSetDlgItemText(hwnd, EF_BIT_11, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_11, "0");
  if ( Byte_Hi & '\x04' ) WinSetDlgItemText(hwnd, EF_BIT_10, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_10, "0");
  if ( Byte_Hi & '\x02' ) WinSetDlgItemText(hwnd, EF_BIT_9, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_9, "0");
  if ( Byte_Hi & '\x01' ) WinSetDlgItemText(hwnd, EF_BIT_8, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_8, "0");

  if ( Byte_Lo & '\x80' ) WinSetDlgItemText(hwnd, EF_BIT_7, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_7, "0");
  if ( Byte_Lo & '\x40' ) WinSetDlgItemText(hwnd, EF_BIT_6, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_6, "0");
  if ( Byte_Lo & '\x20' ) WinSetDlgItemText(hwnd, EF_BIT_5, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_5, "0");
  if ( Byte_Lo & '\x10' ) WinSetDlgItemText(hwnd, EF_BIT_4, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_4, "0");
  if ( Byte_Lo & '\x08' ) WinSetDlgItemText(hwnd, EF_BIT_3, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_3, "0");
  if ( Byte_Lo & '\x04' ) WinSetDlgItemText(hwnd, EF_BIT_2, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_2, "0");
  if ( Byte_Lo & '\x02' ) WinSetDlgItemText(hwnd, EF_BIT_1, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_1, "0");
  if ( Byte_Lo & '\x01' ) WinSetDlgItemText(hwnd, EF_BIT_0, "1");
  else WinSetDlgItemText(hwnd, EF_BIT_0, "0");

  Temp = Byte_Lo & '\x0f';
  if ( Temp == '\x00' ) WinCheckButton(hwnd, RB_0, TRUE);
  else if ( Temp == '\x01' ) WinCheckButton(hwnd, RB_1, TRUE);
       else WinCheckButton(hwnd, RB_2, TRUE);

  WinCheckButton(hwnd, CB_BIT4, (Byte_Lo & '\x10') == '\x10');
  WinCheckButton(hwnd, CB_BIT6, (Byte_Lo & '\x40') == '\x40');
  WinCheckButton(hwnd, CB_BIT7, (Byte_Lo & '\x80') == '\x80');

  WinCheckButton(hwnd, CB_BIT8,  (Byte_Hi & '\x01') == '\x01');
  WinCheckButton(hwnd, CB_BIT9,  (Byte_Hi & '\x02') == '\x02');
  WinCheckButton(hwnd, CB_BIT10, (Byte_Hi & '\x04') == '\x04');
  WinCheckButton(hwnd, CB_BIT13, (Byte_Hi & '\x20') == '\x20');
  WinCheckButton(hwnd, CB_BIT14, (Byte_Hi & '\x40') == '\x40');
  WinCheckButton(hwnd, CB_BIT15, (Byte_Hi & '\x80') == '\x80');

  Temp = (Byte_Hi & '\x18') | (Byte_Lo & '\x20');
  switch (Temp)
    {
    case '\x00': { WinCheckButton(hwnd, RB_9600,   TRUE); break; }
    case '\x08': { WinCheckButton(hwnd, RB_4800,   TRUE); break; }
    case '\x18': { WinCheckButton(hwnd, RB_2400,   TRUE); break; }
    case '\x10': { WinCheckButton(hwnd, RB_1200,   TRUE); break; }
    case '\x20': { WinCheckButton(hwnd, RB_19200,  TRUE); break; }
    case '\x28': { WinCheckButton(hwnd, RB_38400,  TRUE); break; }
    case '\x30': { WinCheckButton(hwnd, RB_57600,  TRUE); break; }
    case '\x38': { WinCheckButton(hwnd, RB_115200, TRUE); break; }
    }
}