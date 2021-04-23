#pragma once
#if defined(CAL_L2a1) || defined(CAL_L2a2) || defined(CAL_L2a3) || defined(CAL_L2a4) || defined(CAL_a1f1) ||           \
    defined(CAL_a2f1) || defined(CAL_a2f3)
#define CAL_ADI
#endif
#if defined(CAL_a1f2) || defined(CAL_a2f4) || defined(CAL_a2f5) || defined(CAL_a2f6) || defined(CAL_a2f7)
#define CAL_AsyBidAsk
#define USE_AsyBidAsk
#endif

#if defined(CAL_a1f3) || defined(CAL_a2f23) || defined(CAL_a2f24) || defined(CAL_a2f25)
#define CAL_SumSizeDiff
#endif

#if defined(CAL_a1f4) || defined(CAL_a2f8)
#define CAL_DMB
#endif

#if defined(CAL_a1f5) || defined(CAL_a1f6) || defined(CAL_a2f9) || defined(CAL_a2f10)
#define CAL_EVOI
#endif

#if defined(CAL_a1f7) || defined(CAL_a1f8) || defined(CAL_a2f11) || defined(CAL_a2f12)
#define CAL_GTVol
#endif

#if defined(CAL_a1f9) || defined(CAL_a2f13) || defined(CAL_a2f14)
#define CAL_L2f1
#endif

#if defined(CAL_a1f10) || defined(CAL_a2f15) || defined(CAL_a2f16) || defined(CAL_a2f16)
#define CAL_MPBVS
#define USE_MPBVS
#endif

#if defined(CAL_a1f11) || defined(CAL_a1f12) || defined(CAL_a2f17) || defined(CAL_a2f18) || defined(CAL_a2f19)
#define CAL_MPB
#endif

#if defined(CAL_a1f13) || defined(CAL_a2f27) || defined(CAL_a2f28)
#define CAL_VOI
#endif

#if defined(CAL_a1f14) || defined(CAL_a1f15) || defined(CAL_a2f2) || defined(CAL_a2f29) || defined(CAL_a2f30)
#define CAL_VPI
#endif

#if defined(CAL_a2f21) || defined(CAL_a2f21) || defined(CAL_a2f22) || defined(CAL_a2f23)
#define CAL_SignVol_1
#endif

#define USE_VOI
#define USE_EVOI
#define USE_DMB
#define USE_MPB
#define USE_MPBVS
#define USE_DMB
#define USE_ADI
#define USE_L2f1
#define USE_GTVol
#define USE_VPI
#define USE_AsyBidAsk

#if defined(USE_EVOI)
#define USE_VOI
#endif
