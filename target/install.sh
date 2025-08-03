#!/bin/sh

REPO="piku235/matter-mobilus-bridge"
PACKAGE_NAME="matter_bridge_gtw.tar.gz"
PACKAGE_URL="https://github.com/$REPO/releases/latest/download/$PACKAGE_NAME"
CONF_FILE="matter-bridge.conf"

if [ ! -f "/mobilus/mobilus" ]; then
  echo "mobilus is not present, aborting"
  exit 1
fi

if [ -d "/opt/matter" ]; then
  echo "matter bridge is already installed"
  exit 1
fi

cleanup() {
  rm -rf /tmp/mmbridge
  rm -f "/tmp/$PACKAGE_NAME"
}

trap cleanup EXIT

echo "Downloading the latest package"
cd /tmp
wget -qO "$PACKAGE_NAME" --no-check-certificate "$PACKAGE_URL"
if [ $? -ne 0 ]; then
  echo "Failed to download the latest package"
  exit 1
fi

echo "Extracting the package"
mkdir mmbridge
gzip -dc "$PACKAGE_NAME" | tar -xf - -C mmbridge .
if [ $? -ne 0 ]; then
  echo "Failed to extract the package"
  exit 1
fi

cd mmbridge

echo -n "Provide the mobilus username: "
read mobilus_username

echo -n "Provide the mobilus password: "
read mobilus_password

cat <<EOF > "/opt/matter/etc/$CONF_FILE"
[mobilus]
username=$mobilus_username
password=$mobilus_password
EOF

echo "Copying files to storage, may take a while ..."
cp /etc/rc.local /etc/rc.local.old # backup
cp -a . /

echo "Stopping mobilus process"
pkill mobilus

echo "Enabling and starting services"
/etc/init.d/mobilus enable
/etc/init.d/matter-bridge enable
/etc/init.d/mobilus start
/etc/init.d/matter-bridge start

sleep 5 # wait a little to be sure

if ps | grep -q [m]atter-bridge && ps | grep -q [m]obilus; then
  echo "SUCCESS!"
  echo "mobilus and matter-bridge are running!"
  echo "Now matter-bridge is in the commissioning mode"
  echo "Scan QR code or type manual code to pair it"
else
  echo "FAILED"
  echo "It seems matter-bridge is not running"
  echo "Are you sure you typed correct username and password?"
  echo "Verify by running vim /opt/matter/etc/$CONF_FILE"
  exit 1
fi
