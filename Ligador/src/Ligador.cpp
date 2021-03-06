// Software básico - Trabalho 02 - Ligador

// Includes:
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <fstream>
#include <iomanip>
#include "Modulo.hpp"

// Defines:
#define DEBUG 0

// Namespace:
using namespace std;

// Function headers
void printTable(map<string, int> table);
void printVectorInt(vector<int> items);
vector<int> concatenateCodes(vector<Modulo*> objs);

// Main function:
int main(int argc, char const *argv[])
{

  if(argc < 2) {
    cout << "Erro: Insira no mínimo 1 arquivo para ligar" << endl;
    cout << "Modo de uso: ligador nome_do_arquivo_sem_obj ..." << endl;
    exit(1);
  }

  int num_modulos = argc-1;
  vector<Modulo*> objs;

  int correction_accumulator;
  vector<int> correction_table;

  map<string, int> global_definitions_table;

  vector<int> output_code;
  string output_name;
  fstream output_file;

  // Opens each file
  for(int i = 1; i < argc; i++) {
    objs.push_back(new Modulo(string(argv[i])));
  }

  // Parses each file
  for(auto const& obj : objs) {
    obj->parse();
  }

  // Prints debug info
  if(DEBUG >= 1){
    for(auto const& obj : objs) {
      obj->printAllData();
    }
  }

  // Generates Correction Factor Table
  correction_accumulator = 0;
  for (auto const &obj : objs) {
    correction_table.push_back(correction_accumulator);
    correction_accumulator += obj->getCodeSize();
  }

  if(DEBUG >= 1) {
    cout << "Correction Table" << endl;
    printVectorInt(correction_table);
  }

  // Generates Global Definitions Table
  for(int i = 0; i < num_modulos; i++) {
    for(auto const& item : objs[i]->getDefinitionsTable()) {
      global_definitions_table[item.first] = item.second + correction_table[i];
    }
  }

  if(DEBUG >= 1) {
    cout << "Global Definitions Table" << endl;
    printTable(global_definitions_table);
  }

  for(int i = 0; i < num_modulos; i++) {
    objs[i]->fixCrossReferences(global_definitions_table);
    objs[i]->fixRelativeAddresses(correction_table[i]);
  }

  output_code = concatenateCodes(objs);

  if(DEBUG >= 1) {
    cout << "Outputted Code" << endl;
    printVectorInt(output_code);
  }

  output_name = argv[1]; // Outputfile is name of the first file
  output_name += ".e"; // followed by .e

  output_file.open(output_name, ios::out);
  if(!output_file.is_open()) {
    cout << "Erro: não é possível criar arquivo de saída " << output_name << endl;
    exit(4);
  }

  for(auto const& i : output_code) {
    output_file << i << " ";
  }
  output_file << endl;

  // Clean up (free allocated memory)
  for(auto const& obj : objs) {
    delete obj;
  }
  output_file.close();

  cout << "Arquivo ligado e salvo em: " << output_name << endl;

  return 0;

}

void printTable(map<string, int> table)
{
  size_t label_size, max = 5;
  string label;
  int address;
  for (auto const &line : table)
  {
    label_size = line.first.length();
    if (label_size > max)
      max = label_size;
  }
  cout << "| LABEL";
  for (size_t i = 5; i < max; i++)
  {
    cout << " ";
  }
  cout << " | ADDRESS |" << endl;
  for (auto const &line : table)
  {
    label = line.first;
    address = line.second;
    cout << "| " << label;
    for (size_t i = label.length(); i < max; i++)
    {
      cout << " ";
    }
    cout << " | " << setw(7) << setfill(' ') << address << " |" << endl;
  }
  cout << endl;
}

void printVectorInt(vector<int> items)
{
  for(auto const &i : items) {
    cout << i << " ";
  }
  cout << endl << endl;
}

vector<int> concatenateCodes(vector<Modulo*> objs)
{
  vector<int> code;

  for(auto const& obj : objs) {
    for(auto const& code_byte : obj->getCode()) {
      code.push_back(code_byte);
    }
  }

  return code;
}
