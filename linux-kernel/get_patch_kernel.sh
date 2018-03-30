#!/bin/bash

wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.23.tar.xz
tar xvf linux-4.4.23.tar.xz
cd linux-4.4.23/arch/x86/kernel/apic
patch -p5 < ../../../../../apic.c.patch
