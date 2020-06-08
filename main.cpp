#include<cstdio>
#include<iostream>
#include<cassert>
#include<fstream>
#include<string>
#include "Cache.h"
using namespace std;
using namespace NameSpaceCache;

const string path = "./";

int main() {
	Cache cache(4096, 8, WAY8, LRU);
	//Cache cache;
	// ±í1
	//Cache cache(1024, 16, DIRECT_MAPPED, LRU);
	//Cache cache(2048, 16, DIRECT_MAPPED, LRU);
	//Cache cache(4096, 16, DIRECT_MAPPED, LRU);
	//Cache cache(8192, 16, DIRECT_MAPPED, LRU);
	//Cache cache(16384, 16, DIRECT_MAPPED, LRU);

	// ±í2
	//Cache cache(4096, 8, DIRECT_MAPPED, LRU);
	//Cache cache(4096, 16, DIRECT_MAPPED, LRU);
	//Cache cache(4096, 32, DIRECT_MAPPED, LRU);
	
	// ±í3
	//Cache cache(4096, 16, DIRECT_MAPPED, LRU);
	//Cache cache(4096, 16, WAY2, LRU);
	//Cache cache(4096, 16, WAY4, LRU);
	//Cache cache(4096, 16, WAY8, LRU);

	//Cache cache(4096, 16, WAY2, FIFO);
	//Cache cache(4096, 16, WAY4, FIFO);
	//Cache cache(4096, 16, WAY8, FIFO);

	//Cache cache(4096, 16, WAY2, RANDOM);
	//Cache cache(4096, 16, WAY4, RANDOM);
	//Cache cache(4096, 16, WAY8, RANDOM);

	//cache.getInput();
	
	cout<<cache.getInformation()<<"\n";
	 cache.getInput();

	ifstream in_file;
	string line;
	in_file.open(path+"trace.txt");
	//stringstream buffer;
	//buffer << in_file.rdbuf();
	//string lines(buffer.str());

	ofstream out_file(path+"res.txt", ios::out);
	string out_;
	unsigned long index_line = 1;
	if (in_file.is_open() == 1) {
		cout << "open file from path: " << path << " successfully" << "\n";
		while (!in_file.eof()) {
			getline(in_file, line);
			if (index_line % 10000 == 0) {
				cout << "current line index is: " << index_line << "\n";
			}
			cache.process(line);
			index_line++;
		}
		out_ += "hit_accuracy of " + to_string(cache.getAccuracy()) + "\n";
		cout << "hit_numL: " << cache.getNumHit() << " " << "visit_times: "<<cache.getNumLoad() << "\n";
		cout << "hit_accuracy of " << cache.getAccuracy() << "\n";

	}
	else {
		cout<< "open file from path: " << path << " failed" << "\n";
		cout << "program has ended!" << "\n";
	}
	in_file.close();
	out_file << out_;
	out_file.close();
}
//74.5344