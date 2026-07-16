// buzzer.c - dos voces, misma logica del codigo del profe (sin floats)
//   PWM_0 = efectos   |   PWM_1 = musica
// Requisito: los DOS PWM con reloj 2 MHz (MFCLK/2) e interrupcion ZERO_EVENT.

#include "buzzer.h"

#ifndef REST
#define REST 0

#define timerClkFreq  2000000u

uint32_t divide(uint32_t dividend, uint32_t divisor) {
    uint32_t quotient = 0, remainder = 0;
    for (int i = 31; i >= 0; i--) {
        remainder <<= 1;
        remainder |= (dividend >> i) & 1;
        if (remainder >= divisor) { remainder -= divisor; quotient |= (1U << i); }
    }
    return quotient;
}

// ---------- VOZ EFECTOS (PWM_0) ----------
static const uint16_t *sN; static const int8_t *sD;
static uint8_t  sLen = 0, sTempo = 80, sThis = 0;
static bool     sLoop = false, sPlay = true;
static uint32_t sWhole, sDur, sWait, sCnt = 0;
static uint16_t sPer = 1000, sDuty = 1000;

void buzzer_play(const uint16_t *notes, const int8_t *durations,
                 uint8_t len, uint8_t tempo, bool loop) {
    DL_Timer_stopCounter(PWM_0_INST);
    sN=notes; sD=durations; sLen=len; sTempo=tempo; sLoop=loop;
    sThis=0; sCnt=0; sDur=0; sWait=0; sPlay=true;
    DL_Timer_setLoadValue(PWM_0_INST, 1000);
    DL_Timer_startCounter(PWM_0_INST);
}

void PWM_0_INST_IRQHandler(void) {
    if (DL_Timer_getPendingInterrupt(PWM_0_INST) != DL_TIMER_IIDX_ZERO) return;
    if (sCnt == sDur || sCnt == sWait) {
        DL_Timer_stopCounter(PWM_0_INST);
        sCnt = 0;
        if (sPlay) {
            if (sThis >= sLen) { if (sLoop) sThis = 0; else { return; } }
            if (sN[sThis] == REST) { sWhole = divide(240000u, sTempo); sPer = 1000; sDuty = 1000; }
            else {
                sWhole = divide(240u*sN[sThis], sTempo);
                sPer   = (uint16_t)divide(timerClkFreq, sN[sThis]);
                sDuty  = sPer / 5u;                 // 20% (como el codigo del profe: suena limpio)
            }
            int8_t dv = sD[sThis];
            if (dv > 0) sDur = divide(sWhole, dv);
            else { sDur = divide(sWhole, -dv); sDur += sDur >> 1; }
            sWait = divide(sDur*3u, 10u);
            sPlay = false; sThis++;
        } else { sDuty = sPer; sPlay = true; }
        DL_Timer_setLoadValue(PWM_0_INST, sPer);
        DL_Timer_setCaptureCompareValue(PWM_0_INST, sDuty, DL_TIMER_CC_0_INDEX);
        DL_Timer_startCounter(PWM_0_INST);
    } else sCnt++;
}

// ---------- VOZ MUSICA (PWM_1) ----------
static const uint16_t *mN; static const int8_t *mD;
static uint8_t  mLen = 0, mTempo = 80, mThis = 0;
static bool     mLoop = false, mPlay = true, mOn = false;
static uint32_t mWhole, mDur, mWait, mCnt = 0;
static uint16_t mPer = 1000, mDuty = 1000;

void buzzer_music_play(const uint16_t *notes, const int8_t *durations,
                       uint8_t len, uint8_t tempo, bool loop) {
    DL_Timer_stopCounter(PWM_1_INST);
    mN=notes; mD=durations; mLen=len; mTempo=tempo; mLoop=loop;
    mThis=0; mCnt=0; mDur=0; mWait=0; mPlay=true; mOn=true;
    DL_Timer_setLoadValue(PWM_1_INST, 1000);
    DL_Timer_startCounter(PWM_1_INST);
}

void buzzer_music_stop(void) {
    DL_Timer_stopCounter(PWM_1_INST);
    mOn = false;
    DL_Timer_setCaptureCompareValue(PWM_1_INST, mPer, DL_TIMER_CC_1_INDEX); // silencio (PWM_1 usa CC1)
}

void PWM_1_INST_IRQHandler(void) {
    if (DL_Timer_getPendingInterrupt(PWM_1_INST) != DL_TIMER_IIDX_ZERO) return;
    if (!mOn) return;
    if (mCnt == mDur || mCnt == mWait) {
        DL_Timer_stopCounter(PWM_1_INST);
        mCnt = 0;
        if (mPlay) {
            if (mThis >= mLen) { if (mLoop) mThis = 0; else { mOn = false; return; } }
            if (mN[mThis] == REST) { mWhole = divide(240000u, mTempo); mPer = 1000; mDuty = 1000; }
            else {
                mWhole = divide(240u*mN[mThis], mTempo);
                mPer   = (uint16_t)divide(timerClkFreq, mN[mThis]);
                mDuty  = mPer / 5u;                 // 20%
            }
            int8_t dv = mD[mThis];
            if (dv > 0) mDur = divide(mWhole, dv);
            else { mDur = divide(mWhole, -dv); mDur += mDur >> 1; }
            mWait = divide(mDur*3u, 10u);
            mPlay = false; mThis++;
        } else { mDuty = mPer; mPlay = true; }
        DL_Timer_setLoadValue(PWM_1_INST, mPer);
        DL_Timer_setCaptureCompareValue(PWM_1_INST, mDuty, DL_TIMER_CC_1_INDEX);
        DL_Timer_startCounter(PWM_1_INST);
    } else mCnt++;
}

// ---------- init ----------
void buzzer_init(void) {
    NVIC_ClearPendingIRQ(PWM_0_INST_INT_IRQN);
    NVIC_EnableIRQ(PWM_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(PWM_1_INST_INT_IRQN);
    NVIC_EnableIRQ(PWM_1_INST_INT_IRQN);
}
#endif