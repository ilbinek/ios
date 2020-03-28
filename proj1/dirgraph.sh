#!/bin/bash
export POSIXLY_CORRECT=yes

# Setup basic variables
DIR=$PWD
IFS=$'\n'
IGNORING=0
IGNORE=""
NORM=0

# Parsing arguments
while [ "$1" != "" ]; do
  case $1 in
    -n )  shift
          NORM="1"
          ;;
    -i )  shift
          IGNORING="1"
          IGNORE=$1
          shift
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
KIB100=0
MIB1=0
MIB10=0
MIB100=0
GIB1=0
O1GIB=0

# Some magic that does what it needs to do
magic_do_what_you_need_to_do() {
  # If should be ignored
  if [[ $IGNORING == "1" ]]; then
    if [[ $1 == *"$IGNORE"* ]]; then
      return
    fi
  fi
  if [[ -d $1 ]]; then
    # If directory
    DIRS=$((DIRS+1))
  elif [[ -f $1 ]]; then
    # If file
    FILES=$((FILES+1))
    SIZE="$(wc -c < "$1")"
    # Add one to desired size
    if [ "$SIZE" -lt 100 ]; then
        B100=$((B100+1))
    elif [ "$SIZE" -lt 1024 ]; then
      KIB1=$((KIB1+1))
    elif [ "$SIZE" -lt 10240 ]; then
      KIB10=$((KIB10+1))
    elif [ "$SIZE" -lt 102400 ]; then
      KIB100=$((KIB100+1))
    elif [ "$SIZE" -lt 1048576 ]; then
      MIB1=$((MIB1+1))
    elif [ "$SIZE" -lt 10485760 ]; then
      MIB10=$((MIB10+1))
    elif [ "$SIZE" -lt 104857600 ]; then
      MIB100=$((MIB100+1))
    elif [ "$SIZE" -lt 1073741824 ]; then
      GIB1=$((GIB1+1))
    elif [ "$SIZE" -ge 1073741824 ]; then
      O1GIB=$((O1GIB+1))
    fi
  fi
}

#Some more magic to get those hastags
magic_get_my_hasgtags() {
  if [[ $NORM == "1" ]]; then
    echo
  else
    HASTAGS=""
    I=0
    while [[ $I -lt $1 ]]; do
      HASTAGS="$HASTAGS""#"
      I=$((I+1))
    done
  fi
  printf "%s" $HASTAGS
  return
}

for entry in $(find "$DIR")
do
  magic_do_what_you_need_to_do "$entry"
done

# Print results
echo "Root directory: ${DIR}"
echo "Directories: ${DIRS}"
echo "All files: ${FILES}"
echo "File size histogram:"
echo "  <100 B  : $(magic_get_my_hasgtags $B100)"
echo "  <1 KiB  : $(magic_get_my_hasgtags $KIB1)"
echo "  <10 KiB : $(magic_get_my_hasgtags $KIB10)"
echo "  <100 KiB: $(magic_get_my_hasgtags $KIB100)"
echo "  <1 MiB  : $(magic_get_my_hasgtags $MIB1)"
echo "  <10 MiB : $(magic_get_my_hasgtags $MIB10)"
echo "  <100 MiB: $(magic_get_my_hasgtags $MIB100)"
echo "  <1 GiB  : $(magic_get_my_hasgtags $GIB1)"
echo "  >=1 GiB : $(magic_get_my_hasgtags $O1GIB)"