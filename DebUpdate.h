#!/bin/bash

# random Zeit erstellen (5 minuten Schritte bis zu 4h)
RANDOM_WAIT=$(( RANDOM % 49 * 300 ))
sleep $RANDOM_WAIT

LOGFILE="/tmp/apt_log.txt"
ERRORFILE="/tmp/apt_error.txt"
SENDER="/opt/send2mqtt.sh"

# Funktion, um Fehler zu protokollieren und die Verarbeitung zu beenden
check_exit() {
    if [ $? -ne 0 ]; then
        echo '{\"devicename\":\"$(hostname)\",\"process\":\"debianupdate\",\"status\":\"failure\",\"message\":\"Upgrade fehlgeschlagen\",\"errors\":\"'$(cat "$ERRORFILE" | tr '\n' ' ')'\"}' 2>&1 | "$SENDER"
        exit 1
    fi
}

# Fuehrt jeden Befehl einzeln aus und prueft den Exit-Status
apt update > "$LOGFILE" 2> "$ERRORFILE"
check_exit

apt full-upgrade -y >> "$LOGFILE" 2>> "$ERRORFILE"
check_exit

apt upgrade -y >> "$LOGFILE" 2>> "$ERRORFILE"
check_exit

apt dist-upgrade -y >> "$LOGFILE" 2>> "$ERRORFILE"
check_exit

apt autoremove -y >> "$LOGFILE" 2>> "$ERRORFILE"
check_exit

# Erfolgreiche Ausf  hrung
echo "{\"devicename\":\"$(hostname)\",\"process\":\"debianupdate\",\"status\":\"success\",\"message\":\"Upgrade erfolgreich\"}" 2>&1 | "$SENDER"
