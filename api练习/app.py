from fastapi import FastAPI
import uvicorn
import mysql.connector
from fastapi.responses import FileResponse
from config import MYSQL_PASSWORD
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
    conn = mysql.connector.connect(
        host="127.0.0.1",
        port=3306,
        user="root",
        password=MYSQL_PASSWORD,
        database="library",
    )
    return conn
    
class BookIn(BaseModel):
    isbn: str
    title: str
    stock: int

class BorrowIn(BaseModel):
    reader : str


@app.get("/hello")
def hello():
    return {"message": "Hello,你是我的第一个接口!"}


@app.get("/books")
def get_books():
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    cur.execute("SELECT * FROM books")
    rows = [dict(r) for r in cur.fetchall()]
    conn.close()
    return rows


@app.get("/books/available")
def get_available():
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    cur.execute("SELECT * FROM books WHERE stock >0")
    rows = [dict(r) for r in cur.fetchall()]
    conn.close()
    return rows

@app.get("/records")
def get_records():
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    cur.execute("""
        SELECT records.id, records.reader, records.borrow_date,
               records.return_date, books.title,
               DATEDIFF(CURDATE(), records.borrow_date) AS days
        FROM records
        JOIN books ON books.isbn = records.isbn
        ORDER BY records.id DESC
    """)
    rows = cur.fetchall()
    conn.close()
    return rows

@app.get("/books/{isbn}")
def get_book(isbn: str):
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    cur.execute("SELECT * FROM books WHERE isbn = %s", (isbn,))
    row = cur.fetchone()
    conn.close()
    if row is None:
        return {"error": "没有这本书"}
    return dict(row)

@app.post("/borrow/{isbn}")
def borrow(isbn: str,data: BorrowIn):
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    try:
        cur.execute("SELECT stock FROM books WHERE isbn = %s", (isbn,))
        row = cur.fetchone()

        if row is None:
           conn.close()
           return {"ok": False, "msg": "没有这本书"}

        if row["stock"] <= 0:
           conn.close()
           return {"ok": False, "msg": "这本书借完了"}

        cur.execute("UPDATE books SET stock = stock - 1 WHERE isbn = %s", (isbn,))
        cur.execute(
            "INSERT INTO records (isbn, reader, borrow_date) VALUES (%s, %s, CURDATE())",
            (isbn, data.reader),
        )
   
        conn.commit()
    except Exception :
        conn.rollback()
        conn.close()
        return {"ok": False, "msg": "借阅失败"}

    conn.close()
    return {"ok": True, "msg":f"借阅成功,读者；{data.reader}"}
@app.post("/return/{record_id}")
def return_book(record_id: int):
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    try:
        cur.execute("SELECT * FROM records WHERE id = %s", (record_id,))
        rec = cur.fetchone()

        if rec is None:
            conn.close()
            return {"ok": False, "msg": "没有这条借阅记录"}

        if rec ["return_date"] is not None:
            conn.close()
            return {"ok": False, "msg": "这本书已经归还过了"}

        cur.execute("UPDATE records SET return_date = CURDATE() WHERE id = %s", (record_id,))
        cur.execute("UPDATE books SET stock = stock + 1 WHERE isbn = %s", (rec["isbn"],))
        conn.commit()
    except Exception:
        conn.rollback()
        conn.close()
        return {"ok": False, "msg": "归还失败,请重试"}

    conn.close()
    return {"ok": True, "msg": f"归还成功,记录编号：{record_id}"}
     
       
@app.get("/search/{keyword}")
def search(keyword: str):
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    cur.execute("SELECT * FROM books WHERE title LIKE %s", ("%" + keyword + "%",))
    rows = [dict(r) for r in cur.fetchall()]
    conn.close()
    return rows

@app.post("/books")
def  create_book(book: BookIn):
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True) 
    try:
        cur.execute(
            "INSERT INTO books (isbn,title,stock) VALUES (%s, %s, %s)",
            (book.isbn,book.title,book.stock),
       )
        conn.commit()
    except Exception as e:
        conn.close()
        return {"ok":False,"msg":f"新增失败： 《{book.title}》"}
    conn.close()
    return {"ok":True,"msg":f"新增成功： 《{book.title}》"}

@app.delete("/books/{isbn}")
def delete_book(isbn: str):
    conn = 打开数据库()
    cur = conn.cursor(dictionary=True)
    cur.execute("DELETE FROM books WHERE isbn = %s",(isbn,))

    if cur.rowcount == 0:
        conn.close()
        return {"ok": False,"msg":"没有这本书"}

    conn.commit()
    conn.close()
    return {"ok": True,"msg":"删除成功"}

@app.get("/")
def index():
    return FileResponse("form.html")


if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8000)