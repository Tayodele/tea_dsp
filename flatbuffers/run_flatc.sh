#!/bin/bash

set -euox pipefail

FLATC=$1
SCHEMA=$2
DIR=$3
$FLATC --rust -o $DIR $SCHEMA
