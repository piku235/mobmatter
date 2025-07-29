# 🏠 Matter Bridge for Mobilus Cosmo GTW

An **uncertified** matter bridge built on the official [Matter SDK](https://github.com/project-chip/connectedhomeip), designed to connect, control and subscribe to events from devices connected to **Mobilus Cosmo GTW** via the Matter protocol.

## Overview

This application acts as a **bridge** between the Matter ecosystem and Mobilus Cosmo GTW, enabling integration of window coverings (blinds, shutters) into Matter-compatible smart home platforms such as Apple HomeKit, Home Assistant and Google Home.

It is a native application targeted to run on the Cosmo GTW itself, which runs OpenWRT 15.05.1 as its underlying operating system.

## Features

- Bridges Mobilus Cosmo GTW-controlled devices into the Matter ecosystem
- Supports window covering devices
- Built on top of the official Matter SDK (CHIP)

## Requirements

For full compatibility, your Cosmo GTW device should be running the firmware version **0.1.7.8** or higher.

This application assumes your Cosmo GTW is fully setup and configured.

**Supported devices**

* Senso - eg ERS actuator
* Cosmo
* C-MR

## Build

This project uses **GN (Generate Ninja)** as its build system, provided by the Matter SDK.

### Prerequisites

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
