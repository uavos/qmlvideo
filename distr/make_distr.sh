#!/bin/sh
#docker build -f 1604.Dockerfile -t qmlvideo1604 .
docker run --name qmlvideo1604 -it -d qmlvideo1604 bash
docker exec -i qmlvideo1604 bash -c "apt-get update && apt-get -y upgrade"
docker cp ../../qmlvideo qmlvideo1604:/

docker exec -i qmlvideo1604 rm -rf /qmlvideo1604/bin
docker exec -i qmlvideo1604 rm -rf /qmlvideo1604/obj
docker exec -i qmlvideo1604 bash -c "cd /qmlvideo && make clean && qmake && make"
docker cp qmlvideo1604:/qmlvideo/bin/libqmlvideo.so ./libqmlvideo_xenial.so

docker stop qmlvideo1604
docker rm qmlvideo1604

#docker build -f 1710.Dockerfile -t qmlvideo1704 .
docker run --name qmlvideo1704 -it -d qmlvideo1704 bash
docker exec -i qmlvideo1704 bash -c "apt-get update && apt-get -y upgrade"
docker cp ../../qmlvideo qmlvideo1704:/

docker exec -i qmlvideo1704 rm -rf /qmlvideo1704/bin
docker exec -i qmlvideo1704 rm -rf /qmlvideo1704/obj
docker exec -i qmlvideo1704 bash -c "cd /qmlvideo && make clean && qmake && make"
docker cp qmlvideo1704:/qmlvideo/bin/libqmlvideo.so ./libqmlvideo_zesty.so

docker stop qmlvideo1704
docker rm qmlvideo1704

#ui
cp ../../qmlvideo/qmlvideo_full.qml qmlvideo_full.qml
cp ../../qmlvideo/qmlvideo_lite.qml qmlvideo_lite.qml
