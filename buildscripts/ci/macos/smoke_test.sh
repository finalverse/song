#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

set -euo pipefail

APP_PATH="${1:-applebuild/mscore.app}"
APP_BINARY="${APP_PATH}/Contents/MacOS/mscore"
EXPORT_INPUT="${2:-src/converter/tests/data/score_elements.mscx}"
EXPORT_INPUT_NAME="$(basename "${EXPORT_INPUT}")"
LOG_ROOT="${HOME}/Library/Application Support/Finalverse Inc."
RUN_LOG="$(mktemp -t song-launch-smoke)"
START_MARKER="$(mktemp -t song-launch-start)"
EXPORT_DIR="$(mktemp -d -t song-export-smoke)"
EXPORT_OUTPUT="${EXPORT_DIR}/score.pdf"
PROCESS_ID=""

print_diagnostics() {
    echo "Song launch output:"
    tail -n 120 "${RUN_LOG}" 2>/dev/null || true

    echo "Song application log:"
    find "${LOG_ROOT}" -type f -path '*/logs/*.log' -newer "${START_MARKER}" \
        -exec tail -n 120 {} \; 2>/dev/null || true
}

stop_app() {
    if [[ -n "${PROCESS_ID}" ]] && kill -0 "${PROCESS_ID}" 2>/dev/null; then
        kill "${PROCESS_ID}" 2>/dev/null || true
        wait "${PROCESS_ID}" 2>/dev/null || true
    fi
}

trap stop_app EXIT

if [[ ! -x "${APP_BINARY}" ]]; then
    echo "Packaged Song executable not found: ${APP_BINARY}"
    exit 1
fi

if [[ ! -f "${EXPORT_INPUT}" ]]; then
    echo "Song export smoke-test score not found: ${EXPORT_INPUT}"
    exit 1
fi

echo "Exporting a score with packaged Finalverse Song Studio"
if ! "${APP_BINARY}" --factory-settings -o "${EXPORT_OUTPUT}" "${EXPORT_INPUT}" >"${RUN_LOG}" 2>&1; then
    echo "Packaged Song failed its headless score export"
    print_diagnostics
    exit 1
fi

if [[ ! -s "${EXPORT_OUTPUT}" ]] || [[ "$(head -c 4 "${EXPORT_OUTPUT}")" != "%PDF" ]]; then
    echo "Packaged Song did not produce a valid PDF"
    print_diagnostics
    exit 1
fi

echo "Launching packaged Finalverse Song Studio with a score"
"${APP_BINARY}" --factory-settings "${EXPORT_INPUT}" >"${RUN_LOG}" 2>&1 &
PROCESS_ID=$!

for _ in $(seq 1 45); do
    if ! kill -0 "${PROCESS_ID}" 2>/dev/null; then
        wait "${PROCESS_ID}" || EXIT_CODE=$?
        echo "Song exited before its main window loaded (exit ${EXIT_CODE:-0})"
        print_diagnostics
        exit 1
    fi

    # Production-style builds omit the DEBUG-level NotationProject and UI
    # context markers. The preceding export proves the score is loadable; the
    # INFO-level controller marker proves the GUI received that same score.
    if find "${LOG_ROOT}" -type f -path '*/logs/*.log' -newer "${START_MARKER}" \
        -exec grep -q "Song main window loaded successfully" {} \; \
        -exec grep -q "ProjectActionsController::openProject.*Try open project:.*${EXPORT_INPUT_NAME}" {} \; \
        -print -quit 2>/dev/null | grep -q .; then
        sleep 3
        if ! kill -0 "${PROCESS_ID}" 2>/dev/null; then
            wait "${PROCESS_ID}" || EXIT_CODE=$?
            echo "Song exited just after its main window loaded (exit ${EXIT_CODE:-0})"
            print_diagnostics
            exit 1
        fi

        echo "Packaged Song opened a score in its main window successfully"
        exit 0
    fi

    sleep 1
done

echo "Song did not load its main window within 45 seconds"
print_diagnostics
exit 1
