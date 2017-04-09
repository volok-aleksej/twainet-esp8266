#!/bin/bash
svn checkout https://github.com/protobuf-c/protobuf-c/tags/v1.2.1/protobuf-c Protobuf
svn patch protobuf.diff Protobuf