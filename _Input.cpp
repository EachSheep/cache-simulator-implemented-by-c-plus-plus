#include "Cache.h"
using namespace NameSpaceCache;

void Cache::getInput(void) {
	using namespace std;
	this->inCacheSize();
	this->inCacheBlockSize();
	this->calNumOfBlocks();
	this->inMappingMethod();
	this->calNumOfSets();
	this->inReplacePolicy();
	if (this->associativity == DIRECT_MAPPED) {
		this->replacement_policy = NONE;
	}
	this->calBit();
	this->init();
}


void Cache::inCacheSize() {
	using namespace std;
	puts("Please input the number of the cache size(Unit:KB), only num 1024,2048,4096,8192,16384 bytes is ok");
	cin >> this->cache_sizes;
	if (cin.fail()) {
		cin.clear();
		cin.sync();
	}
	if (!(this->cache_sizes == 1024
		|| this->cache_sizes == 2048
		|| this->cache_sizes == 4096
		|| this->cache_sizes == 8192
		|| this->cache_sizes == 16384)
		|| (cache_sizes & (~cache_sizes + 1)) != cache_sizes)
	{
		inCacheSize();
	}
}
void Cache::inCacheBlockSize() {
	using namespace std;
	puts("Please input the number of the cacheline size(Unit:Byte), only num 8,16,32 is ok");
	cin >> this->block_size;
	if (cin.fail()) {
		cin.clear();
		cin.sync();
	}
	if (!(block_size == 8
		|| block_size == 16
		|| block_size == 32)
		|| (block_size & (~block_size + 1))
		!= block_size)
	{
		inCacheBlockSize();
	}
}
void Cache::calNumOfBlocks() {
	this->num_of_blocks = cache_sizes / block_size;
}
void Cache::inMappingMethod() {
	using namespace std;
	ul temp = 0;
	puts("Please input the method of assoiativity between main memory and cache:");
	puts("direct_mapped:input 1");
	puts("set_associative:input 2 or 4 or 8");//2,4,8 
	cin >> temp;
	if (cin.fail()) {
		cin.clear();
		cin.sync();
	}
	switch (temp)
	{
	case 1:
		this->associativity = DIRECT_MAPPED;
		this->blocks_in_a_set = 0;
		break;
	case 2:
		this->associativity = WAY2;
		this->blocks_in_a_set = WAY2;
		break;
	case 4:
		this->associativity = WAY4;
		this->blocks_in_a_set = WAY4;
		break;
	case 8:
		this->associativity = WAY8;
		this->blocks_in_a_set = WAY8;
		break;
	default:
		inMappingMethod();
	}
}
void Cache::calNumOfSets() {
	if (this->associativity == DIRECT_MAPPED) {
		this->num_of_sets = cache_sizes / block_size;
	}
	else {
		this->num_of_sets = cache_sizes / this->blocks_in_a_set / block_size;
	}
}
void Cache::inReplacePolicy() {
	using namespace std;
	ul temp;
	puts("Input the replacement policy:");
	puts("Random:input 1");
	puts("FIFO:input 2");
	puts("LRU:input 3");
	cin >> temp;
	if (cin.fail()) {
		cin.clear();
		cin.sync();
	}
	switch (temp)
	{
	case 1:
		this->replacement_policy = RANDOM;
		break;
	case 2:
		this->replacement_policy = FIFO;
		break;
	case 3:
		this->replacement_policy = LRU;
		break;
	default:
		inReplacePolicy();
	}
}
void Cache::calBit() {
	int tmp = this->block_size;
	int cnt = 0;
	while (tmp > 0) {
		tmp >>= 1;
		cnt++;
	}
	cnt--;
	this->bits_need_a_block = cnt;

	tmp = this->num_of_blocks;
	cnt = 0;
	while (tmp > 0) {
		tmp >>= 1;
		cnt++;
	}
	cnt--;
	this->bits_need_all_blocks = cnt;

	tmp = this->num_of_sets;
	cnt = 0;
	while (tmp > 0) {
		tmp >>= 1;
		cnt++;
	}
	cnt--;
	this->bits_need_sets = cnt;

	this->bits_need_tag = 32 - this->bits_need_a_block;
	if (this->associativity == DIRECT_MAPPED) {
		this->bits_need_tag -= this->bits_need_all_blocks;
	}
	else {
		this->bits_need_tag -= this->bits_need_sets;
	}
}


typedef unsigned long ul;
ul Cache::getCacheSize() const {
	return this->cache_sizes;
}
ul Cache::getCacheLineSize() const {
	return this->block_size;
}
Asso Cache::getAsso() const {
	return this->associativity;
}
Repl Cache::getRepl() const {
	return this->replacement_policy;
}
ul Cache::getNumHit() const {
	return this->num_hit;
}
ul Cache::getNumLoad() const {
	return this->num_load;
}
void Cache::setCacheSize(ul i_cache_size) {
	this->cache_sizes = i_cache_size;
}
void Cache::setCacheLineSize(ul i_cache_line_size) {
	this->block_size = i_cache_line_size;
}
void Cache::setAsso(Asso asso) {
	this->associativity = asso;
}
void Cache::setRepl(Repl repl) {
	this->replacement_policy = repl;
}





std::string Cache::getInformation() {
	std::stringstream ss;
	std::string ret = "";
	std::string tmp;
	ret += "Sizes: ";
	ss << this->cache_sizes;
	ss >> tmp;
	ret += tmp + " bytes\n";
	ret += "Block Size: ";
	ss.clear();
	ss << this->block_size;
	ss >> tmp;
	ret += tmp + " bytes\n";
	ret += "Associativity: ";
	ss.clear();
	ss << this->associativity;
	ss >> tmp;
	if (tmp == "1") {
		ret += "DIRECT_MAPPED\n";
	}
	else if (tmp == "2") {
		ret += "2-way\n";
	}
	else if (tmp == "4") {
		ret += "4-way\n";
	}
	else if (tmp == "8") {
		ret += "8-way\n";
	}
	ret += "Replacement Policy: ";
	ss.clear();
	ss << this->replacement_policy;
	ss >> tmp;
	if (tmp == "1") {
		ret += "RANDOM\n";
	}
	else if (tmp == "2") {
		ret += "FIFO\n";
	}
	else if (tmp == "3") {
		ret += "LRU\n";
	}
	else if (tmp == "4") {
		ret += "NONE\n";
	}
	return ret;
}
