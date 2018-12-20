//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

// Modified from https://code.google.com/p/word2vec/
// @chenbingjin 2016-05-16

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <map>
#include <time.h>
#include <unistd.h>
using namespace std;

#define MAX_STRING 100
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40
#define pi 3.1415926535897932384626433832795

const int vocab_hash_size = 30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary

typedef float real;                    // Precision of float numbers

struct vocab_word {
  long long cn;  //��Ƶ
  int *point;    //huffman�����Ӧ�ڽڵ��·��
  char *word, *code, codelen; //���ʣ���Ӧhuffman���룬���볤�ȣ�
};

char train_file[MAX_STRING], triplet_file[MAX_STRING], output_file[MAX_STRING];
char save_vocab_file[MAX_STRING], read_vocab_file[MAX_STRING];
struct vocab_word *vocab; //�ʻ��
int binary = 0, cbow = 1, debug_mode = 2, window = 5, min_count = 5, num_threads = 12, min_reduce = 1;
int *vocab_hash;  //�ʻ��ϣ�����ڿ��ٲ��ң��洢ÿ�����ڴʻ�������λ�á�
long long vocab_max_size = 1000, vocab_size = 0, layer1_size = 100; //����ά��
long long train_words = 0, word_count_actual = 0, iter = 5, file_size = 0, classes = 0;
real alpha = 0.025, starting_alpha, sample = 1e-3;
//�ֱ��Ӧ���ʵ��������ڽڵ���������������ʵ�������sigmoid�����Ľ��Ƽ����
real *syn0, *syn1, *syn1neg, *expTable;

clock_t start;
// hierarchical softmax ����NEG
int hs = 0, negative = 5;
const int table_size = 1e8;
int *table;

// ��ӹ�ϵ��Ϣ�������
real belta = 0.0005, gama = 0.8;
char buf[100000];
int relation_num;
map<string,int> relation2id;
map<int,vector<pair<int,int> > > triplets;
vector<vector<double> > relation_vec; // ��ϵ����
// �����
double rand(double min, double max)
{
    return min + (max-min)*rand()/(RAND_MAX + 1.0);
}
// ��̬�ֲ�
double normal(double x, double miu,double sigma)
{
    return 1.0/sqrt(2*pi)/sigma*exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
}
// ��[min,max]����������̬�ֲ�������
double randn(double miu,double sigma, double min ,double max)
{
    double x,y,dScope;
    do{
        x=rand(min,max);
        y=normal(x,miu,sigma);
        dScope=rand(0.0,normal(miu,miu,sigma));
    }while(dScope>y);
    return x;
}


//�������㷨����Ȩ����˼�롣ÿ���ʵ�Ȩ��Ϊl(w) = [counter(w)]^(3/4) / sum([counter(u)]^(3/4))��u���ڴʵ�D
//  ÿ���ʶ�Ӧһ���߶�, ��[0,1]�Ⱦ��뻮�ֳ�10^8��ÿ������һ���������r��Table[r]����һ��������
void InitUnigramTable() {
  int a, i;
  double train_words_pow = 0;
  double d1, power = 0.75;
  table = (int *)malloc(table_size * sizeof(int));
  // �����ʱ�ͳ����Ȩ��
  for (a = 0; a < vocab_size; a++) train_words_pow += pow(vocab[a].cn, power);
  i = 0;
  d1 = pow(vocab[i].cn, power) / train_words_pow;
  // �����ʱ�Ϊÿ���ʷ���table�ռ�
  for (a = 0; a < table_size; a++) {
    table[a] = i;
    if (a / (double)table_size > d1) {
      i++;
      d1 += pow(vocab[i].cn, power) / train_words_pow;
    }
    if (i >= vocab_size) i = vocab_size - 1;
  }
}

// Reads a single word from a file, assuming space + tab + EOL to be word boundaries
// ���ļ��ж�ȡһ����
void ReadWord(char *word, FILE *fin) {
  int a = 0, ch;
  while (!feof(fin)) {
    ch = fgetc(fin);  //��ȡһ���ַ�
    if (ch == 13) continue; //�س���
    if ((ch == ' ') || (ch == '\t') || (ch == '\n')) {
      if (a > 0) {
        if (ch == '\n') ungetc(ch, fin); //�˻�һ���ַ����ļ�ָ������һλ
        break;
      }
      if (ch == '\n') {
        strcpy(word, (char *)"</s>");
        return;
      } else continue;
    }
    word[a] = ch;
    a++;
    if (a >= MAX_STRING - 1) a--;   // Truncate too long words
  }
  word[a] = 0;
}

// Returns hash value of a word
// ���شʵ�hashֵ
int GetWordHash(char *word) {
  unsigned long long a, hash = 0;
  for (a = 0; a < strlen(word); a++) hash = hash * 257 + word[a]; //257���ƣ�����ʵ�hashֵ
  hash = hash % vocab_hash_size;
  return hash;
}

// Returns position of a word in the vocabulary; if the word is not found, returns -1
// ���ش��ڴʱ��е�����λ�ã��Ҳ�������-1.
int SearchVocab(char *word) {
  unsigned int hash = GetWordHash(word);
  while (1) {
    if (vocab_hash[hash] == -1) return -1;
    if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
    hash = (hash + 1) % vocab_hash_size;
  }
  return -1;
}

// Reads a word and returns its index in the vocabulary
// ���ļ���һ���ʣ����ش��ڴʻ�������λ��
int ReadWordIndex(FILE *fin) {
  char word[MAX_STRING];
  ReadWord(word, fin);
  if (feof(fin)) return -1;
  return SearchVocab(word);
}

// Adds a word to the vocabulary
// ������ӵ��ʻ��
int AddWordToVocab(char *word) {
  unsigned int hash, length = strlen(word) + 1;
  if (length > MAX_STRING) { 
  	length = MAX_STRING; //�ʵĳ��Ȳ��ܳ�MAX_STRING
  	} 
  vocab[vocab_size].word = (char *)calloc(length, sizeof(char));
  strcpy(vocab[vocab_size].word, word);
  vocab[vocab_size].cn = 0;  //��ʼ��ƵΪ0
  vocab_size++;
  // Reallocate memory if needed
  if (vocab_size + 2 >= vocab_max_size) {
    vocab_max_size += 1000;
    vocab = (struct vocab_word *)realloc(vocab, vocab_max_size * sizeof(struct vocab_word));
  }
  hash = GetWordHash(word);
  while (vocab_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size; //���hashֵ��ͻ����������̽��Ŀ��Ŷ�ַ����˳�����²���
  vocab_hash[hash] = vocab_size - 1;
  return vocab_size - 1;
}

// Used later for sorting by word counts
// �ʱ�����ıȽ��㷨cmp�����ݴ�Ƶ����,����
int VocabCompare(const void *a, const void *b) {
    return ((struct vocab_word *)b)->cn - ((struct vocab_word *)a)->cn;
}

// Sorts the vocabulary by frequency using word counts
// ���ݴ�Ƶ����
void SortVocab() {
  int a, size;
  unsigned int hash;
  // Sort the vocabulary and keep </s> at the first position
  qsort(&vocab[1], vocab_size - 1, sizeof(struct vocab_word), VocabCompare);
  for (a = 0; a < vocab_hash_size; a++) vocab_hash[a] = -1;
  size = vocab_size;
  train_words = 0;
  for (a = 0; a < size; a++) {
    // Words occuring less than min_count times will be discarded from the vocab
    // ���ִ���̫�ٵĴ�ֱ�Ӷ�����min_count Ĭ��5
    if ((vocab[a].cn < min_count) && (a != 0)) {
      vocab_size--;
      free(vocab[a].word);
    } else {
      // Hash will be re-computed, as after the sorting it is not actual
      // ���¼���hashֵ
      hash=GetWordHash(vocab[a].word);
      while (vocab_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
      vocab_hash[hash] = a;
      train_words += vocab[a].cn; //�ܴ�Ƶ
    }
  }
  vocab = (struct vocab_word *)realloc(vocab, (vocab_size + 1) * sizeof(struct vocab_word));
  // Allocate memory for the binary tree construction
  for (a = 0; a < vocab_size; a++) {
    vocab[a].code = (char *)calloc(MAX_CODE_LENGTH, sizeof(char));
    vocab[a].point = (int *)calloc(MAX_CODE_LENGTH, sizeof(int));
  }
}

// Reduces the vocabulary by removing infrequent tokens
// ��С�ʻ���Ƴ���Ƶ��С�Ĵ�
void ReduceVocab() {
  int a, b = 0;
  unsigned int hash;
  for (a = 0; a < vocab_size; a++) if (vocab[a].cn > min_reduce) {
    vocab[b].cn = vocab[a].cn;
    vocab[b].word = vocab[a].word;
    b++;
  } else free(vocab[a].word);
  vocab_size = b;
  for (a = 0; a < vocab_hash_size; a++) vocab_hash[a] = -1;
  for (a = 0; a < vocab_size; a++) {
    // Hash will be re-computed, as it is not actual
    hash = GetWordHash(vocab[a].word);
    while (vocab_hash[hash] != -1) hash = (hash + 1) % vocab_hash_size;
    vocab_hash[hash] = a;
  }
  fflush(stdout);
  min_reduce++;
}

// Create binary Huffman tree using the word counts
// Frequent words will have short uniqe binary codes
// ���ݴ�Ƶ����huffman������ƵԽ�����Խ��
void CreateBinaryTree() {
  long long a, b, i, min1i, min2i, pos1, pos2, point[MAX_CODE_LENGTH];
  char code[MAX_CODE_LENGTH];
  long long *count = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
  long long *binary = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
  long long *parent_node = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
  for (a = 0; a < vocab_size; a++) count[a] = vocab[a].cn;
  for (a = vocab_size; a < vocab_size * 2; a++) count[a] = 1e15;
  pos1 = vocab_size - 1;
  pos2 = vocab_size;
  // Following algorithm constructs the Huffman tree by adding one node at a time
  for (a = 0; a < vocab_size - 1; a++) {
    // First, find two smallest nodes 'min1, min2'
    if (pos1 >= 0) {  //�ҵ�һС
      if (count[pos1] < count[pos2]) {
        min1i = pos1;
        pos1--;
      } else {
        min1i = pos2;
        pos2++;
      }
    } else {
      min1i = pos2;
      pos2++;
    }
    if (pos1 >= 0) {  //�ҵڶ�С
      if (count[pos1] < count[pos2]) {
        min2i = pos1;
        pos1--;
      } else {
        min2i = pos2;
        pos2++;
      }
    } else {
      min2i = pos2;
      pos2++;
    }
    count[vocab_size + a] = count[min1i] + count[min2i];
    parent_node[min1i] = vocab_size + a;
    parent_node[min2i] = vocab_size + a;
    binary[min2i] = 1;
  }
  // Now assign binary code to each vocabulary word
  for (a = 0; a < vocab_size; a++) {
    b = a;
    i = 0;
    while (1) {
      code[i] = binary[b];
      point[i] = b;
      i++;
      b = parent_node[b];
      if (b == vocab_size * 2 - 2) break;
    }
    vocab[a].codelen = i; // ���볤��
    vocab[a].point[0] = vocab_size - 2; //?
    for (b = 0; b < i; b++) {
      vocab[a].code[i - b - 1] = code[b];
      vocab[a].point[i - b] = point[b] - vocab_size;
    }
  }
  free(count);
  free(binary);
  free(parent_node);
}

// ��ѵ���ļ���ͳ��ÿ���ʵĴ�Ƶ
void LearnVocabFromTrainFile() {
  char word[MAX_STRING];
  FILE *fin;
  long long a, i;
  for (a = 0; a < vocab_hash_size; a++) vocab_hash[a] = -1;
  fin = fopen(train_file, "rb");
  if (fin == NULL) {
    printf("ERROR: training data file not found!\n");
    exit(1);
  }
  vocab_size = 0;
  AddWordToVocab((char *)"</s>");
  while (1) {
    ReadWord(word, fin);
    if (feof(fin)) break;
    train_words++;
    if ((debug_mode > 1) && (train_words % 100000 == 0)) {
      printf("%lldK%c", train_words / 1000, 13);
      fflush(stdout);
    }
    i = SearchVocab(word);
    if (i == -1) {
      a = AddWordToVocab(word);
      vocab[a].cn = 1;
    } else vocab[i].cn++;
    if (vocab_size > vocab_hash_size * 0.7) ReduceVocab(); //����ʻ���࣬��ɾ����Ƶ��
  }
  SortVocab();
  if (debug_mode > 0) {
    printf("Vocab size: %lld\n", vocab_size);
    printf("Words in train file: %lld\n", train_words);
  }
  file_size = ftell(fin);
  fclose(fin);
}
// ����ʻ��
void SaveVocab() {
  long long i;
  FILE *fo = fopen(save_vocab_file, "wb");
  for (i = 0; i < vocab_size; i++) fprintf(fo, "%s %lld\n", vocab[i].word, vocab[i].cn);
  fclose(fo);
}
//���ļ���ȡ�ʻ㣬���ļ��Ѿ�ͳ�ƺ�ÿ���ʵĴ�Ƶ
void ReadVocab() {
  long long a, i = 0;
  char c;
  char word[MAX_STRING];
  FILE *fin = fopen(read_vocab_file, "rb");
  if (fin == NULL) {
    printf("Vocabulary file not found\n");
    exit(1);
  }
  for (a = 0; a < vocab_hash_size; a++) vocab_hash[a] = -1;
  vocab_size = 0;
  while (1) {
    ReadWord(word, fin);
    if (feof(fin)) break;
    a = AddWordToVocab(word);
    fscanf(fin, "%lld%c", &vocab[a].cn, &c); //��ȡ��Ƶ�����з�
    i++;
  }
  SortVocab();
  if (debug_mode > 0) {
    printf("Vocab size: %lld\n", vocab_size);
    printf("Words in train file: %lld\n", train_words);
  }
  fin = fopen(train_file, "rb");
  if (fin == NULL) {
    printf("ERROR: training data file not found!\n");
    exit(1);
  }
  fseek(fin, 0, SEEK_END);
  file_size = ftell(fin);
  fclose(fin);
}

void ReadTriplets() {
    cout <<"reading triplets" << endl;
    FILE* f_kb = fopen(triplet_file,"r");
    char buf3[40960];
    char buf2[40960];
    char buf1[40960];
    char *word;
    int len = 0;
    int i = 0,j = 0;
    relation_num = 0;
    int bingo_num = 0;
    while (!feof(f_kb)) {
        fgets(buf,20480,f_kb);
        sscanf(buf,"%[^\t]\t%[^\t]\t%[^\t\n]\n", buf1,buf2,buf3);
        string s1=buf1;
        string s2=buf3;
        string s3=buf2; //relation
        len = s1.length();
        word = (char *)malloc((len+1)*sizeof(char));
        //cout << s1 << " " << s3 << " " << s2 << endl;
        s1.copy(word,len,0);
        i = SearchVocab(word);
        if (i == -1) {
            //cout<<"miss head entity:"<<s1<<endl;
            //pass_num += 1;
            continue;
        }else {
          //bingo_num += 1;
        }
        len = s2.length();
        word = (char *)malloc((len+1)*sizeof(char));
        s2.copy(word,len,0);
        j = SearchVocab(word);
        if (j == -1) {
          //cout<<"miss tail entity:"<<s2<<endl;
          continue;
        }
        if (relation2id.count(s3)==0) {
            relation2id[s3] = relation_num;
            relation_num++;
        }
        triplets[i].push_back(make_pair(relation2id[s3],j));
        bingo_num += 1;
    }
    printf("relation_num: %d\t bingo_num: %d\n", relation_num,bingo_num);
}

//��ʼ������ṹ
void InitNet() {
  long long a, b;
  unsigned long long next_random = 1;
  // ����ʵ������ڴ棬��ַ��128�ı���
  a = posix_memalign((void **)&syn0, 128, (long long)vocab_size * layer1_size * sizeof(real));
  if (syn0 == NULL) {printf("Memory allocation failed\n"); exit(1);}
  if (hs) {
    // ����huffman�ڲ��ڵ��ڴ�
    a = posix_memalign((void **)&syn1, 128, (long long)vocab_size * layer1_size * sizeof(real));
    if (syn1 == NULL) {printf("Memory allocation failed\n"); exit(1);}
    // ��ʼ��Ϊ0����
    for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++)
     syn1[a * layer1_size + b] = 0;
  }
  if (negative>0) {
    // ���为�����ʵ������ռ�
    a = posix_memalign((void **)&syn1neg, 128, (long long)vocab_size * layer1_size * sizeof(real));
    if (syn1neg == NULL) {printf("Memory allocation failed\n"); exit(1);}
    // ��ʼ��Ϊ0����
    for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++)
     syn1neg[a * layer1_size + b] = 0;
  }
  for (a = 0; a < vocab_size; a++) 
  	for (b = 0; b < layer1_size; b++) {
    	next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn0[a * layer1_size + b] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / layer1_size;
 	 }
    // ��ϵ������ʼ��
  relation_vec.resize(relation_num);
  for (int i = 0; i < relation_vec.size(); i++)
    relation_vec[i].resize(layer1_size);
  for (int i = 0; i < relation_num; i++)
  {
    for (int ii = 0 ; ii < layer1_size; ii++)
       relation_vec[i][ii] = randn(0,1.0/layer1_size,-6/sqrt(layer1_size),6/sqrt(layer1_size));
  }
  CreateBinaryTree();
}

// ѵ��ģ���̣߳�ѵ������
void *TrainModelThread(void *id) {
  long long a, b, d, cw, word, last_word, sentence_length = 0, sentence_position = 0;
  long long word_count = 0, last_word_count = 0, sen[MAX_SENTENCE_LENGTH + 1];
  long long l1, l2, c, target, label, local_iter = iter;
  unsigned long long next_random = (long long)id;
  real f, g;
  clock_t now;
  real *waddr = (real *)calloc(layer1_size, sizeof(real));  //��Ӧwi+r
  real *neu1 = (real *)calloc(layer1_size, sizeof(real));  //��ӦXw
  real *neu1e = (real *)calloc(layer1_size, sizeof(real)); //��Ӧerror�ۼ���
  FILE *fi = fopen(train_file, "rb");
  //ÿ���̶߳�Ӧһ���ı�
  fseek(fi, file_size / (long long)num_threads * (long long)id, SEEK_SET);
  while (1) {
    if (word_count - last_word_count > 10000) {
      word_count_actual += word_count - last_word_count;
      last_word_count = word_count;
      if ((debug_mode > 1)) {
        now=clock();
        printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, alpha,
         word_count_actual / (real)(iter * train_words + 1) * 100,
         word_count_actual / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
        fflush(stdout);
      }
      alpha = starting_alpha * (1 - word_count_actual / (real)(iter * train_words + 1));
      if (alpha < starting_alpha * 0.0001) alpha = starting_alpha * 0.0001;
    }
    if (sentence_length == 0) {
      while (1) {
        word = ReadWordIndex(fi); //��һ���ʣ��������ڴʻ�������λ��
        if (feof(fi)) break;
        if (word == -1) continue;
        word_count++;
        if (word == 0) break;
        // The subsampling randomly discards frequent words while keeping the ranking same
        // �Ը�Ƶ�ʽ����²������Ը���p������p = 1-[sqrt(t/f(w))+t/f(w)].���Ա������򲻱�
        // �ȼ���ran = sqrt(t/f(w))+t/f(w)������(0,1)�ϵ������r�����r>ran��������
        if (sample > 0) { 
          real ran = (sqrt(vocab[word].cn / (sample * train_words)) + 1) * (sample * train_words) / vocab[word].cn;
          next_random = next_random * (unsigned long long)25214903917 + 11;
          if (ran < (next_random & 0xFFFF) / (real)65536) continue;
        }
        sen[sentence_length] = word;
        sentence_length++;
        // ��1000���ʵ���һ������
        if (sentence_length >= MAX_SENTENCE_LENGTH) break;
      }
      sentence_position = 0;
    }
    // ��ǰ�̴߳�������������ֵ
    if (feof(fi) || (word_count > train_words / num_threads)) {
      word_count_actual += word_count - last_word_count;
      local_iter--;
      if (local_iter == 0) break;
      word_count = 0;
      last_word_count = 0;
      sentence_length = 0;
      fseek(fi, file_size / (long long)num_threads * (long long)id, SEEK_SET);
      continue;
    }
    word = sen[sentence_position];
    if (word == -1) continue;
    for (c = 0; c < layer1_size; c++) neu1[c] = 0;
    for (c = 0; c < layer1_size; c++) neu1e[c] = 0;
    next_random = next_random * (unsigned long long)25214903917 + 11;
	// �������0-5�Ĵ��ڴ�С
    b = next_random % window;
    if (cbow) {  //train the cbow architecture
      // in -> hidden
      cw = 0;
      for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
        c = sentence_position - window + a;
        if (c < 0) continue;
        if (c >= sentence_length) continue;
        last_word = sen[c];
        if (last_word == -1) continue;
        // �����Ĵʽ��������Ӻͣ��õ�Xw
        for (c = 0; c < layer1_size; c++) neu1[c] += syn0[c + last_word * layer1_size];
        cw++;
      }
      if (cw) {
      	// ��һ����
        for (c = 0; c < layer1_size; c++) neu1[c] /= cw;
        // hs������huffman
        if (hs) for (d = 0; d < vocab[word].codelen; d++) {
          f = 0;
          l2 = vocab[word].point[d] * layer1_size; //·�����ڲ��ڵ�
          // Propagate hidden -> output
          // ���ز㵽����㣬��������ݶ�
          // neu1 ��Ӧ Xw�� syn1��Ӧ�ڲ��ڵ������0
          for (c = 0; c < layer1_size; c++) f += neu1[c] * syn1[c + l2]; //�����ڻ�
          if (f <= -MAX_EXP) continue;
          else if (f >= MAX_EXP) continue;
          else f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];//sigmoid
          // 'g' is the gradient multiplied by the learning rate
          // �ڲ��ڵ�0���ݶ�(1-d-sigmoid(Xw��0))Xw��gΪǰ�沿��
          g = (1 - vocab[word].code[d] - f) * alpha;
          
          // Propagate errors output -> hidden
          // ���򴫲�����huffman���������ز�
          // �ۼӵ��ݶȸ�����
          for (c = 0; c < layer1_size; c++) neu1e[c] += g * syn1[c + l2];
          // Learn weights hidden -> output
          // �ڲ��ڵ��������
          for (c = 0; c < layer1_size; c++) syn1[c + l2] += g * neu1[c];
        }
        // NEGATIVE SAMPLING
        if (negative > 0) for (d = 0; d < negative + 1; d++) {
          if (d == 0) {
            target = word; //Ŀ���
            label = 1;   //������
          } else {//����������
            next_random = next_random * (unsigned long long)25214903917 + 11;
            target = table[(next_random >> 16) % table_size];
            if (target == 0) target = next_random % (vocab_size - 1) + 1;
            if (target == word) continue;
            label = 0;
          }
          l2 = target * layer1_size;
          f = 0;
          for (c = 0; c < layer1_size; c++) f += neu1[c] * syn1neg[c + l2]; //�ڻ�
          if (f > MAX_EXP) g = (label - 1) * alpha;
          else if (f < -MAX_EXP) g = (label - 0) * alpha;
          else g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * alpha; //sigmoid
          for (c = 0; c < layer1_size; c++) neu1e[c] += g * syn1neg[c + l2]; //�ۻ�����ݶ�
          for (c = 0; c < layer1_size; c++) syn1neg[c + l2] += g * neu1[c];  //������������
        }
        // hidden -> in
    	  // ���������ļ��������������  
        for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
          c = sentence_position - window + a;
          if (c < 0) continue;
          if (c >= sentence_length) continue;
          last_word = sen[c];
          if (last_word == -1) continue;
          for (c = 0; c < layer1_size; c++) syn0[c + last_word * layer1_size] += neu1e[c];
        }
        // ��ϵ��Ԫ��ѵ��
        // ��word��Ӧ��������Ԫ��(wi,r,t)��������5����Ԫ�飬
        // wi����syn0[word*layer1_size] , r����
        // @chenbingjin 2016-05-17

       // printf("word: %d \tsize: %d\n", word, triplets[word].size());
        if(triplets[word].size() > 0) for (int i = 0; i < triplets[word].size(); ++i)
        {
          int rid = triplets[word][i].first;
          int t = triplets[word][i].second;
          //printf("rid: %d\t t: %d\n", rid, t);
          for (c = 0; c < layer1_size; c++) neu1e[c] = 0; //�ݶ���������
          for (c = 0; c < layer1_size; c++) waddr[c] = syn0[c + word * layer1_size] + relation_vec[rid][c]; // Vwi + Vr
          //printf("waddr done...\n");
          for (d = 0; d < negative + 1; ++d) 
          {
            //  printf("d: %d\n", d);
             if (d == 0) {
                  target = t; //Ŀ���
                  label = 1;   //������
               } else {
                  //����������(wi,r,t')
                  next_random = next_random * (unsigned long long)25214903917 + 11;
                  target = table[(next_random >> 16) % table_size];
                  if (target == 0) target = next_random % (vocab_size - 1) + 1;
                  if (target == t) continue; //��t��ͬ
                  label = 0;
                }
                l2 = target * layer1_size;
                
                f = 0;
                for (c = 0; c < layer1_size; c++) f += waddr[c] * syn1neg[c + l2]; //�ڻ�
                if (f > MAX_EXP) g = (label - 1) * belta;
                else if (f < -MAX_EXP) g = (label - 0) * belta;
                else g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * belta; //sigmoid
                for (c = 0; c < layer1_size; c++) neu1e[c] += g * syn1neg[c + l2]; //�ۻ�����ݶ�
                for (c = 0; c < layer1_size; c++) syn1neg[c + l2] += gama* g * waddr[c];  //������������
          }
          for (c = 0; c < layer1_size; c++) syn0[c + word * layer1_size] += gama * g * neu1e[c];  //���µ�ǰ��wi
          for (c = 0; c < layer1_size; c++) relation_vec[rid][c] += gama * g * neu1e[c];  //����r����
        }
      }
    } else {  //train skip-gram
      //��������棬������Ŀ�꺯���ĶԳ��ԣ�p(u|w) = p(w|u), u in Context(w). ���忴 http://blog.csdn.net/mytestmy/article/details/26969149
      for (a = b; a < window * 2 + 1 - b; a++) if (a != window) {
        c = sentence_position - window + a;
        if (c < 0) continue;
        if (c >= sentence_length) continue;
        last_word = sen[c];
        if (last_word == -1) continue;
        l1 = last_word * layer1_size;
        for (c = 0; c < layer1_size; c++) neu1e[c] = 0;
        // HIERARCHICAL SOFTMAX
        if (hs) for (d = 0; d < vocab[word].codelen; d++) { //����Ҷ�ӽڵ�
          f = 0;
          l2 = vocab[word].point[d] * layer1_size; //point��·���ϵĽڵ�
          // Propagate hidden -> output
          for (c = 0; c < layer1_size; c++) f += syn0[c + l1] * syn1[c + l2]; //�ڻ�
          if (f <= -MAX_EXP) continue;
          else if (f >= MAX_EXP) continue;
          else f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]; //sigmoid
          // 'g' is the gradient multiplied by the learning rate
          g = (1 - vocab[word].code[d] - f) * alpha; //�ݶ�һ����
          // Propagate errors output -> hidden
          for (c = 0; c < layer1_size; c++) neu1e[c] += g * syn1[c + l2]; //���ز�����
          // Learn weights hidden -> output
          for (c = 0; c < layer1_size; c++) syn1[c + l2] += g * syn0[c + l1]; //�����ڲ��ڵ�����
        }
        // NEGATIVE SAMPLING
        if (negative > 0) for (d = 0; d < negative + 1; d++) {
          if (d == 0) {
            target = word;
            label = 1;
          } else {
            next_random = next_random * (unsigned long long)25214903917 + 11;
            target = table[(next_random >> 16) % table_size];
            if (target == 0) target = next_random % (vocab_size - 1) + 1;
            if (target == word) continue;
            label = 0;
          }
          l2 = target * layer1_size;
          f = 0;
          for (c = 0; c < layer1_size; c++) f += syn0[c + l1] * syn1neg[c + l2];
          if (f > MAX_EXP) g = (label - 1) * alpha;
          else if (f < -MAX_EXP) g = (label - 0) * alpha;
          else g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * alpha;
          for (c = 0; c < layer1_size; c++) neu1e[c] += g * syn1neg[c + l2];
          for (c = 0; c < layer1_size; c++) syn1neg[c + l2] += g * syn0[c + l1];
        }
        // Learn weights input -> hidden
        for (c = 0; c < layer1_size; c++) syn0[c + l1] += neu1e[c]; //���µ��ǵ�ǰ�����ĵĴ�����
      }
    }
    sentence_position++;
    if (sentence_position >= sentence_length) {
      sentence_length = 0;
      continue;
    }
  }
  fclose(fi);
  free(neu1);
  free(neu1e);
  pthread_exit(NULL);
}
// ѵ��ģ��
void TrainModel() {
  long int a, b, c, d;
  FILE *fo;
  FILE *fv;
  FILE *fvv;
  // Ĭ��12���߳�
  pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
  printf("Starting training using file %s\n", train_file);
  starting_alpha = alpha;
  if (read_vocab_file[0] != 0) ReadVocab(); else LearnVocabFromTrainFile();
  if (save_vocab_file[0] != 0) SaveVocab();
  if (output_file[0] == 0) return;
  ReadTriplets();
  InitNet();
  if (negative > 0) InitUnigramTable();
  start = clock();
  // �����߳�
  for (a = 0; a < num_threads; a++) pthread_create(&pt[a], NULL, TrainModelThread, &a);
  for (a = 0; a < num_threads; a++) pthread_join(pt[a], NULL);
  // ������
  fv = fopen("vector-orgin.txt", "w");
  fvv = fopen("word2id.txt", "w");
  fo = fopen(output_file, "wb");
  if (classes == 0) {
    // Save the word vectors
    fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
    for (a = 0; a < vocab_size; a++) {
      fprintf(fo, "%s ", vocab[a].word);
      if (binary) for (b = 0; b < layer1_size; b++) fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
      else for (b = 0; b < layer1_size; b++) fprintf(fo, "%lf ", syn0[a * layer1_size + b]);
      fprintf(fo, "\n");
      fprintf(fvv, "%s %ld\n",vocab[a].word,a);
      for (b = 0; b < layer1_size; b++) fprintf(fv, "%lf ", syn0[a * layer1_size + b]);
      fprintf(fv, "\n");
    }
  } else {
    // Run K-means on the word vectors
    // ���������о���
    int clcn = classes, iter = 10, closeid;
    // ����������
    int *centcn = (int *)malloc(classes * sizeof(int)); 
    // ÿ���ʶ�Ӧ���
    int *cl = (int *)calloc(vocab_size, sizeof(int));
    real closev, x;
    // ÿ��������ĵ�
    real *cent = (real *)calloc(classes * layer1_size, sizeof(real));
    // ��ʼ����ÿ���ʷ��䵽һ����
    for (a = 0; a < vocab_size; a++) cl[a] = a % clcn;
    for (a = 0; a < iter; a++) {
      // ���ĵ�����
      for (b = 0; b < clcn * layer1_size; b++) cent[b] = 0;
      for (b = 0; b < clcn; b++) centcn[b] = 1;
      // ����ÿ��������ֵ
      for (c = 0; c < vocab_size; c++) {
        for (d = 0; d < layer1_size; d++) cent[layer1_size * cl[c] + d] += syn0[c * layer1_size + d];
        centcn[cl[c]]++; //��Ӧ����������1
      }
      // �����������
      for (b = 0; b < clcn; b++) {
        closev = 0;
        for (c = 0; c < layer1_size; c++) {
          cent[layer1_size * b + c] /= centcn[b]; //��ֵ
          closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
        }
        closev = sqrt(closev);
        // ���ĵ��һ��
        for (c = 0; c < layer1_size; c++) cent[layer1_size * b + c] /= closev;
      }
      // ���д����·���
      for (c = 0; c < vocab_size; c++) {
        closev = -10;
        closeid = 0;
        for (d = 0; d < clcn; d++) {
          x = 0;
          for (b = 0; b < layer1_size; b++) x += cent[layer1_size * d + b] * syn0[c * layer1_size + b];
          if (x > closev) {
            closev = x;
            closeid = d;
          }
        }
        cl[c] = closeid;
      }
    }
    // Save the K-means classes
    for (a = 0; a < vocab_size; a++) fprintf(fo, "%s %d\n", vocab[a].word, cl[a]);
    free(centcn);
    free(cent);
    free(cl);
  }
  fclose(fo);
  fclose(fv);
  fclose(fvv);
}

int ArgPos(char *str, int argc, char **argv) {
  int a;
  for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
    if (a == argc - 1) {
      printf("Argument missing for %s\n", str);
      exit(1);
    }
    return a;
  }
  return -1;
}

int main(int argc, char **argv) {
  int i;
  if (argc == 1) {
    printf("WORD VECTOR estimation toolkit v 0.1c\n\n");
    printf("Options:\n");
    printf("Parameters for training:\n");
    // �����ļ����ѷִʵ�����
    printf("\t-train <file>\n");
    printf("\t\tUse text data from <file> to train the model\n");
    // �����ļ�����Ԫ������
    printf("\t-triplet <file>\n");
    printf("\t\tUse triplets data from <file> to train the model\n");
	// ����ļ�����������ʾ���
    printf("\t-output <file>\n");
    printf("\t\tUse <file> to save the resulting word vectors / word clusters\n");
    // ������ά�ȣ�Ĭ��100
    printf("\t-size <int>\n");
    printf("\t\tSet size of word vectors; default is 100\n");
    // ���ڴ�С��Ĭ��5
    printf("\t-window <int>\n");
    printf("\t\tSet max skip length between words; default is 5\n");
    // ��Ƶ��ֵ��Ĭ��0���Ը�Ƶ������²���
    printf("\t-sample <float>\n");
    printf("\t\tSet threshold for occurrence of words. Those that appear with higher frequency in the training data\n");
    printf("\t\twill be randomly down-sampled; default is 1e-3, useful range is (0, 1e-5)\n");
    // ���ò��softmax��Ĭ��0��������
    printf("\t-hs <int>\n");
    printf("\t\tUse Hierarchical Softmax; default is 0 (not used)\n");
    // ����NEG��Ĭ��5
    printf("\t-negative <int>\n");
    printf("\t\tNumber of negative examples; default is 5, common values are 3 - 10 (0 = not used)\n");
    // �߳�����Ĭ��12
    printf("\t-threads <int>\n");
    printf("\t\tUse <int> threads (default 12)\n");
    // ��������Ĭ��5
    printf("\t-iter <int>\n");
    printf("\t\tRun more training iterations (default 5)\n");
    // ��Ƶ��С��ֵ��Ĭ��5��С����ֵ����
    printf("\t-min-count <int>\n");
    printf("\t\tThis will discard words that appear less than <int> times; default is 5\n");
    // ѧϰ�ʣ�Ĭ����0.025(skip-gram),0.05(cbow)
    printf("\t-alpha <float>\n");
    printf("\t\tSet the starting learning rate; default is 0.025 for skip-gram and 0.05 for CBOW\n");
    // ��������Ĭ��0
    printf("\t-classes <int>\n");
    printf("\t\tOutput word classes rather than word vectors; default number of classes is 0 (vectors are written)\n");
    // debugģʽ��Ĭ��2
    printf("\t-debug <int>\n");
    printf("\t\tSet the debug mode (default = 2 = more info during training)\n");
    // �����ƴ洢��Ĭ��0���������ļ�ʱ�����ö�����
    printf("\t-binary <int>\n");
    printf("\t\tSave the resulting vectors in binary moded; default is 0 (off)\n");
    // ����ʻ��
    printf("\t-save-vocab <file>\n");
    printf("\t\tThe vocabulary will be saved to <file>\n");
    // ��ȡ��ͳ�ƺô�Ƶ�Ĵʻ��
    printf("\t-read-vocab <file>\n");
    printf("\t\tThe vocabulary will be read from <file>, not constructed from the training data\n");
    // ����ģ�ͣ�1 CBOW��0 skip-gram��Ĭ��1
    printf("\t-cbow <int>\n");
    printf("\t\tUse the continuous bag of words model; default is 1 (use 0 for skip-gram model)\n");
    // ʾ��
    printf("\nExamples:\n");
    printf("./word2vec -train data.txt -triplet triplets.txt -output vec.txt -size 200 -window 5 -sample 1e-4 -negative 5 -hs 0 -binary 0 -cbow 1 -iter 3\n\n");
    return 0;
  }
  output_file[0] = 0;
  save_vocab_file[0] = 0;
  read_vocab_file[0] = 0;
  if ((i = ArgPos((char *)"-size", argc, argv)) > 0) layer1_size = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-train", argc, argv)) > 0) strcpy(train_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-triplet", argc, argv)) > 0) strcpy(triplet_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-save-vocab", argc, argv)) > 0) strcpy(save_vocab_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-read-vocab", argc, argv)) > 0) strcpy(read_vocab_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-debug", argc, argv)) > 0) debug_mode = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-binary", argc, argv)) > 0) binary = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-cbow", argc, argv)) > 0) cbow = atoi(argv[i + 1]);
  if (cbow) alpha = 0.05;
  if ((i = ArgPos((char *)"-alpha", argc, argv)) > 0) alpha = atof(argv[i + 1]);
  if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strcpy(output_file, argv[i + 1]);
  if ((i = ArgPos((char *)"-window", argc, argv)) > 0) window = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-sample", argc, argv)) > 0) sample = atof(argv[i + 1]);
  if ((i = ArgPos((char *)"-hs", argc, argv)) > 0) hs = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-negative", argc, argv)) > 0) negative = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-threads", argc, argv)) > 0) num_threads = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-iter", argc, argv)) > 0) iter = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-min-count", argc, argv)) > 0) min_count = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-classes", argc, argv)) > 0) classes = atoi(argv[i + 1]);
  //cout << triplet_file << endl;
  vocab = (struct vocab_word *)calloc(vocab_max_size, sizeof(struct vocab_word));
  vocab_hash = (int *)calloc(vocab_hash_size, sizeof(int));
  expTable = (real *)malloc((EXP_TABLE_SIZE + 1) * sizeof(real));
  for (i = 0; i < EXP_TABLE_SIZE; i++) {
    expTable[i] = exp((i / (real)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP); // Precompute the exp() table
    expTable[i] = expTable[i] / (expTable[i] + 1);                   // Precompute f(x) = x / (x + 1)
  }
  TrainModel();
  return 0;
}
