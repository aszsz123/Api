DEVICE_NAME=Net_Client
export DEVICE_NAME


#LD_LIBRARY_PATH=./lib:/opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/lib
#LD_LIBRARY_PATH=./lib:/opt/gcc-linaro-7.4.1-2019.02-i686_arm-linux-gnueabihf/lib/
export LD_LIBRARY_PATH

#CROSS_COMPILE=/opt/gcc-linaro-7.4.1-2019.02-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
#CROSS_COMPILE=/opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-fsl-linux-gnueabi-
#CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf-
export CROSS_COMPILE



PROJECT_DIRECTORY=$PWD/..
export PROJECT_DIRECTORY

echo "====================================================================================================================="
echo "    BUILD ENV SET "
