def with_log(func):
    def wrapper(name):
        print("---开始---")
        func(name)
        print("---结束---")
    return wrapper

@with_log
def greet(name):
    return f"Hello, {name}!"

greet("小明")