/*
 * ws protocol handler plugin for "lws-minimal"
 *
 * Written in 2010-2019 by Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * This version uses an lws_ring ringbuffer to cache up to 8 messages at a time,
 * so it's not so easy to lose messages.
 */

#include <string.h>
#include <stdlib.h>
#include <libwebsockets.h>

#include "client-server-protocol.h"

#include "../binance/binance.h"
#include "../bitfinex/bitfinex.h"
#include "../kraken/kraken.h"
#include "../okex/okex.h"
#include "../bitstamp/bitstamp.h"

void
connect_all(const struct per_vhost_data__minimal *vhd);

void
execute_connect(
        const struct per_vhost_data__minimal *vhd,
        int (*fun)(const struct per_vhost_data__minimal *)
);

/* destroys the message when everyone has had a copy of it */

static void
__minimal_destroy_message(void *_msg) {
    struct msg *msg = _msg;

    free(msg->payload);
    msg->payload = NULL;
    msg->len = 0;
}

/*
 * websocket_write_back: 将字符串数据写入目标。
 */
int
websocket_write_back(struct lws *wsi_in, const char *str) {
    if (NULL == str || NULL == wsi_in) {
        return -1;
    }

    int n;
    size_t len = strlen(str);
    unsigned char *out = (unsigned char *) malloc(sizeof(char) * (LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));

    //* setup the buffer */
    memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len);

    //* write out */
    n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);

    lwsl_user("%s: 派出: %s\n", __func__, str);
    //* free the buffer */
    free(out);

    return n;
}

int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                 void *user, void *in, size_t len) {
    struct per_session_data__minimal *pss =
            (struct per_session_data__minimal *) user;
    const struct lws_protocols *prot = lws_get_protocol(wsi);
    struct per_vhost_data__minimal *vhd =
            (struct per_vhost_data__minimal *)
                    lws_protocol_vh_priv_get(lws_get_vhost(wsi),
                                             prot);

    struct client_user_data *wsi_user = (struct client_user_data *) lws_wsi_user(wsi);
    const struct msg *pmsg;
    struct msg amsg;
    int m;

    switch (reason) {

        /* --- protocol lifecycle callbacks --- */

        case LWS_CALLBACK_PROTOCOL_INIT:
            vhd = lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
                                              lws_get_protocol(wsi),
                                              sizeof(struct per_vhost_data__minimal));
            vhd->context = lws_get_context(wsi);
            vhd->protocol = lws_get_protocol(wsi);
            vhd->vhost = lws_get_vhost(wsi);

            vhd->ring = lws_ring_create(sizeof(struct msg), 8, __minimal_destroy_message);
            if (!vhd->ring) {
                return 1;
            }

            connect_all(vhd);
            break;

        case LWS_CALLBACK_PROTOCOL_DESTROY:
            lws_ring_destroy(vhd->ring);
            break;

            /* --- serving callbacks --- */

        case LWS_CALLBACK_ESTABLISHED:
            /* add ourselves to the list of live pss held in the vhd */
        lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
            pss->tail = lws_ring_get_oldest_tail(vhd->ring);
            pss->wsi = wsi;


            // send hello.
            callback_minimal(wsi, LWS_CALLBACK_RECEIVE, user, "hello world", 11);
            break;

        case LWS_CALLBACK_CLOSED:
            /* remove our closing pss from the list of live pss */
        lws_ll_fwd_remove(struct per_session_data__minimal, pss_list, pss, vhd->pss_list);
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            pmsg = lws_ring_get_element(vhd->ring, &pss->tail);
            if (!pmsg) {
                break;
            }

            /* notice we allowed for LWS_PRE in the payload already */
            m = lws_write(wsi, ((unsigned char *) pmsg->payload) + LWS_PRE, pmsg->len, LWS_WRITE_TEXT);
            if (m < (int) pmsg->len) {
                lwsl_err("ERROR %d writing to ws socket\n", m);
                return -1;
            }

            lws_ring_consume_and_update_oldest_tail(
                    vhd->ring,    /* lws_ring object */
                    struct per_session_data__minimal, /* type of objects with tails */
                    &pss->tail,    /* tail of guy doing the consuming */
                    1,        /* number of payload objects being consumed */
                    vhd->pss_list,    /* head of list of objects with tails */
                    tail,        /* member name of tail in objects with tails */
                    pss_list    /* member name of next object in objects with tails */
            );

            /* more to do? */
            if (lws_ring_get_element(vhd->ring, &pss->tail))
                /* come back as soon as we can write more */
                lws_callback_on_writable(pss->wsi);
            break;

            /* --- client callbacks --- */

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            lwsl_err("CLIENT_CONNECTION_ERROR: %s %s\n",
                     in ? (char *) in : "(null)",
                     NULL != wsi_user ? wsi_user->name : "(?)");
            vhd->client_wsi = NULL;
            lws_timed_callback_vh_protocol(vhd->vhost, vhd->protocol,
                                           LWS_CALLBACK_USER, 1);
            break;

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lwsl_user("%s: LWS_CALLBACK_CLIENT_ESTABLISHED %s\n", __func__, wsi_user->name);

            if (NULL != wsi_user && NULL != wsi_user->subscribe) {
                wsi_user->subscribe(wsi);
            }
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            if (len == 0) {
                lwsl_user("零数据!\n");
                break;
            }
            if (NULL != wsi_user && NULL != wsi_user->parse_json) {
                wsi_user->parse_json(in);
                if (strcmp("okex", wsi_user->name) == 0) {
                    // 不要发送二进制文件。
                    break;
                }
            }

            /* if no clients, just drop incoming */
            if (!vhd->pss_list) {
                break;
            }

            if (!lws_ring_get_count_free_elements(vhd->ring)) {
                lwsl_user("丢弃消息!\n");
                break;
            }

            amsg.len = len;
            /* notice we over-allocate by LWS_PRE */
            amsg.payload = malloc(LWS_PRE + len);
            if (!amsg.payload) {
                lwsl_user("OOM: 丢弃消息\n");
                break;
            }

            memcpy((char *) amsg.payload + LWS_PRE, in, len);
            if (!lws_ring_insert(vhd->ring, &amsg, 1)) {
                __minimal_destroy_message(&amsg);
                lwsl_user("丢弃消息!\n");
                break;
            }

            /*
             * let everybody know we want to write something on them
             * as soon as they are ready
             */
            lws_start_foreach_llp(struct per_session_data__minimal **,
                                  ppss, vhd->pss_list)
                    {
                        lws_callback_on_writable((*ppss)->wsi);
                    }
            lws_end_foreach_llp(ppss, pss_list);
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            vhd->client_wsi = NULL;
            lws_timed_callback_vh_protocol(vhd->vhost, vhd->protocol,
                                           LWS_CALLBACK_USER, 1);
            break;

            /* rate-limited client connect retries */


        case LWS_CALLBACK_RECEIVE:
            if (!lws_ring_get_count_free_elements(vhd->ring)) {
                lwsl_user("丢弃消息!\n");
                break;
            }

            amsg.len = len;
            /* notice we over-allocate by LWS_PRE... */
            amsg.payload = malloc(LWS_PRE + len);
            if (!amsg.payload) {
                lwsl_user("OOM: 丢弃消息\n");
                break;
            }

            /* ...and we copy the payload in at +LWS_PRE */
            memcpy((char *) amsg.payload + LWS_PRE, in, len);
            if (!lws_ring_insert(vhd->ring, &amsg, 1)) {
                __minimal_destroy_message(&amsg);
                lwsl_user("丢弃消息!\n");
                break;
            }

            /*
             * let everybody know we want to write something on them
             * as soon as they are ready
             */
            lws_start_foreach_llp(struct per_session_data__minimal **, ppss, vhd->pss_list)
                    {
                        lws_callback_on_writable((*ppss)->wsi);
                    }
            lws_end_foreach_llp(ppss, pss_list);
            break;

        case LWS_CALLBACK_USER:
            lwsl_notice("%s: LWS_CALLBACK_USER\n", __func__);
            connect_all(vhd);
            break;

        default:
            break;
    }
    return 0;
}

void
connect_all(const struct per_vhost_data__minimal *vhd) {
    execute_connect(vhd, binance_connect_client);
    execute_connect(vhd, bitfinex_connect_client);
    execute_connect(vhd, kraken_connect_client);
    execute_connect(vhd, bitstamp_connect_client);
}

void
execute_connect(
        const struct per_vhost_data__minimal *vhd,
        int (*fun)(const struct per_vhost_data__minimal *)
) {
    if (fun(vhd)) {
        lws_timed_callback_vh_protocol(vhd->vhost,
                                       vhd->protocol,
                                       LWS_CALLBACK_USER, 1);
    }
}

void ring_buffer_insert(struct lws *wsi, char *message) {
    struct lws_vhost *vhost = lws_get_vhost(wsi);
    // Find protocol for client.
    const struct lws_protocols *prot = lws_vhost_name_to_protocol(vhost, "lws-minimal-proxy");
    struct per_vhost_data__minimal *vhd =
            (struct per_vhost_data__minimal *) lws_protocol_vh_priv_get(vhost, prot);

    struct msg amsg;
    int n;
    size_t length = strlen(message);

    /* only create if space in ringbuffer */
    n = (int) lws_ring_get_count_free_elements(vhd->ring);
    if (!n) {
        lwsl_user("丢弃消息!\n");
        return;
    }

    amsg.payload = malloc(LWS_PRE + length);
    if (!amsg.payload) {
        lwsl_user("OOM: 丢弃消息\n");
        return;
    }
    memcpy((char *) amsg.payload + LWS_PRE, message, length);
    amsg.len = length;
    n = lws_ring_insert(vhd->ring, &amsg, 1);
    if (n != 1) {
        __minimal_destroy_message(&amsg);
        lwsl_user("丢弃消息!\n");
    }

    /*
     * let everybody know we want to write something on them
     * as soon as they are ready
     */
    lws_start_foreach_llp(struct per_session_data__minimal **, ppss, vhd->pss_list)
            {
                lws_callback_on_writable((*ppss)->wsi);
            }
    lws_end_foreach_llp(ppss, pss_list);
}
