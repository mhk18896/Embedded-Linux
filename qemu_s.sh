 qemu-system-aarch64 \
-machine  virt  \
-cpu cortex-a57 -smp 4  \
-m 4096 \
-kernel  /home/m-hamza-khan/Desktop/project_os/build-cortexa57/tmp/deploy/images/qemuarm64/Image  \
-drive file=/home/m-hamza-khan/Desktop/project_os/build-cortexa57/tmp/deploy/images/qemuarm64/my-image-qemuarm64.rootfs.ext4,format=raw  \
-append "root=/dev/vda " \
-device pci-echodev \
-nic user,hostfwd=tcp::2222-:22 \
-nographic