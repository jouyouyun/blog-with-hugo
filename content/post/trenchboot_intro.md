+++
title = "TrenchBoot Intro"
date = 2023-02-07T10:20:21+08:00
lastmod = 2023-02-14T11:11:59+08:00
tags = ["boot", "measure", "integrity", "drtm", "crtm", "srtm"]
categories = ["security"]
draft = false
+++

TrenchBoot 是一个框架，用于让设备管理者选择适合其目标机器环境的安全引擎。目的是为了提升运行环境中固件及软件的安全性，通过测量启动环境来实现，如 DRTM 技术。

TrenchBoot 框架将启动流程分为三个阶段，分别是：

-   引导阶段(Bootstrap)

    Bootstrap 阶段主要是对现已存在的引导技术，包括 UEFI、grub、shim 等，添加完整性功能。这是 TrenchBoot 的主要关注点，称之为 BIT(Boot Integrity Technology) 。
-   中间阶段(Intermediate)

    Intermediate 阶段是 TrenchBoot 抽象出的，用来执行 TrenchBoot Loader ，以此实现系统启动的完整性。

    TrenchBoot Loader 阶段将执行 TrenchBoot 安全引擎，通过 Dynamic Launch CPU 指令进入安全运行环境，然后对前一段阶段收集的 evidence 进行测试，并构建 DRTM(Dynamic Root of Trusted for Measurement) 。
-   运行阶段(Runtime)

    Runtime 阶段根据测量结果和安全策略决定如何启动目标及赋予权限，目标包括管理程序、操作系统等。

<!--more-->


## 安全引擎 {#安全引擎}

TrenchBoot Security Engine 是 TrenchBoot Loader 的核心，安全引擎复杂处理 BIT 收集的 evidence ，以及根据需求搜集新的 evidence 。同时根据安全策略评估所有 evidence ，并执行策略中定义的强制策略。


### Evidence {#evidence}

Evidence 是 TrenchBoot 中的核心概念，是系统启动阶段事件的记录，应是正确的且不可篡改、仿冒。

通常可用 hash 算法表示事件，典型的应用就是 TPM PCR 。


### BIT {#bit}

BIT 是系统引导阶段硬件及软件的一项能力，通过此功能建立一定程度的完整性，即使用正确的逻辑来启动系统。若逻辑错误，则依赖此完整性进行解封的数据则不可用。

BIT 应能直接或间接的扩展启用及增强系统启动过程中使用完整性的范围。

同时 BIT 支持任意 evidence 格式，并能够将 evidence 格式转换为 TrenchBoot 维护的标准化的格式。这些标准化格式的数据将被安全引擎进行处理。


### Security Processor {#security-processor}

TrenchBoot 的另一核心是 Security Processor，安全处理器。安全处理器是可由多个功能组件构成，由这些组件根据策略评估运行环境的状态。

安全处理器可以包括但不限于的功能有收集额外的 evidence、进行远程证明、解封加密密钥、解密文件/块设备/驱动器等。并根据安全策略控制系统的启动方式，如完全启动、维护模式、失败等。

目前可通过 Dynamic Launch CPU 指令实现安全处理器。


### Launch/Hand Off {#launch-hand-off}

TrenchBoot 的目的是控制系统的启动，这部分是用于配置各种启动目标环境的方式。


## DRTM {#drtm}

DRTM 技术是 TrenchBoot 的一种使用方式，其整体架构为：

{{< figure src="/ox-hugo/trenchboot_drtm_arch.png" width="100%" >}}

DRTM 是以 CRTM(Core Root of Trusted for Measurement)/SRTM(Static Root of Trusted for Measurement) 为锚点，以 Grub 作为 TrenchBoot Security Processor 。在 Grub 中调用 Dynamic Launch CPU 指令(详见： [TrenchBoot Late launch](https://trenchboot.org/documentation/Late_Launch_Overview/))进入安全环境，然后对运行环境进行评估，并基于评估结果和安全策略控制系统运行方式及权限。评估通过后将构建以 TPM 为信任根的 DRTM 环境，为系统运行提供信任基础。

目前 TrenchBoot 项目在计划直接从 UEFI Stub 直接启动 kernel ，并支持更多的 CPU 架构。


## 动态度量 {#动态度量}

可信计算中的动态度量关注范围比 DRTM 更广，除了引导阶段外，还关注的是系统运行后的环境安全，功能主要包括动态收取、执行策略以及上报策略执行结果和当前运行状态。

可信度量的技术方案与 DRTM 不同，其在可信芯片、可信主板、可信固件之上构建一个独立的操作系统，实现可信软件基，用以评估系统的启动和运行。


## 参考文档 {#参考文档}

-   [TrenchBoot Official Document](https://trenchboot.org/documentation/)
-   [TrenchBoot Project Description](https://3mdeb.com/wp-content/uploads/2021/06/TrenchBoot.pdf)
-   [How TrenchBoot is enabling Measured Launch for Open-Source Platform Security](https://trenchboot.org/slides/How_TrenchBoot_is_Enabling_Measured_Launch_for_Open-Source_Platform_Security.pdf)
-   [Windows 基于硬件的信任根](https://learn.microsoft.com/zh-cn/windows/security/threat-protection/windows-defender-system-guard/how-hardware-based-root-of-trust-helps-protect-windows)
