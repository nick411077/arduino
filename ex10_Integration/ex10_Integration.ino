#include <SPI.h>
#include "ozbot.h"

#define BRAKE_TIME		100
#define CROSS_COUNT		129

#define PID_VAL_P		20
#define PID_VAL_I		0
#define PID_VAL_D		20
#define PID_SCALAR		0

#define	REC_SPEED_L		220
#define	REC_SPEED_R		220

#define	ROT_SPEED_L		340
#define ROT_SPEED_R		340

#define	MAX_SPEED_L		1024
#define MAX_SPEED_R		1024
#define CEN_SPEED_L		340
#define CEN_SPEED_R		340
#define	MIN_SPEED_L		-1024
#define MIN_SPEED_R		-1024

#define	CAL_SPEED_L		170
#define CAL_SPEED_R		170

#define	CAL_DZ_SPEED_L	230
#define CAL_DZ_SPEED_R	230


#define ACC_TACH1		900
#define STOP_TACH1		800
#define HOLD_STOP		5

#define ACC_SPEED_L		600 //加速度
#define ACC_SPEED_R		600

#define ERROR_VAL_1	1
#define ERROR_VAL_2	2
#define ERROR_VAL_3	4
#define ERROR_VAL_4	8

uint8_t I, Cnt;
uint8_t Point, Crossroad, Direction;
uint8_t bCnt, bNxtCnt;
uint8_t bStatus, bPassFlag;
int16_t iSecLenL, iSecLenR, iStopLen, iTotalLen;
int16_t DeadZoneL = 0, DeadZoneR = 0;


void CheckNext(void)
{
  OzBot.GetSecDir(bNxtCnt, Direction);
  OzBot.GetSecLen(bNxtCnt, iTotalLen, iSecLenR);
  bPassFlag = bNxtCnt;
  while (Direction == 2) {
    bPassFlag = bPassFlag + 1;
    OzBot.GetSecLen(bPassFlag, iSecLenL, iSecLenR);
    OzBot.GetSecDir(bPassFlag, Direction);
    iTotalLen += iSecLenL;
  }
  if (iTotalLen > ACC_TACH1) {
    iStopLen = iTotalLen - STOP_TACH1;
  }
  else {
    iStopLen = 0;
  }
}
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CheckCount(int iTarCntL)
{
  int16_t iCurCntL, iCurCntR;

  do {
    OzBot.GetCurSecTACH(iCurCntL, iCurCntR);
  } while (iCurCntL <= iTarCntL);

}

void CalDeadZone(void) //矯正光感
{
  iSecLenL = CAL_DZ_SPEED_L + DeadZoneL;
  iSecLenR = CAL_DZ_SPEED_R + DeadZoneR;
  OzBot.ForwardLR(iSecLenL, iSecLenR);
  delay(500);
  OzBot.SetTachInLR(0, 0);
  delay(2000);
  OzBot.TachInDual(iSecLenL, iSecLenR);
  OzBot.StopDual();
  iSecLenR = (iSecLenL - iSecLenR) / 16;
  if (iSecLenR > 80) {
    iSecLenR = 80;
  }
  else if (iSecLenR < -80) {
    iSecLenR = -80;
  }
  if ( iSecLenR >= 0) {
    DeadZoneR = DeadZoneR + iSecLenR;
  }
  else {
    DeadZoneL = DeadZoneL - iSecLenR;
  }
  if ( DeadZoneL >= DeadZoneR) {
    DeadZoneL = DeadZoneL - DeadZoneR;
    DeadZoneR = 0;
  }
  else {
    DeadZoneR = DeadZoneR - DeadZoneL;
    DeadZoneL = 0;
  }
  OzBot.SetMotorDeadZone(DeadZoneL, DeadZoneR);

  OzBot.CursorRC(1, 1);
  OzBot.Display(DeadZoneL);
  OzBot.ClearEOL();
  OzBot.CursorRC(2, 1);
  OzBot.Display(DeadZoneR);
  OzBot.ClearEOL();
}
void CalMode(void) //矯正左右偏移
{
  iSecLenR = -DeadZoneR - CAL_SPEED_R;
  iSecLenL = DeadZoneL + CAL_SPEED_L;
  OzBot.SetVelLR(iSecLenL, iSecLenR);
  OzBot.IrCal(1);
  delay(10000);
  OzBot.StopDual();
}
//------------------------------------------------------------------------
//	�D�{��
//------------------------------------------------------------------------

void setup(void)
{
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  OzBot.SetP(PID_VAL_P);
  OzBot.SetI(PID_VAL_I);
  OzBot.SetD(PID_VAL_D);
  OzBot.SetScalar(PID_SCALAR);
  OzBot.SetIrMode(1);
  OzBot.SetErrScale(ERROR_VAL_1, ERROR_VAL_2, ERROR_VAL_3, ERROR_VAL_4);
  OzBot.SetSpdCtrlL(MIN_SPEED_L, MAX_SPEED_L);
  OzBot.SetSpdCtrlR(MIN_SPEED_R, MAX_SPEED_R);
  OzBot.SetCrossCount(CROSS_COUNT);

  OzBot.SetLineColor(0);
  OzBot.RecTurnControlL(16, -REC_SPEED_L, REC_SPEED_R, 12, 120);
  OzBot.RecTurnControlR( 4,  REC_SPEED_L, -REC_SPEED_R, 12, 120);
  OzBot.ArcTurnControlL(16, 			0,  ROT_SPEED_R, 12, 120);
  OzBot.ArcTurnControlR( 4, ROT_SPEED_L,  0	       , 12, 120);
  OzBot.GetMotorDeadZone(DeadZoneL, DeadZoneR);

}
void loop(void)
{
  if (digitalRead(9) == LOW) //按下
  {
    OzBot.Beep();
    digitalWrite(12, HIGH);
    delay(1500);
    if (digitalRead(9) == 0)
    {
      OzBot.Beep();
      delay(1500);
      if (digitalRead(9) == 0)
      {
        // DeadZone CAL
        OzBot.Beep();
        delay(1000);
        CalDeadZone();
      }
      else
      {
        // IR CAL
        CalMode();
      }
    }
    else
    {
      //REC_ROUTE:
      OzBot.StartRec(1);
      OzBot.AutoBeep(1);
      OzBot.SetStraight(REC_SPEED_L, REC_SPEED_R);
      OzBot.SpdCtrlOn(1);
      do
      {
        OzBot.GetRecStatus(bStatus);
      } while (bStatus != 0);
      OzBot.ForwardDual(REC_SPEED_L);
      delay(BRAKE_TIME);
      OzBot.BrakeDual();
      OzBot.AutoBeep(0);
      OzBot.StopRec();
    }
    digitalWrite(12, LOW);
  }
  else if (digitalRead(8) == LOW)
  {
    //RACE_ROUTE:
    OzBot.GetSecLen(0, iSecLenL, iSecLenR);
    if (iSecLenL > ACC_TACH1)
    {
      iStopLen = iSecLenL - STOP_TACH1;
    }
    else
    {
      iStopLen = 0;
    }
    bNxtCnt = 0;
    digitalWrite(13, HIGH);
    OzBot.Beep();
    delay(1500);
    if(digitalRead(8) == 0)
    {
      OzBot.Beep();
      delay(1500);
      if (digitalRead(8) == 0)
      {
        // DeadZone CAL
        OzBot.Beep();
        delay(1000);
        OzBot.SearchPath(0);
      }
      else
      {
        // IR CAL
        OzBot.SearchPath(1);
      }
      
    }
    else
    {
      OzBot.AutoBeep(0);
      OzBot.SetStraight(CEN_SPEED_L, CEN_SPEED_R);
      OzBot.SpdCtrlOn(2);
      do
      {
        OzBot.GetSecCnt(bCnt);
        if (bNxtCnt == bCnt)
        {
          bNxtCnt = bCnt + 1;
          if (iStopLen > 0)
          {
            OzBot.SetStraight(CEN_SPEED_L, CEN_SPEED_R);
            delay(5);
            OzBot.SetStraight(ACC_SPEED_L, ACC_SPEED_R);
            CheckCount(iStopLen);
            OzBot.SetStraight(0, 0);
            delay(HOLD_STOP);
            OzBot.SetStraight(CEN_SPEED_L, CEN_SPEED_R);
            bNxtCnt = bPassFlag + 1;
          }
          CheckNext();
        }
        OzBot.GetRecStatus(bStatus);
      } while (bStatus != 0);
      OzBot.ForwardDual(CEN_SPEED_L);
      delay(BRAKE_TIME);
      OzBot.BrakeDual();
    }
    digitalWrite(13, LOW);
  }
}
