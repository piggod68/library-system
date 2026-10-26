// ===========================================================================
//  LibraryManager.cpp  ——  类的实现 + 控制台主程序
//    【继承】 Book / EBook 派生自抽象基类 MediaItem
//    【组合】 Library 拥有馆藏与读者，Reader 拥有借阅记录
//    【依赖】 BorrowService 只通过函数参数使用 Library
//
//  编译：
//    g++ -std=c++17 -Wall -Wextra LibraryManager.cpp -o LibraryManager
//    cl  /std:c++17 /EHsc LibraryManager.cpp
// ===========================================================================
#if defined(_MSC_VER)
#  pragma execution_character_set("utf-8")   // MSVC：字符串字面量按 UTF-8 编码
#endif

#include "LibraryManager.h"

#include <algorithm>
#include <cstddef>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>          // 仅用于把控制台输出切换到 UTF-8
#endif

// ===========================================================================
//  util  —— 与业务无关的小工具
// ===========================================================================
namespace util {

std::string today() {
    std::time_t t = std::time(nullptr);
    std::tm lt{};
#if defined(_WIN32)
    ::localtime_s(&lt, &t);      // MSVC 的安全版本
#else
    ::localtime_r(&t, &lt);      // POSIX
#endif
    char buf[16] = {0};
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &lt);
    return std::string(buf);
}

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> out;
    std::string field;
    std::istringstream iss(line);
    while (std::getline(iss, field, delim)) {
        out.push_back(field);
    }
    if (!line.empty() && line.back() == delim) {
        out.emplace_back();      // 补上被 getline 吞掉的末尾空字段
    }
    return out;
}

}  // namespace util

// ===========================================================================
//  MediaItem —— 抽象基类
// ===========================================================================
MediaItem::MediaItem(std::string isbn, std::string title, std::string author, int total)
    : isbn_(std::move(isbn)),
      title_(std::move(title)),
      author_(std::move(author)),
      total_(total),
      available_(total) {
    if (isbn_.empty())  throw std::invalid_argument("ISBN 不能为空");
    if (title_.empty()) throw std::invalid_argument("书名不能为空");
    if (total_ < 0)     throw std::invalid_argument("馆藏总量不能为负数");
}

void MediaItem::borrowOne() {
    if (!stockManaged()) return;                 // 电子书不占实体库存，直接返回
    if (available_ <= 0) {
        throw std::runtime_error("《" + title_ + "》暂无可借副本");
    }
    --available_;
}

void MediaItem::returnOne() {
    if (!stockManaged()) return;
    if (available_ >= total_) {
        throw std::runtime_error("《" + title_ + "》归还状态异常");
    }
    ++available_;
}

std::string MediaItem::stockText() const {
    return "在馆 " + std::to_string(available_) + "/" + std::to_string(total_);
}

std::string MediaItem::description() const {
    std::ostringstream oss;
    oss << "[" << typeName() << "] " << isbn_ << "  《" << title_ << "》  "
        << author_ << "  " << stockText();
    const std::string extra = extraInfo();
    if (!extra.empty()) oss << "  " << extra;
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const MediaItem& item) {
    return os << item.description();
}

// ===========================================================================
//  Book —— 纸质书
// ===========================================================================
Book::Book(std::string isbn, std::string title, std::string author,
           int total, std::string shelf)
    : MediaItem(std::move(isbn), std::move(title), std::move(author), total),
      shelf_(std::move(shelf)) {}

std::string Book::typeName() const { return "纸质书"; }

std::string Book::extraInfo() const { return "书架号 " + shelf_; }

std::string Book::toDataLine() const {
    return "BOOK|" + isbn() + "|" + title() + "|" + author() + "|" +
           std::to_string(total()) + "|" + shelf_;
}

// ===========================================================================
//  EBook —— 电子书
// ===========================================================================
EBook::EBook(std::string isbn, std::string title, std::string author, double sizeMB)
    : MediaItem(std::move(isbn), std::move(title), std::move(author), 0),
      sizeMB_(sizeMB) {
    if (sizeMB_ <= 0.0) throw std::invalid_argument("电子书文件大小必须大于 0");
}

bool EBook::stockManaged() const { return false; }   // 关键：改变基类行为

std::string EBook::typeName() const { return "电子书"; }

std::string EBook::extraInfo() const {
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(1);
    oss << sizeMB_ << " MB";
    return oss.str();
}

std::string EBook::toDataLine() const {
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(1);
    oss << "EBOOK|" << isbn() << "|" << title() << "|" << author() << "|" << sizeMB_;
    return oss.str();
}

std::string EBook::stockText() const { return "在线可读（无副本限制）"; }

// ===========================================================================
//  BorrowRecord
// ===========================================================================
std::string BorrowRecord::toDataLine() const {
    return "REC|" + readerId + "|" + isbn + "|" + title + "|" +
           borrowDate + "|" + (returnDate.empty() ? "-" : returnDate);
}

// ===========================================================================
//  Reader
// ===========================================================================
Reader::Reader(std::string id, std::string name)
    : id_(std::move(id)), name_(std::move(name)) {}

void Reader::addRecord(BorrowRecord record) {
    records_.push_back(std::move(record));
}

bool Reader::markReturned(const std::string& isbn, const std::string& date) {
    for (auto it = records_.rbegin(); it != records_.rend(); ++it) {   // 从最近的找起
        if (it->isbn == isbn && !it->returned()) {
            it->returnDate = date;
            return true;
        }
    }
    return false;
}

std::size_t Reader::activeCount() const {
    return static_cast<std::size_t>(
        std::count_if(records_.begin(), records_.end(),
                      [](const BorrowRecord& r) { return !r.returned(); }));
}

std::string Reader::description() const {
    return "读者 " + id_ + "（" + name_ + "）  累计借阅 " +
           std::to_string(borrowCount()) + " 次，当前未还 " +
           std::to_string(activeCount()) + " 本";
}

// ===========================================================================
//  Library —— 组合：拥有馆藏与读者
// ===========================================================================
bool Library::addItem(ItemPtr item) {
    const std::string key = item->isbn();
    return items_.emplace(key, std::move(item)).second;   // ISBN 重复则返回 false
}

bool Library::removeItem(const std::string& isbn) {
    return items_.erase(isbn) > 0;
}

MediaItem* Library::find(const std::string& isbn) {
    auto it = items_.find(isbn);
    return it == items_.end() ? nullptr : it->second.get();
}

std::vector<MediaItem*> Library::search(const std::string& keyword) const {
    std::vector<MediaItem*> hits;
    for (const auto& kv : items_) {
        const MediaItem& m = *kv.second;
        if (m.isbn() == keyword ||
            m.title().find(keyword) != std::string::npos ||
            m.author().find(keyword) != std::string::npos) {
            hits.push_back(kv.second.get());
        }
    }
    return hits;
}

bool Library::addReader(const std::string& id, const std::string& name) {
    return readers_.emplace(id, Reader(id, name)).second;
}

Reader* Library::findReader(const std::string& id) {
    auto it = readers_.find(id);
    return it == readers_.end() ? nullptr : &it->second;
}

bool Library::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    for (const auto& kv : items_) {                 // 多态：各类型自己序列化
        out << kv.second->toDataLine() << '\n';
    }
    for (const auto& kv : readers_) {
        out << "READER|" << kv.second.id() << "|" << kv.second.name() << '\n';
        for (const BorrowRecord& r : kv.second.records()) {
            out << r.toDataLine() << '\n';
        }
    }
    return true;
}

bool Library::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    items_.clear();
    readers_.clear();

    std::string line;
    std::size_t skipped = 0;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        try {
            const std::vector<std::string> f = util::split(line, '|');
            if (f.size() == 6 && f[0] == "BOOK") {
                addItem(std::make_shared<Book>(f[1], f[2], f[3], std::stoi(f[4]), f[5]));
            } else if (f.size() == 5 && f[0] == "EBOOK") {
                addItem(std::make_shared<EBook>(f[1], f[2], f[3], std::stod(f[4])));
            } else if (f.size() == 3 && f[0] == "READER") {
                addReader(f[1], f[2]);
            } else if (f.size() == 6 && f[0] == "REC") {
                if (Reader* r = findReader(f[1])) {
                    r->addRecord(BorrowRecord{f[1], f[2], f[3], f[4],
                                              f[5] == "-" ? std::string() : f[5]});
                }
            } else {
                ++skipped;
            }
        } catch (const std::exception& e) {         // 单行脏数据不中断整体加载
            ++skipped;
            std::cerr << "  ! 跳过无法解析的数据行：" << line
                      << "（" << e.what() << "）\n";
        }
    }
    if (skipped > 0) {
        std::cerr << "  ! 共跳过 " << skipped << " 行无法识别的数据\n";
    }
    return true;
}

// ===========================================================================
//  BorrowService —— 依赖：只通过参数使用 Library
// ===========================================================================
bool BorrowService::borrow(Library& lib, const std::string& readerId,
                           const std::string& isbn, std::string& msg) {
    MediaItem* item = lib.find(isbn);              // ★ 依赖：库对象由调用方传入
    if (item == nullptr) { msg = "馆藏不存在：" + isbn;    return false; }

    Reader* reader = lib.findReader(readerId);
    if (reader == nullptr) { msg = "读者不存在：" + readerId; return false; }

    try {
        item->borrowOne();          // 多态：纸质书扣库存，电子书内部直接返回
    } catch (const std::exception& e) {
        msg = e.what();             // 库存不足：必须立即返回
        return false;
    }

    reader->addRecord(BorrowRecord{readerId, item->isbn(), item->title(),
                                   util::today(), std::string()});
    msg = "借阅成功：《" + item->title() + "》，日期 " + util::today();
    return true;
}

bool BorrowService::giveBack(Library& lib, const std::string& readerId,
                             const std::string& isbn, std::string& msg) {
    MediaItem* item = lib.find(isbn);
    if (item == nullptr) { msg = "馆藏不存在：" + isbn;    return false; }

    Reader* reader = lib.findReader(readerId);
    if (reader == nullptr) { msg = "读者不存在：" + readerId; return false; }

    if (!reader->markReturned(isbn, util::today())) {
        msg = "该读者没有这本书的未归还记录";
        return false;
    }
    try {
        item->returnOne();
    } catch (const std::exception& e) {
        msg = e.what();
        return false;
    }
    msg = "归还成功：《" + item->title() + "》，日期 " + util::today();
    return true;
}

std::vector<std::pair<std::string, int>>
BorrowService::topBorrowed(const Library& lib, std::size_t k) const {
    std::map<std::string, int> counter;              // 书名 -> 被借次数
    for (const auto& kv : lib.readers()) {
        for (const BorrowRecord& r : kv.second.records()) {
            ++counter[r.title];
        }
    }
    std::vector<std::pair<std::string, int>> rank(counter.begin(), counter.end());
    std::sort(rank.begin(), rank.end(),
              [](const std::pair<std::string, int>& a,
                 const std::pair<std::string, int>& b) {
                  if (a.second != b.second) return a.second > b.second;  // 次数降序
                  return a.first < b.first;                              // 同次数按书名
              });
    if (rank.size() > k) rank.resize(k);
    return rank;
}

// ===========================================================================
//  控制台界面
// ===========================================================================
namespace {

const char* kDataFile = "library_data.txt";

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int readInt(const std::string& prompt, int defaultValue) {
    const std::string s = readLine(prompt);
    if (s.empty()) return defaultValue;
    try {
        return std::stoi(s);
    } catch (const std::exception&) {
        std::cout << "  ! 不是合法整数，按默认值 " << defaultValue << " 处理\n";
        return defaultValue;
    }
}

double readDouble(const std::string& prompt, double defaultValue) {
    const std::string s = readLine(prompt);
    if (s.empty()) return defaultValue;
    try {
        return std::stod(s);
    } catch (const std::exception&) {
        std::cout << "  ! 不是合法数字，按默认值 " << defaultValue << " 处理\n";
        return defaultValue;
    }
}

void seedDemoData(Library& lib) {
    lib.addItem(std::make_shared<Book>("9787115585386", "C++ Primer",
                                       "Stanley B. Lippman", 3, "A-01"));
    lib.addItem(std::make_shared<Book>("9787111213826", "设计模式",
                                       "Erich Gamma", 2, "A-02"));
    lib.addItem(std::make_shared<EBook>("9787111544937", "Effective Modern C++",
                                        "Scott Meyers", 12.5));
    lib.addReader("20230001", "张三");
    lib.addReader("20230002", "李四");
}

void printAllItems(const Library& lib) {
    if (lib.items().empty()) {
        std::cout << "  （馆藏为空）\n";
        return;
    }
    int no = 1;
    for (const auto& kv : lib.items()) {
        std::cout << "  " << no++ << ". " << kv.second->description() << '\n';
    }
}

void printAllReaders(const Library& lib) {
    if (lib.readers().empty()) {
        std::cout << "  （暂无读者）\n";
        return;
    }
    for (const auto& kv : lib.readers()) {
        std::cout << "  " << kv.second.description() << '\n';
    }
}

void addItemFlow(Library& lib) {
    const std::string kind   = readLine("  类型(1=纸质书 2=电子书，回车默认纸质书): ");
    const std::string isbn   = readLine("  ISBN : ");
    const std::string title  = readLine("  书名 : ");
    const std::string author = readLine("  作者 : ");

    bool ok = false;
    if (kind == "2") {
        const double sizeMB = readDouble("  文件大小(MB): ", 10.0);
        ok = lib.addItem(std::make_shared<EBook>(isbn, title, author, sizeMB));
    } else {
        const int total       = readInt("  馆藏总量: ", 1);
        const std::string shf = readLine("  书架号  : ");
        ok = lib.addItem(std::make_shared<Book>(isbn, title, author, total, shf));
    }
    std::cout << (ok ? "  √ 新增成功\n" : "  × 新增失败：该 ISBN 已存在\n");
}

void searchFlow(const Library& lib, const std::string& keyword) {
    const std::vector<MediaItem*> hits = lib.search(keyword);
    if (hits.empty()) {
        std::cout << "  没有匹配的馆藏\n";
        return;
    }
    std::cout << "  共找到 " << hits.size() << " 条：\n";
    for (const MediaItem* m : hits) {
        std::cout << "    " << m->description() << '\n';
    }
}

void rankFlow(const BorrowService& service, const Library& lib) {
    const int k = readInt("  显示前几名? ", 5);
    const std::size_t topN = static_cast<std::size_t>(k < 1 ? 1 : k);
    const std::vector<std::pair<std::string, int>> rank = service.topBorrowed(lib, topN);
    if (rank.empty()) {
        std::cout << "  暂无借阅记录\n";
        return;
    }
    std::cout << "  ---------- 借阅排行榜 ----------\n";
    int no = 1;
    for (const auto& p : rank) {
        std::cout << "   " << no++ << ". 《" << p.first << "》  被借 "
                  << p.second << " 次\n";
    }
}

}  // namespace

int main() {
#if defined(_WIN32)
    ::SetConsoleOutputCP(CP_UTF8);   // 让 Windows 控制台正确显示 UTF-8 中文
#endif

    Library lib;              // 【组合】整个程序的"整体"
    BorrowService service;    // 【依赖】无状态服务
    std::string message;

    if (lib.load(kDataFile)) {
        std::cout << "已从 " << kDataFile << " 载入 " << lib.itemCount()
                  << " 种馆藏、" << lib.readerCount() << " 名读者。\n";
    } else {
        seedDemoData(lib);
        std::cout << "未找到数据文件，已载入演示数据。\n";
    }

    for (;;) {
        std::cout << "\n============ 图书管理系统 ============\n"
                     "   1. 新增馆藏         2. 下架馆藏\n"
                     "   3. 借书             4. 还书\n"
                     "   5. 查询馆藏         6. 借阅排行榜\n"
                     "   7. 读者信息         8. 保存并退出\n"
                     "======================================\n"
                     "请选择(1-8): ";

        std::string choice;
        if (!std::getline(std::cin, choice)) break;

        try {
            if (choice == "1") {
                addItemFlow(lib);
            } else if (choice == "2") {
                const std::string isbn = readLine("  要下架的 ISBN: ");
                std::cout << (lib.removeItem(isbn) ? "  √ 已下架\n"
                                                   : "  × 未找到该 ISBN\n");
            } else if (choice == "3") {
                const std::string readerId = readLine("  读者编号: ");
                const std::string isbn     = readLine("  ISBN    : ");
                const bool ok = service.borrow(lib, readerId, isbn, message);
                std::cout << (ok ? "  √ " : "  × ") << message << '\n';
            } else if (choice == "4") {
                const std::string readerId = readLine("  读者编号: ");
                const std::string isbn     = readLine("  ISBN    : ");
                const bool ok = service.giveBack(lib, readerId, isbn, message);
                std::cout << (ok ? "  √ " : "  × ") << message << '\n';
            } else if (choice == "5") {
                const std::string keyword = readLine("  关键字(直接回车=列出全部): ");
                if (keyword.empty()) printAllItems(lib);
                else                 searchFlow(lib, keyword);
            } else if (choice == "6") {
                rankFlow(service, lib);
            } else if (choice == "7") {
                printAllReaders(lib);
            } else if (choice == "8") {
                std::cout << (lib.save(kDataFile) ? "  √ 数据已保存，再见。\n"
                                                  : "  × 保存失败！\n");
                break;
            } else {
                std::cout << "  请输入 1-8 之间的数字\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  ! 操作出现异常：" << e.what() << '\n';
        }
    }
    return 0;
}