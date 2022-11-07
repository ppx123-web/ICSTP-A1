
subnet=192.168.1.0/24
netprefix=192.168.1
gateway=${netprefix}.253
mac=04:ea:56:$(echo $RANDOM | md5sum |sed 's/../&:/g'|cut -c 1-8)

./create.sh $1 ${subnet} ${netprefix} ${gateway} ${mac}

dir=vm-$1
img=${dir}/vm$1.img

cd httpfiles && python -m http.server --bind $gateway 8000 > /dev/null &

pid=$!

if test -z "$(lsmod | grep kvm)"; then 
    qemu-system-x86_64 -m 4G -boot n  -no-reboot    \
    -nic bridge,id=net$1,br=virbr0,mac=${mac}   \
    -drive format=qcow2,file=${img}
else
    qemu-system-x86_64 -enable-kvm -m 4G -boot n -smp 2 -no-reboot   \
    -nic bridge,id=net$1,br=virbr0,mac=${mac}       \
    -drive format=qcow2,file=${img}
fi

