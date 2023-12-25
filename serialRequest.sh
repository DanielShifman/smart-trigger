#!/bin/bash

scriptDir="$HOME"$'[SCRIPT_DIRECTORY'
pathToExe=$scriptDir"IKEA_IoT_Interface"
toggleLampArgs=("Desk Lamp" "isOn" "auto")
port="/dev/cu.usbmodem2101"
baud=9600

# Change to the script directory
# The original directory is saved so that it can be returned to on exit (if need be)
originalDir=$(pwd)
cd "$scriptDir" || { echo "Failed to change directory to $scriptDir" && exit 1; }

function readSerial() {
  exec 3<$port
  stty -f $port $baud
  actionRegex=$'ACTION: (.+)$'
  while true; do
    read -r -d $'\n' line <&3
    # Get the action from format "ACTION: action"
    # (note the linefeed character is missing due to being specified as the read delimiter)
    if [[ $line =~ $actionRegex ]]; then
      if [[ ${BASH_REMATCH[1]} == "Toggle lamp" ]]; then
        echo "TOGGLING LAMP..."
        # run the executable with the arguments
        "$pathToExe" "${toggleLampArgs[@]}"
      else
        echo "Unimplemented action: ${BASH_REMATCH[1]}"
      fi
    else
      echo "$line"

    fi
  done
}

function resetDir() {
  if [[ $(pwd) == "$originalDir" ]]; then
    exitCode=0
  fi
  { cd "$originalDir" && exitCode=0; } || { echo "Failed to return to original directory" &&  exitCode=1; }
  exit $exitCode
}

# Reset the directory on exit (if need be depending on execution environment)
trap resetDir SIGINT

readSerial
