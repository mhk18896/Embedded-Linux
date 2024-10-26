qemu-system-aarch64  \
-machine  virt  \
-cpu cortex-a57 -smp 4  \
-kernel  /home/m-hamza-khan/Desktop/project_os/build/tmp/deploy/images/qemuarm64/Image  \
-drive file=/home/m-hamza-khan/Desktop/project_os/build/tmp/deploy/images/qemuarm64/core-image-minimal-qemuarm64.rootfs.ext4,format=raw  \
-append "root=/dev/vda " \
-nographic \

