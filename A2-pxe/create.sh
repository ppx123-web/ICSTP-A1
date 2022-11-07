#! /bin/bash

subnet=$2
netprefix=$3
gateway=$4
ip=${netprefix}.$1
mac=$5

vmimg=vm$1.img
dir=vm-$1
img=vm$1.img

mkdir ${dir}
touch ${dir}/run.sh

echo "
if test -z \"\$(lsmod | grep kvm)\"; then 
    qemu-system-x86_64 -m 2G \\
    -nic bridge,id=net$1,br=virbr0,mac=${mac} \\
    -drive format=qcow2,file=${img}
else
    qemu-system-x86_64 -enable-kvm -m 2G \\
    -nic bridge,id=net$1,br=virbr0,mac=${mac} \\
    -drive format=qcow2,file=${img}
fi" > ${dir}/run.sh

chmod +x ${dir}/run.sh

qemu-img create -f qcow2 ${dir}/${img} 10G

echo "
#cloud-config
autoinstall:
  apt:
    disable_components: []
    geoip: true
    preserve_sources_list: false
    primary:
    - arches:
      - amd64
      - i386
      uri: http://mirrors.nju.edu.cn/ubuntu
    - arches:
      - default
      uri: http://ports.ubuntu.com/ubuntu-ports
  drivers:
    install: false
  kernel:
    package: linux-generic
  keyboard:
    layout: us
    toggle: null
    variant: ''
  locale: en_US.UTF-8
  network:
    ethernets:
      ens3:
        addresses:
        - $ip/24
        critical: true
        dhcp-identifier: mac
        gateway4: $gateway
        nameservers:
          addresses:
          - $gateway
          - $gateway
          search:
          - nju.edu.cn
    version: 2
  packages:
    - vim
    - pip
    - python3
    - openssh-server
  late-commands:
    - echo $1 > /target/etc/hostname
    - curtin in-target --target=/target -- apt update
    - curtin in-target --target=/target -- pip install rich
    - curtin in-target --target=/target -- systemctl enable ssh
    - echo StrictHostKeyChecking no >> /target/etc/ssh/ssh_config
    - wget http://192.168.1.253:8000/web.py -O /target/web.py
    - wget http://192.168.1.253:8000/pythonport.service -O /pythonport.service
    - cp /pythonport.service /target/usr/lib/systemd/system/pythonport.service
    - curtin in-target --target=/target -- systemctl enable pythonport.service
    - wget http://192.168.1.253:8000/sshkeys.sh -O /target/sshkeys.sh
    - wget http://192.168.1.253:8000/sshkeys.service -O /sshkeys.service
    - cp /sshkeys.service /target/usr/lib/systemd/system/sshkeys.service
    - curtin in-target --target=/target -- systemctl enable sshkeys.service
  user-data:
    users:
      - default
      - name : user
        sudo : ALL=(ALL) NOPASSWD:ALL
        shell: /bin/bash
        groups: sudo, users, admin
        passwd: \$6\$7/X8ZOE7c07GlSsu\$GkHyvtvTT4cZgzX6ZnDQP6SLcJEbWkgbqVMWGgVqsv6CueEXy9dqFR2To9gaDjJh3dGK2tV41iR8DfKzPuB8s/
        lock_passwd: false
        ssh_pwauth: false
        ssh_authorized_keys:
          - ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQComqtSAnn3sgUoJDGHWLItJ4Z0oravY1Fhpucjkq3TA5UiKkUIe3EEVnnfTVYxoPwsQZi26aTMN5ygPLum2jInWqcTsEPL76n2IzJOgU9dM5NIrYyniDt/hobM7oNNtTyRiM1ST0T6QH8ZY0jM77FPzTyxigjCjvqxN/BRBqD50L8YhwwlPSWJPtnJ49BeLnz0/GXoAcMXYQObKGmNLM1LE5HfuOZRoxg0HdI7J0Y/B4IWR0dqKJFWa1GCU8lmKDwJDoE2PITo6nitd+Ky+57om+Eti6QuzeZKY1k+o5vMD8wEXQw/yDF5f9VxLLlE4tpS/scAMiU1spxAwIosrWmHWX2ZsLPJzVArWe4uL6SJZvmeOfEr7+0TiqyHt1sl+LjaUHgmR97eXNUfooRi+MdrWDk4PKcPg19lPuxKfqC+uCaI2aq9p2M/cU4teR9Nyyjw7KFn2Fa4r/pW4Ilr2WxP5NgIoVRPnxTfQyZ9TmmTM/kiAxZGukmv3gMcDed6TjU= ppx@zcy
        ssh_keys:
          rsa_private: |
            -----BEGIN OPENSSH PRIVATE KEY-----
            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
            IV5pO1PgSFxfUAAAAHcHB4QHpjeQECAwQ=
            -----END OPENSSH PRIVATE KEY-----

          rsa_public: AAAAB3NzaC1yc2EAAAADAQABAAABgQComqtSAnn3sgUoJDGHWLItJ4Z0oravY1Fhpucjkq3TA5UiKkUIe3EEVnnfTVYxoPwsQZi26aTMN5ygPLum2jInWqcTsEPL76n2IzJOgU9dM5NIrYyniDt/hobM7oNNtTyRiM1ST0T6QH8ZY0jM77FPzTyxigjCjvqxN/BRBqD50L8YhwwlPSWJPtnJ49BeLnz0/GXoAcMXYQObKGmNLM1LE5HfuOZRoxg0HdI7J0Y/B4IWR0dqKJFWa1GCU8lmKDwJDoE2PITo6nitd+Ky+57om+Eti6QuzeZKY1k+o5vMD8wEXQw/yDF5f9VxLLlE4tpS/scAMiU1spxAwIosrWmHWX2ZsLPJzVArWe4uL6SJZvmeOfEr7+0TiqyHt1sl+LjaUHgmR97eXNUfooRi+MdrWDk4PKcPg19lPuxKfqC+uCaI2aq9p2M/cU4teR9Nyyjw7KFn2Fa4r/pW4Ilr2WxP5NgIoVRPnxTfQyZ9TmmTM/kiAxZGukmv3gMcDed6TjU= ppx@zcy
        no_ssh_fingerprints: true

  version: 1
" > httpfiles/user-data

