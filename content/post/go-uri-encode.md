+++
title = "Go 笔记: URI Encode/Decode"
date = 2014-06-20T21:20:12+08:00
lastmod = 2019-02-12T14:34:02+08:00
tags = ["go", "uri"]
categories = ["NOTE"]
draft = false
+++

最近在处理路径时，要求统一用 `URI` (通用资源标识符) 的格式来表示。偶尔会遇到 URI 被编码的情况，这就需要解码了，故在此把 `Encode/Decode URI` 的方法记录在此，以备后用。

<!--more-->

```go
package utils

import (
        "net/url"
)

/**
 * Input: filepath
 * Output: encode string
 */
func encodeURI (filepath string) string {
        if len(filepath) < 1 {
                return ""
        }

        u := url.URL{}
        u.Path = filepath
        return u.String()
}

/**
 * Input: URI string
 * Output: filepath
 */
func decodeURI (uri string) string {
        if len(uri) < 1 {
                return ""
        }

        u, err := url.Parse(uri)
        if err != nil {
                return ""
        }

        return u.Path
}
```
