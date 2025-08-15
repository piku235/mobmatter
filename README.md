# 🏠 Matter Bridge for Mobilus Cosmo GTW

[![CI](https://github.com/piku235/matter-mobilus-bridge/actions/workflows/continuous-integration.yml/badge.svg)](https://github.com/piku235/matter-mobilus-bridge/actions/workflows/continuous-integration.yml)

An **uncertified** matter bridge built on the official [Matter SDK](https://github.com/project-chip/connectedhomeip), designed to connect, control and subscribe to events from devices connected to **Mobilus Cosmo GTW** via the Matter protocol.

## Overview

<img width="120" src="qr.png" alt="qr" align="right" />

This application acts as a **bridge** between the Matter ecosystem and Mobilus Cosmo GTW, enabling integration of window coverings (blinds, shutters) into Matter-compatible smart home platforms such as Apple HomeKit, Home Assistant and Google Home.

It is a native application targeted to run on the Cosmo GTW itself, which runs OpenWRT 15.05.1 as its underlying operating system.

**Supported devices:**

* Senso - e.g., ERS actuator
* Cosmo
* C-MR

For devices supported by the Cosmo GTW but not listed above, feel free to request support by [opening an issue](https://github.com/piku235/matter-mobilus-bridge/issues/new).

## Features

- Built on top of the official Matter SDK (CHIP)
- Bridges Cosmo GTW connected devices into the Matter ecosystem
- Supports window covering devices
- Real-time updates of the connected devices
- Runs natively on Cosmo GTW

## Requirements

For full compatibility, your Cosmo GTW should be running the firmware version **0.1.7.8** or higher.

Before installation, it is assumed that your Cosmo GTW is fully set up and configured.

## Installation

Installation is performed on the Cosmo GTW via SSH and the installation script.

How to enable SSH access, you'll find [here](https://forum.arturhome.pl/t/aktywacja-ssh-dla-mobilus-cosmo-gtw/15325).

Once connected via SSH, run the following command to start the installation:

```bash
cd /tmp && wget --no-check-certificate https://raw.githubusercontent.com/piku235/matter-mobilus-bridge/main/target/install.sh
chmod a+x install.sh
./install.sh
```

After successful installation, you can inspect the Matter bridge logs by running:

```bash
logread -e matter
```

On first run, the **matter bridge** enters commissioning mode and remains in it until successfully paired.

You can use the QR code above in this README, or pair manually using this code: `21693312337`.

### Update

To update matter bridge to the latest version, run:

```bash
wget --no-check-certificate -qO- https://raw.githubusercontent.com/piku235/matter-mobilus-bridge/main/target/update.sh | sh
```

### Remove

To remove and rollback changes made by matter bridge run:

```bash
wget --no-check-certificate -qO- https://raw.githubusercontent.com/piku235/matter-mobilus-bridge/main/target/remove.sh | sh
```

## Build

This project uses **GN (Generate Ninja)** as its build system, provided by the Matter SDK.

### Prerequisites

This project targets Linux environments; attempts to build it on non-Linux platforms are likely to fail.

For building project on your host you need to install all required libs and tools, follow the official [Matter SDK Prerequisites](https://project-chip.github.io/connectedhomeip-doc/guides/BUILDING.html#prerequisites).

To build the project for Cosmo GTW, you need to pull and build [OpenWRT](https://openwrt.org/docs/guide-developer/toolchain/use-buildsystem).

The Cosmo GTW uses the Hi-Link HLK-7668A module, so configure OpenWRT for the target platform `ramips` and subtarget `mt76x8`.

Clone the project with its submodules:

```bash
git clone https://github.com/piku235/matter-mobilus-bridge.git
cd matter-mobilus-bridge && git submodule update --init
```

### Host Build

For first-time setup:

```bash
third_party/connectedhomeip/scripts/checkout_submodules.py --platform <host> --shallow
source third_party/connectedhomeip/scripts/bootstrap.sh
gn gen out/host
ninja -C out/host
```

For subsequent builds:

```bash
source export.sh
gn gen out/host
ninja -C out/host
```

### Cosmo GTW Build

For first time use:

```bash
third_party/connectedhomeip/scripts/checkout_submodules.py --platform linux --shallow
source third_party/connectedhomeip/scripts/bootstrap.sh
export OPENWRT_TARGET_DIR=<path_to_openwrt_build>
gn gen out/target --args='import("//build/target.gni")'
ninja -C out/target
```

For subsequent builds:

```bash
source export.sh
export OPENWRT_TARGET_DIR=<path_to_openwrt_build>
gn gen out/target --args='import("//build/target.gni")'
ninja -C out/target
```

## License and Notices

This project is licensed under the [MIT License](LICENSE).

It includes portions of the [Matter SDK](https://github.com/project-chip/connectedhomeip), which is licensed under the Apache License 2.0. Use of Matter SDK does **not** imply compliance, certification, or endorsement by the Connectivity Standards Alliance.

Please see [NOTICE](./NOTICE) for legal notices and attribution related to the Matter SDK.
