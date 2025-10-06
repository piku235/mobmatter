# 🏠 mobmatter - Matter Bridge for Mobilus Cosmo GTW

[![CI](https://github.com/piku235/mobmatter/actions/workflows/continuous-integration.yml/badge.svg)](https://github.com/piku235/mobmatter/actions/workflows/continuous-integration.yml)

An **uncertified** matter bridge built on the official [Matter SDK](https://github.com/project-chip/connectedhomeip), designed to connect, control and subscribe to events from devices connected to **Mobilus Cosmo GTW** via the Matter protocol.

## Overview

<img width="120" src="qr.png" alt="qr" align="right" />

This application acts as a **bridge** between the Matter ecosystem and Mobilus Cosmo GTW, enabling integration of window coverings (blinds, shutters) into Matter-compatible smart home platforms such as Apple HomeKit, Home Assistant and Google Home.

It is a native application targeted to run on the Cosmo GTW itself, which runs OpenWRT 15.05.1 as its underlying operating system.

**Supported devices:**

* Senso - e.g. M35 ERS actuators
* Cosmo - e.g. M35 MR actuators
* C-MR

For devices supported by the Cosmo GTW but not listed above, feel free to request support by [opening an issue](https://github.com/piku235/mobmatter/issues/new).

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

Installation is performed on the Cosmo GTW via SSH. How to enable SSH access, you'll find [here](https://forum.arturhome.pl/t/aktywacja-ssh-dla-mobilus-cosmo-gtw/15325).

Once connected via SSH, run the following command to start the installation of the [**runtime**](https://github.com/piku235/mobilus-gtw-runtime), which is required to run the **mobmatter**:

```bash
wget --no-check-certificate -qO- https://raw.githubusercontent.com/piku235/mobilus-gtw-runtime/main/install.sh | sh
```

After the **runtime** installation, you can finally install the **mobmatter** with this simple command:

```bash
/opt/jungi/bin/pkg install mobmatter
```

Adjust the config `/opt/jungi/etc/mobmatter.conf` and start the **mobmatter** service:

```bash
/etc/init.d/mobmatter start
```

You can inspect the **mobmatter** logs by running:

```bash
logread -e matter
```

On first run, the **mobmatter** enters commissioning mode and remains in it until successfully paired.

You can use the QR code above in this README, or pair manually using this code: `21693312337`.

### Update

```bash
/opt/jungi/bin/pkg update mobmatter
```

### Remove

```bash
/opt/jungi/bin/pkg remove mobmatter
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
git clone https://github.com/piku235/mobmatter.git
cd mobmatter && git submodule update --init
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
export OPENWRT_SYSROOT_DIR=<path_to_openwrt_build>
gn gen out/target --args='import("//build/target.gni")'
ninja -C out/target
```

For subsequent builds:

```bash
source export.sh
export OPENWRT_SYSROOT_DIR=<path_to_openwrt_build>
gn gen out/target --args='import("//build/target.gni")'
ninja -C out/target
```

## License and Notices

This project is licensed under the [MIT License](LICENSE).

It includes portions of the [Matter SDK](https://github.com/project-chip/connectedhomeip), which is licensed under the Apache License 2.0. Use of Matter SDK does **not** imply compliance, certification, or endorsement by the Connectivity Standards Alliance.

Please see [NOTICE](./NOTICE) for legal notices and attribution related to the Matter SDK.
