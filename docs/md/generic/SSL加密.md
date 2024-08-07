# SSL/TLS加密

## 1. 什么是SSL/TLS

SSL全称是Secure Sockets Layer，安全套接字层，它是由网景公司(Netscape)设计的主要用于Web的安全传输协议，目的是为网络通信提供机密性、认证性及数据完整性保障。如今，SSL已经成为互联网保密通信的工业标准。

SSL最初的几个版本(SSL 1.0、SSL2.0、SSL 3.0)由网景公司设计和维护，从3.1版本开始，SSL协议由因特网工程任务小组(IETF)正式接管，并更名为TLS(Transport Layer Security)，发展至今已有TLS 1.0、TLS1.1、TLS1.2这几个版本。

SSL/TLS协议能够提供的安全目标主要包括如下几个：
- 认证性——借助数字证书认证服务器端和客户端身份，防止身份伪造
- 机密性——借助加密防止第三方窃听
- 完整性——借助消息认证码(MAC)保障数据完整性，防止消息篡改
- 重放保护——通过使用隐式序列号防止重放攻击

为了实现这些安全目标，SSL/TLS协议被设计为一个两阶段协议，分为握手阶段和应用阶段：
- 握手阶段也称协商阶段，在这一阶段，客户端和服务器端会认证对方身份(依赖于PKI体系，利用数字证书进行身份认证)，并协商通信中使用的安全参数、密码套件以及MasterSecret。后续通信使用的所有密钥都是通过MasterSecret生成。
- 在握手阶段完成后，进入应用阶段。在应用阶段通信双方使用握手阶段协商好的密钥进行安全通信。

SSL/TLS协议有一个高度模块化的架构，分为很多子协议，如下图所示：
![](https://hl1998-1255562705.cos.ap-shanghai.myqcloud.com/Img/20240608165727.png)
- Handshake协议：包括协商安全参数和密码套件、服务器身份认证(客户端身份认证可选)、密钥交换;
- ChangeCipherSpec 协议：一条消息表明握手协议已经完成;
- Alert 协议：对握手协议中一些异常的错误提醒，分为fatal和warning两个级别，fatal类型的错误会直接中断SSL链接，而warning级别的错误SSL链接仍可继续，只是会给出错误警告;
- Record 协议：包括对消息的分段、压缩、消息认证和完整性保护、加密等。

## 2. 协议流程

## 3. SSL与TLS区别

## 4. 常用的库