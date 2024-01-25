#include <iostream>
#include <variant>
#include <string>
#include <fstream>
#include <sstream>
#include <variant>
#include <vector>
#include <algorithm>

#include "ini.h"

using namespace std;

namespace Ini { //общее

string LoadString(stringstream& input) {
	string result;
	char c;
	for (; input >> c && c != '[' && c != ']';) {
		result.push_back(c);
	}
	return result;
}

ostream& operator<<(ostream& out, const Node& node);

using Comment = string;
using Key = string;
using Value = string;
using Section = pair<Key, Value>;
using Array = vector<Node>;
using Chapter = pair<string, Array>;

bool operator==(const Node& lhs, const Section& rhs) {
	bool result = lhs.IsSection() && lhs.AsSection().first == rhs.first;
	return result;
}

bool operator==(const Node& lhs, const Chapter& rhs) {
	bool result = lhs.IsChapter() && lhs.AsChapter().first == rhs.first;
	return result;
}

}

namespace Ini { //Node

bool Node::IsComment() const {
	return holds_alternative<Comment>(*this);
}
	
bool Node::IsSection() const {
	return holds_alternative<Section>(*this);
}
	
bool Node::IsChapter() const {
	return holds_alternative<Chapter>(*this);
}
	
bool Node::IsArray() const {
	return holds_alternative<Array>(*this);
}

Comment& Node::AsComment() {
	if (!IsComment()) {
		throw logic_error("No an Comment");
	}
	return get<Comment>(*this);
}
	
Section& Node::AsSection() {
	if (!IsSection()) {
		throw logic_error("No an Section");
	}
	return get<Section>(*this);
}
	
Chapter& Node::AsChapter() {
	if (!IsChapter()) {
		throw logic_error("No an Chapter");
	}
	return get<Chapter>(*this);
}
	
Array& Node::AsArray() {
	if (!IsArray()) {
		throw logic_error("No an Array");
	}
	return get<Array>(*this);
}

const Comment& Node::AsComment() const{
	if (!IsComment()) {
		throw logic_error("No an Comment");
	}
	return get<Comment>(*this);
}
	
const Section& Node::AsSection() const{
	if (!IsSection()) {
		throw logic_error("No an Section");
	}
	return get<Section>(*this);
}
	
const Chapter& Node::AsChapter() const{
	if (!IsChapter()) {
		throw logic_error("No an Chapter");
	}
	return get<Chapter>(*this);
}
	
const Array& Node::AsArray() const{
	if (!IsArray()) {
		throw logic_error("No an Array");
	}
	return get<Array>(*this);
}
	
Node::Value& Node::GetValue() {
	return *this;
}
	
const Node::Value& Node::GetValue() const {
	return *this;
}
	
const Node& Node::GetNode(const string& key) const {	
	if (IsArray()) {
		for (const Node& node : AsArray()) {
			if (node.IsSection() && node.AsSection().first == key) {
				return node;
			} else if (node.IsChapter() && node.AsChapter().first == key) {
				return node;
			}
		}
	} else if (IsChapter()) {
		for (const Node& node : AsChapter().second) {
			if (node.IsSection() && node.AsSection().first == key) {
				return node;
			} else if (node.IsChapter() && node.AsChapter().first == key) {
				return node;
			}
		}
	}
	return *this;
}
	
const Node& Node::GetNode(const string& chapter, const string& key) const {
	return GetNode(chapter).GetNode(key);
}

const string Node::AsString() const {
	if (IsSection()) {
		return AsSection().second;
	} else if (IsComment()) {
		return AsComment();
	}
	return "";
}

const string Node::GetValue(const string& chapter, const string& key) const {
	return GetNode(chapter, key).AsString();
}

Node& Node::GetNode(const string& key) {	
	if (IsArray()) {
		for (Node& node : AsArray()) {
			if (node.IsSection() && node.AsSection().first == key) {
				return node;
			} else if (node.IsChapter() && node.AsChapter().first == key) {
				return node;
			}
		}
	} else if (IsChapter()) {
		for (Node& node : AsChapter().second) {
			if (node.IsSection() && node.AsSection().first == key) {
				return node;
			} else if (node.IsChapter() && node.AsChapter().first == key) {
				return node;
			}
		}
	}
	return *this;
}

Node& Node::GetChapter(const string& key) {	
	if (IsArray()) {
		for (Node& node : AsArray()) {
			if (node.IsChapter() && node.AsChapter().first == key) {
				return node;
			}
		}
	}
	return *this;
}

bool Node::AddSection(const string& key, const string& value) {
	Node& node = GetNode(key);
	if (node.IsSection()) {
		node.AsSection().second = value;
		return true;
	} else if (IsArray()) {
		for (auto it = AsArray().begin(); it != AsArray().end(); ++it) {
			if (it->IsChapter()) {
				AsArray().insert(it, Section{key, value});
				return true;
			}
		}
	} else if (IsChapter()) {
		for (auto it = AsChapter().second.begin(); it != AsChapter().second.end(); ++it) {
			if (it->IsSection() && it->AsSection().first == key) {
				it->AsSection().second = value;
				return true;
			}
		}
		AsChapter().second.push_back(Section{key, value});
	}
	return false;
}

bool Node::AddSection(const string& chapter, const string& key, const string& value) {
	Node& node = GetChapter(chapter);
	if (node.IsChapter()) {
		node.AddSection(key, value);
		return true;
	} else if (node.IsArray()) {
		vector<Node> ins;
		ins.push_back(Section{key, value});
		node.AsArray().push_back(Chapter{chapter, ins});
		return true;
	}
	return false;
}

}

namespace Ini { //Ini

Ini::Ini(Node root) : root_(move(root)) {
	}
	
Ini::Ini(istream& input) : root_(move(LoadArray(input, true))) {
	}

void Ini::SaveIni(ostream& out) {
	out << root_;
}
const Node& Ini::GetRoot() const {
	return root_;
}
	
const size_t Ini::Size() const {
	return root_.AsArray().size();
}
	
const Node& Ini::GetNode(const string& key) const {
	return root_.GetNode(key);
}
	
const Node& Ini::GetNode(const string& chapter, const string& key) const {
	return root_.GetNode(chapter, key);
}

const string Ini::GetValue(const string& chapter, const string& key) const {
	return root_.GetValue(chapter, key);
}

bool Ini::SetValue(const string& key, const string& value) {
	return root_.AddSection(key, value);
}

bool Ini::SetValue(const string& chapter, const string& key, const string& value) {
	return root_.AddSection(chapter, key, value);
}

const int Ini::GetValueInt(const std::string& chapter, const std::string& key) const {
	int result = 0;
	try {
		result = stoi(GetValue(chapter, key));
	}
	catch(...) {
		result = 0;
	}
	return result;
}

const bool Ini::GetValueBool(const std::string& chapter, const std::string& key) const {
	string result = GetValue(chapter, key);
	return result == "true";
}

}

namespace Ini { // Вспомогательные функции

Section LoadSection(stringstream& input) {
	char c;
	Section result;
	for (; input >> c && c != '=';) {
		result.first.push_back(c);
	}
	for (; input >> c;) {
		result.second.push_back(c);
	}
	return result;
}

Array LoadArray(istream& input, bool root) {
	string line;
	char c;
	Array result;

	while(true) {
		input >> c;
		input.putback(c);
		if (c == '[' && !root) {
			return result;
		}
		if (!getline(input, line)) {
			break;
		}
		if (line.empty()) {
			continue;
		}
		stringstream ss(line);
		ss >> c;

		if (c == ';' || c == '#') {
			ss.putback(c);
			result.push_back(LoadString(ss));
		} else if (c == '[') {
			Chapter chapter;
			chapter.first = LoadString(ss);
			Array array = LoadArray(input, false);
			
			auto repeat = find_if(result.begin(), result.end(),
				[&chapter](const Node& elem){
					return chapter == elem;
				});
			
			if (repeat == result.end()) {
				chapter.second = array;
				result.push_back(chapter);
			} else {
				for(const auto& elem : array) {
					auto repeat_section = find_if(repeat->AsChapter().second.begin(), repeat->AsChapter().second.end(),
						[&elem](const Node& new_elem){
							return elem == new_elem;
						});
					
					if (repeat_section != repeat->AsChapter().second.end()) {
						*repeat_section = elem;
					} else {
						repeat->AsChapter().second.push_back(elem);
					}
				}
			}
			
		} else {
			ss.putback(c);
			Section section = LoadSection(ss);
			
			auto repeat = find_if(result.begin(), result.end(),
				[&section](const Node& elem){
					return section == elem;
				});
			
			if (repeat != result.end()) {
				*repeat = section;
			} else {
				result.push_back(section);
			}
		}
	}
	return result;
}

ostream& operator<<(ostream& out, const Section& section) {
	if (!section.first.empty())
		out << section.first << " = " << section.second;
	return out;
}

ostream& operator<<(ostream& out, const Array& array) {
	for (const Node& node : array) {
		out << node;
	}
	return out;
}

ostream& operator<<(ostream& out, const Chapter& chapter) {
	out << '\n' << "[" << chapter.first << "]" << '\n';
	for (const auto& section : chapter.second) {
		out << section;
	}
	return out;
}

ostream& operator<<(ostream& out, const Node& node) {
	if (node.IsComment()) {
		out << node.AsComment() << '\n';
	} else if (node.IsArray()) {
		out << node.AsArray() << '\n';
	} else if (node.IsChapter()) {
		out << node.AsChapter();
	} else if (node.IsSection()) {
		out << node.AsSection() << '\n';
	}
	return out;
}

ostream& operator<<(ostream& out, const Ini& ini) {
	out << ini.GetRoot();
	return out;
}

bool operator==(const Node& lhs, const Node& rhs) {
	if (lhs.IsSection() && rhs.IsSection()) {
		return lhs.AsSection().first == rhs.AsSection().first;
	} else if (lhs.IsChapter() && rhs.IsChapter()) {
		return lhs.AsChapter().first == rhs.AsChapter().first;
	} 
	return false;
}

}