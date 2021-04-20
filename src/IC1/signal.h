std::vector<float> features{Ff7_10, Ff7_20, Ff7_40, Ff7_80, Ff8_2, L2a1,  L2a2,  L2f10, L2f10_60, L2f10_80, L2f6, L2f8,
                            L2f9,   L2p1,   L2p2,   VOI,    a1f10, a1f11, a1f12, a1f13, a1f14,    a1f15,    a1f4, a1f6,
                            a1f7,   a1f9,   a2f12,  a2f13,  a2f5,  a2f6,  a2f7,  fpf4,  fpf7,     sa2,      sa3};
double m_signal = ApplyCatboostModel(features);
