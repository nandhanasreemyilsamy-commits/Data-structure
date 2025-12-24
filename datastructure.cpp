#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>
using namespace std;
struct Voter {
    int voterID;
    string name;
    string district;
    bool hasVoted;
    Voter* next;
};
class VoterList {
public:
    Voter* head;
    VoterList() : head(NULL) {}
    void addVoter(int id, string name, string district) {
        Voter* newNode = new Voter{id, name, district, false, NULL};
        if (!head)
            head = newNode;
        else {
            Voter* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }
    void displayVoters() {
        Voter* temp = head;
        while (temp) {
            cout << "ID: " << temp->voterID << " | Name: " << temp->name
                 << " | District: " << temp->district
                 << " | Voted: " << (temp->hasVoted ? "Yes" : "No") << endl;
            temp = temp->next;
        }
    }
};
class VoterHash {
public:
    unordered_map<int, bool> hashTable;
    void registerVoter(int id) { hashTable[id] = false; }
    bool verifyVoter(int id) { return hashTable.find(id) != hashTable.end(); }
    bool hasVoted(int id) { return hashTable[id]; }
    void markVoted(int id) { hashTable[id] = true; }
};
struct District {
    string name;
    int votes;
    vector<District*> subDistricts;
    District(string n) : name(n), votes(0) {}
};
void addSubDistrict(District* parent, District* child) {
    parent->subDistricts.push_back(child);
}
int countVotes(District* node) {
    if (!node) return 0;
    int total = node->votes;
    for (auto child : node->subDistricts)
        total += countVotes(child);
    return total;
}
class PollingGraph {
public:
    int n;
    vector<vector<int>> adj;
    PollingGraph(int size) : n(size), adj(size) {}
    void addConnection(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    void displayConnections() {
        for (int i = 0; i < n; i++) {
            cout << "Station " << i << " connected to: ";
            for (int j : adj[i]) cout << j << " ";
            cout << endl;
        }
    }
};
int main() {
    VoterList voterList;
    VoterHash voterHash;
    queue<int> voteQueue;
    stack<int> voteStack;
    District* root = new District("Central Commission");
    District* state1 = new District("State A");
    District* district1 = new District("District X");
    addSubDistrict(root, state1);
    addSubDistrict(state1, district1);
    PollingGraph graph(3);
    graph.addConnection(0, 1);
    graph.addConnection(1, 2);
    voterList.addVoter(101, "Alice", "District X");
    voterList.addVoter(102, "Bob", "District X");
    voterList.addVoter(103, "Charlie", "District X");
    voterHash.registerVoter(101);
    voterHash.registerVoter(102);
    voterHash.registerVoter(103);
    voterList.displayVoters();
    graph.displayConnections();
    int voters[] = {101, 102, 103};
    for (int id : voters) {
        if (voterHash.verifyVoter(id) && !voterHash.hasVoted(id)) {
            voteQueue.push(id);
        }
    }
    while (!voteQueue.empty()) {
        int voterID = voteQueue.front();
        voteQueue.pop();
        voteStack.push(voterID);
        voterHash.markVoted(voterID);
        district1->votes++;
    }
    if (!voteStack.empty()) {
        int lastVote = voteStack.top();
        voteStack.pop();
        voterHash.hashTable[lastVote] = false;
        district1->votes--;
    }
    cout << "Total votes in " << district1->name << ": " << district1->votes << endl;
    cout << "Total votes in " << root->name << ": " << countVotes(root) << endl;
    return 0;
}
