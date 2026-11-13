# ResearchTopic-UninitiliazedMemory
My research into buffer overflows and memory vulnerabilities, specifically targeting the risks of accessing uninitialized memory.


Uninitialized memory access is an important topic in both the realms of software development and cybersecurity. For the software developer, it can present both bugs and errors. However, for the cybersecurity specialist, it can present a risk to all users of the internet, allowing hackers access to users' delicate information (as was the case in the Heartbleed bug).

Within this topic, I researched information disclosure and memory leaks, undefined behavior and compiler optimizations, alignment and trap faults, and Heisenbugs.


## 1. Security (the most interesting discoveries)
Within online networks, a user attaining access to unauthorized memory is incredibly dangerous. Uninitialized memory reads provide this access. For example, back in 2014, the Heartbleed bug within the very popular OpenSSL came to light. OpenSSL is an online cryptographic software library used to secure communications on online computer networks. It was made to both encode data in transit and use the "heartbeat" extension of TLS (Transport Layer Security) and SSL (Security Sockets Layer) to correctly identify the receiver of all data. The "heartbeat" worked by having the user's computer send a "payload" (a message) and the size of the payload to the server. The server would then echo this message back; however, it would always send back the message by copying the number of bytes the user said the message was. The server never did a safety check to ensure the message actually matched the stated size. So, hackers could lie to the server and send something like "'Apple', size 1000 bytes" and the server would echo back "Apple" and roughly 995 bytes of uninitialized memory. This memory contained data such as usernames, passwords, tokens, and more. Furthermore, there are still segments of the internet that utilize the broken version of OpenSSL, allowing the Heartbleed bug to persist

And this is not the only case of uninitialized memory access threatening the safety of internet users, there is also the case of ASLR (Address Space Layout Randomization). Computers have their memory laid out like neighborhoods. Older computer layouts would always place the active program at the same street address (House #1) and the Standard C library functions at the same street address (House #500). So, a hacker could use the jump from the program to the library functions, gain access to `printf`, and run malicious code. So, ASLR was adopted to randomize the street address. However, the active program and the library functions were still always the same offset away from each other. So, if a hacker could figure out what memory address the program is running from, they could render ASLR useless and find `printf` anyway. Reading from uninitialized memory is one of the ways that they could do this. ASLR, though, is relatively safe and useful. Utilized by nearly every modern computer and operating system, online networks prevent remote code execution to protect users from a hacker gaining access through an info-leak revealing the randomized layout.


## 2. Compiler
When compiling, a programmer can choose to compile without (-O0) or with (-O2) optimizing. When optimizing, a compiler will prune the code to improve speed and efficiency. To do this, it subscribes to a sort of contract:
"This is valid code, and valid code can never result in undefined behavior."
However, this can result in important safety checks being deleted. If you'd like a specific example, look at example1.c within the example directory.

Additionally, an optimizing compiler does not read uninitialized values as just any random, valid value. It reads them as `undef` or `poison`. In the case of `undef`, the compiler treats the value as a wildcard that it can fill in with whatever value makes the math easiest. So, `if (undef==0)` and `if (undef != 0)` may both end up reduced to just their internals if the compiler replaces `undef` with 0 in the first case and `undef` with any other value in the second case. This is worse in the case of `poison`, where the compiler reduces any comparison like `poison == 0` to just `poison`. In this case, the compiler is effectively given permission to either delete the branch or reduce the code, deleting as much code as possible. 


## 3. Hardware
Uninitialized memory combined with hardware set-up can produce errors and faults in code long before the program is ever finished in development and compiled. Some of these faults we have learned about in class, such as segmentation faults, often caused by undefined behavior, and undefined behavior itself. However, there are more possible errors, such as bus errors and trap representations throwing errors.

Bus errors occur due to improper data alignment. When the CPU reads from memory, it does not just grab one byte, it typically grabs in chunks of four or eight bytes (depending on the size of the data type). Thus, it expects memory addresses to be divisible by 4 or 8. Most memory addresses obviously are not divisible by these values, so most uninitialized pointers point to misaligned addresses. On certain computer architectures with strict alignment protocols, trying to read from these misaligned addresses causes the CPU to trigger an Alignment Fault interrupt and throw a `SIGBUS` (bus error) to kill the program.

Trap representations are a little rarer, as they require the uninitialized value to be a special bit pattern. For example, an uninitialized `float` or `double` may end up being the bit pattern for a signaling NaN. The moment you attempt to use this signaling NaN, the CPU kills this process with a `SIGFPE` (floating point exception). And while these might seem rare, on a server running hundreds of processes, some runs will end up with these sorts of errors.


## 4. Software 
Beyond how the computer's memory is set up on the hardware, there are also errors due to the internal software. For example, the way a specific memory allocator allocates memory may end up giving rise to an error. This can yield bugs like Heisenbugs, which are dependent on the memory layout at the specific moment a line of code runs. Furthermore, these bugs are hard to debug because adding any additional lines of code (like a `printf` line to show a specific variable's value) can adjust the memory just enough that the bug disappears.


Additional Resources Utilized:
https://nordvpn.com/blog/what-is-heartbleed-bug/?srsltid=AYCX9vIk_XZEBQk2aa8MUgrrVD6LFuJIxB9Pa36SBn7qf5SWcyCu6Yy_

https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html

​​https://share.gemini.google/A9Dg8TpQyPtA
