# SPDX-License-Identifier: GPL-3.0-only
#
# Finalverse Song Studio
# Copyright (C) 2026 Finalverse Inc.

muse_create_module(ai)

target_sources(ai PRIVATE
    aimodule.cpp
    aimodule.h
    aitypes.cpp
    aitypes.h
    iaiprovider.h
    iaisecretstore.h
    iaiservice.h
    imodelrouter.h

    internal/aiservice.cpp
    internal/aiservice.h
    internal/modelrouter.cpp
    internal/modelrouter.h
    internal/providers/deterministicprovider.cpp
    internal/providers/deterministicprovider.h
    internal/security/ailogredactor.cpp
    internal/security/ailogredactor.h
)

if (MUE_BUILD_AI_TESTS)
    add_subdirectory(tests)
endif()
