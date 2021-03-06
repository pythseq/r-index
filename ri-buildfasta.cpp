// ri-buildfasta.cpp - modified from ri-build.cpp by Taher Mun
// March 20 2018

#include <iostream>
#include <algorithm>
#include <chrono>

#include "utils.hpp"
#include "internal/r_index.hpp"

using namespace ri;
using namespace std;

string out_basename=string();
string bwt_alg="sais";
string input_file=string();
int sa_rate = 512;
bool sais=true;
ulint T = 0;//Build fast index with SA rate = T
bool fast = false;//build fast index
bool acgt = false;

void help(){
	cout << "ri-buildfasta: builds the r-index from a fasta file. Extension .ri is automatically added to output index file" << endl << endl;
	cout << "Usage: ri-buildfasta [options] <input_fasta_name>" << endl;
	cout << "   -o <basename>        use 'basename' as prefix for all index files. Default: basename is the specified input_file_name"<<endl;
	cout << "   -b <basename>        bwt algorithm (sais, bigbwt), default:sais"<<endl;
    cout << "   -acgt                strip out all non-ACGT characters" << endl;
	//cout << "   -fast                build fast index (O(occ)-time locate, O(r log(n/r)) words of space). By default, "<<endl;
	//cout << "                        small index is built (O(occ*log(n/r))-time locate, O(r) words of space)"<<endl;
	//cout << "   -sa_rate <T>         T>0. if used, build the fast index (see option -fast) storing T SA samples before and after each"<<endl;
	//cout << "                        BWT equal-letter run. O(r*T) words of space, O(occ(log(n/r)/T) + log(n/r))-time locate. "<<endl;
	cout << "   -divsufsort          use divsufsort algorithm to build the BWT (fast, 7.5n Bytes of RAM). By default,"<<endl;
	cout << "                        SE-SAIS is used (about 4 time slower than divsufsort, 4n Bytes of RAM)."<<endl;
	cout << "   <input_file_name>    input text file." << endl;
	exit(0);
}

std::string read_fasta(std::ifstream input_file) {
    std::string input;
    std::string line;
    /* newlines won't be added to index */
    while(getline(input_file, line)) {
        /* deliberately ignoring record name lines */
        if (line[0] != '>') {
            std::transform(line.begin(), line.end(), line.begin(), ::toupper);
            input += line;
        }
    }
    return input;
}

void parse_args(char** argv, int argc, int &ptr){

	assert(ptr<argc);

	string s(argv[ptr]);
	ptr++;

	if(s.compare("-o")==0){

		if(ptr>=argc-1){
			cout << "Error: missing parameter after -o option." << endl;
			help();
		}

		out_basename = string(argv[ptr]);
		ptr++;

	}else if(s.compare("-divsufsort")==0){

		sais = false;

	} else if (s.compare("-acgt") == 0) {
        acgt = true;
    /* } else if(s.compare("-fast")==0){

		fast=true;

	}else if(s.compare("-T")==0){

		T = atoi(argv[ptr]);

		if(T<=0){
			cout << "Error: parameter T must be T>0" << endl;
			help();
		}

		ptr++;
		fast=true;

	*/ } else if (s.compare("-b")==0) {
        if (ptr>=argc-1) {
			cout << "Error: missing parameter after -b option." << endl;
			help();
		}
		bwt_alg = string(argv[ptr]);
		ptr++;
    } else{
		cout << "Error: unrecognized '" << s << "' option." << endl;
		help();
	}

}

int main(int argc, char** argv){

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;


	//parse options

	out_basename.clear();
	input_file.clear();
	int ptr = 1;

	if(argc<2 || (std::find_if(argv, argv + argc, [](const char *el){return std::strcmp(el, "-h") == 0 || std::strcmp(el, "--help") == 0;}) != (argv + argc))) help();

	while(ptr<argc-1)
		parse_args(argv, argc, ptr);

	input_file = std::string(argv[ptr]);

	if(out_basename.compare("")==0)
		out_basename = std::string(input_file);



	cout << "Building r-index of input file " << input_file << endl;

    auto t1 = high_resolution_clock::now();
    std::string path;
    std::ofstream out;
    path = std::string(out_basename).append(".ri");
    out = std::ofstream(path);
    out.write((char*)&fast,sizeof(fast));
    r_index idx;
    if (bwt_alg == "sais") { // sais forward
        std::string input;
        input = read_fasta(std::ifstream(input_file));
        std::string path;
        std::cout << "building forward index using sais" << std::endl;
        idx.init_sais(input,true);
    } else if (bwt_alg == "bigbwt") { /* bigbwt forward */
        std::cout << "building forward index using bigbwt" << std::endl;
        idx = r_index<>();
        idx.init_bigbwt(input_file, acgt);
        // next part requires htslib
        std::string fai_path = std::string(out_basename).append(".1.ri");
        std::cout << "generating faidx of sequences, saving to" << fai_path << std::endl;
        ri::build_seqidx(input_file.c_str(), fai_path.c_str());
    } else if (bwt_alg == "from_bwt") {
        std::cout << "building forward index from existing bwt in "  << input_file << std::endl;
        idx = r_index<>();
        idx.from_bwt(input_file);
    } else {
        cerr << "invalid bwt algorithm specified. exiting..." << endl;
        exit(1);
    }
    auto t2 = high_resolution_clock::now();
    ulint total = duration_cast<duration<double, std::ratio<1>>>(t2 - t1).count();
    cout << "Build time : " << get_time(total) << endl;
    std::cout << "writing forward index to " << path << std::endl;
    idx.serialize(out);
}
