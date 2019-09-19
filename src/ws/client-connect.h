#ifndef CCXS_CLIENT_CONNECT_H
#define CCXS_CLIENT_CONNECT_H

#include "data.h"

int
connect_client(const struct per_vhost_data__minimal *vhd, int port, const char *address, const char *path, void *userdata);

#endif //CCXS_CLIENT_CONNECT_H
