# 图书借还系统

一个自己从零写的图书管理系统：前端页面 + 后端接口 + 数据库。

## 技术栈

| 层 | 用了什么 |
|---|---|
| 前端 | HTML + CSS + 原生 JavaScript（fetch） |
| 后端 | FastAPI（Python） |
| 数据库 | MySQL 8 |

## 功能

- 查看全部图书
- 按书号查一本
- 按书名模糊搜索
- 只看有库存的书
- 借书（库存 -1，同时写一条借阅记录）
- 还书（按记录编号还，标记已还，库存 +1）
- 查看借阅记录（谁借了什么、借了几天、还了没）
- 逾期提示（借出超过 30 天，页面标红）
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
| GET | `/records` | 借阅记录（JOIN books 带出书名） |
| POST | `/borrow/{isbn}` | 借书，请求体 `{"reader": "姓名"}` |
| POST | `/return/{record_id}` | 还书，按记录编号 |

## 前置要求

- Python 3.9+
- MySQL 8（本地安装并启动服务）

## 怎么跑

**1. 装 Python 依赖**

```bash
pip install fastapi uvicorn mysql-connector-python
```

**2. 建库（只有第一次需要）**

先打开 `init_mysql.py`，把里面的 `PASSWORD` 改成你自己的 MySQL root 密码，然后：

```bash
python init_mysql.py
```

会创建数据库 `library`、两张表（`books` 书目、`records` 借阅记录），并写入两本演示书。

**3. 配置密码**

把 `config.example.py` 复制一份，改名成 `config.py`，然后把里面的密码改成你的 MySQL root 密码：

```python
MYSQL_PASSWORD = "你的密码"
```

`config.py` 已经加进 `.gitignore`，不会上传到仓库。

**4. 启动后端**

```bash
cd api练习
python app.py
```

看到 `Uvicorn running on http://127.0.0.1:8000` 就是起来了（这个窗口别关）。

**5. 打开前端**

用浏览器打开 `html练习/form.html`，或者用 VSCode 的 Live Server。

## 页面访问方式

**推荐：直接访问后端**

```
http://127.0.0.1:8000/
```

后端已经把 `form.html` 一起发出去了（`@app.get("/")` 返回 `FileResponse`），页面和接口**同源**，不需要额外开前端服务，也不用担心跨域。

**另一种：单独打开页面文件**

用浏览器打开 `html练习/form.html`（这个版本里的请求地址写的是完整的 `http://127.0.0.1:8000`）。

## 部署

临时给外网访问：用 [cpolar](https://www.cpolar.com/) 内网穿透。

```bash
cpolar authtoken <你的token>   # 只需一次
cpolar http 8000               # 每次开机执行
```

会得到一个 `https://xxxx.cpolar.top` 的公网地址，别人打开就能用。

注意：免费版网址每次重启会变，且电脑必须开着。

## 接口文档

启动后访问 http://127.0.0.1:8000/docs

FastAPI 自动生成，可以直接在上面点着测试每个接口。

## 目录结构

```
init_mysql.py        MySQL 建库脚本（只需跑一次）
config.example.py    密码模板（复制成 config.py 再改）
config.py            真实密码（不进仓库）

api练习/
  app.py          后端，9 个接口
  books.db        旧版 SQLite 数据（已不再使用，留作备份）
  db.py           旧版 SQLite 建库脚本（已废弃）

html练习/
  form.html       前端页面（借书表单 + 图书列表）
```

## 踩过的坑

- `from x import y` 少写 `import` → SyntaxError
- 路由顺序：`/books/available` 必须写在 `/books/{isbn}` 之前，否则被通配路径截胡
- 异常时没关数据库连接 → 文件被锁住，后面的请求全部卡死
- 单个元素的元组要带逗号：`(isbn,)`
- SQLite 换 MySQL：占位符从 `?` 变成 `%s`，游标要加 `dictionary=True`

## 数据说明

最初用 SQLite，后来换成了 MySQL。换的过程中**业务逻辑一行没动**，只改了四样：import、连接函数、游标参数、占位符。