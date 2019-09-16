#include <stdlib.h>
#include "depth.h"

void
orderbook_delete(OrderBookLevel2 *order_book) {
    free(order_book->exchange);
    free(order_book->market_name);
    free(order_book);
}
