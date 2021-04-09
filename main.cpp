#include <iostream>
#include <vector>
#include <map>
#include <fstream>

class Container
{
public:
    Container(std::string dataPath);
    int GetKeyPosition(std::string key) const;
    ~Container();
    enum errorCodes {
        KEY_LENGTH_EXCEEDED = -1,
        KEY_NOT_FOUND = -2
    };
    static const int maxKeyLength {5};
private:
    void PrepareData(std::string dataPath);
    void AddWord(std::string word);
    struct Node;
    Node* head;
    std::vector<Node*> notEndedWordPointers;
    void Delete(Node* node);
    int wordCount;
};

struct Container::Node
{
    std::map<char, Node*> children;
    int place;
};

Container::Container(std::string dataPath)
{
    head = new Node();
    head->place = -1;
    wordCount = 0;
    PrepareData(dataPath);
}

Container::~Container()
{
    Delete(head);
}

void Container::Delete(Node* node)
{
    for(auto [key, child]: node->children)
    {
        Delete(child);
    }
    delete node;
}

void Container::PrepareData(std::string dataPath)
{
    std::fstream data(dataPath);
    notEndedWordPointers.assign(5,NULL);
    notEndedWordPointers[0] = head;
    while(!data.eof())
    {
        std::string word;
        std::string croppedWord = "";
        data >> word;
        for(char symbol: word)
        {
            if(isalpha(symbol))
            {
                croppedWord += symbol;
                if(croppedWord.length() == maxKeyLength) break;
            }
        }
        if(croppedWord.length() == 0) continue;
        AddWord(croppedWord);
        wordCount += 1;
    }
}

void Container::AddWord(std::string word)
{
    for(int depth = maxKeyLength - 1; depth >= 0; --depth)
    {
        if((int)word.length() < depth) notEndedWordPointers[depth] = NULL;
        if(notEndedWordPointers[depth] == NULL) continue;
        Node* currentNode = notEndedWordPointers[depth];
        if(currentNode->children.count(word[depth]) == 0)
        {
            Node* child = new Node;
            child->place = wordCount - depth;
            currentNode->children.emplace(word[depth], child);
        }
        if(depth != maxKeyLength - 1)
        {
            notEndedWordPointers[depth + 1] = currentNode->children[word[depth]];
        }
    }
}

int Container::GetKeyPosition(std::string key) const
{
    if(key.length() > maxKeyLength)
    {
        return KEY_LENGTH_EXCEEDED;
    }
    Node* currentNode = head;
    for(auto letter: key)
    {
        if(currentNode->children.count(letter) == 0)
        {
            return KEY_NOT_FOUND;
        }
        currentNode = currentNode->children[letter];
    }
    return currentNode->place;
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    if(argc != 2)
    {
        std::cout << "Program must have only one argument: data file name.\n";
        return 1;
    }
    Container ctr(argv[1]);
    std::cout<<"To exit the program enter \"/exit\"\n";
    std::string input;
    bool term = false;
    while(!term)
    {
        std::cin >> input;
        if(input == "/exit")
        {
            term = true;
            continue;
        }
        int response = ctr.GetKeyPosition(input);
        if(response == ctr.KEY_NOT_FOUND)
        {
            std::cout << "Key not found.\n";
        }
        else if(response == ctr.KEY_LENGTH_EXCEEDED)
        {
            std::cout << "Key length exceeded (" << ctr.maxKeyLength << ").\n";
        }
        else std::cout << response << '\n';
    }
}
