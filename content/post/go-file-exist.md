+++
title = "Go 笔记: 判断文件是否存在"
author = ["Wen"]
date = 2014-06-20T21:38:21+08:00
lastmod = 2019-02-09T21:38:54+08:00
tags = ["go", "file"]
categories = ["NOTE"]
draft = false
+++

使用 GoLang 判断文件是否存在。

```go
package utils

import (
        "os"
)

func isFileExist(filename string) bool {
        if len(filename) < 1 {
                return false
        }

        _, err := os.Stat(filename)

        return err == nil || os.IsExist(err)
}
```
