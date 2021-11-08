# ff
License: **GPL-3.0-or-later**  

A Linux Kernel Module opposite of /dev/zero works with setting all binary 0's to 1. 
 
## Example Usage
```bash
dd if=/dev/ff of=test.bin bs=1024 count=1
```

## Details 
Run make in the lkm directory to build the kernel object.
```bash
make
```

Insert the kernel object into the kernel with: 
```bash
sudo insmod ./ff.ko
```

When you want to, remove the kernel object from the kernel with:
```bash
sudo rmmod ff
```

You need linux-headers to build the kernel object. 
You might need to disable secure boot in order for you to insert the kernel object into the kernel. 
You might want to sign the kernel object to bypass disabling secure boot.
