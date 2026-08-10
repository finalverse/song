#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

set -euo pipefail

repo_dir="$(git rev-parse --show-toplevel)"
cd "${repo_dir}"

# Keep output filename-only. Never print a matching line or credential value.
secret_patterns='gh[pousr]_[A-Za-z0-9]{30,}|github_pat_[A-Za-z0-9_]{30,}|AKIA[0-9A-Z]{16}|-----BEGIN (RSA |EC |OPENSSH )?PRIVATE KEY-----'

matched_files="$(git grep --cached -l -I -E "${secret_patterns}" -- . \
    ':(exclude)tools/security/check_staged_secrets.sh' || true)"

if [[ -n "${matched_files}" ]]; then
    echo >&2 "Commit blocked: staged files contain a possible credential or private key."
    echo >&2 "Review these files without printing their matching lines:"
    printf '%s\n' "${matched_files}" >&2
    exit 1
fi

origin_url="$(git config --get remote.origin.url || true)"
case "${origin_url}" in
    https://*@github.com/*)
        echo >&2 "Commit blocked: the origin URL contains embedded authentication information."
        echo >&2 "Use a credential-free GitHub URL and a credential manager instead."
        exit 1
        ;;
esac

echo "Credential safety check passed."
