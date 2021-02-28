FROM debian:buster
RUN apt update -y
RUN apt install -y git make gcc debhelper dpkg-dev cmake libboost-dev libfox-1.6-dev libpng-dev libphysfs-dev vim
RUN git clone https://github.com/ennorehling/csmapfx.git
RUN cd csmapfx && s/deb-build
RUN dpkg -i ../csmapfx_*_amd64.deb
VOLUME /data

