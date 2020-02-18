/* Begin FCbegin */
//-------------------------------------------------------------------------
// STAY / W      Resource file BUSYINDC.XME
//-------------------------------------------------------------------------
// Created 26\10\2019 19:20
//-------------------------------------------------------------------------

#include "BUSYINDC.h"

/* End FCbegin */
/* Begin FCinclude */
#include "SERVERADDR.h"
/* End FCinclude */
/* Begin FChead */
PICMODULEBEG(BUSYINDC)

static StayFD _nn_BUSYINDC = {NULL,TAG_PIC,0,0,0,0,"BUSYINDC",NULL};
/* End FChead */
/* Begin FCtypedef */
/* End FCtypedef */
/* Begin FCfield */
DWORD TypeMask;
StayFD _n_TypeMask = {&TypeMask,0,0,0,CLS_L,0,"TypeMask",""};
StayFD *_TypeMask = &_n_TypeMask;

/* End FCfield */
/* Begin FCextern */
extern StayWindow _n_WndBusy;
extern StayWindow _n_WndSetAddr;
/* End FCextern */
/* Begin FCdataset */
/* End FCdataset */
/* Begin FCmenu */
/* End FCmenu */
/* Begin FCframe */

IWStayWin *_init_WndBusy ()
{
 IWStayWin *item[8];
 CreateFrame ("WndBusy", 5, 5, 245, 145, STM_MODIFIED, STW_CENTER, "Триває обробка", &item[0]);
 item[0]->CreateItem ("STAYSTATIC", NULL, 39, 11, 166, 20, STM_MODIFIED, 0, 0, "З\'єднання з сервером.", NULL);
 item[0]->CreateItem ("STAYSTATIC", NULL, 42, 40, 160, 48, STM_MODIFIED, 0, 0, "Програма може деякий час не відповідати.", NULL);
 return item[0];
}
StayWindow _n_WndBusy = {NULL,TAG_FRAME,0,0,0,"WndBusy",_init_WndBusy};
StayWindow *WndBusy = &_n_WndBusy;


IWStayWin *_init_WndSetAddr ()
{
 IWStayWin *item[8];
 CreateFrame ("WndSetAddr", 63, 131, 326, 219, STM_MODIFIED, STW_CENTER, "Настройка адреси сервера", &item[0]);
 item[0]->CreateItem ("STAYEDIT", "J_SRVIP", 140, 18, 161, 18, STM_MODIFIED, 0, FLD1, "", &item[1]);
 item[1]->SetProperty ("source", (DWORD)&_n_J_SRVIP);
 item[1]->Release ();
 item[0]->CreateItem ("STAYEDIT", "J_SRVPORT", 140, 41, 161, 18, STM_MODIFIED, 0, FLD2, "", &item[1]);
 item[1]->SetProperty ("source", (DWORD)&_n_J_SRVPORT);
 item[1]->Release ();
 item[0]->CreateItem ("STAYBUTTON", NULL, 92, 142, 142, 24, STM_CLICKED, 0, BUT1, "\"Enter\" Підтвердити", NULL);
 item[0]->CreateItem ("STAYSTATIC", NULL, 22, 18, 45, 18, STM_NORMAL, 0, 0, "IP:", NULL);
 item[0]->CreateItem ("STAYSTATIC", NULL, 22, 41, 45, 18, STM_NORMAL, 0, 0, "Порт:", NULL);
 item[0]->CreateItem ("STAYBUTTON", NULL, 92, 112, 142, 24, STM_CLICKED, 0, BUT2, "Тест", NULL);
 item[0]->CreateItem ("STAYBUTTON", NULL, 13, 167, 300, 20, STM_HIDDEN, 0, BUT3, "Встановити час очікування відповіді", NULL);
 item[0]->CreateItem ("STAYRADIO", "TypeMask", 17, 62, 295, 20, STM_MODIFIED, 0, RBUT1, "Алгоритм передачі для поганого зв\'язку", &item[1]);
 item[1]->SetProperty ("source", (DWORD)_TypeMask);
 item[1]->SetProperty ("mask", 0);
 item[1]->Release ();
 item[0]->CreateItem ("STAYRADIO", "TypeMask", 17, 86, 296, 20, STM_MODIFIED, 0, RBUT2, "Алгоритм передачі для доброго зв\'язку", &item[1]);
 item[1]->SetProperty ("source", (DWORD)_TypeMask);
 item[1]->SetProperty ("mask", 1);
 item[1]->Release ();
 return item[0];
}
StayWindow _n_WndSetAddr = {NULL,TAG_FRAME,0,0,0,"WndSetAddr",_init_WndSetAddr};
StayWindow *WndSetAddr = &_n_WndSetAddr;

/* End FCframe */
/* Begin FCtabl */
void *BUSYINDC[] =
 {NULL,
  &_nn_BUSYINDC,
  &_n_TypeMask,
  (StayFD *)WndBusy,
  (StayFD *)WndSetAddr,
  NULL};

PICMODULEEND(BUSYINDC)

/* End FCtabl */
/* Begin FTfield */
/* End FTfield */
/* Begin FTdataset */
/* End FTdataset */
/* Begin FTtcm */
/* End FTtcm */
/* Begin FTframe */
/* End FTframe */
/* Begin FTtabl */
/* End FTtabl */
/* Begin FCmmm */
/* End FCmmm */
/* Begin FMfield */
/* End FMfield */
/* Begin FMdataset */
/* End FMdataset */
/* Begin FMframe */
/* End FMframe */
/* Begin FMend */
/* End FMend */
/* Begin FTend */
/* End FTend */
/* Begin FCend */
/* End FCend */
