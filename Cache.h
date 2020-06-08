#pragma once
#include<bitset>
#include<iostream>
#include<sstream>
#include<queue>
#include<cmath>
#include<random>
#include <cstdlib>
#include<ctime>
#include<vector>
#include<string>

namespace NameSpaceCache {

	enum Associativities { DIRECT_MAPPED = 1, WAY2 = 2, WAY4 = 4, WAY8 = 8 };
	typedef enum Associativities Asso;
	enum ReplacementPolicy { RANDOM = 1, FIFO, LRU, NONE };
	typedef enum ReplacementPolicy Repl;

	class Cache {
		typedef unsigned long ul;
	public:
		const static ul MAX_CACHE_LINE = 2048;//16 * 1024 / 8;

	private:
		ul cache_sizes = 0; //1k,2k,4k,8k,16k(16384 bytes)
		ul block_size = 0;// 8,16,32, bytes
		ul num_of_blocks = 0;// cache_sizes=block_size*num_of_blocks

	private:
		ul blocks_in_a_set = 0;//1-way(Direct_MAPPED),2-way,4-way,8-way
		ul num_of_sets = 0;//num_of_sets= cache_sizes/i_cache_set/block_size
		Asso associativity = DIRECT_MAPPED;

	private:
		Repl replacement_policy = RANDOM;

	private:
		ul bits_need_a_block = 0; 
		ul bits_need_all_blocks = 0;	 
		ul bits_need_sets = 0;
		ul bits_need_tag = 0;
		std::bitset<32> cache_item[MAX_CACHE_LINE];
		bool is_valid[MAX_CACHE_LINE];

		std::queue<ul> fifo[MAX_CACHE_LINE];

		struct Node {
			ul index = 0;
			ul num = 0;
			/*friend bool operator<(const Node& pir1, const Node& pir2) {
				return pir1.num > pir2.num;
			}*/
			Node(ul index = 0, ul num = 0) {
				this->index = index;
				this->num = num;
			}
		};
		struct S_LRU {
			Node node[8];
			S_LRU() {
				for (int i = 0; i < 8; i++) {
					this->node[i].index = i;
				}
			}
			/*void set_priority_q(ul way) {
				for (int i = 0; i < way; i++) {
					this->pq.push(node[i]);
				}
			}*/
			//std::priority_queue < Node> pq;
		};
		S_LRU lru[MAX_CACHE_LINE];
		ul timestamp = 0;

	private:
		ul num_load = 0;	// the total number of load
		ul num_hit = 0;	// the total number of hit

	private:
		void init();
	public:

		Cache();
		Cache(ul cache_sizes, ul block_size, Asso asso, Repl repl);

	private:
		void inCacheSize();
		void inCacheBlockSize();
		void calNumOfBlocks();
		void inMappingMethod();
		void calNumOfSets();
		void inReplacePolicy();
		void calBit();

	public:
		void getInput(void);
		ul getCacheSize() const;
		ul getCacheLineSize() const;
		Asso getAsso() const;
		Repl getRepl() const;
		ul getNumHit() const;
		ul getNumLoad() const;
		void setCacheSize(ul  cache_sizes);
		void setCacheLineSize(ul block_size);
		void setAsso(Asso asso);
		void setRepl(Repl repl);
		~Cache() {}

	private:
		ul index_of_current_block = 0;
		bool isHit(std::bitset<32>& address);
		void readDataFromMem(std::bitset<32> address);
		void readDataFromMemRandom(std::bitset<32> address);
		void readDataFromMemFIFO(std::bitset<32> address);
		void readDataFromMemLRU(std::bitset<32> address);

		// 总的处理函数的代码, 调用上面private的四个函数
	public:
		void process(std::string line);
	private:
		double accuracy = 0;
	public:
		double getAccuracy() {
			return this->num_hit * 1. / this->num_load;
		}
		std::string getInformation();
	};
}