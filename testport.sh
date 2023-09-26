#!/bin/bash

#set -x

#
# Variables Section
#==============================================================
# list process to monitor in the variable below.

PROGRAM1="asm 6001"

# variable checks to see if $PROGRAM1
# is running.

APPCHK=$(ps ux | grep -c "$PROGRAM1")

#==================================================================

# The 'if' statement below checks to see if the process is running
# with the 'ps' command.  If the value is returned as a '0? then
# an email will be sent and the process will be safely restarted.
#

if [ "$APPCHK" = '1' ];

then

	echo "Starting up the Test Port!"
        cd ~/MUD/vtr/area
	../src/asm 6001 &

else

	echo "The Test Port is already running!"

fi

echo "$APPCHK"

exit

