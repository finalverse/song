/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Finalverse Song Studio
 * Copyright (C) 2026 Finalverse Inc.
 */
#pragma once

#include <string>

#include "modularity/imoduleinterface.h"
#include "global/types/retval.h"

namespace finalverse::song::ai {

class IAiSecretStore : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IAiSecretStore)

public:
    virtual ~IAiSecretStore() = default;

    virtual muse::Ret storeSecret(const std::string& providerId, const std::string& credentialName,
                                  const std::string& secret) = 0;
    virtual muse::RetVal<std::string> loadSecret(const std::string& providerId,
                                                 const std::string& credentialName) const = 0;
    virtual muse::Ret removeSecret(const std::string& providerId, const std::string& credentialName) = 0;
};

}
