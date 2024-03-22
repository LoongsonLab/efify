This tool makes an efi format kernel from an elf format kernel input.

## Usage

1. copy your elf kernel file here named 'kernel'
2. modify these variables in Makefile according to your kernel:

```bash
KERNEL_LOAD_ADDRESS ?= 0x9000000000200000
KERNEL_ENTRY_ADDRESS ?= 0x90000000011b7000
```

3. make sure there are loongarch cross tools in your path;make and you will get kernel.efi, it is the efi kernel you need

```bash
    make
```

4. use kernel.efi with qemu like this:

```bash
    ./qemu-system-loongarch64 -bios ./QEMU_EFI.fd -kernel kernel.efi ...
```

## about efi stub

The libefistub.a here is generated from a build of upstream linux kernel source:

```bash
    cd driver/firmware/efi/libstub
    loongarch64-linux-gnu-ar rcs libefistub.a *.stub.o
    loongarch64-linux-gnu-strip -d libefistub.a
```

Use the efi stub your kernel can interact with UEFI bios.

Refer to [kernel EFI stub doc](https://docs.kernel.org/admin-guide/efi-stub.html) for more information.

The kernel.efi will be loaded and executed by UEFI bios, just like elf binary loaded and executed by linux kernel. It is in PE file format, and its entry point is set to efi_pe_entry, which is in libefistub.a. efi_pe_entry will call back to decompress_kernel function in string.c, which just copy kernel.bin to its linked address.

## TODO

To make things simple, the decompress process is not integrated yet.
