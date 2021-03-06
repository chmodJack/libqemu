./configure \
	--target-list=riscv32-softmmu \
	--extra-cflags=-fPIC\ -Ofast \
	--extra-ldflags=-Ofast \
	--disable-werror \
	--disable-stack-protector \
	--disable-slirp \
	--disable-tcg-interpreter \
	--disable-malloc-trim \
	--with-coroutine=ucontext \
	--disable-blobs \
	--disable-containers \
	--enable-system \
	--disable-user \
	--disable-linux-user \
	--disable-bsd-user \
	--disable-docs \
	--disable-guest-agent \
	--disable-guest-agent-msi \
	--enable-pie \
	--disable-modules \
	--disable-module-upgrades \
	--disable-debug-tcg \
	--disable-debug-info \
	--disable-sparse \
	--disable-gnutls \
	--disable-nettle \
	--disable-gcrypt \
	--disable-auth-pam \
	--disable-sdl \
	--disable-sdl-image \
	--disable-gtk \
	--disable-vte \
	--disable-curses \
	--disable-iconv \
	--disable-vnc \
	--disable-vnc-sasl \
	--disable-vnc-jpeg \
	--disable-vnc-png \
	--disable-cocoa \
	--disable-virtfs \
	--disable-mpath \
	--disable-xen \
	--disable-xen-pci-passthrough \
	--disable-brlapi \
	--disable-curl \
	--disable-membarrier \
	--enable-fdt \
	--disable-kvm \
	--disable-hax \
	--disable-hvf \
	--disable-whpx \
	--disable-rdma \
	--disable-pvrdma \
	--disable-vde \
	--disable-netmap \
	--disable-linux-aio \
	--disable-linux-io-uring \
	--disable-cap-ng \
	--disable-attr \
	--disable-vhost-net \
	--disable-vhost-vsock \
	--disable-vhost-scsi \
	--disable-vhost-crypto \
	--disable-vhost-kernel \
	--disable-vhost-user \
	--disable-spice \
	--disable-rbd \
	--disable-libiscsi \
	--disable-libnfs \
	--disable-smartcard \
	--disable-libusb \
	--disable-live-block-migration \
	--disable-usb-redir \
	--disable-lzo \
	--disable-snappy \
	--disable-bzip2 \
	--disable-lzfse \
	--disable-zstd \
	--disable-seccomp \
	--enable-coroutine-pool \
	--disable-glusterfs \
	--disable-tpm \
	--disable-libssh \
	--disable-numa \
	--disable-libxml2 \
	--disable-tcmalloc \
	--disable-jemalloc \
	--disable-avx2 \
	--disable-avx512f \
	--disable-replication \
	--disable-opengl \
	--disable-virglrenderer \
	--disable-xfsctl \
	--disable-qom-cast-debug \
	--disable-tools \
	--disable-vxhs \
	--disable-bochs \
	--disable-cloop \
	--disable-dmg \
	--disable-qcow1 \
	--disable-vdi \
	--disable-vvfat \
	--disable-qed \
	--disable-parallels \
	--disable-sheepdog \
	--disable-crypto-afalg \
	--disable-capstone \
	--disable-debug-mutex \
	--disable-libpmem \
	--disable-xkbcommon

