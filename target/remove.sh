#!/bin/sh

if [ ! -d "/opt/jungi" ]; then
  echo "mobmatter is not installed"
  exit 1
fi

echo "Disabling and stopping services"
/etc/init.d/mobilus disable
/etc/init.d/mobmatter disable
/etc/init.d/mobilus stop 2>/dev/null
/etc/init.d/mobmatter stop 2>/dev/null

rm -f /etc/init.d/mobilus
rm -f /etc/init.d/mobmatter
mv /etc/rc.local.old /etc/rc.local

echo "Removing mobmatter from storage ..."
rm -rf /opt/jungi
rmdir /opt 2>/dev/null # remove if empty

echo "Starting mobilus process"
/mobilus/mobilus &

echo "Complete"
