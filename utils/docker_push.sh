#!/bin/sh

usage()
{
  echo "Usage: $0 version"
  exit 1
}

test $# = 1 || usage


version=$1
docker build docker/linux -t persalys/linux:${version} && docker push persalys/linux:${version}
docker build docker/mingw -t persalys/mingw:${version} && docker push persalys/mingw:${version}
