#!/bin/bash
# Usage: ./utils/build_locally.sh [variant] [img_ver]

set -x -e

if test "$#" -lt 1
then
  echo -e "1. linux\n2. mingw\n3. salome\n\n> "
  read variant
else
  variant="$1"
fi

case $variant in
  "1" | "linux")
    variant="linux"
    ;;
  "2" | "mingw")
    variant="mingw"
    ;;
  "3" | "salome")
    variant="salome"
    ;;
  "4" | "fedora")
    variant="fedora"
    ;;
  *)
    echo "sorry?"
    exit 1
    ;;
esac

if test "$#" -lt 2
then
  img="persalys/${variant}"
  docker build docker/${variant} -t ${img}
else
  img="persalys/${variant}:$2"
  docker pull ${img}
fi

docker run --rm --volume `pwd`:/io ${img} sh -c "/io/docker/${variant}/run_docker_build.sh `id -u`:`id -g`"
