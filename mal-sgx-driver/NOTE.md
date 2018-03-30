Modified SGX Driver for Branch Shadowing
================================================================================
Sangho Lee <sangho@gatech.edu>

We modified a Linux SGX driver from Intel to carry out the branch shadowing
attack.

A function *isgx_ioctl_branch()* has colliding branch instructions with our
target and we check their branch prediction results with LBR (*enable_lbr*,
*filter_lbr*, *disable_lbr*, *print_lbr*).

We manually inserted a lot of nop instructions for alignment and changed the
base address of an enclave.

NOTE: This PoC code checks the branch prediction results when an enclave is
about to be destroyed (*isgx_ioctl_enclave_destroy*), not every timer interrupt
for simplicy and manageability.
