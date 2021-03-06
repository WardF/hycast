@mainpage Hycast Package

@section contents Table of Contents
- \ref sysconfig

<hr>

@section sysconfig Operating-System Configuration

The following items are necessary in order for a program to send and receive
multicast UDP packets:
- The local host must accept packets whose destination IP address is _not_ that of the
  host:
  - RHEL 7: In the directory `/etc/sysctl.d/`, ensure that the following
    parameters have the indicated values:

        net.ipv4.conf.default.rp_filter = 2
        net.ipv4.conf.all.rp_filter = 2

   (The second entry might not be necessary.) If the values are new, then the
   command `sysctl -p` must be executed.

- A mapping must exist between multicast IP addresses and a network interface:
  - RHEL 6 & 7: The command `ip route` will show the mappings. A default mapping can
    be added via the command
    <blockquote><code>
	route add -net 240.0.0.0 netmask 240.0.0.0 dev <em>iface</em>
    </code></blockquote>
    where _`iface`_ will be the default multicast interface (e.g., `eth0`).

- Any firewall on the host mustn't interfere:
  - iptables(8): This firewall can be completely disabled via the command
    `iptables -F`.