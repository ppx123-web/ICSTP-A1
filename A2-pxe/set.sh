virsh net-destroy default

echo "
<!--
WARNING: THIS IS AN AUTO-GENERATED FILE. CHANGES TO IT ARE LIKELY TO BE
OVERWRITTEN AND LOST. Changes to this xml configuration should be made using:
  virsh net-edit default
or other application using the libvirt API.
-->

<network>
  <name>default</name>
  <uuid>8df95b25-1517-4e6c-9fc8-24f452e7396f</uuid>
  <forward mode='nat'/>
  <bridge name='virbr0' stp='on' delay='0'/>
  <mac address='52:54:00:75:5d:10'/>
  <ip address='192.168.1.253' netmask='255.255.255.0'>
    <tftp root='/srv/tftp'/>
    <dhcp>
      <range start='192.168.1.1' end='192.168.1.254'/>
      <bootp file='pxelinux.0'/>
    </dhcp>
  </ip>
</network>
" > default.xml

cp default.xml /etc/libvirt/qemu/networks/default.xml
virsh net-start default
