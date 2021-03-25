float sp = trigger_data->sp;
// HUSKY_TRADE_INFO("sp {}", sp);
#ifdef CAL_fpf1
float fpf1 = fp_factor.fpf1;
float fpf1_sp = fpf1 / sp;
#endif
#ifdef CAL_fpf3
float fpf3 = fp_factor.fpf3;
float fpf3_sp = fpf3 / sp;
#endif
#ifdef CAL_fpf4
float fpf4 = fp_factor.fpf4;
float fpf4_sp = fpf4 / sp;
#endif
#ifdef CAL_fpf5
float fpf5 = fp_factor.fpf5;
float fpf5_sp = fpf5 / sp;
#endif
#ifdef CAL_fpf6
float fpf6 = fp_factor.fpf6;
float fpf6_sp = fpf6 / sp;
#endif
#ifdef CAL_fpf7
float fpf7 = fp_factor.fpf7;
float fpf7_sp = fpf7 / sp;
#endif
#ifdef CAL_fpf8
float fpf8 = fp_factor.fpf8;
float fpf8_sp = fpf8 / sp;
#endif
#ifdef CAL_fpf9
float fpf9 = fp_factor.fpf9;
float fpf9_sp = fpf9 / sp;
#endif
#ifdef CAL_fpf10
float fpf10 = fp_factor.fpf10;
float fpf10_sp = fpf10 / sp;
#endif
#ifdef CAL_fpf11
float fpf11 = fp_factor.fpf11;
float fpf11_sp = fpf11 / sp;
#endif
#ifdef CAL_fpf12
float fpf12 = fp_factor.fpf12;
float fpf12_sp = fpf12 / sp;
#endif
#ifdef CAL_fpf13
float fpf13 = fp_factor.fpf13;
float fpf13_sp = fpf13 / sp;
#endif
#ifdef CAL_fpf14
float fpf14 = fp_factor.fpf14;
float fpf14_sp = fpf14 / sp;
#endif
#ifdef CAL_fpf15
float fpf15 = fp_factor.fpf15;
float fpf15_sp = fpf15 / sp;
#endif
#ifdef CAL_fpf16
float fpf16 = fp_factor.fpf16;
float fpf16_sp = fpf16 / sp;
#endif
#ifdef CAL_fpf17
float fpf17 = fp_factor.fpf17;
float fpf17_sp = fpf17 / sp;
#endif
#ifdef CAL_fpf18
float fpf18 = fp_factor.fpf18;
float fpf18_sp = fpf18 / sp;
#endif
#ifdef CAL_EVOI
float EVOI = trigger_data->EVOI();
LOG_INFO2("[{}] EVOI:{.5f}", __FUNCTION__, EVOI);
float EVOI_sp = EVOI / sp;
#endif
#ifdef CAL_GTVol
float GTVol = trigger_data->GTVol();
LOG_INFO2("[{}] GTVol:{:.5f}", __FUNCTION__, GTVol);
float GTVol_sp = GTVol / sp;
#endif
#ifdef CAL_L2f1
float L2f1 = trigger_data->L2f1;
LOG_INFO2("[{}] L2f1:{:.5f}", __FUNCTION__, L2f1);
float L2f1_sp = L2f1 / sp;
#endif
#ifdef CAL_L2f2
float L2f2 = trigger_data->L2f2;
LOG_INFO2("[{}] L2f2:{:.5f}", __FUNCTION__, L2f2);
float L2f2_sp = L2f2 / sp;
#endif
#ifdef CAL_L2f6
float L2f6 = trigger_data->L2f6;
LOG_INFO2("[{}] L2f6:{:.5f}", __FUNCTION__, L2f6);
float L2f6_sp = L2f6 / sp;
#endif
#ifdef CAL_L2f7
float L2f7 = trigger_data->L2f7;
LOG_INFO2("[{}] L2f7:{:.5f}", __FUNCTION__, L2f7);
float L2f7_sp = L2f7 / sp;
#endif
#ifdef CAL_L2f8
float L2f8 = trigger_data->L2f8;
LOG_INFO2("[{}] L2f8:{:.5f}", __FUNCTION__, L2f8);
float L2f8_sp = L2f8 / sp;
#endif
#ifdef CAL_L2f9
float L2f9 = trigger_data->L2f9;
LOG_INFO2("[{}] L2f9:{:.5f}", __FUNCTION__, L2f9);
float L2f9_sp = L2f9 / sp;
#endif
#ifdef CAL_L2f10
float L2f10 = trigger_data->L2f10;
LOG_INFO2("[{}] L2f10:{:.5f}", __FUNCTION__, L2f10);
float L2f10_sp = L2f10 / sp;
#endif
#ifdef CAL_L2f10_20
float L2f10_20 = trigger_data->L2f10_20;
LOG_INFO2("[{}] L2f10_20:{:.5f}", __FUNCTION__, L2f10_20);
float L2f10_20_sp = L2f10_20 / sp;
#endif
#ifdef CAL_L2f10_40
float L2f10_40 = trigger_data->L2f10_40;
LOG_INFO2("[{}] L2f10_40:{:.5f}", __FUNCTION__, L2f10_40);
float L2f10_40_sp = L2f10_40 / sp;
#endif
#ifdef CAL_L2f10_60
float L2f10_60 = trigger_data->L2f10_60;
LOG_INFO2("[{}] L2f10_60:{:.5f}", __FUNCTION__, L2f10_60);
float L2f10_60_sp = L2f10_60 / sp;
#endif
#ifdef CAL_L2f10_80
float L2f10_80 = trigger_data->L2f10_80;
LOG_INFO2("[{}] L2f10_80:{:.5f}", __FUNCTION__, L2f10_80);
float L2f10_80_sp = L2f10_80 / sp;
#endif
#ifdef CAL_L2f11
float L2f11 = trigger_data->L2f11;
LOG_INFO2("[{}] L2f10_11:{:.5f}", __FUNCTION__, L2f10_11);
float L2f11_sp = L2f11 / sp;
#endif
#ifdef CAL_L2f12
float L2f12 = trigger_data->L2f12;
LOG_INFO2("[{}] L2f10_12:{:.5f}", __FUNCTION__, L2f10_12);
float L2f12_sp = L2f12 / sp;
#endif
#ifdef CAL_L2f13
float L2f13 = trigger_data->L2f13;
LOG_INFO2("[{}] L2f13:{:.5f}", __FUNCTION__, L2f13);
float L2f13_sp = L2f13 / sp;
#endif
#ifdef CAL_L2f14
float L2f14 = trigger_data->L2f14;
LOG_INFO2("[{}] L2f14:{:.5f}", __FUNCTION__, L2f14);
float L2f14_sp = L2f14 / sp;
#endif
#ifdef CAL_L2f15
float L2f15 = trigger_data->L2f15;
LOG_INFO2("[{}] L2f15:{:.5f}", __FUNCTION__, L2f15);
float L2f15_sp = L2f15 / sp;
#endif
#ifdef CAL_PID
float PID = trigger_data->PID();
float PID_sp = PID / sp;
#endif
#ifdef CAL_MPB
float MPB = trigger_data->MPB();
float MPB_sp = MPB / sp;
#endif
#ifdef CAL_MPBVS
float MPBVS = trigger_data->MPBVS();
float MPBVS_sp = MPBVS / sp;
#endif
#ifdef CAL_DMB
float DMB = trigger_data->DMB();
float DMB_sp = DMB / sp;
#endif
#ifdef CAL_OIR
float OIR = trigger_data->OIR();
float OIR_sp = OIR / sp;
#endif
#ifdef CAL_SignVol_1
float SignVol_1 = trigger_data->SignVol_1();
float SignVol_1_sp = SignVol_1 / sp;
#endif
#ifdef CAL_VPI
float VPI = trigger_data->VPI();
float VPI_sp = VPI / sp;
#endif
#ifdef CAL_a1f1
float a1f1 = trigger_data->a1f1;
LOG_INFO2("[{}] a1f1:{:.5f}", __FUNCTION__, a1f1);
float a1f1_sp = a1f1 / sp;
#endif
#ifdef CAL_a1f2
float a1f2 = trigger_data->a1f2;
LOG_INFO2("[{}] a1f2:{:.5f}", __FUNCTION__, a1f2);
float a1f2_sp = a1f2 / sp;
#endif
#ifdef CAL_a1f3
float a1f3 = trigger_data->a2f3;
LOG_INFO2("[{}] a1f3:{:.5f}", __FUNCTION__, a1f3);
float a1f3_sp = a1f3 / sp;
#endif
#ifdef CAL_a1f4
float a1f4 = trigger_data->a1f4;
LOG_INFO2("[{}] a1f4:{:.5f}", __FUNCTION__, a1f4);
float a1f4_sp = a1f4 / sp;
#endif
#ifdef CAL_a1f5
float a1f5 = trigger_data->a1f5;
LOG_INFO2("[{}] a1f5:{:.5f}", __FUNCTION__, a1f5);
float a1f5_sp = a1f5 / sp;
#endif
#ifdef CAL_a1f6
float a1f6 = trigger_data->a1f6;
LOG_INFO2("[{}] a1f6:{:.5f}", __FUNCTION__, a1f6);
float a1f6_sp = a1f6 / sp;
#endif
#ifdef CAL_a1f7
float a1f7 = trigger_data->a1f7;
LOG_INFO2("[{}] a1f7:{:.5f}", __FUNCTION__, a1f7);
float a1f7_sp = a1f7 / sp;
#endif
#ifdef CAL_a1f8
float a1f8 = trigger_data->a1f8;
LOG_INFO2("[{}] a1f8:{:.5f}", __FUNCTION__, a1f8);
float a1f8_sp = a1f8 / sp;
#endif
#ifdef CAL_a1f9
float a1f9 = trigger_data->a1f9;
LOG_INFO2("[{}] a1f9:{:.5f}", __FUNCTION__, a1f9);
float a1f9_sp = a1f9 / sp;
#endif
#ifdef CAL_a1f10
float a1f10 = trigger_data->a1f10;
LOG_INFO2("[{}] a1f10:{:.5f}", __FUNCTION__, a1f10);
float a1f10_sp = a1f10 / sp;
#endif
#ifdef CAL_a1f11
float a1f11 = trigger_data->a1f11;
LOG_INFO2("[{}] a1f11:{:.5f}", __FUNCTION__, a1f11);
float a1f11_sp = a1f11 / sp;
#endif
#ifdef CAL_a1f13
float a1f13 = trigger_data->a1f13;
LOG_INFO2("[{}] a1f13:{:.5f}", __FUNCTION__, a1f13);
float a1f13_sp = a1f13 / sp;
#endif
#ifdef CAL_a1f14
float a1f14 = trigger_data->a1f14;
LOG_INFO2("[{}] a1f14:{:.5f}", __FUNCTION__, a1f14);
float a1f14_sp = a1f14 / sp;
#endif
#ifdef CAL_a2f1
float a2f1 = trigger_data->a2f1;
LOG_INFO2("[{}] a2f1:{:.5f}", __FUNCTION__, a2f1);
float a2f1_sp = a2f1 / sp;
#endif
#ifdef CAL_a2f2
float a2f2 = trigger_data->a2f2;
LOG_INFO2("[{}] a2f2:{:.5f}", __FUNCTION__, a2f2);
float a2f2_sp = a2f2 / sp;
#endif
#ifdef CAL_a2f3
float a2f3 = trigger_data->a2f3;
LOG_INFO2("[{}] a2f3:{:.5f}", __FUNCTION__, a2f3);
float a2f3_sp = a2f3 / sp;
#endif
#ifdef CAL_a2f4
float a2f4 = trigger_data->a2f4;
LOG_INFO2("[{}] a2f4:{:.5f}", __FUNCTION__, a2f4);
float a2f4_sp = a2f4 / sp;
#endif
#ifdef CAL_a2f5
float a2f5 = trigger_data->a2f5;
LOG_INFO2("[{}] a2f5:{:.5f}", __FUNCTION__, a2f5);
float a2f5_sp = a2f5 / sp;
#endif
#ifdef CAL_a2f6
float a2f6 = trigger_data->a2f6;
LOG_INFO2("[{}] a2f6:{:.5f}", __FUNCTION__, a2f6);
float a2f6_sp = a2f6 / sp;
#endif
#ifdef CAL_a2f7
float a2f7 = trigger_data->a2f7;
LOG_INFO2("[{}] a2f7:{:.5f}", __FUNCTION__, a2f7);
float a2f7_sp = a2f7 / sp;
#endif
#ifdef CAL_a2f8
float a2f8 = trigger_data->a2f8;
LOG_INFO2("[{}] a2f8:{:.5f}", __FUNCTION__, a2f8);
float a2f8_sp = a2f8 / sp;
#endif
#ifdef CAL_a2f9
float a2f9 = trigger_data->a2f9;
LOG_INFO2("[{}] a2f9:{:.5f}", __FUNCTION__, a2f9);
float a2f9_sp = a2f9 / sp;
#endif
#ifdef CAL_a2f10
float a2f10 = trigger_data->a2f10;
LOG_INFO2("[{}] a2f10:{:.5f}", __FUNCTION__, a2f10);
float a2f10_sp = a2f10 / sp;
#endif
#ifdef CAL_a2f11
float a2f11 = trigger_data->a2f11;
LOG_INFO2("[{}] a2f11:{:.5f}", __FUNCTION__, a2f11);
float a2f11_sp = a2f11 / sp;
#endif
#ifdef CAL_a2f12
float a2f12 = trigger_data->a2f12;
LOG_INFO2("[{}] a2f12:{:.5f}", __FUNCTION__, a2f12);
float a2f12_sp = a2f12 / sp;
#endif
#ifdef CAL_a2f13
float a2f13 = trigger_data->a2f13;
LOG_INFO2("[{}] a2f13:{:.5f}", __FUNCTION__, a2f13);
float a2f13_sp = a2f13 / sp;
#endif
#ifdef CAL_a2f14
float a2f14 = trigger_data->a2f14;
LOG_INFO2("[{}] a2f14:{:.5f}", __FUNCTION__, a2f14);
float a2f14_sp = a2f14 / sp;
#endif
#ifdef CAL_a2f15
float a2f15 = trigger_data->a2f15;
LOG_INFO2("[{}] a2f15:{:.5f}", __FUNCTION__, a2f15);
float a2f15_sp = a2f15 / sp;
#endif
#ifdef CAL_a2f16
float a2f16 = trigger_data->a2f16;
LOG_INFO2("[{}] a2f16:{:.5f}", __FUNCTION__, a2f16);
float a2f16_sp = a2f16 / sp;
#endif
#ifdef CAL_a2f17
float a2f17 = trigger_data->a2f17;
LOG_INFO2("[{}] a2f17:{:.5f}", __FUNCTION__, a2f17);
float a2f17_sp = a2f17 / sp;
#endif
#ifdef CAL_a2f18
float a2f18 = trigger_data->a2f18;
float a2f18_sp = a2f18 / sp;
#endif
#ifdef CAL_a2f19
float a2f19 = trigger_data->a2f19;
float a2f19_sp = a2f19 / sp;
#endif
#ifdef CAL_a2f21
float a2f21 = trigger_data->a2f21;
float a2f21_sp = a2f21 / sp;
#endif
#ifdef CAL_a2f22
float a2f22 = trigger_data->a2f22;
#endif
#ifdef CAL_a2f23
float a2f23 = trigger_data->a2f23;
#endif
#ifdef CAL_a2f24
float a2f24 = trigger_data->a2f24;
float a2f24_sp = a2f24 / sp;
#endif
#ifdef CAL_a2f25
float a2f25 = trigger_data->a2f25;
float a2f25_sp = a2f25 / sp;
#endif
#ifdef CAL_a2f26
float a2f26 = trigger_data->a2f26;
#endif
#ifdef CAL_a2f27
float a2f27 = trigger_data->a2f27;
#endif
#ifdef CAL_a2f28
float a2f28 = trigger_data->a2f28;
#endif
#ifdef CAL_a2f29
float a2f29 = trigger_data->a2f29;
#endif
#ifdef CAL_a2f30
float a2f30 = trigger_data->a2f30;
#endif
#ifdef CAL_AsyBidAsk
float AsyBidAsk = trigger_data->AsyBidAsk();
float AsyBidAsk_sp = AsyBidAsk / sp;
#endif
#ifdef CAL_VOI
float VOI = trigger_data->VOI();
float VOI_sp = VOI / sp;
#endif
#ifdef CAL_a1f12
float a1f12 = trigger_data->a1f12;
float a1f12_sp = a1f12 / sp;
#endif
#ifdef CAL_a1f15
float a1f15 = trigger_data->a1f15;
LOG_INFO2("[{}] a1f15:{:.5f}", __FUNCTION__, a1f15);
float a1f15_sp = a1f15 / sp;
#endif
#ifdef CAL_Ff1
float Ff1 = trigger_data->Ff1;
float Ff1_sp = Ff1 / sp;
#endif
#ifdef CAL_Ff2
float Ff2 = trigger_data->Ff2;
#endif
#ifdef CAL_Ff3
float Ff3 = trigger_data->Ff3;
#endif
#ifdef CAL_Ff4
float Ff4 = trigger_data->Ff4;
#endif
#ifdef CAL_Ff5
float Ff5 = trigger_data->Ff5;
float Ff5_sp = Ff5 / sp;
#endif
#ifdef CAL_Ff6
float Ff6 = trigger_data->Ff6;
float Ff6_sp = Ff6 / sp;
#endif
#ifdef CAL_Ff7_10
float Ff7_10 = trigger_data->Ff7_10;
LOG_INFO2("[{}] Ff7_10:{:.5f}", __FUNCTION__, Ff7_10);
float Ff7_10_sp = Ff7_10 / sp;
#endif
#ifdef CAL_Ff7_20
float Ff7_20 = trigger_data->Ff7_20;
float Ff7_20_sp = Ff7_20 / sp;
#endif
#ifdef CAL_Ff7_40
float Ff7_40 = trigger_data->Ff7_40;
float Ff7_40_sp = Ff7_40 / sp;
#endif
#ifdef CAL_Ff7_80
float Ff7_80 = trigger_data->Ff7_80;
float Ff7_80_sp = Ff7_80 / sp;
#endif
#ifdef CAL_Ff7
float Ff7 = trigger_data->Ff7;
LOG_INFO2("[{}] Ff7:{:.5f}", __FUNCTION__, Ff7);
float Ff7_sp = Ff7 / sp;
#endif
#ifdef CAL_Ff8_1
float Ff8_1 = trigger_data->Ff8_1;
LOG_INFO2("[{}] Ff8_1:{:.5f}", __FUNCTION__, Ff8_1);
float Ff8_1_sp = Ff8_1 / sp;
#endif
#ifdef CAL_Ff8_2
float Ff8_2 = trigger_data->Ff8_2;
float Ff8_2_sp = Ff8_2 / sp;
#endif
#ifdef CAL_L2a1
float L2a1 = trigger_data->L2a1;
LOG_INFO2("[{}] L2a1:{:.5f}", __FUNCTION__, L2a1);
float L2a1_sp = L2a1 / sp;
#endif
#ifdef CAL_L2a2
float L2a2 = trigger_data->L2a2;
LOG_INFO2("[{}] L2a2:{:.5f}", __FUNCTION__, L2a2);
float L2a2_sp = L2a2 / sp;
#endif
#ifdef CAL_L2a3
float L2a3 = trigger_data->L2a3;
LOG_INFO2("[{}] L2a3:{:.5f}", __FUNCTION__, L2a3);
float L2a3_sp = L2a3 / sp;
#endif
#ifdef CAL_L2a4
float L2a4 = trigger_data->L2a4;
LOG_INFO2("[{}] L2a4:{:.5f}", __FUNCTION__, L2a4);
float L2a4_sp = L2a4 / sp;
#endif
#ifdef CAL_L2p1
float L2p1 = trigger_data->L2p1;
LOG_INFO2("[{}] L2p1:{:.5f}", __FUNCTION__, L2p1);
float L2p1_sp = L2p1 / sp;
#endif
#ifdef CAL_L2p2
float L2p2 = trigger_data->L2p2;
LOG_INFO2("[{}] L2p2:{:.5f}", __FUNCTION__, L2p2);
float L2p2_sp = L2p2 / sp;
#endif
#ifdef CAL_L2f10_10
float L2f10_10 = trigger_data->L2f10_10;
LOG_INFO2("[{}] L2f10_10:{:.5f}", __FUNCTION__, L2f10_10);
float L2f10_10_sp = L2f10_10 / sp;
#endif
#ifdef CAL_sa2
float sa2 = trigger_data->sa2;
LOG_INFO2("[{}] sa2:{:.5f}", __FUNCTION__, sa2);
#endif
#ifdef CAL_sa3
float sa3 = trigger_data->sa3;
LOG_INFO2("[{}] sa3:{:.5f}", __FUNCTION__, sa3);
#endif
#ifdef CAL_sab
float sab = trigger_data->sab;
#endif
#ifdef CAL_Tf1
float Tf1 = trigger_data->Tf1;
float Tf1_sp = Tf1 / sp;
#endif
#ifdef CAL_Tf2
float Tf2 = trigger_data->Tf2;
float Tf2_sp = Tf2 / sp;
#endif
#ifdef CAL_Tf3
float Tf3 = trigger_data->Tf3;
float Tf3_sp = Tf3 / sp;
#endif
#ifdef CAL_Tf4
float Tf4 = trigger_data->Tf4;
float Tf4_sp = Tf4 / sp;
#endif
#ifdef CAL_Tf5
float Tf5 = trigger_data->Tf5;
float Tf5_sp = Tf5 / sp;
#endif
#ifdef CAL_Tf6
float Tf6 = trigger_data->Tf6;
float Tf6_sp = Tf6 / sp;
#endif
#ifdef CAL_Tf7
float Tf7 = trigger_data->Tf7;
float Tf7_sp = Tf7 / sp;
#endif
#ifdef CAL_Ff7_20_sp
float Ff7_20_sp = Ff7_20 / sp;
#endif
#ifdef CAL_Ff7_40_sp
float Ff7_40_sp = Ff7_40 / sp;
#endif
#ifdef CAL_Ff7_80_sp
float Ff7_80_sp = Ff7_80 / sp;
#endif
#ifdef CAL_L2a1_sp
float L2a1_sp = L2a1 / sp;
#endif
#ifdef CAL_L2f1_sp
#endif
#ifdef CAL_L2f6_sp
float L2f6_sp = L2f6 / sp;
#endif
#ifdef CAL_L2f8_sp
float L2f8_sp = L2f8 / sp;
#endif
#ifdef CAL_L2f10_20_sp
float L2f10_20_sp = L2f10_20 / sp;
#endif
#ifdef CAL_L2f10_40_sp
float L2f10_40_sp = L2f10_40 / sp;
#endif
#ifdef CAL_L2f10_60_sp
float L2f10_60_sp = L2f10_60 / sp;
#endif
#ifdef CAL_L2f10_80_sp
float L2f10_80_sp = L2f10_80 / sp;
#endif
#ifdef CAL_L2f10_sp
float L2f10_sp = L2f10 / sp;
#endif
#ifdef CAL_L2f11_sp
float L2f11_sp = L2f11 / sp;
#endif
#ifdef CAL_L2f12_sp
float L2f12_sp = L2f12 / sp;
#endif
#ifdef CAL_L2f13_sp
float L2f13_sp = L2f13 / sp;
#endif
#ifdef CAL_L2f14_sp
float L2f14_sp = L2f14 / sp;
#endif
#ifdef CAL_L2f15_sp
float L2f15_sp = L2f15 / sp;
#endif
#ifdef CAL_L2f2_sp
float L2f2_sp = L2f2 / sp;
#endif
#ifdef CAL_L2p1_sp
float L2p1_sp = L2p1 / sp;
#endif
#ifdef CAL_a1f1_sp
float a1f1_sp = a1f1 / sp;
#endif
#ifdef CAL_a1f10_sp
float a1f10_sp = a1f10 / sp;
#endif
#ifdef CAL_a1f11_sp
float a1f11_sp = a1f11 / sp;
#endif
#ifdef CAL_a1f4_sp
float a1f4_sp = a1f4 / sp;
#endif
#ifdef CAL_a1f7_sp
float a1f7_sp = a1f7 / sp;
#endif
#ifdef CAL_a1f8_sp
float a1f8_sp = a1f8 / sp;
#endif
#ifdef CAL_a2f4_sp
float a2f4_sp = a2f4 / sp;
#endif
#ifdef CAL_a2f5_sp
float a2f5_sp = a2f5 / sp;
#endif
#ifdef CAL_a2f6_sp
float a2f6_sp = a2f6 / sp;
#endif
#ifdef CAL_a2f7_sp
float a2f7_sp = a2f7 / sp;
#endif
#ifdef CAL_Ff8_1_sp
float Ff8_1_sp = Ff8_1 / sp;
#endif
#ifdef CAL_Ff8_2_sp
float Ff8_2_sp = Ff8_2 / sp;
#endif
#ifdef CAL_a2f12_sp
float a2f12_sp = a2f12 / sp;
#endif
#ifdef CAL_AsyBidAsk_sp
float AsyBidAsk_sp = AsyBidAsk / sp;
#endif
#ifdef CAL_MPB_sp
float MPB_sp = MPB / sp;
#endif
#ifdef CAL_FPC1_IC1
float FPC1_IC1{-990};
#endif
#ifdef CAL_FPC2_IC1
float FPC2_IC1{-990};
#endif
#ifdef CAL_FPC3_IC1
float FPC3_IC1{-990};
#endif
#ifdef CAL_FPC4_IC1
float FPC4_IC1{-990};
#endif
#ifdef CAL_FPD1_IC1
float FPD1_IC1{-990};
#endif
#ifdef CAL_FPD2_IC1
float FPD2_IC1{-990};
#endif
#ifdef CAL_FPD3_IC1
float FPD3_IC1{-990};
#endif
#ifdef CAL_FPD4_IC1
float FPD4_IC1{-990};
#endif
#ifdef CAL_FPC1_IH1
float FPC1_IH1{-990};
#endif
#ifdef CAL_FPC2_IH1
float FPC2_IH1{-990};
#endif
#ifdef CAL_FPC3_IH1
float FPC3_IH1{-990};
#endif
#ifdef CAL_FPC4_IH1
float FPC4_IH1{-990};
#endif
#ifdef CAL_FPD1_IH1
float FPD1_IH1{-990};
#endif
#ifdef CAL_FPD2_IH1
float FPD2_IH1{-990};
#endif
#ifdef CAL_FPD3_IH1
float FPD3_IH1{-990};
#endif
#ifdef CAL_FPD4_IH1
float FPD4_IH1{-990};
#endif
#ifdef CAL_FPC1_IF1
float FPC1_IF1{-990};
#endif
#ifdef CAL_FPC2_IF1
float FPC2_IF1{-990};
#endif
#ifdef CAL_FPC3_IF1
float FPC3_IF1{-990};
#endif
#ifdef CAL_FPC4_IF1
float FPC4_IF1{-990};
#endif
#ifdef CAL_FPD1_IF1
float FPD1_IF1{-990};
#endif
#ifdef CAL_FPD2_IF1
float FPD2_IF1{-990};
#endif
#ifdef CAL_FPD3_IF1
float FPD3_IF1{-990};
#endif
#ifdef CAL_FPD4_IF1
float FPD4_IF1{-999};
#endif
#ifdef CAL_Ff7_sp
#endif
