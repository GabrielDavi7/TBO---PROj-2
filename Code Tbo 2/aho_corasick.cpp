#include "aho_corasick.h"
#include <queue>

void AhoCorasick::inserir(const std::string& palavra) {
    TrieNode* atual = raiz;
    for (char c : palavra) {
        if (!atual->filhos[c]) {
            atual->filhos[c] = new TrieNode();
        }
        atual = atual->filhos[c];
    }
    atual->fimPalavra = true;
    atual->palavras.insert(palavra);
}

void AhoCorasick::construirFalhas() {
    std::queue<TrieNode*> fila;
    raiz->falha = raiz;

    // Para filhos diretos da raiz, ponteiro de falha é raiz
    for (auto& [c, filho] : raiz->filhos) {
        filho->falha = raiz;
        fila.push(filho);
    }

    // BFS para construir falhas nos níveis mais profundos
    while (!fila.empty()) {
        TrieNode* atual = fila.front();
        fila.pop();

        for (auto& [c, filho] : atual->filhos) {
            TrieNode* f = atual->falha;

            // Seguir falha até achar um filho com caractere c ou voltar pra raiz
            while (f != raiz && !f->filhos.count(c)) {
                f = f->falha;
            }

            if (f->filhos.count(c) && f->filhos[c] != filho) {
                filho->falha = f->filhos[c];
            } else {
                filho->falha = raiz;
            }

            // Herdar palavras do nó de falha
            filho->palavras.insert(filho->falha->palavras.begin(), filho->falha->palavras.end());

            fila.push(filho);
        }
    }
}

std::map<std::string, std::vector<int>> AhoCorasick::buscar(std::string_view texto) const {
    std::map<std::string, std::vector<int>> resultados;
    TrieNode* atual = raiz;

    for (size_t i = 0; i < texto.size(); ++i) {
        char c = texto[i];

        // Segue os ponteiros de falha enquanto não achar caminho com c
        while (atual != raiz && !atual->filhos.count(c)) {
            atual = atual->falha;
        }

        if (atual->filhos.count(c)) {
            atual = atual->filhos.at(c);
        }

        // Para todas as palavras terminando neste nó, registra ocorrência
        for (const auto& palavra : atual->palavras) {
            int pos = static_cast<int>(i) - static_cast<int>(palavra.size()) + 1;
            if (pos >= 0)
                resultados[palavra].push_back(pos);
        }
    }
    return resultados;
}
