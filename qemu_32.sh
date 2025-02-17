qemu_system_arm="/home/m-hamza-khan/Desktop/project_os/qemu/build/qemu-system-arm"

$qemu_system_arm \
-machine  virt \
-cpu cortex-a15  -smp 1 \
-m 4G \
-kernel  /home/m-hamza-khan/Desktop/project_os/build-arm32/tmp/deploy/images/qemuarm/zImage \
-drive file=/home/m-hamza-khan/Desktop/project_os/build-arm32/tmp/deploy/images/qemuarm/image-32-qemuarm.rootfs.ext4,format=raw,id=rootfs,if=virtio \
-append "root=/dev/vda" \
-device pci-mat-dev \
-nographic 