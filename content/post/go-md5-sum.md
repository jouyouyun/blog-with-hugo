+++
title = "Go 笔记: md5 sum"
author = ["Wen"]
date = 2014-06-22T22:54:38+08:00
lastmod = 2019-02-09T21:39:06+08:00
tags = ["go", "md5"]
categories = ["NOTE"]
draft = false
+++

使用 Go 语言实现字符串与文件的 MD5 值计算，但是大文件不要使用此方法。

```go
package utils

import (
        "crypto/md5"
        "io/ioutil"
        "strconv"
)

const (
        MD5_STR_LEN = 32
)

func md5ByteToStr(bytes [16]byte) string {
        str := ""

        for _, b := range bytes {
                s := strconv.FormatInt(int64(b), 16)
                if len(s) == 1 {
                        str += "0" + s
                } else {
                        str += s
                }
        }

        return str
}

func SumStrMd5(str string) (string, bool) {
        if len(str) < 1 {
                return "", false
        }

        md5Byte := md5.Sum([]byte(str))
        md5Str := md5ByteToStr(md5Byte)
        if len(md5Str) < MD5_STR_LEN {
                return "", false
        }

        return md5Str, true
}

func SumFileMd5(filename string) (string, bool) {
        if !IsFileExist(filename) {
                return "", false
        }

        contents, err := ioutil.ReadFile(filename)
        if err != nil {
                return "", false
        }

        md5Byte := md5.Sum(contents)
        md5Str := md5ByteToStr(md5Byte)
        if len(md5Str) < MD5_STR_LEN {
                return "", false
        }

        return md5Str, true
}
```
