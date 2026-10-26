
try:
  age = int(input("你几岁了"))
  print (f"你今年{age}岁")

except ValueError:
  print("请输入数字")
  