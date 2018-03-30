# The branch shadowing attack
*Branch shadowing* is a side-channel attack on Intel Software Guard Extensions
(SGX) that can reveal fine-grained control flows (branch granularity) in an SGX
enclave. This attack is possible because SGX does not clear branch history when
switching from enclave to non-enclave mode, leaving fine-grained branch
direction traces for the outside world to observe, resulting in an accurate
branch-prediction-based side channel.

This repository contains *partial* proof-of-concept code snippets of the branch
shadowing attack, including
 1. A modified Linux SGX driver (*mal-sgx-driver*) to execute and monitor
colliding branch instructions with Last Branch Record (LBR) trace
 2. Kernel patch (*linux-kernel*) and Local Advanced Programmable Interrupt
Controller (LAPIC) module (*lapic-freq-module*) to manipulate timer interrupt
frequency
 3. A kernel module to disable L1/L2 cache (*disable-cache-module*)
 
## More details
* Paper (**USENIX Security 2017**):
  https://gts3.org/assets/papers/2017/lee:sgx-branch-shadow.pdf
* Slides: https://gts3.org/assets/papers/2017/lee:sgx-branch-shadow-slides.pdf
* Talk: https://www.youtube.com/watch?v=XyvZDIIeLZI

## Demo
### Attacking RSA exponentiation
https://www.youtube.com/watch?v=jf9PanlF374

## Note
We **DO NOT** release an end-to-end, fully automated attack and our target
program. This is because, currently, our attack needs a lot of manual efforts
especially for aligning target and attack branch instructions. However, it does
not mean that our attack cannot be automated; with (a lot of) engineering
efforts, we believe that our attack can be fully automated.

We have a plan to release a fully-automated version of the branch shadowing
attack, but it will not be released in the near future.

Also, the current code attacks target branch instructions when an enclave
terminates for simplicity and manageability. Revising the code to attack
targets for every timer interrupt is an easy task.

## Contributors
* [Sangho Lee]
* [Ming-Wei Shih] 
* Prasun Gera
* [Taesoo Kim]
* [Hyesoon Kim] 
* [Marcus Peinado]

[Sangho Lee]: <http://www.cc.gatech.edu/~slee3036>
[Ming-Wei Shih]: <https://www.cc.gatech.edu/~mshih6/>
[Taesoo Kim]: <https://taesoo.kim/>
[Hyesoon Kim]: <https://www.cc.gatech.edu/~hyesoon/>
[Marcus Peinado]: <https://www.microsoft.com/en-us/research/people/marcuspe/>
