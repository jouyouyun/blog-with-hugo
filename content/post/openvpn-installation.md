+++
title = "OpenVPN Installation"
date = 2016-12-27T21:38:21+08:00
lastmod = 2019-02-12T14:35:40+08:00
tags = ["openvpn"]
categories = ["BLOG"]
draft = false
+++

## 安装所需软件 {#安装所需软件}

Arch: `yaourt -S openvpn easy-rsa`


## 生成证书 {#生成证书}


### Server 端 {#server-端}

-   Copy template file

```shell
sudo mkdir -p /etc/openvpn
sudo cp -R /etc/easy-rsa /etc/openvpn
```

-   Config vars

取消并修改以下项:

<!--more-->

```shell
set_var EASYRSA_REQ_COUNTRY     "CN"
set_var EASYRSA_REQ_PROVINCE    "Hongkong"
set_var EASYRSA_REQ_CITY        "Hongkong"
set_var EASYRSA_REQ_ORG         "jouyouyun.info"
set_var EASYRSA_REQ_EMAIL       "wen@jouyouyun.iofn"
set_var EASYRSA_REQ_OU          "Jouyouyun OpenVPN"
```

-   创建根证书

`ca` 证书需要输入密码，这个密码是给服务器端和客户端签名时用的

```shell
easyrsa init-pki
easyrsa build-ca
```

-   创建并签名服务器端证书

```shell
easyrsa gen-req <server name> nopass
easyrsa sign server <server name>
```

-   创建Diffie-Hellman证书

该证书主要作用是确保共享KEY安全穿越不安全网络

```shell
easyrsa gen-dh
```

-   创建并签名客户端证书

```shell
easyrsa gen-req <client name> nopass
easyrsa sign client <client name>
```


## 配置服务器端 {#配置服务器端}

复制一份模板文件(`/usr/share/openvpn/examples/server.conf`)到 `/etc/openvpn` 目录, 然后开始修改相关项.

然后将证书文件放在 `/etc/openvpn` 目录下, 需要的文件包括:

```shell
easy-rsa/pki/ca.crt
easy-rsa/pki/dh.pem
easy-rsa/pki/issued/<server name>.crt
easy-rsa/pki/private/<server name>.key
```


## 配置客户端 {#配置客户端}

复制一份模板文件 `/usr/share/openvpn/examples/client.conf`, 然后开始修改相关项.

将以下证书文件与配置文件放在一起, 需要的文件包括:

```shell
easy-rsa/pki/ca.crt
easy-rsa/pki/issued/<client name>.crt
easy-rsa/pki/private/<client name>.key
```


## 开启路由转发 {#开启路由转发}

```shell
sed -i '/net.ipv4.ip_forward/s/0/1/' /etc/sysctl.conf
sysctl -p
# 允许vpn客户端所在网段流量转发到其它网卡
iptables -I FORWARD -s 10.8.0.0/24 -j ACCEPT
# 将vpn客户端的流量转到eth0，允许vpn客户端上网，即NAT
iptables -t nat -A POSTROUTING -s 10.8.0.0/24 -o eth0 -j  MASQUERADE
```


## 示例 {#示例}

认证可以通过证书认证也可以使用用户名密码认证，推荐使用用户名密码认证, 这样方便添加用户.


### 证书认证 {#证书认证}

-   服务端

```shell
port 1194
proto tcp
dev tun

ca /etc/openvpn/ca.crt
cert /etc/openvpn/server.crt
key /etc/openvpn/server.key
dh /etc/openvpn/dh.pem

server 10.8.0.0 255.255.255.0
ifconfig-pool-persist /etc/openvpn/ipp.txt
push "redirect-gateway def1 bypass-dhcp"
push "dhcp-option DNS 8.8.8.8"
push "dhcp-option DNS 8.8.4.4"
client-to-client
keepalive 10 120
cipher AES-256-CBC
;comp-lzo # 禁用压缩，如果开启客户端配置中也需要开启
max-clients 100
persist-key
persist-tun

status /var/log/openvpn/openvpn-status.log
log         /var/log/openvpn/openvpn.log
log-append  /var/log/openvpn/openvpn.log
verb 3
```

-   客户端

```shell
client
dev tun
proto tcp

resolv-retry infinite
remote <your vps ip> 1194
nobind

persist-key
persist-tun

ca ca.crt
cert client.crt
key client.key

cipher AES-256-CBC
#comp-lzo
verb 3
```


### 用户名密码认证 {#用户名密码认证}

需要加入 `auth-user-pass-verify` ，开启用户密码脚本, 脚本示例, 读取 `/etc/openvpn/passwd` 文件:

```shell
#!/bin/sh
###########################################################
# checkpsw.sh (C) 2004 Mathias Sundman <mathias@openvpn.se>
#
# This script will authenticate OpenVPN users against
# a plain text file. The passfile should simply contain
# one row per user with the username first followed by
# one or more space(s) or tab(s) and then the password.

PASSFILE="/etc/openvpn/passwd"
LOG_FILE="/var/log/openvpn/openvpn-password.log"
TIME_STAMP=`date "+%Y-%m-%d %T"`

###########################################################

if [ ! -r "${PASSFILE}" ]; then
  echo "${TIME_STAMP}: Could not open password file \"${PASSFILE}\" for reading." >> ${LOG_FILE}
  exit 1
fi

CORRECT_PASSWORD=`awk '!/^;/&&!/^#/&&$1=="'${username}'"{print $2;exit}' ${PASSFILE}`

if [ "${CORRECT_PASSWORD}" = "" ]; then
  echo "${TIME_STAMP}: User does not exist: username=\"${username}\", password=\"${password}\"." >> ${LOG_FILE}
  exit 1
fi

if [ "${password}" = "${CORRECT_PASSWORD}" ]; then
  echo "${TIME_STAMP}: Successful authentication: username=\"${username}\"." >> ${LOG_FILE}
  exit 0
fi

echo "${TIME_STAMP}: Incorrect password: username=\"${username}\", password=\"${password}\"." >> ${LOG_FILE}
exit 1
```

-   服务端

```shell
port 1194
proto tcp
dev tap

#不要求客户端有证书
client-cert-not-required
username-as-common-name

script-security 3 system
#使用脚本验证密码
auth-user-pass-verify /etc/openvpn/checkpsw.sh via-env

ca   /etc/openvpn/keys/ca.crt
cert /etc/openvpn/keys/server.crt
key  /etc/openvpn/keys/server.key
dh   /etc/openvpn/keys/dh1024.pem

server 10.8.6.0 255.255.255.0

#保存已有的用户和ip的对应关系
ifconfig-pool-persist ipp.txt

#允许客户端之间互访
client-to-client

keepalive 10 120

user nobody
group nogroup

persist-key
persist-tun

#保存日志
status openvpn-status.log
#日志冗余级别
verb 3
```

-   客户端

客户端配置文件中去掉于证书相关的配置，加入 `auth-user-pass` 打开用户名密码验证.
可以加入 `auth-nocache` 可以在断线后防止内存中保存用户名和密码来提高安全性。


## 参考 {#参考}

-   [Linux 下OpenVPN 密钥认证 和 用户名/密码认证 笔记](http://blog.chinaunix.net/uid-24250828-id-3536671.html)
