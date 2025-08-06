#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "kmp.h"

using namespace std;

// Função que cria a tabela de prefixo (LPS)
vector<int> buildPrefixTable(const string& pattern) {
    int n = pattern.size();
    vector<int> lps(n, 0);
    int j = 0; // comprimento do prefixo atual

    for (int i = 1; i < n; i++) {
        while (j > 0 && pattern[i] != pattern[j] && pattern[j] != '*') {
            j = lps[j - 1];
        }

        // Permitir wildcard no padrão: '*' pode casar com qualquer caractere
        if (pattern[i] == pattern[j] || pattern[j] == '*') {
            j++;
            lps[i] = j;
        }
    }
    return lps;
}
// Algoritmo KMP com suporte ao wildcard '*'
vector<int> KMP(const string& text, const string& pattern) {
    vector<int> posicoes;
    if (pattern.empty()) return posicoes;

    vector<int> lps = buildPrefixTable(pattern);
    int j = 0; // índice do padrão

    for (int i = 0; i < (int)text.size(); i++) {
        while (j > 0 && !(pattern[j] == '*' || pattern[j] == text[i])) {
            j = lps[j - 1];
        }

        if (pattern[j] == '*' || pattern[j] == text[i]) {
            j++;
        }

        if (j == (int)pattern.size()) {
            posicoes.push_back(i - j + 1); // ocorrência encontrada
            j = lps[j - 1];
        }
    }

    return posicoes;
}
