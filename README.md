> [!WARNING]  
> This project is still in development and may be unstable.

# tf-fs
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)

**tf-fs** is a minimal block-based filesystem.

## Design

The design of tf-fs is driven by a single goal:  
**maximize usable storage by minimizing filesystem overhead**.

To achieve this, **tf-fs**:

- Uses a single-level namespace to avoid directory metadata
- Relies on fixed metadata allocation rather than dynamic growth
- Favors simple, static on-disk structures over flexible but more complex abstractions

The result is a simple, predictable filesystem that prioritizes space efficiency.

## Platform Support

**tf-fs** is currently Windows only and uses WinFsp to mount the filesystem
to the operating system. Linux and macOS support may be added in the future.

## Disk Image

**tf-fs** uses a **disk image file (`.img`)** that represents the backing storage
for the filesystem. All filesystem structures are stored directly within this image.

A disk image must be created before mounting **tf-fs**. The size of the image determines the total filesystem capacity.

Example:

```bash
fsutil file createnew disk.img 67108864
```

> [!NOTE]
> You must update `DISK_MB` in [`disk.h`](disk/disk.h) to the size of your disk image before building.

## Build instructions

**tf-fs** uses CMake to build, please follow the following steps to build:

### Requirements

- C compiler (GCC, Clang, or MSVC)
- CMake 3.15+
- [WinFsp](https://winfsp.dev/rel/)

### Build steps

1. **Clone the repository**

    ```bash
    git clone https://github.com/Hudson111-s/tf-fs.git
    cd tf-fs
    ```
    
2. **Build**

    ```bash
    cmake -S . -B build
    cmake --build build
    ```

## Usage

```bash
tf-fs <disk.img> <mountpoint>
```

## Acknowledgments

This project uses WinFsp - Windows File System Proxy, Copyright (C) Bill Zissimopoulos.

WinFsp is used under a special exception to the GPLv3 that allows linking with Open Source software. You can find the WinFsp project and license at https://github.com/winfsp/winfsp.

## Contribution

Contributions are very welcome! Feel free to open [issues](https://github.com/Hudson111-s/tf-fs/issues) or submit [pull requests](https://github.com/Hudson111-s/tf-fs/pulls).

## Author

Created with ❤️ by [Hudson111-s](https://github.com/Hudson111-s)
