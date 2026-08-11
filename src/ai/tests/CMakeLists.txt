# SPDX-License-Identifier: GPL-3.0-only
#
# Finalverse Song Studio
# Copyright (C) 2026 Finalverse Inc.

set(MODULE_TEST ai_tests)

set(MODULE_TEST_SRC
    ${CMAKE_CURRENT_LIST_DIR}/ailogredactortests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/airequesthandletests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/deterministicprovidertests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modelroutertests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/scorecontextbudgetertests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/scorecontextserializertests.cpp
)

set(MODULE_TEST_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/..)
set(MODULE_TEST_LINK ai)
set(MODULE_TEST_DATA_ROOT ${CMAKE_CURRENT_LIST_DIR})

include(SetupGTest)
