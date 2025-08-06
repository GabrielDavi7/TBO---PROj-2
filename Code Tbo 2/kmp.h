#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

// Função principal para buscar padrão no texto usando KMP com wildcard '*'
std::vector<int> KMP(const std::string& text, const std::string& pattern);

#endif 
