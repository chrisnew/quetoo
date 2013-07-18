#!/bin/bash -ex

#
# Build entry point for Apple OS X.
#
function build() {
	source ~/.profile
	autoreconf -i
	./configure ${CONFIGURE_FLAGS}
	make -C apple ${MAKE_OPTIONS} ${MAKE_TARGETS}
}

source ./common.sh