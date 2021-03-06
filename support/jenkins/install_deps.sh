dnf -y install openssh-clients rsync check check-devel clang-analyzer libtool

dnf -y install SDL2-devel SDL2_image-devel \
SDL2_mixer-devel libcurl-devel physfs-devel glib2-devel \
libjpeg-turbo-devel zlib-devel ncurses-devel libxml2-devel libxml2

dnf -y install SDL2-devel.i686 SDL2_image-devel.i686 \
SDL2_mixer-devel.i686 libcurl-devel.i686 physfs-devel.i686 glib2-devel.i686 \
libjpeg-turbo-devel.i686 zlib-devel.i686 ncurses-devel.i686 libxml2-devel.i686 \
libxml2.i686 glibc-devel.i686 libX11-devel.i686 libXext-devel.i686 mesa-libGL-devel.i686

dnf -y install mingw64-SDL2 mingw64-SDL2_image mingw64-SDL2_mixer \
mingw64-curl mingw64-physfs mingw64-glib2 mingw64-libjpeg-turbo \
mingw64-zlib mingw64-pkg-config mingw64-libxml2 mingw64-SDL2_image mingw64-pdcurses

dnf -y install mingw32-SDL2 mingw32-SDL2_image mingw32-SDL2_mixer \
mingw32-curl mingw32-physfs mingw32-glib2 mingw32-libjpeg-turbo \
mingw32-zlib mingw32-pkg-config mingw32-libxml2 mingw32-SDL2_image mingw32-pdcurses

dnf -y upgrade
dnf -y clean all
