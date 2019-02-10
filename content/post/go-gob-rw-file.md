+++
title = "Go 笔记: Gob Write/Read File"
date = 2014-07-07T13:06:21+08:00
lastmod = 2019-02-10T17:53:25+08:00
tags = ["go", "gob"]
categories = ["NOTE"]
draft = false
+++

`Gob` 使用实例，将结构体写入文件里，在需要的时候再从文件中读取出来。

```go
package main

import (
        "bytes"
        "encoding/gob"
        "fmt"
        "io/ioutil"
        "os"
)

type Writer struct {
        Id    int
        Name  string
        Value string
}

type Reader struct {
        Name  string
        Value string
}

func writeToFile(filename string, data interface{}) bool {
        if len(filename) < 1 {
                fmt.Println("writeToFile filename invalid")
                return false
        }

        var w bytes.Buffer

        enc := gob.NewEncoder(&w)
        if err := enc.Encode(data); err != nil {
                fmt.Printf("Encode data error: %v\n", err)
                return false
        }

        if fp, err := os.Create(filename); err != nil {
                fmt.Printf("Open '%s' error: %v\n", filename, err)
                return false
        } else {
                defer fp.Close()
                fp.WriteString(w.String())
                fp.Sync()
        }

        return true
}

func readFromFile(filename string, data interface{}) {
        if len(filename) < 1 || data == nil {
                fmt.Println("readFromFile args invalid")
                return
        }

        contents, err := ioutil.ReadFile(filename)
        if err != nil {
                fmt.Printf("ReadFile '%s' error: %v\n", filename, err)
                return
        }

        r := bytes.NewBuffer(contents)
        dec := gob.NewDecoder(r)

        if err := dec.Decode(data); err != nil {
                fmt.Printf("Decode error: %v\n", err)
                return
        }

        return
}

func main() {
        wData := Writer{0, "firefox", "Web Broswer"}

        writeToFile("/tmp/w", &wData)

        rData := Reader{}
        readFromFile("/tmp/w", &rData)
        fmt.Printf("%v\n", rData)

        // test map
        w := map[string]string{
                "sublime-text": "Sublime Text",
                "firefox":      "Web Broswer",
                "terminator":   "Terminator",
        }
        writeToFile("/tmp/map", &w)
        r := make(map[string]string)
        readFromFile("/tmp/map", &r)
        fmt.Printf("%v\n", r)
}
```
