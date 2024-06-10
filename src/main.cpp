#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "AdaFinder.h"
#include "BaselineFinder.h"
#include "BicliqueFinder.h"
#include "ParAdaFinder.h"

void PrintOptions() {
  fprintf(stderr, "\n Options: \n");
  fprintf(stderr, "  -s \t[1-7] algorithm selection <default = 7>\n");
  fprintf(stderr, "\t1 - MBEA\n");
  fprintf(stderr, "\t2 - iMBEA\n");
  fprintf(stderr, "\t3 - FMBE\n");
  fprintf(stderr, "\t4 - PMBE\n");
  fprintf(stderr, "\t5 - AdaMBE (Dynamic subgraph using bitmap only)\n");
  fprintf(stderr, "\t6 - AdaMBE (Neighbor caching only)\n");
  fprintf(stderr, "\t7 - AdaMBE\n");
  fprintf(stderr, "\t8 - ParAdaMBE (Parallel version of AdaMBE)\n");
  fprintf(stderr, "  -o \t[1-3] algorithm selection <default = 2>\n");
  fprintf(stderr, "\t1 - random order\n");
  fprintf(stderr, "\t2 - increasing order based on degree\n");
  fprintf(stderr, "\t3 - unilateral order mentioned in ooMBEA\n");
  fprintf(stderr, "  -i <filename>  input bigraph filename (*.adj) \n");
  fprintf(stderr, "  -t <number>    thread number <default = 1> \n");
  fprintf(stderr,
          "  -x <value>     least number of left vertices in biclique <default "
          "= 1>\n");
  fprintf(stderr,
          "  -y <value>     least number of right vertices in biclqiue "
          "<default = 1>\n");

  fprintf(stderr, "\n");
}

void PrintMemory(char *fn = nullptr) {
  FILE *fp = (fn == nullptr || strlen(fn) == 0) ? stdout : fopen(fn, "a+");
  int pid = getpid();
  unsigned int vmhwm = get_proc_vmhwm(pid);
  if (fn != nullptr) fseek(fp, 0, SEEK_END);
  fprintf(fp, "Memory Usage : %lfMB\n", vmhwm / 1000.0);
  if (fn != NULL) fclose(fp);
}

void ExpFinderTest(char *graph_name, int finder_sel, int thread_num, char *fn,
                   int lb = 1, int rb = 1, double miu = 1,
                   OrderEnum order = RInc) {
  BiGraph *G = new BiGraph(graph_name);
  if (lb != 1 || rb != 1) {
    G->Prune1H(lb, rb);
    // G->Prune2H(lb, rb);
  }

  if (lb < rb || G->GetLSize() < G->GetRSize()) {
    G->Transpose();
    std::swap(lb, rb);
  }

  if(G->GetLDegree() > G->GetRDegree()*100){
    G->Transpose();
    std::swap(lb, rb);
  }

 G->Reorder(order);
  // G->PrintProfile();
  BicliqueFinder *finder;
  switch (finder_sel) {
    // baseline approach
    case 0:
      finder = new MineLMBCFinder(G);
      break;
    case 1:
      finder = new MbeaFinder(G);
      break;
    case 2:
      finder = new ImbeaFinder(G);
      break;
    case 3:
      finder = new FmbeFinder(G);
      break;
    case 4:
      finder = new PmbeFinder(G);
      break;
    case 5:
      finder = new AdaMBEFinder_DSB(G);
      break;
    case 6:
      finder = new AdaMBEFinder_NC(G);
      break;
    case 7:
      finder = new AdaMBEFinder(G, order);
      break;
    case 8:
      finder = new ParAdaMBEFinder(G, thread_num);
  }
  finder->Execute(lb, rb);
  if (fn != nullptr) {
    // FILE *fp = fopen(fn, "a+");
    // fprintf(fp, "Graph : %s\t Finder:%d\n", graph_name, finder_sel);
    // fclose(fp);
  }
  // PrintMemory(fn);
  printf("%s, ", extractFileName(graph_name));
  finder->SetThreads(thread_num);
  finder->PrintResult(fn);
  delete finder;
  delete G;
}

int main(int argc, char *argv[]) {
  int opt;
  int thread_num = 1;
  double miu = 1;
  opterr = 0;
  char graph_name[80];
  char *output_fn = nullptr;
  int sel = 7, lb = 1, rb = 1;
  bool graph_name_flag = false;
  OrderEnum order = RInc;

  while ((opt = getopt(argc, argv, "i:s:t:x:y:u:o:l")) != -1) {
    switch (opt) {
      case 'i':
        memcpy(graph_name, optarg, strlen(optarg) + 1);
        graph_name_flag = true;
        break;
      case 's':
        sel = atoi(optarg);
        break;
      case 'x':
        lb = atoi(optarg);
        break;
      case 'y':
        rb = atoi(optarg);
        break;
      case 'l':
        output_fn = new char[100];
        sprintf(output_fn, "%s-log.txt", graph_name);
        break;
      case 't':
        thread_num = atoi(optarg);
        break;
      case 'o':
        order = (OrderEnum)atoi(optarg);
        break;
      case 'u':
        miu = atof(optarg);
        break;
      default:
        fprintf(stderr, "[Warning] Unknown operation. ASCII code: %d\n", opt);
        break;
    }
  }
  if (!graph_name_flag) {
    PrintOptions();
    return 0;
  }

  ExpFinderTest(graph_name, sel, thread_num, output_fn, lb, rb, miu, order);
}
