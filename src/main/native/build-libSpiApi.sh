#!/bin/bash -e
###
# #%L
# **********************************************************************
# ORGANIZATION  :  Pi4J
# PROJECT       :  Pi4J :: JNI Native Binding Library for LinuxFS
# FILENAME      :  build-libSPIapi.sh
#
# This file is part of the Pi4J project. More information about
# this project can be found here:  https://pi4j.com/
# **********************************************************************
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# #L%
###

# ----------------------------------------------
# build latest Pi4J SPIapi JNI Wrapper Library
# ----------------------------------------------

echo
echo "============================================================================="
echo " STARTED BUILDING Pi4J-LinuxFS JNI NATIVE LIBRARY: ('${ARCH}/libpi4j-SPIapi')"
echo "============================================================================="
echo " - FOR ARCHITECTURE   : ${ARCH}"
echo " - USING COMPILER     : ${CC}"
echo " - USING CROSS PREFIX : ${CROSS_PREFIX}"
echo "-----------------------------------------------------------------------------"
echo

# ------------------------------------------------------
# BUILD LIB SPIapi DEPENDENCY LIBRARY
# ------------------------------------------------------

# ------------------------------------------------------
# BUILD LIBPI4J-SPIapi
# ------------------------------------------------------
echo
echo "====================================="
echo "BUILDING: ${ARCH}/libpi4j-SPIapi"
echo "====================================="

# perform compile
mkdir -p lib/${ARCH}/pi4j-SPIapi
make clean all \
  --always-make \
  CROSS_PREFIX=${CROSS_PREFIX} \
  CC=${CC} \
  ARCH=${ARCH} \
  TARGET=lib/${ARCH}/pi4j-SPIapi/libpi4j-SPIapi.so $@

echo
echo "-----------------------------------------------------------------------------"
echo " FINISHED BUILDING Pi4J-SPIapi JNI NATIVE LIBRARY: ('${ARCH}/libpi4j-SPIapi')"
echo "-----------------------------------------------------------------------------"
echo