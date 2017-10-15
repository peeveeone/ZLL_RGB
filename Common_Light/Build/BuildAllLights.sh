#!/bin/bash
#Usage below
#    1. make sure that environment Path variable must contain C:\NXP\bstudio_nxp\msys\bin in the beginning!
#    2. ./BuildAllLights.sh <JN5168 or JN5169>
echo "Building Lights from JN-AN-1171"

start=$(date +"%T")
echo "Start Time : $start"
    

    make JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 clean >> BuildLog_Light_DimmableLight_JN5168_ME_CLEAN.txt &


    echo "Cleaning; Please wait"
    wait


    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=1 NUMBER_ENDPOINTS=1		>> BuildLog_Light_ColorLight_JN5168_ME_01_INV.txt &
    echo "Building; 1/8 - Please wait"
    wait

    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=0 NUMBER_ENDPOINTS=1		>> BuildLog_Light_ColorLight_JN5168_ME_01.txt &
    echo "Building; 2/8 - Please wait"
    wait

    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=1 NUMBER_ENDPOINTS=2		>> BuildLog_Light_ColorLight_JN5168_ME_02_INV.txt &
    echo "Building; 3/8 - Please wait"
    wait

    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=0 NUMBER_ENDPOINTS=2		>> BuildLog_Light_ColorLight_JN5168_ME_02.txt &
    echo "Building; 4/8 - Please wait"
    wait
    
    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=1 NUMBER_ENDPOINTS=3		>> BuildLog_Light_ColorLight_JN5168_ME_03_INV.txt &
    echo "Building; 5/8 - Please wait"
    wait

    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=0 NUMBER_ENDPOINTS=3		>> BuildLog_Light_ColorLight_JN5168_ME_03.txt &
    echo "Building; 6/8 - Please wait"
    wait
    
    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=1 NUMBER_ENDPOINTS=4		>> BuildLog_Light_ColorLight_JN5168_ME_04_INV.txt &
    echo "Building; 7/8 - Please wait"
    wait

    make -B JENNIC_SDK=JN-SW-4168 JENNIC_CHIP=JN5168 LIGHT=Light_ColorLight RGB=1 INVERT=0 NUMBER_ENDPOINTS=4		>> BuildLog_Light_ColorLight_JN5168_ME_04.txt &
    echo "Building; 8/8 - Please wait"
    wait




end=$(date +"%T")
echo "End Time : $end"

echo "Done !!!"
