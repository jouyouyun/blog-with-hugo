+++
title = "gorm 关联查询"
author = ["Wen"]
date = 2018-12-05T21:38:21+08:00
lastmod = 2019-02-10T17:05:50+08:00
tags = ["gorm", "association"]
categories = ["BLOG"]
draft = false
+++

定义了一个 `User` 和 `Company`, `User` 中可以包含多个 `Company`, 如下:

```go
type User struct {
        ID        int        `gorm:"TYPE:int(11);NOT NULL;PRIMARY_KEY;INDEX"`
        Name      string     `gorm:"TYPE: VARCHAR(255); DEFAULT:'';INDEX"`
        Companies []Company  `gorm:"FOREIGNKEY:UserId;ASSOCIATION_FOREIGNKEY:ID"`
        CreatedAt time.Time  `gorm:"TYPE:DATETIME"`
        UpdatedAt time.Time  `gorm:"TYPE:DATETIME"`
        DeletedAt *time.Time `gorm:"TYPE:DATETIME;DEFAULT:NULL"`
}

type Company struct {
        gorm.Model
        Industry int    `gorm:"TYPE:INT(11);DEFAULT:0"`
        Name     string `gorm:"TYPE:VARCHAR(255);DEFAULT:'';INDEX"`
        Job      string `gorm:"TYPE:VARCHAR(255);DEFAULT:''"`
        UserId   int    `gorm:"TYPE:int(11);NOT NULL;INDEX"`
}
```

在查询 `User` 时希望把 `Company` 的信息也一并查询, 有以下三种方法:


## Related {#related}

使用 `Related` 方法, 需要把把 `User` 查询好, 然后根据 `User` 定义中指定的 `FOREIGNKEY` 去查找 `Company`, 如果没定义, 则调用时需要指定, 如下:

```go
var u User
db.First(&u)
db.Model(&u).Related(&u.Companies).Find(&u.Companies)
```

`User` 列表时遍历列表一一查询 `Company`


## Association {#association}

使用 `Association` 方法, 需要把把 `User` 查询好, 然后根据 `User` 定义中指定的 `AssociationForeignKey` 去查找 `Company`, **必须定义**, 如下:

```go
var u User
db.First(&u)
db.Model(&u).Association("Companies").Find(&u.Companies)
```


## Preload {#preload}

使用 `Preload` 方法, 在查询 `User` 时先去获取 `Company` 的记录, 如下:

```go
// 查询单条 user
var u User
db.Debug().Preload("Companies").First(&u)
// 对应的 sql 语句
// SELECT * FROM users LIMIT 1;
// SELECT * FROM companies WHERE user_id IN (1);

// 查询所有 user
var list []User
db.Debug().Preload("Companies").Find(&list)
// 对应的 sql 语句
// SELECT * FROM users;
// SELECT * FROM companies WHERE user_id IN (1,2,3...);
```

---

本文中是一对多, 一对一的也是类似的, 完整代码见: [GORM Related](https://github.com/jouyouyun/examples/tree/master/gorm/related)
