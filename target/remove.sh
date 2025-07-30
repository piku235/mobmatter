#!/bin/sh

if [ ! -d "/opt/matter" ]; then
  echo "matter bridge is not installed"
  exit 1
fi

echo "Disabling and stopping services"
/etc/init.d/mobilus disable
/etc/init.d/matter-bridge disable
/etc/init.d/mobilus stop 2>/dev/null
/etc/init.d/matter-bridge stop 2>/dev/null

rm -f /etc/init.d/mobilus
rm -f /etc/init.d/matter-bridge
mv /etc/rc.local.old /etc/rc.local

echo "Removing matter bridge from storage ..."
rm -rf /opt/matter
rmdir /opt 2>/dev/null # remove if empty

echo "Starting mobilus process"
/mobilus/mobilus &

echo "Complete"
