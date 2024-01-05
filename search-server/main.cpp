#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        document_count_++;
        const vector<string> words = SplitIntoWordsNoStop(document);
        double word_share_in_document = 1. / words.size();
        for (const string& word : words)
            word_to_documents_freqs_[word][document_id] += word_share_in_document;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        vector<Document> matched_documents = FindAllDocuments(query_words.plus_words, query_words.minus_words);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    map<string, map<int, double>> word_to_documents_freqs_;

    set<string> stop_words_;

    int document_count_ = 0;

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word))
                words.push_back(word);
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query;
        vector<string> words = SplitIntoWordsNoStop(text);
        for (const string& word : words) {
            if (word[0] == '-')
                query.minus_words.insert(word.substr(1));
            else
                query.plus_words.insert(word);
        }
        return query;
    }

    double ComputeIDF(const string& word) const{
        return log(document_count_ / static_cast<double>(word_to_documents_freqs_.at(word).size()));
    }

    map<int, double> DocumentToRelevance(const set<string>& query_words) const {
        map<int, double> document_to_relevance;

        for (const string& query_word : query_words) {
            if (word_to_documents_freqs_.count(query_word)) {
                double idf = ComputeIDF(query_word);
                for (const auto& [document_id, tf] : word_to_documents_freqs_.at(query_word))
                    document_to_relevance[document_id] += idf * tf;
            }
        }
        return document_to_relevance;
    }

    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        vector<Document> matched_documents;
        map<int, double> document_to_relevance = DocumentToRelevance(query_words);

        if (!document_to_relevance.empty() && !minus_words.empty()) {
            for (const string& minus_word : minus_words) {
                if (word_to_documents_freqs_.count(minus_word))
                    for (const pair<int, double>& relevance_pair : word_to_documents_freqs_.at(minus_word))
                        if (document_to_relevance.count(relevance_pair.first))
                            document_to_relevance.erase(relevance_pair.first);
            }
        }

        if (!document_to_relevance.empty())
            for (const auto& [document_id, relevance] : document_to_relevance)
                matched_documents.push_back({ document_id, relevance });
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}
