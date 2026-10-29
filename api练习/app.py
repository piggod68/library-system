from fastapi import FastAPI
import uvicorn
import sqlite3
from pydantic import BaseModel

app = FastAPI()

from fastapi.middleware.cors import CORSMiddleware

app.add_middleware(
       CORSMiddleware,
       allow_origins={"*"},
       allow_methods={"*"},
       allow_headers={"*"},
)


def 打开数据库():
    conn = sqlite3.connect("books.db")
    conn.row_factory = sqlite3.Row
    return conn
    
class BookIn(BaseModel):
    isbn: str
    title: str
    stock: int
  


@app.get("/hello")
def hello():
    return {"message": "Hello,你是我的第一个接口!"}


@app.get("/books")
def get_books():
    conn = 打开数据库()
    cur = conn.cursor()
    cur.execute("SELECT * FROM books")
    rows = [dict(r) for r in cur.fetchall()]
    conn.close()
    return rows


@app.get("/books/available")
def get_available():
    conn = 打开数据库()
    cur = conn.cursor()
    cur.execute("SELECT * FROM books WHERE stock >0")
    rows = [dict(r) for r in cur.fetchall()]
    conn.close()
    return rows

@app.get("/books/{isbn}")
def get_book(isbn: str):
    conn = 打开数据库()
    cur = conn.cursor()
    cur.execute("SELECT * FROM books WHERE isbn = ?", (isbn,))
    row = cur.fetchone()
    conn.close()
    if row is None:
        return {"error": "没有这本书"}
    return dict(row)

@app.post("/borrow/{isbn}")
def borrow(isbn: str):
    conn = 打开数据库()
    cur = conn.cursor()
    cur.execute("SELECT stock FROM books WHERE isbn = ?", (isbn,))
    row = cur.fetchone()

    if row is None:
        conn.close()
        return {"ok": False, "msg": "没有这本书"}

    if row["stock"] <= 0:
        conn.close()
        return {"ok": False, "msg": "这本书借完了"}

    cur.execute("UPDATE books SET stock = stock - 1 WHERE isbn = ?", (isbn,))
    conn.commit()
    conn.close()
    return {"ok": True, "msg": "借阅成功"}
@app.post("/return/{isbn}")
def return_book(isbn: str):
    conn = 打开数据库()
    cur = conn.cursor()
    cur.execute("SELECT stock FROM books WHERE isbn = ?", (isbn,))
    row = cur.fetchone()

    if row is None:
        conn.close()
        return {"ok": False, "msg": "没有这本书"}

    cur.execute("UPDATE books SET stock = stock + 1 WHERE isbn = ?", (isbn,))
    conn.commit()
    conn.close()
    return {"ok": True, "msg": "还书成功"}
@app.get("/search/{keyword}")
def search(keyword: str):
    conn = 打开数据库()
    cur = conn.cursor()
    cur.execute("SELECT * FROM books WHERE title LIKE ?", ("%" + keyword + "%",))
    rows = [dict(r) for r in cur.fetchall()]
    conn.close()
    return rows

@app.post("/books")
def  create_book(book: BookIn):
    conn = 打开数据库()
    cur = conn.cursor() 
    cur.execute(
        "INSERT INTO books (isbn,title,stock) VALUES (?, ?, ?)",
        (book.isbn,book.title,book.stock),
    )
    conn.commit()
    conn.close()
    return {"ok":True,"msg":f"新增成功： 《{book.title}》"}

if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8000)