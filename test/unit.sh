#!/bin/bash

redis-cli flushdb

redis-cli set unit_get_ok ok
redis-cli set unit_del_ok ok
redis-cli hset unit_hget_ok test ok
redis-cli hset unit_h_ok test1 val1
redis-cli hset unit_h_ok test2 val2
redis-cli hset unit_h_ok test3 val3
redis-cli hset unit_hdel_ok test ok
redis-cli set unit_keys_a a
redis-cli set unit_keys_b b
redis-cli set unit_keys_c c
redis-cli mset unit_mget_a val1 unit_mget_b val2 unit_mget_c val3
redis-cli -p 6380 -n 1 flushdb
