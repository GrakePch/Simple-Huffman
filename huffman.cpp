#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

using namespace std;

// Huffman Tree Node
class hfNode {
   private:
    bool isLeaf = false;
    hfNode* l;
    hfNode* r;
    char c;
    int f;

   public:
    hfNode(int freq) : f{freq} {}

    hfNode(char c, int freq) : c{c}, f{freq}, isLeaf{true} {}

    void setLeft(hfNode* n) { this->l = n; }
    void setRight(hfNode* n) { this->r = n; }
    hfNode* getLeft() const { return this->l; }
    hfNode* getRight() const { return this->r; }

    char getChar() const { return this->c; }
    int getFreq() const { return this->f; }
    bool getIsLeaf() const { return this->isLeaf; }

    hfNode* join(hfNode* r) {
        hfNode* newNode = new hfNode(this->f + r->f);
        newNode->setLeft(this);
        newNode->setRight(r);
        return newNode;
    }

    void print(bool newLine = true) const {
        if (isLeaf)
            cout << f << "[" << c << "]";
        else
            cout << f;
        if (newLine)
            cout << endl;
    }

    void printTree(int layer = 0) const {
        for (int i = 0; i < layer; i++)
            cout << "-";
        this->print();
        if (this->isLeaf) return;
        this->l->printTree(layer + 1);
        this->r->printTree(layer + 1);
    }
};

// Lambda Func to compare HfNode
auto cmp = [](const hfNode* l, const hfNode* r) { return l->getFreq() > r->getFreq(); };

// Gen Funcs
void genWorkingQ(
    priority_queue<hfNode*, vector<hfNode*>, decltype(cmp)>* q,
    unordered_map<char, int> m);
hfNode* genHuffmanTree(
    priority_queue<hfNode*, vector<hfNode*>, decltype(cmp)>* q,
    bool debug = true);
void genHuffmanCodes(unordered_map<char, string>* codeMap, hfNode* n, string currCode);

// Print Funcs
void printMap(unordered_map<char, int>& m);
template <typename Q>
void printQ(Q q);
char* getCode(unordered_map<char, string>& codes, char c, char char0 = '0', char char1 = '1');
void printCodeMap(unordered_map<char, string>& codes, char char0 = '0', char char1 = '1');

int main() {
    string input;
    int inputLength = 0;
    unordered_map<char, int> m;
    unordered_map<char, string> huffmanCodes;
    cout << "HUFFMAN CODING" << endl;
    cout << "> ENTER TEXT TO BE COMPRESSED" << endl;
    getline(cin, input);
    inputLength = input.length();

    // Generate frequencies map: input => map<char, freq>
    for (int i = 0; i < inputLength; i++) {
        if (!m.count(input[i])) {
            m[input[i]] = 0;
        }
        m[input[i]]++;
    }

    // Gen and push initial hfNodes to priority queue
    priority_queue<hfNode*, vector<hfNode*>, decltype(cmp)> workingQ(cmp);

    // Print info
    cout << "========" << endl;
    cout << "UNCOMPRESSED LENGTH: " << inputLength << " chars" << endl;
    cout << "UNCOMPRESSED SIZE: " << inputLength * 8 << " bits" << endl;
    genWorkingQ(&workingQ, m);
    // cout << "Char frequencies: ";
    // printQ(workingQ);
    hfNode* root = genHuffmanTree(&workingQ, false);
    // root->printTree();
    string currCode;
    genHuffmanCodes(&huffmanCodes, root, currCode);
    cout << "CODING MAP: " << endl;
    printCodeMap(huffmanCodes);

    cout << "COMPRESSED DATA: " << endl;
    int encodedSize = 0;
    for (int i = 0; i < inputLength; i++) {
        encodedSize += huffmanCodes[input[i]].length();
        cout << getCode(huffmanCodes, input[i]);
    }
    cout << endl;
    cout << "COMPRESSED SIZE: " << encodedSize << " bits" << endl;
    cout << "COMPRESSION RATIO (NET): " << (float)(inputLength * 8) / encodedSize << endl;
    cout << "SPACE SAVING (NET): " << 1 - (float)encodedSize / (inputLength * 8)<< endl;

    return 0;
}

void printMap(unordered_map<char, int>& m) {
    for (auto iter = m.begin(); iter != m.end(); ++iter)
        cout << iter->first << ": " << iter->second << endl;
}

void genWorkingQ(priority_queue<hfNode*, vector<hfNode*>, decltype(cmp)>* q, unordered_map<char, int> m) {
    for (auto iter = m.begin(); iter != m.end(); ++iter) {
        q->push(new hfNode(iter->first, iter->second));
    }
}

template <typename Q>
void printQ(Q q) {
    for (; !q.empty(); q.pop()) {
        q.top()->print(false);
        cout << "  ";
    }
    cout << endl;
}

char* getCode(unordered_map<char, string>& codes, char c, char char0, char char1) {
    char* code = new char[codes[c].length() + 1];
    for (int i = 0; i < codes[c].length(); i++)
        code[i] = codes[c][i] == '0' ? char0 : char1;
    code[codes[c].length()] = '\0';
    return code;
}

void printCodeMap(unordered_map<char, string>& codes, char char0, char char1) {
    for (int i = 0; i < 256; i++)
        if (codes.count((char)i))
            cout << (char)i << ": " << getCode(codes, (char)i, char0, char1) << endl;
}

hfNode* genHuffmanTree(priority_queue<hfNode*, vector<hfNode*>, decltype(cmp)>* q, bool debug) {
    while (q->size() >= 2) {
        hfNode* minFNode = q->top();
        q->pop();
        hfNode* nextMinFNode = q->top();
        q->pop();
        hfNode* combinedNode = nextMinFNode->join(minFNode);
        q->push(combinedNode);
        if (debug) {
            cout << "---" << endl;
            cout << "L: ";
            nextMinFNode->print();
            cout << "R: ";
            minFNode->print();
            cout << "N: ";
            combinedNode->print();
            cout << "Q: ";
            printQ(*q);
        }
    }

    hfNode* root = q->top();
    q->pop();
    return root;
}

void genHuffmanCodes(unordered_map<char, string>* codeMap, hfNode* n, string currCode) {
    if (n->getIsLeaf()) {
        (*codeMap)[n->getChar()] = currCode.length() ? currCode : "0";
        return;
    }
    genHuffmanCodes(codeMap, n->getLeft(), currCode + "0");
    genHuffmanCodes(codeMap, n->getRight(), currCode + "1");
};
