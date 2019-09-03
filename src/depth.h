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
    char exchange[20];
    // 交易对
    char market_name[20];
    double market;
    // 时间戳
    double time;
    // 潜伏
    long latency;
    long id;
    // 买单
    Order bids[100];
    // 卖单
    Order asks[100];
} OrderBookLevel2;

#endif //CJSON_EXAMPLE_DEPTH_H
