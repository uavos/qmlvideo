FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install --fix-missing

RUN apt-get install -y --force-yes \
    lsb-release

RUN echo "deb http://apt.uavos.com/ all main experimental"| tee /etc/apt/sources.list.d/uavos.list; gpg --keyserver keyserver.ubuntu.com --recv B5517CF1; gpg --export --armor B5517CF1 | apt-key add -; apt-get update

RUN apt-get install -y --force-yes \
      build-essential \
      qt5-qmake \
      qt5-default \
      libavcodec-dev \
      libavformat-dev \
      libavutil-dev \
      libswscale-dev \
      libavdevice-dev \
      libavfilter-dev \
      uavos \
      qtmultimedia5-dev \
      qtdeclarative5-dev \
      qtscript5-dev \
      git \
      libvtk6.2 \
      vtk6 \
      libjasper1 \
      gstreamer1.0-plugins-ugly \
      gstreamer1.0-plugins-bad \
      gstreamer1.0-plugins-good