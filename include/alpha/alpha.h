#pragma once

#include <core.h>
#include <common/reload.h>

namespace husky {
    class alpha : public reload {
    public:
        alpha();

        /** alpha implementations should implement these methods */
        // customized alpha name, e.g. ni_alpha_2.0
        virtual std::string name() { return ""; }

        // do calculation
        virtual void calc(uint di = UINT32_MAX) {};

        // init before market starts
        virtual void do_initialize() {};

        // finalize after market ends
        virtual void do_finalize() {};

        // live custom reload
        virtual void do_custom_reload() {};
        /** alpha implementations should implement these methods */

        /** alpha implementations are expected to call these methods */
        // send new order with objective qty
        void insert(double price, oms::Side side, int qty);

        // cancel all orders
        static void cancel_all();
        /** alpha implementations are expected to call these methods */


        /** below are system usage */
        void init();

        // system reload
        void do_reload() override;

        // close position
        bool close_all(double bid1, double ask1, int aggressive_factor);

        bool close_all_to_zero(double bid1, double ask1, int aggressive_factor);

        // send new order with direct qty
        // void insert_direct(double price, oms::Side side, int qty);

        // send new order by contract index
        // virtual void insert_ii(double price, oms::Side side, int qty, unsigned int ii) {};

        // cancel order by contract index
        // virtual void cancel_by_ii(unsigned int ii) {};

        // load data
        void dr_load(std::string p_cname, husky::basic_matrix<md::l2> *, EXCHANGE exchange);

        char *get_trade_contract();

        char *get_alpha_id();

        int get_net_position();

    private:
        void read_reloadable();

    public:
        int normal_lot;
        int heavy_lot;
    };
}