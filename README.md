rshell
====
this is a basic command shell. It displays the username followed by a `$` prompt.


hw3
===
Install Instructions for rshell
====
```
    $git clone https://github.com/antonvinod/rshell.git
    $cd rshell
    $git checkout hw3
    $make
    $bin/rshell
```

bin/rshell will allow you to run and test the rshell for hw3.

rshell - limitations
===
* for `cd` and `^c` you need to hit enter again. The rshell program in still running though, so the bug is not too severe.
*program doesn't run with connectors and io redirection/piping
* program has issues with flags `ls-a` and commands with arguments `echo hello`.


