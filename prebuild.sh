#!/bin/bash
# protobuf
rm -rf protobuf
svn checkout https://github.com/protobuf-c/protobuf-c/tags/v1.2.1/protobuf-c protobuf
svn patch protobuf.diff protobuf
# generate and patch files
rm -f twainet/*.pb-c.*
cd twainet
protoc-c --c_out=. ipc.proto client_server.proto remote_log.proto
cd ..
git apply proto_gen.diff
# ssl
rm -rf ssl
svn checkout https://github.com/willemwouters/ESP8266/trunk/sdk/esp_iot_sdk_v0.9.1/examples/IoT_Demo/include/ssl --depth empty
cd ssl
svn up ssl_bigint.h ssl_bigint_impl.h ssl_config.h ssl_crypto_misc.h ssl_crypto.h ssl_os_int.h
cd ..
svn patch ssl.diff ssl
