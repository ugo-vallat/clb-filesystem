# A tiny educational kernel for Raspberry Pi 3
A tiny kernel *incrementally built* for OS education.
Start with minimal, baremetal code. Then add kernel features in small doses.
Each experiment is self-contained and can run on both QEMU and real Raspberry Pi 3 hardware.

## Helpful documentation and tutorials
* [AArch64 cheatsheet](aarch64-cheatsheet.md)
  * [ARMv8 Instruction Set Overview](https://www.cs.princeton.edu/courses/archive/spr21/cos217/reading/ArmInstructionSetOverview.pdf)
  * [AArch64 Full Beginner's Assembly Tutorial](https://mariokartwii.com/armv8/)
  * [Official AArch64 ISA doc](https://developer.arm.com/documentation/ddi0602/latest/)
  * [Official AArch64 registers doc](https://developer.arm.com/documentation/ddi0601/latest/)
  * [Official AArch64/Aarch32 address translation doc](https://developer.arm.com/documentation/100940/latest/)
* [QEMU cheetsheet](qemu.md)
* [Using GDB to debug kernel](gdb.md)
* [Inspect kernel binary](inspect-kernel-binary.md)
* [Official Raspberry Pi boot sequence documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#boot-sequence)
* [Explications sur les OS](https://wiki.osdev.org/Introduction)
* [OS xv6](https://github.com/mit-pdos/xv6-riscv)
* [Filesystem xv6](https://pekopeko11.sakura.ne.jp/unix_v6/xv6-book/en/File_system.html)
* [Dépôt d'origine](https://github.com/mpoquet/raspberry-pi-os-qemu)

## Experiments
1. **Filesystem / VFS / RAMF**
      * [explication générales](filesystem/filesystem.md)
2. **Implémentation du VFS**
      * [Syscall et vfs](filesystem/vfs.md)
3. **Implémentation du filesystem**
      * [Inodes](filesystem/fs.md)

## Acknowledgements
1. Sergey Matyukevich, Alexandre Venito et al. made the educational kernel and most of the resources in https://github.com/s-matyukevich/raspberry-pi-os
2. Felix Xiaozhu Lin et al. adapted the codebase to make it work on QEMU and improved the content in https://github.com/fxlin/p1-kernel
3. Millian Poquet adapted the codebase for his operating system design course in https://github.com/mpoquet/raspberry-pi-os-qemu


/
├── bin
│   └── ls
└── usr
    └── tom
        ├── file1
        ├── file2
        └── file3
