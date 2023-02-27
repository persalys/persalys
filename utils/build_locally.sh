#!/bin/bash

echo -e "1. linux\n2. mingw\n3. salome\n\n> "
read choice
case $choice in
  1)
    img="linux"
    ;;
  2)
    img="mingw"
    ;;
  3)
    img="salome"
    ;;
  *)
    echo "sorry?"
    exit 1
    ;;
esac

docker build docker/${img} -t persalys/${img} && docker run --rm --volume `pwd`:/io persalys/${img} sh -c "/io/docker/${img}/run_docker_build.sh `id -u` `id -g`"
