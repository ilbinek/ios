#!/bin/bash
OSIXLY_CORRECT=yes

# Setup basic variables
DIR=$PWD
IFS=$'\n'
IGNORE=""
NORM=0

# Parsing arguments
while [ "$1" != "" ]; do
  case $1 in
    -n )  shift
          NORM=1
          ;;
    -i )  shift
          IGNORE=$1
          ;;
    * )   DIR=$1
          shift
          ;;
  esac
done

# Result variables
DIRS=0
FILES=0
B100=0
KIB1=0
KIB10=0
MIB1=0
MIB10=0
MIB100=0
GIB1=0
O1GIB=0

# Some magic that does what it needs to do
magic_do_what_you_need_to_do() {
  if [[ -d $1 ]]; then
    DIRS=$((DIRS+1))
  elif [[ -f $1 ]]; then
    FILES=$((FILES+1))
  fi
}

for entry in $(find "$DIR")
do
  magic_do_what_you_need_to_do $entry
done

# Print results
echo "Root directory: ${DIR}"
echo "Directories: ${DIRS}"
echo "All files: ${FILES}"
echo "File size histogram:"
echo "  <100 B  :"
echo "  <1 KiB  :"
echo "  <10 KiB :"
echo "  <100 KiB:"
echo "  <1 MiB  :"
echo "  <10 MiB :"
echo "  <100 MiB:"
echo "  <1 GiB  :"
echo "  >=1 GiB :"
