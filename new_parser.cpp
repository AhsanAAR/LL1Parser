#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include <map>
#include <set>

using namespace std;

vector<string> ssplit(string s, string delimiter) {
	vector<string> ans;
	size_t pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != string::npos) {
		token = s.substr(0, pos);
		ans.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	ans.push_back(s);
	return ans;
}

vector<vector<string>> read_csv_table(string fn) {
	vector<vector<string>> content;
	vector<string> row;
	string line, word;

	fstream file(fn, ios::in);

	while (getline(file, line))
	{
		row.clear();

		stringstream str(line);

		while (getline(str, word, ',')) {
			if (word == "COMMA")
				word = ",";
			row.push_back(word);
		}
		content.push_back(row);
	}

	return content;
}

int main()
{
	string parse_table_fn, grammar_fn;
	cout << "Enter the parse table file name." << endl;
	cin >> parse_table_fn;
	auto table = read_csv_table(parse_table_fn);

	map<string, map<string, int>> parse_table;
	vector<string> columns = table[0];
	vector<string> rows;

	for (int i = 0; i < table.size(); i++) {
		rows.push_back(table[i][0]);
	}

	for (int i = 1; i < table.size(); i++) {
		for (int j = 1; j < table[i].size(); j++) {
			if (table[i][j] != "-") {
				parse_table[rows[i]][columns[j]] = stoi(table[i][j]);
			}
		}
	}

	cout << "Enter the grammar file name." << endl;
	cin >> grammar_fn;
	ifstream reader(grammar_fn);
	string line;
	vector<vector<string>> productions;
	set<string> non_terminals;

	while (!reader.eof()) {
		getline(reader, line);
		auto production = ssplit(line, "\t");
		productions.push_back(production);
		non_terminals.insert(production[0]);
	}
	reader.close();

	stack<string> parse_stack;
	parse_stack.push("$");
	parse_stack.push("PROG");

	stack<string> input_stack;
	input_stack.push("$");

	stack<string> temp_stack;
	string inp_name;
	cout << "Please input the file you want to parse: ";
	cin >> inp_name;

	reader.open(inp_name);

	while (!reader.eof()) {
		string lit;
		reader >> lit;
		temp_stack.push(lit);
	}

	while (!temp_stack.empty()) {
		input_stack.push(temp_stack.top());
		temp_stack.pop();
	}

	string p_top, i_top;

	while (!parse_stack.empty() && !input_stack.empty() && !(parse_stack.top() == "$" && input_stack.top() == "$")) {
		p_top = parse_stack.top();
		i_top = input_stack.top();
		
		if (non_terminals.find(p_top) != non_terminals.end()) {
			if (parse_table[p_top].find(i_top) != parse_table[p_top].end()) {
				// entry found
				parse_stack.pop();
				int production_number = parse_table[p_top][i_top];
				auto p = productions[production_number - 1];
				for (int j = p.size() - 1; j >= 1; j--) {
					if(p[j] != "~")
						parse_stack.push(p[j]);
				}
			}
			else {
				// entry not found panic mode recovery
				cout << "Parsing error found at " << i_top;
				input_stack.pop();
			}
		}
		else {
			if (p_top == i_top) {
				parse_stack.pop();
				input_stack.pop();
			}
			else {
				cout << "Parsing error found at " << i_top;
				input_stack.pop();
			}
		}
	}

	if (parse_stack.top() == "$" && input_stack.top() == "$") {
		cout << "accepted the input.";
	}
	else {
		cout << "not acceptable";
	}

	return 0;
}