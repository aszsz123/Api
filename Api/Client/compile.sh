#!/bin/bash

source env.sh
echo "====================================================================================================================="
echo "  START BUILD 				    "
echo "====================================================================================================================="
#check environment
if [ ${#DEVICE_NAME} -gt 0 ]
then
	echo " ## Device Name : "$DEVICE_NAME" ##"
else
	echo " #### do \"source env.sh\" at project top directory ####"

	exit 0
fi

OPTIONFILE="optionfile"

# check $2, $3 parameter

if [ "$1" = "s" ]
then
	echo "$2" > $OPTIONFILE
	echo "$3" >> $OPTIONFILE
fi

# check compile option file exist
if [ -e $OPTIONFILE ]
then
	echo "! found file : "$OPTIONFILE
else
# make null file
	echo "" > $OPTIONFILE
fi

# declare variable
declare -a ALIST
declare -i count=0

declare -i buildnumber=0
VERSIONINFO="100"

# file open
exec 10<$OPTIONFILE

# read file data
while read LINE <&10; do

ALIST[$count]=$LINE
((count+=1))

done

# file close
exec 10>&-

# store number of read data
ELEMENTS=${#ALIST[@]}

# check number of read data
if (( ELEMENTS > 1))
then
	echo "! found S/W version & build number"

VERSIONINFO=${ALIST[0]}
buildnumber=${ALIST[1]}

((buildnumber+=1))

((buildnumber = buildnumber % 1000))
echo "> Current S/W version is  : "$VERSIONINFO
echo "> Current build number is : "$buildnumber
echo "====================================================================================================================="
else
echo ">  no S/W version"
echo ">  no Build number"
echo "====================================================================================================================="
fi

#########################################################################
#########################################################################

TARGETFILE=$DEVICE_NAME


make clean

# check compile option file exist
echo "====================================================================================================================="
echo ">  BUILD LOG ..."
make -j4 DFLAGS="-DBUILD_NUMBER=$buildnumber -DSW_VERSION=$VERSIONINFO" #컴파일 속도 개선
# make DFLAGS="-DBUILD_NUMBER=$buildnumber -DSW_VERSION=$VERSIONINFO"

echo "====================================================================================================================="
if [ -e $TARGETFILE ]
	then
	echo "#######----------- build Release OK ------------------"


BUILD_NUM=`printf "%03d" $buildnumber`

APPFILE=""$TARGETFILE"_"$VERSIONINFO"_"$BUILD_NUM""
	mv -f $TARGETFILE $APPFILE

echo "> $APPFILE"

else
	echo "######------------ build Release FAIL!! ----------------"
	echo
	exit 0
fi
# update option file
echo $VERSIONINFO > $OPTIONFILE
echo $buildnumber >> $OPTIONFILE
echo "====================================================================================================================="

rm *.o
#cd ./ubuntuRootfs/
#./mkopfs.sh&

