#ifndef CJSON_EXAMPLE_DEPTH_H
#define CJSON_EXAMPLE_DEPTH_H

// 单
typedef struct order {
    double price;
    double amount;
} Order;

#ifndef EMPTY_ORDER
#define EMPTY_ORDER (Order){.price=-1.0, .amount=-1.0}
#endif //EMPTY_ORDER

typedef struct {
    // 兑换所
    char *exchange;
    // 交易对
    char *market_name;
    double market;
    // 时间戳
    double time;
    // 潜伏
    long latency;
    long id;

    unsigned int bids_length;
    // 出价：买方深度
    Order bids[100];
    unsigned int asks_length;
    // 问：卖家深度
    Order asks[100];
} OrderBookLevel2;

void
orderbook_delete(OrderBookLevel2 *order_book);

#endif //CJSON_EXAMPLE_DEPTH_H
