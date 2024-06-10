#include <iostream>

using namespace std;

#include "source/Lmbc.h"
#include "source/ParLmbc.h"
#include "source/undirectedgraph.h"
#include <unistd.h>
#include <chrono>
#define VMHWM_LINE 21
unsigned int get_proc_vmhwm(unsigned int pid){

	char file_name[64]={0};
	FILE *fd;
	char line_buff[512]={0};
	sprintf(file_name,"/proc/%d/status",pid);

	fd =fopen(file_name,"r");
	if(nullptr == fd){
		return 0;
	}

	char name[64];
	int vmhwm;
	for (int i=0; i<VMHWM_LINE-1;i++){
		fgets(line_buff,sizeof(line_buff),fd);
	}

    while(true)
    {
	    fgets(line_buff,sizeof(line_buff),fd);
	    sscanf(line_buff,"%s %d",name,&vmhwm);
        if(!strcmp(name, "VmHWM:"))
        {
            break;
        }
    }
    
    fclose(fd);

	return vmhwm;
}

void PrintMemory(char* fn = nullptr) {
  FILE* fp = (fn == nullptr || strlen(fn) == 0) ? stdout : fopen(fn, "a+");
  int pid = getpid();
  unsigned int vmhwm = get_proc_vmhwm(pid);
  if (fn != nullptr) fseek(fp, 0, SEEK_END);
  fprintf(fp, "Memory Usage : %lfMB\n", vmhwm / 1000.0);
  if (fn != NULL) fclose(fp);
}

char* extractFileName(char* path) {
    size_t len = std::strlen(path);
    const char* start = path;
    const char* end = path + len;

    // Find last slash position
    for (const char* p = end - 1; p >= start; --p) {
        if (*p == '/') {
            start = p + 1;
            break;
        }
    }

    // Find .adj position
    for (const char* p = start; p < end; ++p) {
        if (*p == '.') {
            if (std::strncmp(p, ".adj", 4) == 0) {
                size_t extractedLen = p - start;
                char* extractedName = new char[extractedLen + 1];
                std::strncpy(extractedName, start, extractedLen);
                extractedName[extractedLen] = '\0';
                return extractedName;
            } else {
                break;
            }
        }
    }

    return nullptr;
}

double utils::collection_time = 0;
int main(int argc, char** argv){

	undirectedgraph g;

  g.readInBiAdjList(argv[1]);
	// g.readInBiEdgeList(argv[1]);

  // g.readInAdjList(argv[1]);
	// cout << g.numV() << "\n";

	// cout << g.numE() << "\n";


  int thread_num = 64;
  if(argc >= 3)thread_num = atoi(argv[2]);
	//cout << "Sequential MBE algorithm\n\n\n";

	// Lmbc lmbc(g);

	// lmbc.run("lmbc");

	// cout << "number of maximal biclique found: " << lmbc.getCount() << "\n\n";

	// cout << "Parallel MBE algorithm: " << argv[1] << endl;;

	ParLmbc parlmbc(g);
  auto start = chrono::high_resolution_clock::now();
  parlmbc.runParMBE("parlmbc", thread_num);
  //parlmbc.runParMBESeq("parlmbcseq");
  // parlmbc.printTime();
  //cout << "collection time: " << utils::get_collection_time() << endl;
  auto end = chrono::high_resolution_clock::now();
  auto elapsed = chrono::duration<double>(end - start);
  char* file = extractFileName(argv[1]);

  cout << file << ", ParMBE" << thread_num << ", " << elapsed.count() << ", "
       << get_proc_vmhwm(getpid()) / 1000.0 << ", " << parlmbc.getCount()
       << endl;
  // cout << "number of maximal biclique found: " << parlmbc.getCount() <<
  // "\n\n"; auto end = chrono::high_resolution_clock::now(); auto elapsed =
  // chrono::duration<double>(end - start); PrintMemory(); cout << "Total
  // processing time: " << elapsed.count() << "s" <<endl;
}
