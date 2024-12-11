#!/bin/bash

if test "$#" -lt 1
then
  echo -e "1. linux\n2. mingw\n3. salome\n\n> "
  read choice
else
  choice="$1"
fi

case $choice in
  "1" | "linux")
    img="linux"
    ;;
  "2" | "mingw")
    img="mingw"
    ;;
  "3" | "salome")
    img="salome"
    ;;
  *)
    echo "sorry?"
    exit 1
    ;;
esac

docker build docker/${img} -t persalys/${img} && docker run --rm --volume `pwd`:/io persalys/${img} sh -c "/io/docker/${img}/run_docker_build.sh `id -u` `id -g`"
