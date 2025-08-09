#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <string>

struct TrieNodeTrie {
    std::unordered_map<char, TrieNodeTrie*> filhos;
    bool ehFim = false;
};

class Trie {
private:
    TrieNodeTrie* raiz;
public:
    Trie();
    ~Trie();
    void inserir(const std::string& palavra);
    bool buscar(const std::string& palavra);
    void liberar(TrieNodeTrie* no);
};

#endif
