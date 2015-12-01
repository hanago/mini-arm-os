Build a minimal multi-tasking OS kernel for ARM from scratch

Prerequisites
-------------
- [QEMU with an STM32 microcontroller implementation](http://beckus.github.io/qemu_stm32/)
  - Build instructions
```
./configure --disable-werror --enable-debug \
    --target-list="arm-softmmu" \
    --extra-cflags=-DSTM32_UART_NO_BAUD_DELAY \
    --extra-cflags=-DSTM32_UART_ENABLE_OVERRUN \
    --disable-gtk
make
```
- [GNU Toolchain for ARM](https://launchpad.net/gcc-arm-embedded)
- Set `$PATH` accordingly

Steps
-----
* `00-Semihosting`
  - Minimal semihosted ARM Cortex-M "Hello World"
* `00-HelloWorld`
  - Enable STM32 USART to print trivial greetings
* `01-HelloWorld`
  - Almost identical to the previous one but improved startup routines
* `02-ContextSwitch-1`
  - Basic switch between user and kernel mode
* `03-ContextSwitch-2`
  - system call is introduced as an effective method to return to kernel
* `04-Multitasking`
  - Two user tasks are interatively switching
* `05-TimerInterrupt`
  - Enable SysTick for future scheduler implementation
* `06-Preemptive`
  - Basic preemptive scheduling
* `07-Threads`
  - Implement user-level threads

Building and Verification
-------------------------
* Changes the current working directory to the specified one and then
```
make
make qemu
```

* You can use genromfs to generate rom file system image.
```
http://romfs.sourceforge.net/
```

Licensing
---------
`mini-arm-os` is freely redistributable under the two-clause BSD License.
Use of this source code is governed by a BSD-style license that can be found
in the `LICENSE` file.

Reference
---------
* [Baking Pi – Operating Systems Development](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/)
* [romfs information](http://romfs.sourceforge.net/)