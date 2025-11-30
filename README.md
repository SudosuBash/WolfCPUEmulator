## Wolf CPU Emulator
这个项目是一个<strong>模拟器</strong>，旨在实现自己一直想做的Wolf CPU.

目前算是1/2成品(代码基本上写完了，但是一堆功能需要验证，也就是一堆bug没修)

toyOS 日后将会基于此模拟器进行彻底重写。
# 构建
在附带 GNU 工具的系统上(包括Windows)，执行以下shell命令
```shell
cd WolfCPUEmulator
make
```
即可在根目录下生成一个名为 <strong>emulator</strong> 的可执行文件。

# 目前支持的选项
```shell
emu.exe -lb {bios file} # 加载bios文件
        -dbg "on" # 是否开启Debug模式
        -help "help" # 加载帮助文档
```

# 注意:给研究者/源码阅读者的一点说明
由于项目日后会考虑迁移到 Turing Complete(首选) 或者 Verilog, logisim 等平台中，所以可能有些代码可能会显得很奇怪和刻意.<br>
如果在阅读源码的过程中觉得有些代码很奇怪的话，不用觉得奇怪，这很正常。
