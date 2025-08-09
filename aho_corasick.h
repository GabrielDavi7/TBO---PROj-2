#ifndef AHO_CORASICK_H
#define AHO_CORASICK_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>
#include <string_view>

// Estrutura para cada nó da trie
struct TrieNode {
    std::unordered_map<char, TrieNode*> filhos; // Filhos por caractere
    TrieNode* falha = nullptr;                  // Ponteiro de falha
    bool fimPalavra = false;                     // Indica fim da palavra
    std::set<std::string> palavras;             // Palavras terminando aqui

    ~TrieNode() {
        for (auto& [_, filho] : filhos) {
            delete filho;
        }
    }
};

class AhoCorasick {
private:
    TrieNode* raiz = new TrieNode();

public:
    AhoCorasick() = default;

    // Não permitir cópia (por simplicidade)
    AhoCorasick(const AhoCorasick&) = delete;
    AhoCorasick& operator=(const AhoCorasick&) = delete;

    ~AhoCorasick() { delete raiz; }

    // Insere palavra na trie
    void inserir(const std::string& palavra);

    // Constroi o autômato (ponteiros de falha) - chamar UMA vez após inserir todas as palavras
    void construirFalhas();

    // Busca múltiplas palavras no texto e retorna mapa palavra -> posições encontradas
    std::map<std::string, std::vector<int>> buscar(std::string_view texto) const;
};

#endif 
