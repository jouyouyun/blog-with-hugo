+++
title = "Git Rebase 回退"
author = ["Wen"]
date = 2018-04-12T21:38:21+08:00
lastmod = 2019-02-09T23:08:22+08:00
tags = ["git", "rebase", "reflog", "recovery"]
categories = ["BLOG"]
draft = false
+++

在开发新功能或修复 `bug` 时，一般需要新建一个分支，然后在它之上进行开发。在多人协同开发时，经常会同时修改相同的分支或相同的文件，这时就需要进行 `rebase` 操作在提交你的代码之前。

但有时 `rebase` 会带来不想要的提交，或处理冲突时误操作执行了 `git rebase --skip` 。此时就想要回退到之前的提交，恢复自己的数据，=reflog= 就可以帮助你完成数据恢复。


## reflog {#reflog}

当你工作时，=Git= 会在你每次修改了 `HEAD` 时悄悄地将改动记录下来。当你提交或修改分支时，=reflog= 就会更新。通过 `git reflog` 命令可以查看当前的状态：

```shell
7115aff (HEAD -> master, tag: 3.1.21, origin/master, origin/HEAD) HEAD@{0}: pull: Fast-forward
c0a16ee HEAD@{1}: pull: Fast-forward
abf0bfb (tag: 3.1.20) HEAD@{2}: pull: Fast-forward
c469d2c HEAD@{3}: pull: Fast-forward
9148742 HEAD@{4}: pull: Fast-forward
0dc73dd HEAD@{5}: pull: Fast-forward
4dea91e (gerrit/master) HEAD@{6}: checkout: moving from rename-mousearea to master
0dc73dd HEAD@{7}: reset: moving to 0dc73ddd5f28118c428cfb952a5cfea557239aef
0dc73dd HEAD@{8}: rebase -i (finish): returning to refs/heads/rename-mousearea
0dc73dd HEAD@{9}: rebase -i (pick): updating HEAD
```

找到你提交所在的 `HEAD` 后，如 `HEAD@{8}` ，就可以用命令下面的命令来恢复你的代码了：

`git reset --hard HEAD@{8}`

同样这种方法也适用与其它的 `Git` 误操作。
