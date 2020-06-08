#include "Cache.h"
using namespace NameSpaceCache;


void Cache::init() {
	for (ul i = 0; i < this->MAX_CACHE_LINE; i++) {
		this->cache_item[i].reset();
		this->is_valid[i] = false;
	}
}
Cache::Cache() { srand(time(0)); }
Cache::Cache(ul cache_sizes = 1024, ul block_size = 8,
	Asso associativity = DIRECT_MAPPED, Repl replacement_policy = NONE) {
	srand(time(0));
	this->cache_sizes = cache_sizes;
	this->block_size = block_size;
	//this->i_cache_set = i_cache_set;
	this->associativity = associativity;
	this->replacement_policy = replacement_policy;

	this->blocks_in_a_set = (ul)associativity;
	this->num_of_blocks = cache_sizes / block_size;
	this->num_of_sets = cache_sizes / this->blocks_in_a_set / block_size;
	//this->calNumOfBlocks();
	//this->calNumOfSets();
	if (this->associativity == DIRECT_MAPPED) {
		this->replacement_policy = NONE;
	}
	this->calBit();
	this->init();
	/*if (associativity != DIRECT_MAPPED) {
		for (int i = 0; i < MAX_CACHE_LINE; i++) {
			lru[i].set_priority_q((ul)associativity);
		}
	}*/
}

bool Cache::isHit(std::bitset<32>& address) {
	bool ret = false;
	// get index of line
	std::bitset<32> index_of_block;
	if (this->associativity == DIRECT_MAPPED) {
		// get index of block
		for (int i = this->bits_need_a_block, j = 0; i < this->bits_need_all_blocks + this->bits_need_a_block; i++, j++) {
			index_of_block[j] = address[i];
		}
		this->index_of_current_block = index_of_block.to_ulong();

		if (this->is_valid[index_of_current_block] == true) {
			ret = true;
			for (int i = 31, j = 31; i > (31ul - this->bits_need_tag); i--, j--) {
				if (address[i] != this->cache_item[index_of_current_block][j]) {
					ret = false;
					break;
				}
			}
		}
	}
	else {
		ul beg_ = this->bits_need_a_block;
		ul end_ = this->bits_need_a_block + this->bits_need_sets;
		for (int i = beg_, j = 0; i < end_; i++, j++) {
			index_of_block[j] = address[i];
		}
		this->index_of_current_block = index_of_block.to_ulong();//index of set
		//std::cout << this->index_of_current_block << "\n";
		beg_ = this->blocks_in_a_set * this->index_of_current_block;
		end_ = beg_ + this->blocks_in_a_set;
		for (int k = beg_; k < end_; k++) {
			if (this->is_valid[k] == true) {
				ret = true;
				for (int i = 31; i > (31ul - this->bits_need_tag); i--) {
					if (address[i] != this->cache_item[k][i]) {
						ret = false;
					}
				}
				if (ret == true) {
					if (this->replacement_policy == LRU) {
						this->lru[this->index_of_current_block].node[k - beg_].num = this->timestamp;////
						this->timestamp++;
						//std::cout << this->timestamp << "\n";
					}
					break;
				}

			}
		}

	}
	return ret;
}

void Cache::readDataFromMem(std::bitset<32> address) {
	for (int i = 31, j = 31; i > (31ul - this->bits_need_tag); i--, j--) {
		this->cache_item[this->index_of_current_block][j] = address[i];
	}
	this->is_valid[this->index_of_current_block] = true;
}

void Cache::readDataFromMemRandom(std::bitset<32> address) {
	ul index_replace = rand() % this->blocks_in_a_set;
	//std::cout << index_replace << std::"\n";

	ul write_place = this->index_of_current_block * this->blocks_in_a_set + index_replace;
	for (int i = 31, j = 31; i > (31ul - this->bits_need_tag); i--, j--) {
		this->cache_item[write_place][j] = address[i];
	}
	this->is_valid[write_place] = true;
}

void Cache::readDataFromMemFIFO(std::bitset<32> address) {
	ul beg_ = this->index_of_current_block * this->blocks_in_a_set;
	ul end_ = beg_ + this->blocks_in_a_set;
	for (int k = beg_; k < end_; k++) {
		if (this->is_valid[k] == false) {
			for (int i = 31; i > (31ul - this->bits_need_tag); i--) {
				this->cache_item[k][i] = address[i];
			}
			this->is_valid[k] = true;
			this->fifo[this->index_of_current_block].push(k);
			return;
		}
	}

	ul write_place = this->fifo[this->index_of_current_block].front();
	this->fifo[this->index_of_current_block].pop();
	for (int i = 31; i > (31ul - this->bits_need_tag); i--) {
		this->cache_item[write_place][i] = address[i];
	}
	//this->is_valid[write_place] = true;
	this->fifo[this->index_of_current_block].push(write_place);
}

void Cache::readDataFromMemLRU(std::bitset<32> address) {
	ul beg_ = this->index_of_current_block * this->blocks_in_a_set;
	ul end_ = beg_ + this->blocks_in_a_set;
	for (int k = beg_; k < end_; k++) {
		//std::cout << "this->is_valid[k]: " << this->is_valid[beg_] << "\n";
		if (this->is_valid[k] == false) {
			for (int i = 31; i > (31ul - this->bits_need_tag); i--) {
				this->cache_item[k][i] = address[i];
			}
			this->is_valid[k] = true;
			Node& tmp = this->lru[this->index_of_current_block].node[k - beg_];
			tmp.num = this->timestamp;
			this->timestamp++;

			//std::cout << this->is_valid[k] << "\n";
			//std::cout << this->timestamp << "\n";
			//std::cout << beg_ << " " << end_ << " " << k - beg_ << "\n\n";
			return;
		}
	}
	S_LRU& tmp = this->lru[this->index_of_current_block];
	ul write_place = 0;
	ul maxx = tmp.node[0].num;
	for (int i = 1; i < this->blocks_in_a_set; i++) {
		if (maxx > tmp.node[i].num) {
			write_place = i;
			maxx = tmp.node[i].num;
		}
	}
	//std::cout << write_place << std::endl;
	tmp.node[write_place].num = timestamp++;/////////////////////////////////////
	write_place += beg_;
	//this->timestamp++;
	//std::cout << this->timestamp << "\n";
	for (int i = 31; i > (31ul - this->bits_need_tag); i--) {
		this->cache_item[write_place][i] = address[i];
	}
	//this->is_valid[write_place] = true;
}


void Cache::process(std::string line) {
	using namespace std;
	// transfer to base 10
	//cout << line << "\n";
	stringstream ss(line);
	ul index = 0;
	ss >> hex >> index;
	bitset<32> address(index);

	bool is_hit = isHit(address);
	this->num_load++;
	if (is_hit == true) {
		this->num_hit++;
	}
	else {
		if (this->replacement_policy == NONE) {
			this->readDataFromMem(address);
		}
		else if (this->replacement_policy == RANDOM) {
			this->readDataFromMemRandom(address);
		}
		else if (this->replacement_policy == FIFO) {
			this->readDataFromMemFIFO(address);
		}
		else if (this->replacement_policy == LRU) {
			//cout << "Î´ÃüÖÐ" << '\n';
			this->readDataFromMemLRU(address);
		}
	}

	return;
}