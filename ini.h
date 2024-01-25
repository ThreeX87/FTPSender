#pragma once

#include <iostream>
#include <variant>
#include <string>
#include <fstream>
#include <sstream>
#include <variant>
#include <vector>
#include <algorithm>

namespace Ini {

class Node;

using Comment = std::string;
using Key = std::string;
using Value = std::string;
using Section = std::pair<Key, Value>;
using Array = std::vector<Node>;
using Chapter = std::pair<std::string, Array>;

Array LoadArray(std::istream& input, bool root = true);
//класс для содержания и работы с элементами Ini файла
class Node final : private std::variant<std::nullptr_t, Chapter, Array, Section, Comment> {
public:
	using variant::variant;
	using Value = variant;
	//проверка типа элемента
	bool IsComment() const;
	bool IsSection() const;
	bool IsChapter() const;
	bool IsArray() const;
	//получение типа элемента
	const Comment& AsComment() const;
	const Section& AsSection() const;
	const Chapter& AsChapter() const;
	const Array& AsArray() const;
	Comment& AsComment();
	Section& AsSection();
	Chapter& AsChapter();
	Array& AsArray();
	//геттеры
	Value& GetValue();
	const Value& GetValue() const;
	const Node& GetNode(const std::string& key) const;
	const Node& GetNode(const std::string& chapter, const std::string& key) const;
	const std::string GetValue(const std::string& chapter, const std::string& key) const;
	const std::string AsString() const;
	//сеттеры
	bool AddSection(const std::string& key, const std::string& value);
	bool AddSection(const std::string& chapter, const std::string& key, const std::string& value);
private:
	Node& GetNode(const std::string& key);
	Node& GetChapter(const std::string& key);

};

class Ini {
public:
	explicit Ini(Node root);
	explicit Ini(std::istream& input);
	
	void SaveIni(std::ostream& out);
	const Node& GetRoot() const;
	bool SetValue(const std::string& key, const std::string& value);
	bool SetValue(const std::string& chapter, const std::string& key, const std::string& value);
	const size_t Size() const;
	const Node& GetNode(const std::string& key) const;
	const Node& GetNode(const std::string& chapter, const std::string& key) const;
	const std::string GetValue(const std::string& chapter, const std::string& key) const;
	const int GetValueInt(const std::string& chapter, const std::string& key) const;
	const bool GetValueBool(const std::string& chapter, const std::string& key) const;

private:
	Node root_;
};

bool operator==(const Node& lhs, const Node& rhs);
bool operator==(const Node& lhs, const Section& rhs);
bool operator==(const Node& lhs, const Chapter& rhs);
std::ostream& operator<<(std::ostream& out, const Section& section);
std::ostream& operator<<(std::ostream& out, const Array& array);
std::ostream& operator<<(std::ostream& out, const Chapter& chapter);
std::ostream& operator<<(std::ostream& out, const Node& node);
std::ostream& operator<<(std::ostream& out, const Ini& ini);

}
