#!/bin/sh

REPO="piku235/mobmatter"
PACKAGE_NAME="mobmatter-gtw.tar.gz"
PACKAGE_URL="https://github.com/$REPO/releases/latest/download/$PACKAGE_NAME"
CONF_FILE="mobmatter.conf"

if [ ! -f "/mobilus/mobilus" ]; then
  echo "mobilus is not present, aborting"
  exit 1
fi

if [ -d "/opt/jungi" ]; then
  echo "mobmatter is already installed"
  exit 1
fi

cleanup() {
  rm -rf /tmp/mobmatter
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
mkdir mobmatter
gzip -dc "$PACKAGE_NAME" | tar -xf - -C mobmatter .
if [ $? -ne 0 ]; then
  echo "Failed to extract the package"
  exit 1
fi

cd mobmatter

echo -n "Provide the mobilus username: "
read mobilus_username

echo -n "Provide the mobilus password: "
read mobilus_password

cat <<EOF > "opt/jungi/etc/$CONF_FILE"
MOBILUS_USERNAME=$mobilus_username
MOBILUS_PASSWORD=$mobilus_password
EOF

echo "Copying files to storage, may take a while ..."
cp /etc/rc.local /etc/rc.local.old # backup
cp -a . /

echo "Stopping mobilus process"
pkill mobilus

echo "Enabling and starting services"
/etc/init.d/mobilus enable
/etc/init.d/mobmatter enable
/etc/init.d/mobilus start
/etc/init.d/mobmatter start

sleep 5 # wait a little to be sure

if ps | grep -q [m]obmatter && ps | grep -q [m]obilus; then
  echo "SUCCESS!"
  echo "mobilus and mobmatter are running!"
  echo "Now mobmatter is in the commissioning mode"
  echo "Scan QR code or type manual code to pair it"
else
  echo "FAILED"
  echo "It seems mobmatter is not running"
  echo "Are you sure you typed correct username and password?"
  echo "Verify by running vim /opt/jungi/etc/$CONF_FILE"
  exit 1
fi
