#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>
#include <windows.h>

using namespace std;
using namespace std::filesystem;

struct File {
	string path;
};

struct Directory
{
	string path;
	vector<File> file;
	vector<Directory> children;
};

Directory GetStructure(string);
string  GetJson(Directory, int);
int GetFileSize(const char*);
string GetItemName(string);
string GetDataCreation(string);
string GetValueWithComa(string);
string Replace(string);
string GetValue(string);

int main()
{
	cout << "Please enter directory path:"<<endl;
	string path;
	cin >> path;

	if (!is_directory(path)) {
		cout << "Directory not found"<<endl;
		return -1;
	}

	try
	{
		cout << "Creating structure..." << endl;
		Directory res = GetStructure(path);
		cout << "Creating json structure..." << endl;
		string json = GetJson(res, 1);
		cout << "Writing to file..." << endl;
		ofstream out;
		out.open("test.json");

		if (out.is_open())
		{
			out << json;;
		}

		out.close();
		cout << "Writed!!!!";
	}
	catch (const char* msg) {
		cout << msg << endl;
	}
}

string  GetJson(Directory direcory, int countTab = 1) {
	string tab = string(countTab, '\t');
	string dirTab = string(countTab - 1, '\t');
	string filesTab = string(countTab + 1, '\t');
	string nLine = "\n";

	string name = GetValue("Name") + ":" + GetValueWithComa(GetItemName(direcory.path));
	string dataCreation = GetValue("DateCreated") + ":" + GetValueWithComa(GetDataCreation(direcory.path));
	string files = GetValue("Files") + ": [" + nLine;

	for (int i = 0; i < direcory.file.size(); i++)
	{
		File file = direcory.file[i];
		string fileName = GetValue("Name") + ":" + GetValueWithComa((GetItemName(file.path)));
		string filesize = GetValue("Size") + ":" + GetValueWithComa((to_string(GetFileSize(file.path.c_str())) + " B"));
		string filepath = GetValue("Path") + ":" + Replace(GetValue(file.path));

		files += tab + "{" + nLine + filesTab + fileName + nLine + filesTab + filesize + nLine + filesTab + filepath + nLine + tab + "}";

		if (i != direcory.file.size() - 1) {
			files += "," + nLine;
		}
	}

	string children = GetValue("Childen") + ": [";

	for (int i = 0; i < direcory.children.size(); i++)
	{
		children += nLine + GetJson(direcory.children[i], countTab + 1);
		if (i != direcory.children.size() - 1) {
			children += ",";
		}
	}

	string endArr = nLine + tab + "]";
	files += endArr + "," + nLine;
	children += endArr + nLine;
	string result = dirTab + "{" + nLine + tab + name + nLine + tab + dataCreation + nLine + tab + files + tab + children + nLine + dirTab + "}";

	return result;
}

string Replace(string str) {
	size_t index = 0;

	while (true) {
		index = str.find("\\", index);
		if (index == std::string::npos) break;
		str.replace(index, 1, "\\\\");
		index += 2;
	}

	return str;
}

string GetValueWithComa(string value) {
	return "\"" + value + "\",";
}

string GetValue(string value) {
	return "\"" + value + "\"";
}

string GetItemName(string path) {
	return path.substr(path.find_last_of("/\\") + 1);
}

string GetDataCreation(string path) {
	struct stat t_stat;

	if (stat(path.c_str(), &t_stat) == 0)
	{
		struct tm* tm;
		char buf[200];
		tm = localtime(&t_stat.st_ctime);
		strftime(buf, sizeof(buf), "%a, %d.%m.%Y %H:%M:%S", tm);
		return string(buf);
	}

	return "";
}

int GetFileSize(const char* filepath)
{
	int filesize = -1;

#ifdef linux
	struct stat fileStats;
	if (stat(filepath.c_str(), &fileStats) != -1)
		filesize = fileStats.st_size;
#else
	wstring wstr(filepath, filepath + strlen(filepath));
	HANDLE hFile = CreateFile(wstr.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile > 0)
	{
		filesize = ::GetFileSize(hFile, NULL);
	}

	CloseHandle(hFile);
#endif

	return filesize;
}

Directory GetStructure(string path) {
	Directory dir;
	dir.path = path;

	for (const auto& entry : directory_iterator(path)) {

		if (is_directory(entry.path())) {
			dir.children.push_back(GetStructure(entry.path().string()));
		}

		else {
			File f;
			f.path = entry.path().string();
			dir.file.push_back(f);
		}

	}

	return dir;
}