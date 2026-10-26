class Book:
    def __init__(self, title, stock):
        self.title = title
        self.stock = stock


    def borrow(self):
        if self.stock > 0:
            self.stock -= 1
            return True
        else:
            return False


    def info(self):
        return f"书名: {self.title}, 库存: {self.stock}本"
      

b1 = Book("python入门", 3)
b2 = Book("java入门", 2 )



class Reader:
    def __init__(self,name):
       self.name = name
       self.borrowed = 0


    def borrow_one(self):
        self.borrowed+=1

r = Reader("小明")
r.borrow_one()
r.borrow_one()
print(r.borrowed)      