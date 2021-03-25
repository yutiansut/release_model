#pragma once

#include <string>
#include <vector>

namespace husky {

    typedef std::string EXCHANGE;
    const static EXCHANGE SHFE = "SHFE";
    const static EXCHANGE CZCE = "CZCE";
    const static EXCHANGE CFFEX = "CFFEX";
    const static EXCHANGE DCE = "DCE";
    const static EXCHANGE INE = "INE";

    const std::vector<std::string> shfe_ticker_list =  {"hc", "ni", "sn", "au", "rb", "wr", "bu", "pb", "cu", "zn", "ru", "al", "ag", "fu", "ss", "sp"};
    const std::vector<std::string> czce_ticker_list =  {"SR", "OI", "PM", "WH", "CJ", "CF", "SM", "AP", "CY", "UR", "ZC", "SA", "TA", "FG", "RI", "MA", "RM", "JR", "SF", "RS", "LR", "PF"};
    const std::vector<std::string> dce_ticker_list =   {"i", "pp", "rr", "c", "fb", "v", "j", "m", "jd", "b", "y", "l", "p", "eg", "eb", "cs", "a", "jm", "bb", "pg", "lh"};
    const std::vector<std::string> cffex_ticker_list = {"T", "TS", "TF", "IF", "IH", "IC", "IO"};
    const std::vector<std::string> ine_ticker_list =   {"nr", "sc", "lu", "bc"};
    const std::vector<std::string> old_ticker_list =   {"ER", "RO", "WS", "WT"};

    enum ETicker {
        hc1,hc2,hc3,
        ni1,ni2,ni3,
        sn1,sn2,sn3,
        au1,au2,au3,
        rb1,rb2,rb3,
        wr1,wr2,wr3,
        bu1,bu2,bu3,
        pb1,pb2,pb3,
        cu1,cu2,cu3,
        zn1,zn2,zn3,
        ru1,ru2,ru3,
        al1,al2,al3,
        ag1,ag2,ag3,
        fu1,fu2,fu3,
        ss1,ss2,ss3,
        sp1,sp2,sp3,
        SR1,SR2,SR3,
        OI1,OI2,OI3,
        PM1,PM2,PM3,
        WH1,WH2,WH3,
        CJ1,CJ2,CJ3,
        CF1,CF2,CF3,
        SM1,SM2,SM3,
        AP1,AP2,AP3,
        CY1,CY2,CY3,
        UR1,UR2,UR3,
        ZC1,ZC2,ZC3,
        SA1,SA2,SA3,
        TA1,TA2,TA3,
        FG1,FG2,FG3,
        RI1,RI2,RI3,
        MA1,MA2,MA3,
        RM1,RM2,RM3,
        JR1,JR2,JR3,
        SF1,SF2,SF3,
        RS1,RS2,RS3,
        LR1,LR2,LR3,
        i1, i2, i3,
        pp1,pp2,pp3,
        rr1,rr2,rr3,
        c1, c2, c3,
        fb1,fb2,fb3,
        v1, v2, v3,
        j1, j2, j3,
        m1, m2, m3,
        jd1,jd2,jd3,
        b1, b2, b3,
        y1, y2, y3,
        l1, l2, l3,
        p1, p2, p3,
        eg1,eg2,eg3,
        eb1,eb2,eb3,
        cs1,cs2,cs3,
        a1, a2, a3,
        jm1,jm2,jm3,
        bb1,bb2,bb3,
        pg1,pg2,pg3,
        T1, T2, T3,
        TS1,TS2,TS3,
        TF1,TF2,TF3,
        IF1,IF2,IF3,
        IH1,IH2,IH3,
        IC1,IC2,IC3,
        IO1,IO2,IO3,
        nr1,nr2,nr3,
        sc1,sc2,sc3,
        lu1,lu2,lu3,
        PF1,PF2,PF3,
        bc1,bc2,bc3
    };

}