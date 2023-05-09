#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;

        Node(const Type &val, Node *next) : value(val), next_node(next) {}

        Type value;
        Node *next_node = nullptr;
    };

    template<typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node *node)
                : node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType *;
        using reference = ValueType &;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type> &other) noexcept
                : node_(other.node_) {
        }

        BasicIterator &operator=(const BasicIterator &rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<Type> &rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type> &rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<const Type> &rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type> &rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator &operator++() {
            assert(node_);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) {
            auto this_copy(*this);
            ++(*this);
            return this_copy;
        }

        [[nodiscard]] reference operator*() const {
            assert(node_);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const {
            assert(node_);
            return &node_->value;
        }

    private:
        Node *node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type &;
    using const_reference = const value_type &;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        assign(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList &other) : SingleLinkedList() {
        assign(other.begin(), other.end());
    }

    ~SingleLinkedList() {
        clear();
    }

    SingleLinkedList &operator=(const SingleLinkedList &other) {
        if (this != &other) {
            SingleLinkedList temp(other);
            this->swap(temp);
        }
        return *this;
    }

    Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    Iterator end() noexcept {
        return Iterator(nullptr);
    }

    ConstIterator begin() const noexcept {
        return cbegin();
    }

    ConstIterator end() const noexcept {
        return cend();
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    ConstIterator before_begin() const noexcept {
        return ConstIterator(&head_);
    }

    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node *>(&head_));
    }

    [[nodiscard]] size_t size() const noexcept {
        return size_;
    }

    [[nodiscard]] bool empty() const noexcept {
        return size_ == 0;
    }

    void push_front(const Type &value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    Iterator insert_after(ConstIterator pos, const Type &value) {
        auto &prev_node = pos.node_;
        assert(prev_node);
        prev_node->next_node = new Node(value, prev_node->next_node);
        ++size_;
        return Iterator{prev_node->next_node};
    }

    void pop_front() noexcept {
        assert(head_.next_node != nullptr);
        erase_after(before_begin());

    }

    Iterator erase_after(ConstIterator pos) noexcept {
        Node *removed_node = pos.node_->next_node;
        pos.node_->next_node = removed_node->next_node;
        delete removed_node;
        --size_;
        return Iterator(pos.node_->next_node);
    }

    void clear() noexcept {
        while (head_.next_node) {
            delete std::exchange(head_.next_node, head_.next_node->next_node);
            --size_;
        }
    }

    void swap(SingleLinkedList &other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }


private:
    template<typename InputIterator>
    void assign(InputIterator from, InputIterator to) {
        SingleLinkedList<Type> tmp;
        auto node_ptr = tmp.before_begin();

        while (from != to) {
            node_ptr = tmp.insert_after(node_ptr, *from);
            ++from;
        }
        swap(tmp);
    }

    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (&lhs == &rhs)
           || (lhs.size() == rhs.size()
               && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}