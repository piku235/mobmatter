#!/bin/sh

REPO="piku235/mobmatter"
PACKAGE_NAME="mobmatter-gtw.tar.gz"
PACKAGE_URL="https://github.com/$REPO/releases/latest/download/$PACKAGE_NAME"
MATTER_BRIDGE_BIN="/opt/jungi/bin/mobmatter"

if [ ! -f "$MATTER_BRIDGE_BIN" ]; then
  echo "mobmatter is not installed"
  exit 1
fi

BIN_VERSION=$("$MATTER_BRIDGE_BIN" --version | awk '{print $2}')
LATEST_VERSION=$(wget --no-check-certificate -qO- https://api.github.com/repos/$REPO/releases/latest | grep '"tag_name":' | sed -E 's/^[[:space:]]*"tag_name": *"([^"]+)",?/\1/')

if [ "$BIN_VERSION" = "$LATEST_VERSION" ]; then
  echo "mobmatter is already at the latest $BIN_VERSION"
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

echo "Stopping mobmatter"
/etc/init.d/mobmatter stop

echo "Looking for outdated files"

if ! cmp -s etc/init.d/mobmatter /etc/init.d/mobmatter; then
  echo "Updating mobmatter service definition"
  cp etc/init.d/mobmatter /etc/init.d/mobmatter
fi

echo "Checking files"

# new or existing
for sourcefile in opt/jungi/lib/*; do
  filename=$(basename "$sourcefile")
  destfile="/opt/jungi/lib/$filename"

  if [ ! -f "$destfile" ]; then
    cp -a "$sourcefile" "$destfile"
    echo "$destfile: NEW"
  elif ! cmp -s "$sourcefile" "$destfile"; then
    cp -a "$sourcefile" "$destfile"
    echo "$destfile: MODIFIED"
  fi
done

# obsolete
for sourcefile in /opt/jungi/lib/*; do
  filename=$(basename "$sourcefile")
  destfile="opt/jungi/lib/$filename"

  if [ ! -f "$destfile" ]; then
    rm -f "$sourcefile"
    echo "$sourcefile: REMOVED"
  fi
done

echo "Updating mobmatter"
cp opt/jungi/bin/mobmatter /opt/jungi/bin/mobmatter

echo "Starting mobmatter"
/etc/init.d/mobmatter start

sleep 5 # wait a little to be sure

if ps | grep -q [m]obmatter; then
  echo "SUCCESS!"
  echo "mobmatter is running!"
else
  echo "FAILED"
  echo "something went wrong, check logread for more"
  exit 1
fi
