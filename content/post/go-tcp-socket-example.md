+++
title = "Go 笔记: 简单的 TCP Socket 示例"
author = ["Wen"]
date = 2014-07-20T00:54:21+08:00
lastmod = 2019-02-09T22:27:54+08:00
tags = ["go", "tcp", "socket"]
categories = ["NOTE"]
draft = false
+++

一个简单的 `TCP Socket` 的示例，使用 `gob` 处理传输的数据。


## `server.go` {#server-dot-go}

```go
package main

import (
        "bytes"
        "encoding/gob"
        "fmt"
        "io"
        "net"
)

type personInfo struct {
        Name  string
        Age   int32
        Phone string
        Addr  string
}

const (
        BUF_SIZE = 1024
)

func handleConnection(conn *net.TCPConn) {
        if conn == nil {
                return
        }
        defer conn.Close()

        data := make([]byte, BUF_SIZE)
        for {
                n, err := conn.Read(data)
                if err == io.EOF {
                        fmt.Println("Read connection data failed:", err)
                        break
                }
                fmt.Printf("Read %v byte data\n", n)

                b := bytes.NewBuffer(data)
                dec := gob.NewDecoder(b)

                info := personInfo{}
                dec.Decode(&info)
                fmt.Printf("Server receive data: %v\n", info)
        }
        return
}

func main() {
        tcpAddr, err := net.ResolveTCPAddr("tcp", "127.0.0.1:8087")
        if err != nil {
                fmt.Println("ResolveTCPAddr failed:", err)
                return
        }

        l, err := net.ListenTCP("tcp", tcpAddr)
        if err != nil {
                fmt.Println("Listen port error:", err)
                return
        }

        for {
                conn, err := l.AcceptTCP()
                fmt.Printf("The client: %s connected\n", conn.RemoteAddr().String())
                if err != nil {
                        fmt.Println("Accept error:", err)
                        continue
                }

                go handleConnection(conn)
        }
}
```


## `client.go` {#client-dot-go}

```go
package main

import (
        "bytes"
        "encoding/gob"
        "fmt"
        "net"
        "time"
)

type personInfo struct {
        Name  string
        Age   int32
        Phone string
        Addr  string
}

func main() {
        tcpAddr, err := net.ResolveTCPAddr("tcp", "127.0.0.1:8087")
        if err != nil {
                fmt.Println("ResolveTCPAddr failed:", err)
                return
        }

        conn, err := net.DialTCP("tcp", nil, tcpAddr)
        if err != nil {
                fmt.Println("Create connection failed:", err)
                return
        }

        info := personInfo{"Maya", 18, "13277085678", "Hubei China"}
        var b bytes.Buffer
        enc := gob.NewEncoder(&b)
        enc.Encode(&info)
        for {
                conn.Write(b.Bytes())
                fmt.Println(b.Bytes())
                <-time.After(time.Second * 3)
        }
        conn.Close()
}
```
