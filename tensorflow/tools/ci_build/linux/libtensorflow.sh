#!/usr/bin/env bash
# Copyright 2016 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
#
# Script to produce binary releases for libtensorflow (C API, Java jars etc.).
# Intended to be run inside a docker container. See libtensorflow_docker.sh

set -ex
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# See comments at the top of this file for details.
source "${SCRIPT_DIR}/../builds/libtensorflow.sh"

SUFFIX="-cpu-linux-"
if [ "${TF_NEED_CUDA}" == "1" ]; then
  SUFFIX="-gpu-linux-"
fi
if [ "${TF_NEED_ROCM}" == "1" ]; then
  SUFFIX="-rocm-linux-"
fi

build_libtensorflow_tarball "${SUFFIX}$(uname -m)"
chmod go+w lib_package/*
bazel clean --expunge
