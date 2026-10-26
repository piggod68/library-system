# s = [ "Hello", "World", "Python", "Programming"]

# print(s[0:4:2])  # Output: Hello, World



# for i in range(10):
#     num=int(input("Enter a number: "))
#     nu / / /

#邮箱格式的验证

#1.接受用户输入的邮箱
# mail = input("请输入邮箱地址：")


#2.使用正则表达式验证邮箱格式
# if mail.count("@") == 1 and mail.count(".") >= 1:
#     print("邮箱格式正确")
# else:
#     print("邮箱格式不正确")

#元组


# print(type(t1))  # 输出：<class 'tuple'>

# print(t1[0])  # 输出：80

# print(t1[-1])  # 输出：95

# print(t1[2:5])  # 输出：(80, 70, 90)

# print(t1.count(90))  # 输出：2

# print (t1.index(100))  # 输出：5


#解包
# ti = (8080, 70, 90, 100, 85, 75, 90, 95)
# a, b, c, d, e, f, g, h = ti
# print(a)  # 输出：8080

# x, *y, z = ti
# print(x)  # 输出：8080
# print(y)  # 输出：[70, 90, 100, 85, 75, 90]
# print(z)  # 输出：95


# a = 10
# b = 20
# # a, b = b, a
# # print(a)  # 输出：20
# # print(b)  # 输出：10
# t = (b, a)
# a, b = t
# print(a) 


# a = 100
# b = 200
# c = 300

# c, b, a = a, b, c
# print(a)  # 输出：300
# print(b)  # 输出：200
# print(c)  # 输出：100
# students = (
#     ("s001", "王林", 85, 92, 78),
#     ("s002", "李明", 90, 88, 95),
#     ("s003", "张伟", 75, 80, 82),
#     ("s004", "刘洋", 92, 95, 88),
#     ("s005", "陈杰", 88, 90, 85)
# )

# for student in students:
#     total_score = sum(student[2:])  # 计算总分
#     average_score = total_score / 3  # 计算平均分
#     print(f"学号: {student[0]}, 姓名: {student[1]}, 总分: {total_score}, 平均分: {average_score:.2f}")


# chinese_scores = [student[2] for student in students]
# highest_chinese_score = max(chinese_scores)
# print(f"最高语文成绩: {highest_chinese_score}")



# dict1 = {
#     "王林" :650, "李慕婉"  : 720, "张伟" : 580, "刘洋" : 690, "陈杰" : 710
#     }



# print(f"所有学生的成绩：{dict1}")

# print(type(dict1))  # 输出：<class 'dict'>

# print(dict1["王林"])  # 输出：650

shopping_cart = {}

# 1. 制作菜单
# print("欢迎来到购物车系统！")


# menu = """
# #######购物车菜单 #######
# #    1. 添加购物车      #
# #    2. 修改购物车      #
# #    3. 删除购物车      #
# #    4. 查看购物车      #   
# #    5. 退出购物车      #
# ########################

# """



# #2.执行的操作
# while True :
#  choice = input(menu + "请输入操作编号(1-5)：")

#  match choice:
#     case "1": #添加购物车
#         goods_name = input("请输入商品名称：")
#         goods_price = float(input("请输入商品价格："))
#         goods_num = int(input("请输入商品数量："))

#         if goods_name in shopping_cart:
#              print(f"{goods_name} 已经在购物车中，更新数量和价格。")
#         else:
#              shopping_cart[goods_name] = {"price": goods_price, "num": goods_num}
#              print(f"{goods_name} 已添加到购物车。")

#     case "2": #修改购物车   
#         goods_name = input("请输入要修改的商品名称：")
#         goods_price = float(input("请输入新的商品价格："))
#         goods_num = int(input("请输入新的商品数量："))

#         if goods_name in shopping_cart:
#             shopping_cart[goods_name] = {"price": goods_price, "num": goods_num}
#             print(f"{goods_name} 已更新。")
#         else:
#             print(f"{goods_name} 不在购物车中。")

#     case "3": #删除购物车
#         goods_name = input("请输入要删除的商品名称：")
#         if goods_name in shopping_cart:
#             del shopping_cart[goods_name]
#             print(f"{goods_name} 已从购物车中删除。")
#         else:
#             print(f"{goods_name} 不在购物车中。")

#     case "4": #查看购物车1
#         if shopping_cart:
#             print("购物车中的商品：")
#             for name, info in shopping_cart.items():
#                 print(f"  {name}: 价格={info['price']}, 数量={info['num']}")
#         else:
#             print("购物车是空的。")

#     case "5": #退出购物车
#         print("感谢使用购物车系统！")
#         break
#     case _: #无效输入
#         print("无效的操作编号，请输入1-5之间的数字。")




# def rectangle_area(length, width):
#     """计算矩形的面积"""
#     return length * width

# print(rectangle_area(5, 3))  
def circle_area_len(radius):
    return 3.14 * radius ** 2,round(2*3.14 * radius,1)

al=circle_area_len(5)
print(al)
#输出 (78.5, 31.400000000000002)
#多个返回值可以用元组的形式返回，调用时也可以用元组接收。