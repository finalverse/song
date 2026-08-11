# SPDX-License-Identifier: GPL-3.0-only
# MuseScore-Studio-CLA-applies

# Preserve the framework's platform-specific install layout, then expose the
# binary destination relative to CMAKE_INSTALL_PREFIX. install() destinations
# must remain relative so CPack can stage them inside generated installers.
include("${MUSE_FRAMEWORK_PATH}/buildscripts/cmake/GetPaths.cmake")

set(INSTALL_BIN_DIR "${INSTALL_SUBDIR}")
