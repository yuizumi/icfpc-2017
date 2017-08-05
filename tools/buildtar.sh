#!/bin/bash

declare -r ID=d3aeebce-2ac4-49f9-9568-0b177a60c502

set -ex

cd "$(dirname $0)/.."
WORK_DIR="$(mktemp -d $HOME/build.XXXXXX)"
cp README.md "${WORK_DIR}/README"
mkdir "${WORK_DIR}/src"
cp -R [A-Za-z]* "${WORK_DIR}/src"
(cd ai && cmake . && make)
cp -p ai/newbie "${WORK_DIR}/punter"
cp -p tools/install.sh "${WORK_DIR}/install"
touch "${WORK_DIR}/PACKAGES"
tar cfz "${HOME}/icfp-${ID}.tar.gz" -C "${WORK_DIR}" .
rm -fr "${WORK_DIR}"
