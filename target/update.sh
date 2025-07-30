#!/bin/sh

REPO="piku235/matter-mobilus-bridge"
PACKAGE_NAME="matter_bridge_gtw.tar.gz"
PACKAGE_URL="https://github.com/$REPO/releases/latest/download/$PACKAGE_NAME"
MATTER_BRIDGE_BIN="/opt/matter/bin/matter-bridge"

if [ ! -f "$MATTER_BRIDGE_BIN" ]; then
  echo "matter bridge is not installed"
  exit 1
fi

BIN_VERSION=$("$MATTER_BRIDGE_BIN" --version | awk '{print $2}')
LATEST_VERSION=$(wget --no-check-certificate -qO- https://api.github.com/repos/$REPO/releases/latest | grep '"tag_name":' | sed -E 's/^[[:space:]]*"tag_name": *"([^"]+)",?/\1/')

if [ "$BIN_VERSION" = "$LATEST_VERSION" ]; then
  echo "matter bridge is already at the latest version: $BIN_VERSION"
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

echo "Looking for outdated files"

if ! cmp -s etc/init.d/matter-bridge /etc/init.d/matter-bridge; then
  echo "Updating matter bridge service definition"
  cp etc/init.d/matter-bridge /etc/init.d/matter-bridge
fi

# copy new or update
for sourcefile in opt/matter/lib/*; do
  filename=$(basename "$sourcefile")
  destfile="/opt/matter/lib/$filename"

  if [ ! -f "$destfile" ]; then
    echo "Copying new $destfile"
    cp "$sourcefile" "$destfile"
  elif ! cmp -s "$sourcefile" "$destfile"; then
    echo "Updating $filename"
    cp "$sourcefile" "$destfile"
  fi
done

# remove obsolete
for sourcefile in /opt/matter/lib/*; do
  filename=$(basename "$sourcefile")
  destfile="opt/matter/lib/$filename"

  if [ ! -f "$destfile" ]; then
    echo "Removing $sourcefile"
    rm -f "$sourcefile"
  fi
done

echo "Updating matter bridge"
cp opt/matter/bin/matter-bridge /opt/matter/bin/matter-bridge

echo "Starting matter bridge"
/etc/init.d/matter-bridge start

if ps | grep -q [m]atter-bridge; then
  echo "SUCCESS!"
  echo "matter-bridge is running!"
else
  echo "FAILED"
  echo "something went wrong, check logread for more"
  exit 1
fi
