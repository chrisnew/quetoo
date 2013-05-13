#!/bin/bash

set -e
set -x

CHROOT=`echo $JOB_NAME|cut -d\-  -f3-10`

if ([ "${CHROOT}" == "mingw64" ] || [ "${CHROOT}" == "mingw32" ])
then
	MINGW_TARGET=${CHROOT}
	CHROOT="fedora-18-x86_64"
	DEPS="${MINGW_ARCH}-SDL ${MINGW_ARCH}-SDL_image ${MINGW_ARCH}-SDL_mixer ${MINGW_ARCH}-curl ${MINGW_ARCH}-physfs ${MINGW_ARCH}-glib2 ${MINGW_ARCH}-libjpeg-turbo libtool ${MINGW_ARCH}-zlib ${MINGW_ARCH}-pkg-config ${MINGW_ARCH}-pdcurses"
else
	DEPS="SDL-devel SDL_image-devel SDL_mixer-devel curl-devel physfs-devel glib2-devel libjpeg-turbo-devel libtool zlib-devel ncurses-devel check check-devel"
fi


function init_chroot() {
	/usr/bin/mock -r ${CHROOT} --clean
	/usr/bin/mock -r ${CHROOT} --init
	/usr/bin/mock -r ${CHROOT} --install ${DEPS}
	/usr/bin/mock -r ${CHROOT} --copyin ${WORKSPACE} "/tmp/quake2world"

}


function destroy_chroot() {
	/usr/bin/mock -r ${CHROOT} --clean
}

function archive_workspace() {
	rm -Rf ${WORKSPACE}/jenkins-quake2world*
	/usr/bin/mock -r ${CHROOT} --copyout "/tmp/quake2world-${CHROOT}" "${WORKSPACE}/${BUILD_TAG}"
	cd ${WORKSPACE}
	tar czf ${BUILD_TAG}.tgz ${BUILD_TAG}
}