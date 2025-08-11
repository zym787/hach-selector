#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef _TIMER_H_GLOBALS_
#define PEXT
#else
#define PEXT extern
#endif


#define SEC (1000)
typedef struct
{
    uint16 timeDbg;
    uint16 timeCmd;
    uint16 timeMilli;
    uint32 timeWaitMill;
    uint32 everySec;
}_TIMER_T;

PEXT _TIMER_T timerPara;


PEXT void TIM2_Init(uint16 arr,uint16 psc);
PEXT void TIM3_Init(uint16 arr,uint16 psc);
PEXT void TIM4_Init(uint16 arr,uint16 psc);
PEXT void TIM5_Init(uint16 arr,uint16 psc);
PEXT void TIM6_Init(uint16 arr,uint16 psc);
PEXT void TIM7_Init(uint16 arr,uint16 psc);






#undef PEXT
#endif






















