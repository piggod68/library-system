# -*- coding: utf-8 -*-
"""初始化 MySQL 数据库：建库、建表、写入演示数据。
运行前请先改下面的密码为你自己的 root 密码。
"""
import mysql.connector

PASSWORD = "改成你自己的密码"

conn = mysql.connector.connect(host="127.0.0.1", port=3306, user="root", password=PASSWORD)
cur = conn.cursor()

cur.execute("CREATE DATABASE IF NOT EXISTS library CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci")
cur.execute("USE library")
cur.execute("""
    CREATE TABLE IF NOT EXISTS books (
        isbn  VARCHAR(32) PRIMARY KEY,
        title VARCHAR(200),
        stock INT
    ) CHARACTER SET utf8mb4
""")

cur.execute("SELECT COUNT(*) FROM books")
if cur.fetchone()[0] == 0:
    cur.execute("INSERT INTO books (isbn, title, stock) VALUES (%s, %s, %s)", ("B001", "python入门", 3))
    cur.execute("INSERT INTO books (isbn, title, stock) VALUES (%s, %s, %s)", ("B002", "算法导论", 1))
    print("已写入两本演示书")
else:
    print("表里已有数据，跳过")

conn.commit()
cur.execute("SELECT * FROM books")
for r in cur.fetchall():
    print("  ", r)
cur.close(); conn.close()
print("完成")