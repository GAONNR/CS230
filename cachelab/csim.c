
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"

typedef struct{
	int valid;
	int tag;
	int actime;
}line;

typedef struct{
	line *lines;
}set;

typedef struct{
	int nset;
	int nline;
	set *sets;
}type_cache;

int s, E, b, vflag, hflag;
char tracefile[100];
int hits, misses, evics;

void take_argu(int argc, char *argv[]);
int process(type_cache *cache, char *buf);
int getbit(int addr, int set_or_tag);
void mod_actime(type_cache *cache, int set, int n);
void printHelp();
int atox();
//0: MISS 1: HIT 2: MH 3: ME 4:MEH

int main(int argc, char *argv[])
{
	FILE *trace;
	type_cache cache;
	char buf[100];
	int ret, i, j;

	take_argu(argc, argv);
	if (hflag){
		printHelp();
		return 0;
	}

	trace = fopen(tracefile, "r");

	//make the spaces for cache
	cache.nset = 2 << s;
	cache.nline = E;
	cache.sets = (set *)malloc(sizeof(set) * cache.nset);

	for (i = 0; i < cache.nset; i++){
		cache.sets[i].lines = (line *)malloc(sizeof(line) * cache.nline);

		for (j = 0; j < cache.nline ; j++)
			cache.sets[i].lines[j].valid = 0;
	}
	//end of making cache

	while (fgets(buf, 100, trace) != NULL){
		if (buf[0] != ' ') continue;

		ret = process(&cache, buf);

		if (vflag){
			switch (ret){
				case 0:
					printf("%s miss\n", buf + 1);
					break;
				case 1:
					printf("%s hit\n", buf + 1);
					break;
				case 2:
					printf("%s miss hit\n", buf + 1);
					break;
				case 3:
					printf("%s miss eviction\n", buf + 1);
					break;
				case 4:
					printf("%s miss eviction hit\n", buf + 1);
					break;
				default:
					break;
			}
		}
	}

	fclose(trace);
	printSummary(hits, misses, evics);
	return 0;
}

void take_argu(int argc, char *argv[]){
	char chk_h[] = "-h", chk_v[] = "-v", chk_s[] = "-s";
	char chk_E[] = "-E", chk_b[] = "-b", chk_t[] = "-t";
	int i;

	for (i = 1; i < argc; i++){
		if (!strcmp(argv[i], chk_h)){
			hflag = 1;
		}
		else if (!strcmp(argv[i], chk_v)){
			vflag = 1;
		}
		else if (!strcmp(argv[i], chk_s)){
			i++;
			s = atoi(argv[i]);
		}
		else if	(!strcmp(argv[i], chk_E)){
			i++;
			E = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], chk_b)){
			i++;
			b = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], chk_t)){
			i++;
			strcpy(tracefile, argv[i]);
		}
	}
}

int process(type_cache *cache, char *buf){
	int i, addr, set, tag, max = 0, tmp = 0;
	char op;

	op = buf[1];
	addr = atox(buf);

	set = getbit(addr, 0);
	tag = getbit(addr, 1);

	for (i = 0; i < cache->nline; i++){
		if (cache->sets[set].lines[i].valid && cache->sets[set].lines[i].tag == tag){
			//hit
			hits++;
			if (op == 'M') hits++;

			mod_actime(cache, set, i);
			return 1;
		}
	}

	misses++;
	for (i = 0; i < cache->nline; i++){
		if (!cache->sets[set].lines[i].valid){
			//empty line: no eviction
			cache->sets[set].lines[i].valid = 1;
			cache->sets[set].lines[i].tag = tag;

			mod_actime(cache, set, i);
			if(op == 'M'){
				hits++;
				return 2;
			}
			return 0;
		}
	}

	evics++;
	for (i = 0; i < cache->nline; i++){
		if (cache->sets[set].lines[i].actime > max){
			max = cache->sets[set].lines[i].actime;
			tmp = i;
		}
	}
	cache->sets[set].lines[tmp].valid = 1;
	cache->sets[set].lines[tmp].tag = tag;
	mod_actime(cache, set, tmp);
	if (op == 'M'){
		hits++;
		return 4;
	}
	return 3;
}

int getbit(int addr, int set_or_tag){
	int bitmask, temp;

	if (!set_or_tag){
		temp = addr >> b;
		bitmask = ~((~0) << s);
		bitmask = bitmask & temp;
	}
	else{
		temp = addr >> (s + b);
		bitmask = ~((~0) << (31 - s - b));
		bitmask = bitmask & temp;
	}

	return bitmask;
}

//least recently used
void mod_actime(type_cache *cache, int set, int n){
	int i;
	for (i = 0; i < cache->nline; i++){
		if (cache->sets[set].lines[i].valid)
			cache->sets[set].lines[i].actime++;
	}
	cache->sets[set].lines[n].actime = 0;
}

void printHelp(){
	printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
	printf("• -h: Optional help flag that prints usage info\n");
	printf("• -v: Optional verbose flag that displays trace info\n");
	printf("-s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
	printf("• -E <E>: Associativity (number of lines per set)\n");
	printf("• -b <b>: Number of block bits (B = 2b is the block size)\n");
	printf("• -t <tracefile>: Name of the valgrind trace to replay\n");
}

int atox(char *buf){
	int x = 3, addr = 0;

	while(buf[x] != ','){
		if(buf[x] >= '0' && buf[x] <= '9')
			addr = addr * 0x10 + buf[x] - '0';
		else if(buf[x] >= 'a' && buf[x] <= 'f')
			addr = addr * 0x10 + buf[x] - 'a' + 10;

		x++;
	}

	return addr;
}
