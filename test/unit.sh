#!/bin/bash

REDIS_VOLATILE_PORT=6379
REDIS_PERSISTENT_PORT=6380

REDIS_VOLATILE_NUMBER=0
REDIS_PERSISTENT_NUMBER=1

redis-cli -p $REDIS_VOLATILE_PORT -n $REDIS_VOLATILE_NUMBER flushdb

redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT set unit_get_ok ok
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT set unit_del_ok ok
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT hset unit_hget_ok test ok
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT hset unit_h_ok test1 val1
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT hset unit_h_ok test2 val2
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT hset unit_h_ok test3 val3
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT hset unit_hdel_ok test ok
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT set unit_keys_a a
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT set unit_keys_b b
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT set unit_keys_c c
redis-cli -n $REDIS_VOLATILE_NUMBER -p $REDIS_VOLATILE_PORT mset unit_mget_a val1 unit_mget_b val2 unit_mget_c val3

redis-cli -n $REDIS_PERSISTENT_NUMBER -p $REDIS_PERSISTENT_PORT -n 1 flushdb
