#pragma once
// ===========================================================================
//  LibraryManager.h  ——  面向对象程序设计实验：组合 / 依赖 / 继承
//
//  本文件集中定义全部类，三种类间关系一目了然：
//
//    【继承】 MediaItem（抽象基类） <|-- Book
//                                   <|-- EBook
//    【组合】 Library ◆— MediaItem（馆藏）      Library ◆— Reader（读者档案）
//             Reader  ◆— BorrowRecord（借阅记录）
//    【依赖】 BorrowService ⋯> Library（只作函数参数，不保存指针/引用成员）
//
//  小函数（访问器）写在类内联，其余实现在 LibraryManager.cpp。
// ===========================================================================
#include <cstddef>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ------------------------------ 工具函数 ---------------------------------
namespace util {
std::string today();                                             // yyyy-MM-dd
std::vector<std::string> split(const std::string& line, char delim);
}  // namespace util

// ===========================================================================
// 【继承 · 多态 · 封装】抽象基类：所有馆藏资源的公共接口
// ===========================================================================
class MediaItem {
public:
    MediaItem(std::string isbn, std::string title, std::string author, int total);
    virtual ~MediaItem() = default;              // 多态基类必须有虚析构函数

    // 纯虚接口：由派生类各自实现
    virtual std::string typeName()  const = 0;
    virtual std::string extraInfo() const = 0;
    virtual std::string toDataLine() const = 0;

    // 只读访问器（封装：外部只能读，不能改）
    const std::string& isbn()   const noexcept { return isbn_; }
    const std::string& title()  const noexcept { return title_; }
    const std::string& author() const noexcept { return author_; }
    int total()     const noexcept { return total_; }
    int available() const noexcept { return available_; }

    // 库存状态只能通过这两个接口改变
    void borrowOne();
    void returnOne();

    virtual bool stockManaged() const { return true; }   // 电子书覆盖为 false
    std::string description() const;                     // 统一展示文本

protected:
    virtual std::string stockText() const;               // 库存文案

private:
    std::string isbn_, title_, author_;
    int total_, available_;
};

std::ostream& operator<<(std::ostream& os, const MediaItem& item);

// ===========================================================================
// 【继承】Book：纸质书，只实现自己特有的部分
// ===========================================================================
class Book : public MediaItem {
public:
    Book(std::string isbn, std::string title, std::string author,
         int total, std::string shelf);
    std::string typeName()  const override;
    std::string extraInfo() const override;
    std::string toDataLine() const override;
    const std::string& shelf() const noexcept { return shelf_; }

private:
    std::string shelf_;
};

// ===========================================================================
// 【继承 + 多态】EBook：电子书，覆盖 stockManaged() 改变借阅行为
// ===========================================================================
class EBook : public MediaItem {
public:
    EBook(std::string isbn, std::string title, std::string author, double sizeMB);
    bool stockManaged() const override;                  // 不占实体库存
    std::string typeName()  const override;
    std::string extraInfo() const override;
    std::string toDataLine() const override;
    double sizeMB() const noexcept { return sizeMB_; }

protected:
    std::string stockText() const override;

private:
    double sizeMB_;
};

// ===========================================================================
// 借阅记录（值类型，被 Reader 以值语义【组合】持有）
// ===========================================================================
struct BorrowRecord {
    std::string readerId;
    std::string isbn;
    std::string title;        // 书名快照，便于排行榜统计
    std::string borrowDate;
    std::string returnDate;   // 空串表示尚未归还

    bool returned() const noexcept { return !returnDate.empty(); }
    std::string toDataLine() const;
};

// ===========================================================================
// 【组合】Reader：读者持有自己的借阅记录（has-a，记录随读者一同销毁）
// ===========================================================================
class Reader {
public:
    Reader() = default;                                  // 作为 map 的值类型需要
    Reader(std::string id, std::string name);
    const std::string& id()   const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }

    void addRecord(BorrowRecord record);
    bool markReturned(const std::string& isbn, const std::string& date);
    std::size_t borrowCount() const noexcept { return records_.size(); }
    std::size_t activeCount() const;
    const std::vector<BorrowRecord>& records() const noexcept { return records_; }
    std::string description() const;

private:
    std::string id_, name_;
    std::vector<BorrowRecord> records_;      // ★ 组合：整体没了，部分也没了
};

// ===========================================================================
// 【组合】Library：书库拥有全部馆藏与读者档案（has-a）
// ===========================================================================
class Library {
public:
    using ItemPtr = std::shared_ptr<MediaItem>;  // 多态必须存指针，避免对象切片

    bool addItem(ItemPtr item);
    bool removeItem(const std::string& isbn);
    MediaItem* find(const std::string& isbn);
    std::vector<MediaItem*> search(const std::string& keyword) const;

    bool addReader(const std::string& id, const std::string& name);
    Reader* findReader(const std::string& id);

    const std::map<std::string, ItemPtr>& items()   const noexcept { return items_; }
    const std::map<std::string, Reader>&  readers() const noexcept { return readers_; }
    std::size_t itemCount()   const noexcept { return items_.size(); }
    std::size_t readerCount() const noexcept { return readers_.size(); }

    bool save(const std::string& path) const;
    bool load(const std::string& path);

private:
    std::map<std::string, ItemPtr> items_;    // ★ 组合：ISBN  -> 馆藏
    std::map<std::string, Reader>  readers_;  // ★ 组合：编号  -> 读者
};

// ===========================================================================
// 【依赖】BorrowService：不持有 Library 成员，只在调用时通过参数使用它
// ===========================================================================
class BorrowService {
public:
    bool borrow(Library& lib, const std::string& readerId,
                const std::string& isbn, std::string& msg);
    bool giveBack(Library& lib, const std::string& readerId,
                  const std::string& isbn, std::string& msg);
    std::vector<std::pair<std::string, int>> topBorrowed(const Library& lib,
                                                         std::size_t k) const;
};