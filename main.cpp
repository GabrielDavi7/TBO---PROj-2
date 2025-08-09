// g++ -std=c++17 -o programa main.cpp kmp.cpp aho_corasick.cpp trie.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <chrono>

#include "kmp.h"
#include "aho_corasick.h"
#include "trie.h"
#include "utf8.h"

using namespace std;

// Converte UTF-8 para minúsculas Unicode
string toLowerUtf8(const string& s) {
    string out;
    auto it = s.begin();
    while (it != s.end()) {
        uint32_t cp = utf8::next(it, s.end());
        cp = towlower(cp);
        utf8::append(cp, back_inserter(out));
    }
    return out;
}

// Lê arquivo texto em modo binário
string lerArquivo(const string& path) {
    ifstream f(path, ios::binary);
    if (!f) {
        cerr << "Erro ao abrir: " << path << '\n';
        return "";
    }
    return {istreambuf_iterator<char>(f), istreambuf_iterator<char>()};
}

// Remove espaços início/fim
inline void trim(string& s) {
    auto start = find_if_not(s.begin(), s.end(), ::isspace);
    auto end = find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    s = (start < end) ? string(start, end) : "";
}

// Lê dicionário e insere na Trie
void lerDicionario(const string& path, Trie& dic) {
    ifstream f(path, ios::binary);
    if (!f) { cerr << "Erro abrir dicionário\n"; return; }
    string line;
    while (getline(f, line)) {
        trim(line);
        if (!line.empty()) dic.inserir(toLowerUtf8(line));
    }
}

// Verifica ortografia: palavras não encontradas na Trie
void verificarOrtografia(const string& texto, Trie& dic) {
    unordered_set<string> erradas;
    string palavra;
    auto it = texto.begin();
    while (it != texto.end()) {
        uint32_t cp = utf8::next(it, texto.end());
        if (iswalpha(cp)) {
            cp = towlower(cp);
            utf8::append(cp, back_inserter(palavra));
        } else {
            if (!palavra.empty()) {
                if (!dic.buscar(palavra)) erradas.insert(palavra);
                palavra.clear();
            }
        }
    }
    if (!palavra.empty() && !dic.buscar(palavra)) erradas.insert(palavra);

    cout << "Palavras incorretas: " << erradas.size() << '\n';
    for (auto& p : erradas) cout << p << '\n';
}

// Lê palavras do console para busca
vector<string> lerPalavras() {
    vector<string> ps;
    cout << "Digite palavras (vazio para terminar):\n";
    string l;
    while (getline(cin, l)) {
        trim(l);
        if (l.empty()) break;
        ps.push_back(toLowerUtf8(l));
    }
    return ps;
}

// Busca ingênua simples
vector<int> buscaIngenua(const string& texto, const string& p) {
    vector<int> pos;
    int n = (int)texto.size(), m = (int)p.size();
    for (int i = 0; i <= n - m; ++i) {
        int j = 0;
        while (j < m && texto[i+j] == p[j]) ++j;
        if (j == m) pos.push_back(i);
    }
    return pos;
}

// Executa busca KMP
void executarKMP(const string& texto, const vector<string>& palavras) {
    if (palavras.empty()) {
        cout << "Nenhuma palavra para buscar.\n";
        return;
    }
    for (auto& p : palavras) {
        auto p_norm = toLowerUtf8(p);
        cout << "KMP: \"" << p << "\": ";
        auto start = chrono::high_resolution_clock::now();
        auto pos = KMP(texto, p_norm);
        auto end = chrono::high_resolution_clock::now();
        if (pos.empty()) cout << "Nenhuma ocorrencia.\n";
        else {
            for (auto i : pos) cout << i << " ";
            cout << '\n';
        }
        cout << "Tempo: " << chrono::duration<double, milli>(end-start).count() << " ms\n";
    }
}

// Executa busca ingênua
void executarIngenua(const string& texto, const vector<string>& palavras) {
    if (palavras.empty()) {
        cout << "Nenhuma palavra para buscar.\n";
        return;
    }
    for (auto& p : palavras) {
        auto p_norm = toLowerUtf8(p);
        cout << "Ingenuo: \"" << p << "\": ";
        auto start = chrono::high_resolution_clock::now();
        auto pos = buscaIngenua(texto, p_norm);
        auto end = chrono::high_resolution_clock::now();
        if (pos.empty()) cout << "Nenhuma ocorrencia.\n";
        else {
            for (auto i : pos) cout << i << " ";
            cout << '\n';
        }
        cout << "Tempo: " << chrono::duration<double, milli>(end-start).count() << " ms\n";
    }
}

// Executa busca Aho-Corasick sequencial
void executarAhoCorasick(const string& texto, const vector<string>& palavras) {
    if (palavras.empty()) {
        cout << "Nenhuma palavra para buscar.\n";
        return;
    }

    AhoCorasick ac;
    map<string, string> mapa_norm;
    for (auto& p : palavras) {
        auto p_norm = toLowerUtf8(p);
        mapa_norm[p] = p_norm;
        ac.inserir(p_norm);
    }
    ac.construirFalhas();

    auto start = chrono::high_resolution_clock::now();
    auto resultados = ac.buscar(texto);
    auto end = chrono::high_resolution_clock::now();

    cout << "Aho-Corasick resultados:\n";
    for (auto& [orig, norm] : mapa_norm) {
        auto it = resultados.find(norm);
        if (it == resultados.end() || it->second.empty())
            cout << orig << ": Nenhuma ocorrencia.\n";
        else {
            sort(it->second.begin(), it->second.end());
            cout << orig << ": ";
            for (int pos : it->second) cout << pos << " ";
            cout << '\n';
        }
    }
    cout << "Tempo: " << chrono::duration<double, milli>(end - start).count() << " ms\n";
}

int main() {
    string texto = lerArquivo("texto.txt");
    if (texto.empty()) {
        cerr << "Erro ou arquivo vazio\n";
        return 1;
    }
    texto = toLowerUtf8(texto);
    cout << "Texto com " << texto.size() << " bytes carregado\n";

    Trie dicionario;
    lerDicionario("pt_BR.dic", dicionario);

    vector<string> palavras;

    while (true) {
        cout << "\n1 - Ler palavras\n2 - Ingenua\n3 - KMP\n4 - Aho-Corasick\n5 - Ortografia\n0 - Sair\nEscolha: ";
        string op;
        getline(cin, op);
        if (op.empty()) continue;
        int escolha = -1;
        try { escolha = stoi(op); } catch (...) { cout << "Inválido\n"; continue; }
        if (escolha == 0) break;

        switch (escolha) {
            case 1: palavras = lerPalavras(); break;
            case 2: executarIngenua(texto, palavras); break;
            case 3: executarKMP(texto, palavras); break;
            case 4: executarAhoCorasick(texto, palavras); break;
            case 5: verificarOrtografia(texto, dicionario); break;
            default: cout << "Inválido\n";
        }
    }
    cout << "Fim\n";
    return 0;
}
