# Copyright 2017 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_TYPE := driver

MODULE_SRCS := \
    $(LOCAL_DIR)/cdc-eth-function.c \

MODULE_STATIC_LIBS := system/ulib/ddk system/ulib/sync system/dev/lib/usb

MODULE_LIBS := system/ulib/driver system/ulib/zircon system/ulib/c

MODULE_HEADER_DEPS := system/ulib/inet6

include make/module.mk
