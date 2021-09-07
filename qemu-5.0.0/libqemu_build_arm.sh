. ./libqemu_objs_arm.sh
cd arm-softmmu
c++ \
	-O2 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 \
	-L${PWD}/dtc/libfdt -Wl,--warn-common -Wl,-z,relro -Wl,-z,now \
	-pie -m64 -fPIC -Ofast -Ofast \
	-shared -o libqemu-system-arm.so \
	${OBJS} ../libqemuutil.a \
	-lgio-2.0 -lgobject-2.0 -lglib-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 \
	-lpulse -lpixman-1 -lutil -lfdt -Wl,-rpath=. -lz -lgthread-2.0 -pthread -lglib-2.0 -lrt

cp libqemu-system-arm.so ../../lib
cp ../libqemu/libqemu.h ../../include
