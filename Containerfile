#
# build:
#    $ podman build -t xword:20231219 .
#
# run:
#    $ podman run --rm -e DISPLAY --privileged --net=host  \
#           -v /tmp/.X11-unix:/tmp/.X11-unix:z             \
#           -v $HOME/.Xauthority:/root/.Xauthority:z       \
#           -v $HOME/.XWord:/root/.XWord:z                 \
#           -v $HOME/path/to/xword-dir:/puzzles            \
#        xword:20231219 /puzzles/2023-12-18.jpz
#
FROM docker.io/library/ubuntu:23.10
COPY . /home/xword
RUN apt update && \
#
# Although it's one apt-get, there are two sets of packages on the
# command-line list: those we need FOR COMPILING, and those we need FOREVER.
# We enumerate them all, even though some of the forever packages are
# brought in as dependencies by their -dev packages, because we want
# to do a cleanup at the end (see purge/autoremove below) and by
# listing them explicitly we protect them from autoremove.
    apt-get -y --no-install-recommends install gcc g++ libgtk-3-0 libgtk-3-dev curl bzip2 patch make zlib1g-dev liblua5.1 liblua5.1-0-dev lua-filesystem libexpat-dev libcurl3-nss libcurl4-nss-dev ca-certificates libsm6 && \
#
# Makefile links against -llua5.1, but that's not the Debian name.
#
    ln -s liblua5.1.so /usr/lib/x86_64-linux-gnu/liblua51.so && \
    cd /home/xword && \
#
# Grumble. This fixes two compile-time problems with all versions of g++
# that I've tried.
#
    sed -i -e 's/^\(class GridSelectionHandler :\) \(wxEvtHandler\)/\1 public \2/' src/XGridCtrl.cpp && \
    sed -i -e '/^#include <cassert>/a #include <cwctype>' \
           -e 's/^\(PUZ_API bool \)puz::/\1/' puz/puzstring.cpp && \
#
# Preliminary setup, then compile. This is what takes the longest time.
#
    ./build-wxwidgets.sh 3.1.5 Release && \
    ./premake5-linux --wx-config=/root/wxWidgets-3.1.5/bin/wx-config \
                     --wx-config-release=/root/wxWidgets-3.1.5/bin/wx-config \
                     --wx-prefix=/root/wxWidgets-3.1.5 \
                     gmake && \
    cd build/gmake && \
    make && \
#
# Manual muckery required. See:
#   https://github.com/jpd236/xword/commit/705c1425e5162a67603e79aa49dd3c6a3238ee18
#
    cd ../.. && \
    mkdir -p share/XWord lib/XWord && \
    ln -s ../../images share/XWord/images && \
    ln -s ../../scripts lib/XWord/scripts && \
    ln -s ../../bin/Debug/libluapuz.so lib/XWord/luapuz.so && \
    ln -s ../../bin/Debug/scripts/libs/libc-luacurl.so lib/XWord/c-luacurl.so && \
    ln -s ../../bin/Debug/scripts/libs/libc-task.so lib/XWord/c-task.so && \
    ln -s ../../bin/Debug/scripts/libs/liblxp.so lib/XWord/lxp.so && \
    ln -s ../../bin/Debug/libyajl.so lib/XWord/yajl.so && \
    ln -s /usr/lib/x86_64-linux-gnu/lua/5.1/lfs.so lib/XWord/lfs.so && \
#
# This is horrible. Containerfile 'COPY' (see top) does not preserve mtimes.
# But there's a script, py2lua, that embeds mtime in its output. With a new
# mtime, the script runs at XWord init time, and pops up an annoying modal
# window with "Updating...success!" messages that is very hard to get rid of.
# Theoretically I could figure out how to run the py2lua script as a step
# here, but I've wasted much too much time on this already and I'm lazy.
#
    touch --date=@$(sed -ne 's/^-- modtime: //p' <scripts/download/gui/wxFB.lua) scripts/download/gui/wxFB.py && \
    touch --date=@$(sed -ne 's/^-- modtime: //p' <scripts/xworddebug/wxFB.lua) scripts/xworddebug/wxFB.py && \
#
# Clean up, to minimize the final size of our container image
#
    rm -rf .git build wxWidgets-3.1.5 wxWidgets-3.1.5.tar.gz && \
    apt -y purge gcc g++ libgtk-3-dev patch make zlib1g-dev libexpat-dev libcurl4-nss-dev && \
    apt -y autoremove && \
    apt clean
WORKDIR /home/xword
ENTRYPOINT ["/home/xword/bin/Debug/XWord"]
