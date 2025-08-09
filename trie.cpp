// trie.cpp
#include "trie.h"
#include <cctype>

Trie::Trie() {
    raiz = new TrieNodeTrie();
}

Trie::~Trie() {
    liberar(raiz);
}

void Trie::inserir(const std::string& palavra) {
    TrieNodeTrie* no = raiz;
    for (char c : palavra) {
        if (!std::isalpha(c)) continue;
        c = std::tolower(c);
        if (!no->filhos[c]) {
            no->filhos[c] = new TrieNodeTrie();
        }
        no = no->filhos[c];
    }
    no->ehFim = true;
}

bool Trie::buscar(const std::string& palavra) {
    TrieNodeTrie* no = raiz;
    for (char c : palavra) {
        if (!std::isalpha(c)) continue;
        c = std::tolower(c);
        if (!no->filhos[c]) return false;
        no = no->filhos[c];
    }
    return no->ehFim;
}

void Trie::liberar(TrieNodeTrie* no) {
    for (auto& par : no->filhos) {
        liberar(par.second);
    }
    delete no;
}
