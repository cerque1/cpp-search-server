#pragma once

#include <iterator>
#include <vector>

template <typename Iterator>
class IteratorRange{
public:
    explicit IteratorRange(const Iterator range_begin, const Iterator range_end) : range_begin_(range_begin), range_end_(range_end){
    }

    Iterator begin() const{
        return range_begin_;
    }
    
    Iterator end() const{
        return range_end_;
    }

    int size(){
        return distance(range_begin_, range_end_);
    }
private:
    Iterator range_begin_;
    Iterator range_end_;
};

template <typename Iterator>
class Paginator{
public:
    explicit Paginator(const Iterator range_begin, const Iterator range_end, size_t page_size) : page_size_(page_size){
        if(distance(range_begin, range_end) == 0)
                return;
        Iterator next_iter = range_begin;
        for(int i = 0; i < (distance(range_begin, range_end) / page_size_) + 1; i++){
            if(distance(next_iter, range_end) <= 0 || next_iter == range_end){
                break;
            }
            else if(distance(next_iter, range_end) / page_size > 0){
                pages_.push_back(IteratorRange(next_iter, next(next_iter, page_size_)));
                next_iter = next(next_iter, page_size_);
            }
            else if(distance(next_iter, range_end) % page_size_ != 0){
                pages_.push_back(IteratorRange(next_iter, next(next_iter, distance(next_iter, range_end))));
                next_iter = range_end;
            }
        }
    }

    typename std::vector<IteratorRange<Iterator>>::const_iterator begin() const{
        return pages_.begin();
    }

    typename std::vector<IteratorRange<Iterator>>::const_iterator end() const{
        return pages_.end();
    }
    
    size_t size() const{
        return pages_.size();
    }

private:
    int page_size_;
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size){
    return Paginator(begin(c), end(c), page_size);
}