#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "${script_dir}/.." && pwd)"
source_svg="${script_dir}/logo/song-app-icon.svg"
output_dir="${repo_dir}/share/icons/AppIcon"
iconset_dir="$(mktemp -d "${TMPDIR:-/tmp}/song-iconset.XXXXXX")/SongStudio.iconset"

cleanup() {
    rm -rf "$(dirname "${iconset_dir}")"
}
trap cleanup EXIT

command -v rsvg-convert >/dev/null
command -v magick >/dev/null
command -v iconutil >/dev/null

mkdir -p "${iconset_dir}"

for size in 16 24 32 48 64 96 128 256 512 1024 2048; do
    rsvg-convert -w "${size}" -h "${size}" "${source_svg}" \
        -o "${output_dir}/MS4_AppIcon_${size}x${size}.png"
done

cp "${output_dir}/MS4_AppIcon_16x16.png" "${iconset_dir}/icon_16x16.png"
cp "${output_dir}/MS4_AppIcon_32x32.png" "${iconset_dir}/icon_16x16@2x.png"
cp "${output_dir}/MS4_AppIcon_32x32.png" "${iconset_dir}/icon_32x32.png"
cp "${output_dir}/MS4_AppIcon_64x64.png" "${iconset_dir}/icon_32x32@2x.png"
cp "${output_dir}/MS4_AppIcon_128x128.png" "${iconset_dir}/icon_128x128.png"
cp "${output_dir}/MS4_AppIcon_256x256.png" "${iconset_dir}/icon_128x128@2x.png"
cp "${output_dir}/MS4_AppIcon_256x256.png" "${iconset_dir}/icon_256x256.png"
cp "${output_dir}/MS4_AppIcon_512x512.png" "${iconset_dir}/icon_256x256@2x.png"
cp "${output_dir}/MS4_AppIcon_512x512.png" "${iconset_dir}/icon_512x512.png"
cp "${output_dir}/MS4_AppIcon_1024x1024.png" "${iconset_dir}/icon_512x512@2x.png"

iconutil -c icns "${iconset_dir}" -o "${output_dir}/MS4_AppIcon.icns"

magick \
    "${output_dir}/MS4_AppIcon_16x16.png" \
    "${output_dir}/MS4_AppIcon_24x24.png" \
    "${output_dir}/MS4_AppIcon_32x32.png" \
    "${output_dir}/MS4_AppIcon_48x48.png" \
    "${output_dir}/MS4_AppIcon_64x64.png" \
    "${output_dir}/MS4_AppIcon_96x96.png" \
    "${output_dir}/MS4_AppIcon_128x128.png" \
    "${output_dir}/MS4_AppIcon_256x256.png" \
    "${output_dir}/MS4_AppIcon.ico"

echo "Generated Finalverse Song Studio app icons from ${source_svg}"
