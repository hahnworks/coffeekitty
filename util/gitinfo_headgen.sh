#!/bin/bash

INCLUDEDIR="include"
if [ ! -d "$INCLUDEDIR" ]; then
    mkdir -p "$INCLUDEDIR"
fi
FILEPATH="$INCLUDEDIR/gitinfo.h"

HASH=$(git rev-parse --short HEAD)
BRANCH=$(git rev-parse --abbrev-ref HEAD)
TAG=$(git tag --points-at HEAD)

if [ -n "$TAG" ]; then
    VERSION="$TAG"
else
    VERSION="${BRANCH}@${HASH}"
fi

cat <<EOF
/* This file is automatically generated by gitinfo_headgen.sh.
 * Do not edit this file directly.
 */

#ifndef GITINFO_H
#define GITINFO_H

#define GIT_VERSION "$VERSION"

#define GIT_HASH "$HASH"
#define GIT_BRANCH "$BRANCH"
#define GIT_TAG "$TAG"

#endif

EOF
