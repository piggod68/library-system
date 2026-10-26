import sqlite3

conn = sqlite3.connect("books.db")
cur = conn.cursor()

cur.execute("""
       CREATE TABLE IF NOT EXISTS books(
       isbn TEXT PRIMARY KEY,
       title TEXT,
       stock INTEGER
       )
""")

cur.execute("INSERT OR REPLACE INTO books VALUES(?,?,?)",("B001","python入门",3))
cur.execute("INSERT OR REPLACE INTO books VALUES (?, ?, ?)", ("B002", "算法导论", 1))

conn.commit()

cur.execute("SELECT* FROM books")
for row in cur.fetchall():
    print(row)

conn.close()

            