#!/bin/bash

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="/tmp/image_processor_unit_build_make"

cmake -S "$REPO_ROOT" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --target test_image_processor_unit
"$BUILD_DIR/test_image_processor_unit"
