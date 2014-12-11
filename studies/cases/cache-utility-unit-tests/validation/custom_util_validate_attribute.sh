#!/bin/bash

OUTPUTFILE="$1"
NUM_NODES="%%num_nodes%%"
FAILLOG="%%fail_file%%"
EXPECTED_DELIVERED="%%expected_delivered%%"

DO_DELIVERED=$(cat ${OUTPUTFILE} | grep "Received," | wc -l)

HAS_STATS=$(grep -l "^Data objects in cache: " ${OUTPUTFILE} | wc -l)
if [ "${HAS_STATS}" != "1" ]; then
    echo "No stats data object." >> ${FAILLOG}
    exit 1
fi

if [ "${DO_DELIVERED}" != "${EXPECTED_DELIVERED}" ]; then
    echo "Wrong DO delivered. ${DO_DELIVERED} != ${EXPECTED_DELIVERED}" >> ${FAILLOG}
    exit 1
fi

NUM_DO=$(grep "^Data objects in cache: " ${OUTPUTFILE} | sed 's/.*: \(.*\)/\1/')
NUM_INSERTED=$(grep "^Total data objects inserted: " ${OUTPUTFILE} | sed 's/.*: \(.*\) (.*/\1/')
NUM_EVICTED=$(grep "^Total data objects evicted: " ${OUTPUTFILE} | sed 's/.*: \(.*\) (.*/\1/')

if [ "${NUM_DO}" != "2" ]; then
    echo "${NUM_DO} != 2, fail!" >> ${FAILLOG}
    exit 1
fi


if [ "${NUM_INSERTED}" != "3" ]; then
    echo "Incorrect number inserted: ${NUM_INSERTED} != 3" >> ${FAILLOG}
    exit 1
fi

if [ "${NUM_EVICTED}" != "1" ]; then 
    echo "Incorrect number evicted: ${NUM_EVICTED} != 1" >> ${FAILLOG}
    exit 1
fi

ATTR_UTIL=$(grep " 73728 " ${OUTPUTFILE} | uniq | awk '{print $5;}')
if [ "${ATTR_UTIL}" != "0.60" ]; then
    echo "Incorrect utility computation: ${NBR_UTIL} != 0.60"
    exit 1
fi

NORMAL_UTIL=$(grep " 72704 " ${OUTPUTFILE} | uniq | awk '{print $5;}')
if [ "${NORMAL_UTIL}" != "1.00" ]; then
    echo "Incorrect utility computation: ${NORMAL_UTIL} != 1.00"
    exit 1
fi

exit 0