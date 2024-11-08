#include "request_queue.h"
#include <algorithm>

RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server){
}
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    return AddFindRequest(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
    });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}

int RequestQueue::GetNoResultRequests() const {
    return count_if(requests_.begin(), requests_.end(), [](const QueryResult& res){
        return res.count_results_ == 0;
    });
}

RequestQueue::QueryResult::QueryResult(int time_result, int count_results) : time_result_(time_result), count_results_(count_results){}