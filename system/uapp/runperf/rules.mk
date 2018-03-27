# Copyright 2018 The Fuchsia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_NAME := runperf

MODULE_TYPE := userapp

MODULE_GROUP := test

MODULE_SRCS += \
    $(LOCAL_DIR)/runperf.c \

MODULE_LIBS := \
    system/ulib/fdio \
    system/ulib/launchpad \
    system/ulib/zircon \
    system/ulib/c \
    system/ulib/unittest \

include make/module.mk
