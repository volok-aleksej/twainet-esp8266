#!/bin/bash
# protobuf
rm -r protobuf
svn checkout https://github.com/protobuf-c/protobuf-c/tags/v1.2.1/protobuf-c protobuf
svn patch protobuf.diff protobuf
# ssl
rm -r ssl
svn checkout https://github.com/willemwouters/ESP8266/trunk/sdk/esp_iot_sdk_v0.9.1/examples/IoT_Demo/include/ssl --depth empty
cd ssl
svn up ssl_bigint.h ssl_bigint_impl.h ssl_config.h ssl_crypto_misc.h ssl_crypto.h ssl_os_int.h
cd ..
svn patch ssl.diff ssl