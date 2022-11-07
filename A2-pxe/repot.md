# PXE 实验报告

赵超懿 191870271

## 整体流程

1. 准备一个netboot的文件夹，在ubuntu的官方网站中，22.04不再提供mini.iso,所以需要根据[官网教程](https://discourse.ubuntu.com/t/netbooting-the-live-server-installer/14510) 制作启动文件。
2. 在有了DHCP和TFTP后即可以顺利启动，按照第一步中的连接添加内核参数。自动安装user-data同样在[官网](https://ubuntu.com/server/docs/install/autoinstall-reference)说明通过一次手动安装得到。具体参数的修改也有说明。
3. 实现实验的要求，对user-data的参数进行修改
4. 进行测试

总结，实验还是很折磨的。

我采用了Qemu/kvm作为虚拟机，使用网桥搭建子网

## 遇到的重要问题

### 网络

我尝试了多种网络结构。

首先使用qemu的user模式，该模式可以指定tftp和自带DHCP，对于自动启动安装十分方便，但是不能实现组网。看手册时没有认真看，浪费了不少时间。最后使用bridge模式，根据手册，该模式创建一个tap并接到网桥上，开启ip转发后配置iptable即可。这里我没有自己创建网桥，而是借用了libvirt的default网桥，通过修改ip，dhcp tftp bootp等参数，方便的实现了dhcp，tftp的启动。

网络后续遇到一个令人难以理解的问题，在上述的网络结构下，虚拟机之间无法互相连接。根本问题在于qemu创建虚拟机不会使用随机mac，会导致具有相同mac的vm之间无法路由。这个问题困扰了我很久。

### 实现配置统一ssh的公钥和私钥

这里被ubuntu给坑了。

开始我世界使用late-command直接通过http导入，但是由于在late-command的阶段，user的文件夹并未创建，于是我直接在此处创建并导入密钥。这会导致密钥以及用户文件的权限非常混乱，需要chown和chmod修改。（感觉调整好是可以的，但是这里我太混乱了，没往下弄）

然后我去查了cloud-init的文档，发现可以使用user-data中的选项来解决，按照手册配置，结果只导入了authoried_key，后续尝试了write_files,runcmd都没有成功，这里不清楚原因（ubuntu官网没有详细说明，只说使用了cloud-init）。（感觉是ubuntu自己弄了一个配置，但是又用了cloud-init，配置之间有相互冲突，但是没有说明）

最后使用systemd来实现这件事，创建一个脚本，在late-command阶段配置好，然后在开机执行后自我删除，解决了pssh的问题。


## 可以改进的地方

网络：这告诉我直接使用libvir就没这么多事。手动配置确实踩了很多坑，也还了计网欠下的债。其实在开始就应该想清楚这些的。有时候封装好的高级工具很好用，比如libvirt，直接使用会很简单。（参数比qemu要简单很多）

配置ssh：在配置的时候，我做的时候混乱了，应该遵循找到问题——寻找方法——解决问题，但是我思路不太清晰。

http输出机器序号：我偷懒将机器的hostname设为机器号，访问时返回即可。

在实验后，对虚拟机的网络，ssh等的理解更加深入。

我的做法存在的问题，并行安装需要等待前一个虚拟机读取配置后再开始下一个安装（只能串行化），如果想要并行，需要在安装时使用qemu的user模式，需要全部安装好后才能启动联网。两种方法各有缺点

做之前没有想清楚，导致遇到问题后思路混乱，浪费了很多时间。

## 脚本使用

请在使用前先安装libvirt,使用sudo运行set.sh，安装好虚拟机第一次启动时请确保在安装时启动http仍在工作。并且不要并行安装，同时将ubuntu的镜像和rsa的key放到httpfiles文件夹中,将能够启动的netboot从httpfiles tftp文件夹下移到tftp-root目录
