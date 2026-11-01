# 图书借还系统

一个自己从零写的图书管理系统：前端页面 + 后端接口 + 数据库。

## 技术栈

| 层 | 用了什么 |
|---|---|
| 前端 | HTML + CSS + 原生 JavaScript（fetch） |
| 后端 | FastAPI（Python） |
| 数据库 | SQLite |

## 功能

- 查看全部图书
- 按书号查一本
- 按书名模糊搜索
- 只看有库存的书
- 借书（库存 -1）
- 还书（库存 +1）
- 新增图书（Pydantic 自动校验）
- 删除图书

## 接口列表

| 方法 | 路径 | 说明 |
|---|---|---|
| GET | `/hello` | 测试接口 |
| GET | `/books` | 全部图书 |
| GET | `/books/available` | 只返回有库存的 |
| GET | `/books/{isbn}` | 按书号查 |
| GET | `/search/{keyword}` | 按书名模糊搜 |
| POST | `/books` | 新增图书（请求体） |
| DELETE | `/books/{isbn}` | 删除图书 |
| POST | `/borrow/{isbn}` | 借书 |
| POST | `/return/{isbn}` | 还书 |

## 怎么跑

**1. 装依赖**

```bash
pip install fastapi uvicorn
```

**2. 建数据库（只有第一次需要）**

```bash
cd api练习
python db.py
```

会生成 `books.db`，并写入两本演示书。

**3. 启动后端**

```bash
cd api练习
python app.py
```

看到 `Uvicorn running on http://127.0.0.1:8000` 就是起来了（这个窗口不要关）。

**4. 打开前端**

用浏览器打开 `html练习/form.html`，或者用 VSCode 的 Live Server。

## 接口文档

启动后访问 http://127.0.0.1:8000/docs

FastAPI 自动生成，可以直接在上面点着测试每个接口。

## 目录结构

```
api练习/
  app.py       后端，9 个接口
  db.py        建库脚本（只需跑一次）
  books.db     数据库

html练习/
  form.html    前端页面（借书表单 + 图书列表）
```

## 说明

学习项目，边学边做。数据用 SQLite，后续计划换成 MySQL。

## 踩过的坑

- `from x import y` 少写 `import` → SyntaxError
- 路由顺序：`/books/available` 必须写在 `/books/{isbn}` 之前，否则被通配路径截胡
- 异常时没关数据库连接 → 文件被锁住，后面的请求全部卡死
- 单个元素的元组要带逗号：`(isbn,)`